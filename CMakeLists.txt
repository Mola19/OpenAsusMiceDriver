cmake_minimum_required(VERSION 3.5.0)
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

add_subdirectory(dependencies/hidapi ${CMAKE_BINARY_DIR}/dependencies/hidapi)
target_link_libraries(${PROJECT_NAME} hidapi::hidapi)

set_target_properties(openasusmicedriver
	PROPERTIES PUBLIC_HEADER "OpenAsusMiceDriver.hpp;AsusMiceDriver.hpp;Devices.hpp;ReadCallback.hpp")

if (UNIX)
	include(GNUInstallDirs)

	install(TARGETS openasusmicedriver
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openasusmicedriver)
endif (UNIX)

if (MSVC)
	install(FILES ${CMAKE_BINARY_DIR}/dependencies/hidapi/src/windows/Release/hidapi.dll DESTINATION bin)
	install(FILES ${CMAKE_BINARY_DIR}/dependencies/hidapi/src/windows/Release/hidapi.lib DESTINATION lib)
	install(FILES ${CMAKE_SOURCE_DIR}/dependencies/hidapi/hidapi/hidapi.h DESTINATION include)

	install(TARGETS openasusmicedriver
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif (MSVC)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
