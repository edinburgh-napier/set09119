#include "Path.h"

#include <Windows.h>
#include <filesystem>

// this will point to a path relative to this file's path (Path.h/../resources), even when you build and move the executable
std::string ResourcesPath() 
{ 
	// Try first to get the folder in the source directory
	// If it does not exist, get the folder from the binary directory
	auto resources_folder = std::filesystem::path(__FILE__).parent_path().string() + "/resources";
	if (!std::filesystem::exists(resources_folder))
	{
		char fname[MAX_PATH];
		auto nSize = GetModuleFileNameA(NULL, fname, MAX_PATH);
		if (nSize == 0)
			return ""; // error! return empty string
		resources_folder = std::filesystem::path(fname).parent_path().string() + "/resources";
		if (!std::filesystem::exists(resources_folder))
			return "";
	}
	resources_folder.push_back('/');
	return resources_folder;
}