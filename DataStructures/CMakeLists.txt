project(DataStructures)

find_package(OpenCASCADE REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCASCADE_INCLUDE_DIR}
                                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

set (OCCT_LIBS TKSTEP;TKIGES;TKXSBase;TKBRep;TKG3d;TKTopAlgo;TKMath;TKernel;TKGeomBase)

foreach(LIB ${OCCT_LIBS})
    target_link_libraries(${PROJECT_NAME} PUBLIC
    "${OpenCASCADE_LIBRARY_DIR}/lib${LIB}.so")
endforeach()
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
