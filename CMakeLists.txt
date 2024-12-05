cmake_minimum_required(VERSION 3.14)
project(IMUFHE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDDARD 14)
set(CMAKE_CXX_STANDDARD_REQUIRED ON)

find_package(OpenFHE CONFIG REQUIRED)

set(CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS})
include_directories(${OPEN_MP_INCLUDES})
include_directories(${OpenFHE_INCLUDE})
include_directories(${OpenFHE_INCLUDE}/third-party/include)
include_directories(${OpenFHE_INCLUDE}/core)
include_directories(${OpenFHE_INCLUDE}/pke)
include_directories(${OpenFHE_INCLUDE}/binfhe)

link_directories(${OpenFHE_LIBDIR})
link_directories(${OPENMP_LIBRARIES})


file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


