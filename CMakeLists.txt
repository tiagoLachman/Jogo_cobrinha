cmake_minimum_required(VERSION 3.27)

set(VCPKG_DIR "C:/vcpkg/installed/x64-windows")
set(VCPKG_TARGET_TRIPLET "x64-windows"
      CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")

set(SRCROOT src)
set(INCROOT include)

project(Jogo_cobrinha)

set(SRC
      ${SRCROOT}/main.cpp
      
)

include_directories("C:/vcpkg/installed/x64-windows/include")
link_directories("C:/vcpkg/installed/x64-windows/lib")

add_executable(Jogo_cobrinha ${SRC})

target_include_directories(Jogo_cobrinha PRIVATE ${INCROOT})