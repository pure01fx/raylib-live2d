target_sources(${LIB_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismRenderer_Raylib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismRenderer_Raylib.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismShader_Raylib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismShader_Raylib.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismClippingContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CubismClippingContext.hpp
)

target_include_directories(${LIB_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Raylib/src
)
