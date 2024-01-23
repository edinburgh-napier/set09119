project(set09119)
cmake_minimum_required(VERSION 3.17)

#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# Set the directory where the executables will be stored.
set(EXECUTABLE_OUTPUT_PATH
    ${PROJECT_BINARY_DIR}/bin
    CACHE PATH
    "Directory where executables will be stored"
)

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_OUTPUT_PATH} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_OUTPUT_PATH} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_OUTPUT_PATH} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/contrib/glm
	${CMAKE_SOURCE_DIR}/contrib/tinyobjloader
	${CMAKE_SOURCE_DIR}/contrib/glfw-3.3.2/include
	${CMAKE_SOURCE_DIR}/contrib/glew-2.1.0/include
)

set(LIBRARIES
	OpenGL32.lib
	${CMAKE_SOURCE_DIR}/contrib/glfw-3.3.2/lib-vc2019/glfw3dll.lib
	${CMAKE_SOURCE_DIR}/contrib/glew-2.1.0/lib/Release/x64/glew32.lib
)

set(glew_dll ${CMAKE_SOURCE_DIR}/contrib/glew-2.1.0/bin/Release/x64/glew32.dll)
set(glfw_dll ${CMAKE_SOURCE_DIR}/contrib/glfw-3.3.2/lib-vc2019/glfw3.dll)

# Function to copy DLLs to the output folder
function(copy_dlls tgt)
    add_custom_command(TARGET ${tgt} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${glew_dll}
        "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${glfw_dll}
        "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Copying GLEW and GLFW DLLs to the output folder"
    )
endfunction()


add_subdirectory(01_particle_animation)
add_subdirectory(02_particles_framework)
add_subdirectory(03_constraints_framework)
add_subdirectory(04_rigid_body_framework)