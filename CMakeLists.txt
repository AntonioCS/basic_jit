cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project("JIT Video series" DESCRIPTION "Following JIT youtube playlist https://www.youtube.com/playlist?list=PLvdK1vRmp8wMzH4w_8sQ30NKU3Bt4Cc-M" VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_EXTENSIONS FALSE)

if (MSVC)
	add_definitions(-D_UNICODE -DUNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
	add_definitions(-D_ATL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif()

#msvc options
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

add_subdirectory(video1)
add_subdirectory(video2) 
add_subdirectory(video3) 
