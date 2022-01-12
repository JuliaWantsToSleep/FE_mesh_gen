cmake_minimum_required(VERSION 3.0.0)

project(FE_mesh_gen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(DataStructures)
add_subdirectory(StepTranslator)
add_subdirectory(MeshGenMethods)
add_subdirectory(Solution)
add_subdirectory(main)
