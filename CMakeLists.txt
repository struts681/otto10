cmake_minimum_required(VERSION 3.29)
project(wvtc_automation C)

set(CMAKE_CXX_STANDARD 17)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/include
	)

file(GLOB all_SRCS
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	)


add_executable(wvtc_automation ${all_SRCS})

