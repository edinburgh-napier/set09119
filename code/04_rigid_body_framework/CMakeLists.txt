project(04_rigid_body_framework)
cmake_minimum_required(VERSION 3.17)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    PhysicsEngine.cpp
	PhysicsObject.cpp
	Application.cpp
	Force.cpp
	Mesh.cpp
)

set(HEADER_FILES
	Shader.h 
	Camera.h 
	Mesh.h
	Application.h
	PhysicsEngine.h
	PhysicsObject.h
	Force.h
)

set(executable_name ${PROJECT_NAME})

set( ALL_SOURCE_FILES ${SOURCE_FILES} ${HEADER_FILES} )
include_directories( ${INCLUDE_DIRS} )
add_executable(${executable_name} ${ALL_SOURCE_FILES} )
set_target_properties(${executable_name} PROPERTIES OUTPUT_NAME ${executable_name} CLEAN_DIRECT_OUTPUT 1 DEBUG_POSTFIX "d")
target_link_libraries(${executable_name} ${LIBRARIES} )

file(COPY resources DESTINATION ../bin) # Copy the resources so that when you package the executable, you can package the resources subfolder too
file(COPY resources DESTINATION .)		# Copy the resources so that when debugging, you can see the resources, as the working directory is not at the executable location
