cmake_minimum_required(VERSION 4.0)
project(ipass_webgpu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BACKEND "WGPU" CACHE STRING "Backend implementation of WebGPU")
set_property(CACHE BACKEND PROPERTY STRINGS WGPU DAWN)

include(FetchContent)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/source/shaders/*.wgsl"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"
)

add_executable(ipass_webgpu ${SOURCES})

set_target_properties(ipass_webgpu PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  COMPILE_WARNING_AS_ERROR ON
)

if(MSVC)
  target_compile_options(ipass_webgpu PRIVATE /W4)
else()
  target_compile_options(ipass_webgpu PRIVATE -Wall -Wextra -pedantic)
endif()

# ------------------------------------------
# WebGPU Backend Config
# ------------------------------------------
if(EMSCRIPTEN) # Building for WASM is still kinda broken, need to fix
  set_target_properties(ipass_webgpu PROPERTIES SUFFIX ".html")
  target_link_options(ipass_webgpu PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")

  FetchContent_Declare(webgpu_hpp
    GIT_REPOSITORY https://github.com/eliemichel/WebGPU-Cpp
    GIT_TAG main
  )
  FetchContent_MakeAvailable(webgpu_hpp)
  target_include_directories(ipass_webgpu PRIVATE ${webgpu_hpp_SOURCE_DIR})

else() # WGPU or DAWN backend
  # Common dependencies for native backends
  if(MINGW)
    set(WEBGPU_BACKEND "WGPU" CACHE STRING "" FORCE)
    set(WGPU_NATIVE_RUNTIME "gnu" CACHE STRING "" FORCE)
  endif()

  # Fetch WebGPU-distribution
  FetchContent_Declare(webgpu
    GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
    GIT_TAG 17dcd42a7683355e7a40ac4e97e77f36dff5b5ab
  )

  # Fetch GLFW
  FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
  )
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

  # Fetch glfw3webgpu
  set(GLFW3WEBGPU_BACKEND ${BACKEND} CACHE STRING "" FORCE)
  FetchContent_Declare(glfw3webgpu
    GIT_REPOSITORY https://github.com/eliemichel/glfw3webgpu
    GIT_TAG main
  )

  # Fetch webgpu.hpp
  FetchContent_Declare(webgpu_hpp
    GIT_REPOSITORY https://github.com/eliemichel/WebGPU-Cpp
    GIT_TAG main
  )

  FetchContent_MakeAvailable(webgpu glfw glfw3webgpu webgpu_hpp)

  if(WIN32)
    target_compile_definitions(glfw3webgpu PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
  endif()

  target_link_libraries(ipass_webgpu PRIVATE webgpu glfw glfw3webgpu)
  target_include_directories(ipass_webgpu PRIVATE
    ${webgpu_SOURCE_DIR}/include
    ${webgpu_hpp_SOURCE_DIR}
  )
  target_compile_definitions(ipass_webgpu PRIVATE WEBGPU_BACKEND_${BACKEND})

  # Copy wgpu_native.dll for WGPU backend
  if(BACKEND STREQUAL "WGPU")
    add_custom_command(TARGET ipass_webgpu POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_BINARY_DIR}/_deps/wgpu-windows-x86_64-gnu-release-src/lib/wgpu_native.dll"
      "$<TARGET_FILE_DIR:ipass_webgpu>"
      COMMENT "Copying wgpu_native.dll to output directory"
    )
  endif()
endif()

if(NOT EMSCRIPTEN)
  enable_testing()
  add_subdirectory(tests)
endif()