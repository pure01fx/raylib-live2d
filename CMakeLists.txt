cmake_minimum_required(VERSION 3.10)

project(raylib-live2d)

# Configure raylib
add_subdirectory(Raylib)

# Configure Cubism Framework
set(FRAMEWORK_SOURCE ../../../RaylibRendering RaylibRendering)
add_subdirectory(Framework)

# Configure raylib-live2d
add_subdirectory(Lib)

# Configure test executable
# You can refer to this to link raylib-live2d
add_executable(raylib-live2d-test Test/main.c)
target_link_libraries(raylib-live2d-test PRIVATE raylib-live2d raylib)

# Copy Core dll to build result dir
add_custom_command(
  TARGET raylib-live2d-test
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy ${CMAKE_CURRENT_SOURCE_DIR}/Core/dll/windows/x86_64/Live2DCubismCore.dll $<TARGET_FILE_DIR:raylib-live2d-test>
)
