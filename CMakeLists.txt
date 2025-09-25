cmake_minimum_required(VERSION 3.10)

project(SuadoMathParser VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_SHARED "Whether make it shared or static" ON)

if (NOT MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

	option(BUILD_64BITS "x64 or x86" ON)
	if(BUILD_64BITS)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
	endif()
endif()

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

if(BUILD_SHARED)
	add_library(SuadoMathParser SHARED ${SOURCES})
	set_target_properties(SuadoMathParser PROPERTIES POSITION_INDEPENDENT_CODE ON)
	target_compile_definitions(SuadoMathParser PRIVATE SMP_EXPORT=1)
else()
    add_library(SuadoMathParser STATIC ${SOURCES})
endif()

set_target_properties(SuadoMathParser PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(SuadoMathParser PUBLIC ${PROJECT_SOURCE_DIR}/include/SuadoMathParser)

add_subdirectory(${PROJECT_SOURCE_DIR}/example)