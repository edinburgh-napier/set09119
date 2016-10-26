cmake_minimum_required(VERSION 3.3)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(PhysicsProject)
SET(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})
foreach (config ${CMAKE_CONFIGURATION_TYPES})
  string (TOUPPER ${config} config)
  set_target_properties(${_project} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}"
    ARCHIVE_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}"
    LIBRARY_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}"
    #PDB_OUTPUT_DIRECTORY_${config} ${sym_path}
    VS_INTERMEDIATE_DIRECTORY_${config} "temp"
  )
  SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}")
  SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}")
  SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}")
  SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}")
  SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}")
  SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${config} "${OUTPUT_DIRECTORY}/${config}")
  SET( VS_INTERMEDIATE_DIRECTORY_${config} "temp/${config}")
  link_directories("${OUTPUT_DIRECTORY}/${config}")
endforeach ()

include(DownloadProject.cmake)
download_project(PROJ enu_gfx
  GIT_REPOSITORY      https://github.com/edinburgh-napier/set08116_framework
  GIT_TAG             master
  ${UPDATE_DISCONNECTED_IF_AVAILABLE}
)
add_subdirectory(${enu_gfx_SOURCE_DIR} ${enu_gfx_BINARY_DIR})
include_directories(${enu_gfx_SOURCE_DIR}/src ${enu_graphics_framework_incs}) 

#Grab physics framework
file(GLOB_RECURSE LIB_SOURCE_FILES ../lib_phys_utils/*.cpp ../lib_phys_utils/*.h)
add_library(lib_phys_utils STATIC ${LIB_SOURCE_FILES})

#Grab our actual files
file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)
add_executable(PhysicsCoursework ${SOURCE_FILES})
#dependencies
target_link_libraries(PhysicsCoursework enu_graphics_framework lib_phys_utils)
target_include_directories(PhysicsCoursework PUBLIC ../lib_phys_utils) 
add_dependencies(PhysicsCoursework enu_graphics_framework lib_phys_utils)
	
#copy resources to build post build script
add_custom_command(TARGET PhysicsCoursework POST_BUILD  
COMMAND ${CMAKE_COMMAND} -E copy_directory  "${CMAKE_SOURCE_DIR}/../res" $<TARGET_FILE_DIR:PhysicsCoursework>)

#set outDir as debugging directory for our project
if(${MSVC})
file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/PhysicsCoursework.vcxproj.user" 
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>
	<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">
	<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">
		<LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	</PropertyGroup>
	<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">
		<LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>
		<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
	</PropertyGroup>
	</Project>")
endif()
	
#Enable Faster Builds	
if(${MSVC})
      target_compile_options(enu_graphics_framework PUBLIC /MP)
ENDIF()

set_target_properties(enu_graphics_framework PROPERTIES FOLDER "DEPS")
#set_target_properties(zlibstatic PROPERTIES FOLDER "DEPS")
#set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "DEPS")
set_target_properties(glm_dummy PROPERTIES FOLDER "DEPS")
#set_target_properties(uninstall PROPERTIES FOLDER "DEPS")
#set_target_properties(assimp PROPERTIES FOLDER "DEPS")
