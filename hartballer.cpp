/*
  Hartballer perfect sphere rendering
  Copyright (C) 2015 Peter Wagener ( wagenerp@mail.uni-paderborn.de )
  
  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the 
  Free Software Foundation, either version 3 of the License, or (at your option)
  any later version. 
  This program is distributed in the hope that it will be useful, but WITHOUT 
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
  details. 
  You should have received a copy of the GNU General Public License along with 
  this program. If not, see <http://www.gnu.org/licenses/>.
*/

#define TITLE "hartballer"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

#define ITERATE_TIMEOUT -1
#define AUTORENDER 1

#define DIYYMA_MAIN 1

#include <diyyma/diyyma.h>
#include <IL/il.h>

#include "spheres.h"
#include "config.h"

FPSCameraComponent  *camera;
AssetReloader       *reloader;
SphereRenderPass    *rp_spheres;

Matrixf MV;
Matrixf MVP;

int init(int argn, char **argv) {
  ilInit();
  
  vfs_registerPath("shader/",REPOSITORY_MASK_SHADER);
  vfs_registerPath("meshes/",REPOSITORY_MASK_MESH);
  vfs_registerPath("textures/",REPOSITORY_MASK_TEXTURE);
  
  camera=new FPSCameraComponent();
  camera->grab();
  camera->pos.set(-1.2,1.8,1.7);
  camera->angles.set(0,-0.4,0.6);
  camera->P.setPerspective(80,1.0/0.75,ZNEAR,ZFAR);
  camera->compute();
  camera->speed=0.1;
  
  rp_spheres=new SphereRenderPass();
  rp_spheres->grab();
  rp_spheres->setContextSource(camera);
  if (1) {
    int a=10;
    Vector4f *data;
    float r=0.5/a;
    float d=1.0/a;
    int i=0;
    data=(Vector4f*)malloc(a*a*a*sizeof(Vector4f));
    for(int x=0;x<a;x++) for(int y=0;y<a;y++) for(int z=0;z<a;z++) 
      data[i++].set((x+0.5)*d,(y+0.5)*d,(z+0.5)*d,r);
    rp_spheres->setData(data,a*a*a);
    free((void*)data);
  }
  
  registerComponent(reloader=new AssetReloader());
  reloader->grab();
  reloader->delay=0.1;
  *reloader+=reg_shd()->get("inst-insphere");
  
  registerComponent(camera);
  registerComponent(rp_spheres);
  
  return 1;
}

void render_pre() {
  glClearColor(0.1,0.1,0.12,0);
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  MV=
    camera->V;
  MVP=
    camera->VP;
}

void render_post() {
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  glMatrixMode(GL_MODELVIEW); glLoadMatrixf(&MVP.a11);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
  glDepthMask(0);
  
  // render axis cross
  glBegin(GL_LINES);
    glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(100,0,0);
    glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,100,0);
    glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,100);
  glEnd();
  
  
  // render slice
  glColor4f(1,1,1,0.5);
  glPushMatrix();
    glTranslatef(0.25,0,0); // offset of the world + slice
    glScalef(1,1,1); // and scale of the world
      
    glBegin(GL_QUADS);
      glVertex3f(0,-0.1,-0.1);
      glVertex3f(0,1.1,-0.1);
      glVertex3f(0,1.1,1.1);
      glVertex3f(0,-0.1,1.1);
    
    glEnd();
  glPopMatrix();
  
  // render boundary
  glPushMatrix();
    
    glTranslatef(0,0,0); // offset
    glScalef(1,1,1); // and scale of the world
    
    glBegin(GL_QUADS);
      
      //front (-x), back (+x), right (-y), left (+y), bottom (-z), top (+z)
      
      glColor4f(1,0,0,0.2);
      glVertex3f(0,0,0); glVertex3f(0,1,0); glVertex3f(0,1,1); glVertex3f(0,0,1);
      glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1);
      glColor4f(0,1,0,0.2);
      glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1);
      glVertex3f(0,1,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(0,1,1);
      glColor4f(0,0,1,0.2);
      glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(0,1,0);
      glVertex3f(0,0,1); glVertex3f(1,0,1); glVertex3f(1,1,1); glVertex3f(0,1,1);
      
    glEnd();
  glPopMatrix();
  
  glDepthMask(GL_TRUE);
}

void iterate(double dt, double t) {
}

void event(const SDL_Event *ev) {
  switch(ev->type) {
    case SDL_KEYDOWN:
      switch(ev->key.keysym.sym) {
        case SDLK_F4:
          if (ev->key.keysym.mod&(KMOD_LALT|KMOD_RALT))
            quit();
          break;
      }
      break;
  }
}

void cleanup() {
}
