cmake_minimum_required(VERSION 3.0.0)
project(openasusmicedriver VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB SOURCES
    "OpenAsusMiceDriver.cpp"
	"AsusMiceDriver.cpp"
	"ReadCallback.cpp"
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(openasusmicedriver SHARED ${SOURCES})

set(BUILD_SHARED_LIBS TRUE)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_subdirectory(dependencies/hidapi)
target_link_libraries(${PROJECT_NAME} hidapi)

add_subdirectory(dependencies/simpleble/simpleble ${CMAKE_BINARY_DIR}/dependencies/simpleble)
target_link_libraries(${PROJECT_NAME} simpleble::simpleble)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
