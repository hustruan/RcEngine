RcEngine
========

RcEngine is a graphics engine implemented in C++,  inspired by OGRE and [KlayGE]. 

###Features:
1. Direct3D11 or OpenGL 4.2+ rendering system.
2. XML baded Effect system, HLSL or GLSL shaders, caching of bytecode (not finished).
3. Support all kinds of shader, including vertex, geometry, tesselation, pixel, compute shaders. 
3. Multiple render path, including forward, light pre-pass deferred lighting, tile based deferred shading, Forward+.
4. Hardware skeletal animation, animation blend, crossfade.
5. Shadow map for directional light and spot light, including PSSM, VSM, PCF.
6. Inbuilt simple UI system, distance field font (not finished).
7. Model/Animation/Material import from FBX or OGRE Binary mesh format.

###Todo:
1. Change to Entity-Componment architecture.
2. Add support for physical engine.
3. Add support for lua script.


[KlayGE]: http://www.klayge.org/
