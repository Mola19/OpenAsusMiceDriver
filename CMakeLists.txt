cmake_minimum_required(VERSION 3.5.0)
project(openasusmousedriver VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB SOURCES
	"OpenAsusMouseDriver.cpp"
	"AsusMouseDriver.cpp"
	"ReadCallback.cpp"
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(openasusmousedriver SHARED ${SOURCES})

set(BUILD_SHARED_LIBS TRUE)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set_target_properties(openasusmousedriver
	PROPERTIES PUBLIC_HEADER "OpenAsusMouseDriver.hpp;AsusMouseDriver.hpp;Devices.hpp;ReadCallback.hpp")

if (UNIX)
	target_include_directories(${PROJECT_NAME} PUBLIC "/usr/include/hidapi")

	include(GNUInstallDirs)

	install(TARGETS openasusmousedriver
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/openasusmousedriver)
endif (UNIX)

if (WIN32)
	add_subdirectory(dependencies/hidapi ${CMAKE_BINARY_DIR}/dependencies/hidapi)
	target_link_libraries(${PROJECT_NAME} hidapi::hidapi)

	install(FILES ${CMAKE_BINARY_DIR}/dependencies/hidapi/src/windows/Release/hidapi.dll DESTINATION bin)
	install(FILES ${CMAKE_BINARY_DIR}/dependencies/hidapi/src/windows/Release/hidapi.lib DESTINATION lib)
	install(FILES ${CMAKE_SOURCE_DIR}/dependencies/hidapi/hidapi/hidapi.h DESTINATION include)

	install(TARGETS openasusmousedriver
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif (WIN32)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
