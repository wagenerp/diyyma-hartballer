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

#include <diyyma/renderpass.h>
#include <diyyma/util.h>
class SphereRenderPass : 
  public IRenderPass,
  public ISceneContextReferrer {
  
  public:
    enum {
      // I was under the impression that buffer objects have at maximum
      // some 2^16 bytes but it seems that is not the case. If not all spheres
      // are rendered, lower this number to span them across multiple buffers.
      MAX_BUFFER_COUNT = 4000000000,
      MAX_BUFFER_SIZE = 16*MAX_BUFFER_COUNT,
      
      
      POSITION_LOCATION = 0,
      SPHERE_DATA_LOCATION = 1,

    };
  
  protected:
    GLuint _bVertices;
    ARRAY(GLuint,_bInstances);
    Shader *_shd;
    size_t _count;
    
    GLint _u_MVP;
    GLint _u_MV;
  
  public:
    SphereRenderPass() {
      _shd=reg_shd()->get("inst-insphere");
      _shd->grab();
      _u_MVP =_shd->locate("u_MVP");
      _u_MV  =_shd->locate("u_MV");
      
      this->flags|=RP_DEPTH_TEST;
      
      ARRAY_INIT(_bInstances);
      
      glGenBuffers(1,&_bVertices);
      {
        Vector2f buf[4];
        int i=0;
        buf[i++].set(-1,-1);
        buf[i++].set(1,-1);
        buf[i++].set(1,1);
        buf[i++].set(-1,1);
        glBindBuffer(GL_ARRAY_BUFFER,_bVertices);
        glBufferData(GL_ARRAY_BUFFER,sizeof(buf),buf,GL_STATIC_DRAW);
        glEnableVertexAttribArray(POSITION_LOCATION);
        glVertexAttribPointer(POSITION_LOCATION,2,GL_FLOAT,GL_FALSE,0,0);
      }
      glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    
    ~SphereRenderPass() {
      glDeleteBuffers(1,&_bVertices);
      glDeleteBuffers(_bInstances_n,_bInstances_v);
    }
    
    
    void setData(Vector4f *data, size_t count) {
      
      _count=count;
      size_t cb=sizeof(Vector4f)*count;
      size_t nbuf=cb/MAX_BUFFER_SIZE;
      Vector4f *p=data,*e=data+count;
      int i;
      if (nbuf*MAX_BUFFER_SIZE<cb) nbuf++;
      
      if (_bInstances_n<nbuf) {
        i=nbuf-_bInstances_n;
        ARRAY_SETSIZE(_bInstances,nbuf);
        glGenBuffers(i,&_bInstances_v[_bInstances_n-i]);
      } else if (_bInstances_n>nbuf) {
        i=_bInstances_n-nbuf;
        glDeleteBuffers(i,&_bInstances_v[_bInstances_n-i]);
        ARRAY_SETSIZE(_bInstances,nbuf)
      }
      
      for(i=0;i<nbuf;i++) {
        glBindBuffer(GL_ARRAY_BUFFER,_bInstances_v[i]);
        glBufferData(
          GL_ARRAY_BUFFER,
          (i+1==nbuf)?(cb-(nbuf-1)*MAX_BUFFER_SIZE):MAX_BUFFER_SIZE,p,
          GL_STATIC_DRAW);
        p+=MAX_BUFFER_COUNT;
      }
      
      glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    
    virtual void render() {
      int i;
      SceneContext ctx;
      
      glEnable(GL_DEPTH_TEST);
      
      ctx=_contextSource->context();
  
      _shd->bind();
      if (-1!=_u_MV  ) glUniformMatrix4fv(_u_MV ,1,0,&ctx.MV.a11);
      if (-1!=_u_MVP ) glUniformMatrix4fv(_u_MVP,1,0,&ctx.MVP.a11);
      
      for(i=0;i<_bInstances_n;i++) {
        
        glBindBuffer(GL_ARRAY_BUFFER,_bVertices);
        glEnableVertexAttribArray(POSITION_LOCATION);
        glVertexAttribPointer(POSITION_LOCATION,2,GL_FLOAT,GL_FALSE,0,0);
        
        glBindBuffer(GL_ARRAY_BUFFER,_bInstances_v[i]);
        glEnableVertexAttribArray(SPHERE_DATA_LOCATION);
        glVertexAttribPointer(
          SPHERE_DATA_LOCATION,4,GL_FLOAT,GL_FALSE,sizeof(Vector4f),0);
        glVertexAttribDivisor(SPHERE_DATA_LOCATION,1);
        
        glDrawArraysInstanced(
          GL_QUADS,0,4,
          (i+1==_bInstances_n) 
          ? _count-(_bInstances_n-1)*MAX_BUFFER_COUNT
          : MAX_BUFFER_SIZE);
        glBindBuffer(GL_ARRAY_BUFFER,0);
      }
      _shd->unbind();
      
    
    }
    virtual int event(const SDL_Event *ev) { return 0; }
    virtual void iterate(double dt, double time) { }
  
};
