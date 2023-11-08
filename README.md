# set09119

**Software Requirements:**

* Latest stable CMake (GUI version helps)
* Visual Studio Community

**Project Build Instructions:**

* Run CMake
* Set Source Folder to the "code" directory
* Set Build Folder to a directory called "build", under "code" (e.g. /your/path/code/build)
* Run Configure, and just select default compilers, Visual Studio x64 should be the one that appears
* Run Generate
* Now, in the Build folder, you should see the solution file set09119.sln

**How to add your own project into the solution, the CMake way**

* Copy paste the project you want to work from, into a folder along side the original folder, e.g. /code/02_simulation to /code/02_simulation_v2
* Edit the file /code/CMakeLists.txt and add a "add_subdirectory" line to match the others
* Edit the CMakeLists file in the new folder, e.g. /code/02_simulation_v2/CMakeLists.txt, and in the first line, replace the old project name with the new (e.g. 02_simulation to 02_simulation_v2)
* Re-run configure and generate in CMake

**How to add a new source file into your project, the CMake way**

* Go to the project's folder
* Create the file
* Edit CMakeLists.txt file in that project folder and add your file in the appropriate list, if it's .h or .cpp
* In CMake, do Configure and Generate

**How to submit one or more projects for assessment**

* Go to the folder /your-repository-path/code (**not** build)
* Please only use numbers, letters and underscores for source folders
* Select all project folders, and add them to an archive file, e.g. the folders /code/02_simulation_mytest and /code/copy_of_02_for_assessment_3
* Name the archive like "\<matric_number\>_\<some identifier of your choice\>.zip", e.g. "40414148_my_cw2.zip" or "40414487_coursework.rar"

**C++ essentials that you will encounter while writing your coursework**

In the practicals and coursework we're using classes, member functions, inheritance, references and pass-by-reference. Pointers are used in the provided code, but you will not have to do much pointer-related work, or dynamic memory allocation. We are using inheritance to extend object functionality (a rigid body inherits all the properties of a particle that inherits all the properties of a generic static physics object). Get familiarised with std::vector (C++ resizable array), and look up its functions in www.cppreference.com. Also use www.learncpp.com to refresh or read more on any particular topic.
