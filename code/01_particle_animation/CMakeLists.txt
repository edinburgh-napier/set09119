project(01_particle_animation)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    01_particle_animation.cpp
)

set(HEADER_FILES
	Shader.h 
	Camera.h 
	Mesh.h
)

set(executable_name ${PROJECT_NAME})

set( ALL_SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} )
include_directories( ${INCLUDE_DIRS} )
add_executable(${executable_name} ${ALL_SOURCE_FILES} )
set_target_properties(${executable_name} PROPERTIES OUTPUT_NAME ${executable_name} CLEAN_DIRECT_OUTPUT 1 DEBUG_POSTFIX "d")
target_link_libraries(${executable_name} ${LIBRARIES} )

