# diyyma-hartballer
Perfect sphere renderer based on the diyyma framework

There i was just minding my own business when i was nerdsniped by a fellow
student and just could not help but implement this proof of concept.

Hartballer is capable of rendering a large number of perfect spheres, placed
statically with arbitrary position and radius utilizing instanced rendering
of billboards, being scaled and aligned in the vertex shader. 
These billboards are then subjected to local raycasting in the fragment shader, 
determining whether or not the fragment is actually part of the sphere under
question.
Phong shading and updating of the depth buffer value does the rest.

## files

config.h
  common defines shared between shader and application code

hartballer.cpp
  main application code

spheres.h
  sphere render pass, C-side of the renderer

shader/inst-insphere.fsd
  fragment shader of the renderer
shader/inst-insphere.vsd
  vertex shader of the renderer