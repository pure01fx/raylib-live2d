# raylib-live2d

## Notice

### After DoDrawModel is called

* The blend mode will be restored to the default value (BLEND_ALPHA).
* The renderer will call rlViewport(rlGetFramebufferWidth(), rlGetFramebufferHeight()) and restore FBO status after rendering.
* This library assumes that you only use CCW. It won't be a problem if you never change it.
* GL_STENCIL_TEST is not disabled and glColorMask(1, 1, 1, 1) is not called in this library compared with the official OpenGL renderer.
* glClearColor(1, 1, 1, 1) will be called.

## Debugging

You can copy Resources/ in the zip downloaded from Live2D official page to the target directory of raylib-live2d-test.

## Environment

Due to some copyright issues, you should download the core by yourself at [Live2D official page](https://www.live2d.com/download/cubism-sdk/download-native/). See Core/README.txt

## Notice

### After DoDrawModel is called

* Backface culling status is chaned.
* The blend mode will be restored to the default value (BLEND_ALPHA).
* The renderer will call rlViewport(rlGetFramebufferWidth(), rlGetFramebufferHeight()) and restore FBO status after rendering.
* This library assumes that you only use CCW. It won't be a problem if you never change it.
* GL_STENCIL_TEST is not disabled and glColorMask(1, 1, 1, 1) is not called in this library compared with the official OpenGL renderer.
* glClearColor(1, 1, 1, 1) will be called.

## Debugging

You can copy Resources/ in the zip downloaded from Live2D official page to the target directory of raylib-live2d-test.

## Environment

Due to some copyright issues, you should download the core by yourself at [Live2D official page](https://www.live2d.com/download/cubism-sdk/download-native/). See Core/README.txt

## License

You should check the LICENSE.md file in framework/ first

All changes are under the Apache License 2.0

## Todo

1. Anisotropy
2. CMake
3. Voice and lipSync
4. Disposal (l2dReleaseModel, l2dUnload?, textures...)
