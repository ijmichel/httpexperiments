# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ian/Documents/CS498/httpexperiments

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ian/Documents/CS498/httpexperiments/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/listener.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/listener.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/listener.dir/flags.make

CMakeFiles/listener.dir/listener.c.o: CMakeFiles/listener.dir/flags.make
CMakeFiles/listener.dir/listener.c.o: ../listener.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ian/Documents/CS498/httpexperiments/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/listener.dir/listener.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/listener.dir/listener.c.o   -c /home/ian/Documents/CS498/httpexperiments/listener.c

CMakeFiles/listener.dir/listener.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/listener.dir/listener.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ian/Documents/CS498/httpexperiments/listener.c > CMakeFiles/listener.dir/listener.c.i

CMakeFiles/listener.dir/listener.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/listener.dir/listener.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ian/Documents/CS498/httpexperiments/listener.c -o CMakeFiles/listener.dir/listener.c.s

CMakeFiles/listener.dir/listener.c.o.requires:

.PHONY : CMakeFiles/listener.dir/listener.c.o.requires

CMakeFiles/listener.dir/listener.c.o.provides: CMakeFiles/listener.dir/listener.c.o.requires
	$(MAKE) -f CMakeFiles/listener.dir/build.make CMakeFiles/listener.dir/listener.c.o.provides.build
.PHONY : CMakeFiles/listener.dir/listener.c.o.provides

CMakeFiles/listener.dir/listener.c.o.provides.build: CMakeFiles/listener.dir/listener.c.o


# Object files for target listener
listener_OBJECTS = \
"CMakeFiles/listener.dir/listener.c.o"

# External object files for target listener
listener_EXTERNAL_OBJECTS =

listener: CMakeFiles/listener.dir/listener.c.o
listener: CMakeFiles/listener.dir/build.make
listener: CMakeFiles/listener.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ian/Documents/CS498/httpexperiments/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable listener"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/listener.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/listener.dir/build: listener

.PHONY : CMakeFiles/listener.dir/build

CMakeFiles/listener.dir/requires: CMakeFiles/listener.dir/listener.c.o.requires

.PHONY : CMakeFiles/listener.dir/requires

CMakeFiles/listener.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/listener.dir/cmake_clean.cmake
.PHONY : CMakeFiles/listener.dir/clean

CMakeFiles/listener.dir/depend:
	cd /home/ian/Documents/CS498/httpexperiments/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ian/Documents/CS498/httpexperiments /home/ian/Documents/CS498/httpexperiments /home/ian/Documents/CS498/httpexperiments/cmake-build-debug /home/ian/Documents/CS498/httpexperiments/cmake-build-debug /home/ian/Documents/CS498/httpexperiments/cmake-build-debug/CMakeFiles/listener.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/listener.dir/depend

