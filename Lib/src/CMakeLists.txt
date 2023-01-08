target_sources(raylib-live2d
    PRIVATE
    ../include/raylib-live2d.h
    dll.cpp
    LAppAllocator.cpp # For CubismFramework::StartUp
    LAppAllocator.hpp
    LAppDefine.cpp
    LAppDefine.hpp
    LAppModel.cpp
    LAppModel.hpp
    LAppPal.cpp
    LAppPal.hpp
)
