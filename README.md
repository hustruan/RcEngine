RcEngine
========

RcEngine is a graphics engine implemented in C++,  inspired by OGRE and [KlayGE]. 

###Features:

- Direct3D11 or OpenGL 4.2+ rendering system.
- XML baded Effect system, HLSL or GLSL shaders, caching of bytecode (not finished).
- Support all kinds of shader, including vertex, geometry, tesselation, pixel, compute shaders. 
- Multiple render path, including forward, light pre-pass deferred lighting, tile based deferred shading, Forward+.
- Hardware skeletal animation, animation blend, crossfade.
- Shadow map for directional light and spot light, including PSSM, VSM, PCF.
- Inbuilt simple UI system, distance field font (not finished).
- Model/Animation/Material import from FBX or OGRE Binary mesh format.

###Todo:
- Change to Entity-Componment architecture.
- Add support for physical engine.
- Add support for lua script.

###Samples:

**GUI**
![](https://github.com/hustruan/RcEngine/tree/master/RcEngine/Samples/GUIApp.png)



[KlayGE]: http://www.klayge.org/
