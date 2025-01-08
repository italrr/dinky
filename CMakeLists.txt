cmake_minimum_required(VERSION 3.10)

project(dinky CXX C)

set(CMAKE_C_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-std=c++11 -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/builds/cmake)

set (dinky_src
    src/common/Tools.cpp
    src/common/Types.cpp
    src/renderer/Bitmap.cpp
    src/renderer/Image.cpp
)

add_library(dinky_bin ${dinky_src})

IF(LINUX)
    find_package(Freetype REQUIRED)
    target_include_directories(dinky_bin PRIVATE ${FREETYPE_INCLUDE_DIRS})
	target_link_libraries(dinky_bin -m64 -lfreetype)
ENDIF(LINUX)

IF(MINGW)
    target_link_libraries(dinky_bin -m64 -lfreetype)
ENDIF(MINGW)

add_executable(dinky src/dinky.cpp)
target_link_libraries(dinky dinky_bin)