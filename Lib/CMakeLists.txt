cmake_minimum_required(VERSION 3.16)

set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(CORE_PATH ${SDK_ROOT_PATH}/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/Framework)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect architecture.
if(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:x64")
  set(ARCH x86_64)
elseif(CMAKE_EXE_LINKER_FLAGS STREQUAL "/machine:X86")
  set(ARCH x86)
else()
  message(FATAL_ERROR "[${APP_NAME}] Invalid linker flag ${CMAKE_EXE_LINKER_FLAGS}.")
endif()

# Import Cubism Core as static library.
add_library(Live2DCubismCore STATIC IMPORTED)
set(CORE_LIB_PREFIX ${CORE_PATH}/dll/windows/${ARCH})
set_target_properties(Live2DCubismCore PROPERTIES
  IMPORTED_LOCATION ${CORE_LIB_PREFIX}/Live2DCubismCore.lib
  INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

add_library(raylib-live2d STATIC)
add_subdirectory(src)
target_include_directories(raylib-live2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(raylib-live2d PRIVATE ${FRAMEWORK_PATH}/src)
target_include_directories(raylib-live2d PRIVATE ../RaylibRendering)
target_include_directories(raylib-live2d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../core/include)
target_link_libraries(raylib-live2d
  Live2DCubismCore
  Framework
  raylib
  # Solve the MSVCRT confliction.
  debug -NODEFAULTLIB:libcmtd.lib
  optimized -NODEFAULTLIB:libcmt.lib
)

# Build in multi-process.
target_compile_options(raylib-live2d PRIVATE /MP)
