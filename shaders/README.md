# Shaders

All the shaders of the engine is located within this directory. They are somewhat sorted.

All Shaders are written in OpenGL GLSL 330 compliant code, with some minor differences. All of the differences are listed below.

## Config

A lot of the time we came into the situation that the shaders required some of the config settings in order to do what it was suppose to. This required us to use uniform variables so we could set these after the config was loaded.

Since our shader loading is minimal, it meant that we had to set them whenever we used a shader, subjecting it to quite a few unnecessary state changes.

To solve this, we use the `_CFG_` variable. This variable is replaced by the value of the config at that time before the shader is compiled.

This means two things:

1. No more state changes during runtime due to config settings
2. Compiler optimizations as these are now constant values

The latter of these two things is significant as GLSL can never perform compile-time optimizations on variables that are uniform.

As an example, a loop from 0 to UNIFORM_VALUE will always be a loop, as it goes towards a currently unknown value. If you replace the UNIFORM_VALUE with a constant value, the loop may be unrolled.

We realize that this requires the shaders to be recompiled if any configurations are changed. This is not really an issue as the engine mostly reinitializes the entire state when configurations are changed through the normal means, such as options menu.

### Example

An example usage of this variable is:

```glsl
#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;

out vec2 Texcoord;

const vec2 screenRes = _CFG_.Graphics.resolution;
uniform vec2 guiOffset;

float convertCoords(float x, float s) {
  return ((x - s/2)/(s/2));
}

void main() {
  Texcoord = texcoord;
  vec2 newPos = position + guiOffset;

  newPos.x =  convertCoords(newPos.x, screenRes.x);
  newPos.y = -convertCoords(newPos.y, screenRes.y);

  gl_Position = vec4(newPos, 0.0, 1.0);
}
```

The above example is taken from our `GUI.vs` shader. `_CFG_.Graphics.resolution` will be replaced with the variable that is stored within the config, for instance `vec2(1024.0, 768.0)`.

### Settings

The settings that are currently available can be found in [src/Utils/CFG.hpp#L114](../src/Utils/CFG.hpp#L114). All the `keys` in that map are available in the shaders.

## Layout Bindings

Version 4.20 of GLSL added a neat feature of binding textures to specific texture image units from within the shader instead of doing this outside of GLSL.

However, this feature is so new that most ultrabooks (that we also use to demonstrate on) does not support this. Since this is the only feature of 4.20 we actually use, the Shader and Program loading within the Engine has been written to support this without requiring 4.20

This means that you can do the following on samplers

```glsl
layout(binding=0) uniform sampler2D diffuseTex;
```

while still only needing version 3.30 in GLSL.
