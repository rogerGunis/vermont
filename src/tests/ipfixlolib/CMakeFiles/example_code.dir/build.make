# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/simon/da/lockfree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/simon/da/lockfree

# Include any dependencies generated for this target.
include src/tests/ipfixlolib/CMakeFiles/example_code.dir/depend.make

# Include the progress variables for this target.
include src/tests/ipfixlolib/CMakeFiles/example_code.dir/progress.make

# Include the compile flags for this target's objects.
include src/tests/ipfixlolib/CMakeFiles/example_code.dir/flags.make

src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o: src/tests/ipfixlolib/CMakeFiles/example_code.dir/flags.make
src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o: src/tests/ipfixlolib/example_code.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/simon/da/lockfree/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o"
	cd /home/simon/da/lockfree/src/tests/ipfixlolib && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/example_code.dir/example_code.o -c /home/simon/da/lockfree/src/tests/ipfixlolib/example_code.cc

src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example_code.dir/example_code.i"
	cd /home/simon/da/lockfree/src/tests/ipfixlolib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/simon/da/lockfree/src/tests/ipfixlolib/example_code.cc > CMakeFiles/example_code.dir/example_code.i

src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example_code.dir/example_code.s"
	cd /home/simon/da/lockfree/src/tests/ipfixlolib && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/simon/da/lockfree/src/tests/ipfixlolib/example_code.cc -o CMakeFiles/example_code.dir/example_code.s

src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.requires:
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.requires

src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.provides: src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.requires
	$(MAKE) -f src/tests/ipfixlolib/CMakeFiles/example_code.dir/build.make src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.provides.build
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.provides

src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.provides.build: src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.provides.build

# Object files for target example_code
example_code_OBJECTS = \
"CMakeFiles/example_code.dir/example_code.o"

# External object files for target example_code
example_code_EXTERNAL_OBJECTS =

src/tests/ipfixlolib/example_code: src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o
src/tests/ipfixlolib/example_code: src/common/ipfixlolib/libipfixlolib.a
src/tests/ipfixlolib/example_code: src/common/libcommon.a
src/tests/ipfixlolib/example_code: src/tests/ipfixlolib/CMakeFiles/example_code.dir/build.make
src/tests/ipfixlolib/example_code: src/tests/ipfixlolib/CMakeFiles/example_code.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable example_code"
	cd /home/simon/da/lockfree/src/tests/ipfixlolib && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example_code.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/tests/ipfixlolib/CMakeFiles/example_code.dir/build: src/tests/ipfixlolib/example_code
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/build

src/tests/ipfixlolib/CMakeFiles/example_code.dir/requires: src/tests/ipfixlolib/CMakeFiles/example_code.dir/example_code.o.requires
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/requires

src/tests/ipfixlolib/CMakeFiles/example_code.dir/clean:
	cd /home/simon/da/lockfree/src/tests/ipfixlolib && $(CMAKE_COMMAND) -P CMakeFiles/example_code.dir/cmake_clean.cmake
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/clean

src/tests/ipfixlolib/CMakeFiles/example_code.dir/depend:
	cd /home/simon/da/lockfree && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/simon/da/lockfree /home/simon/da/lockfree/src/tests/ipfixlolib /home/simon/da/lockfree /home/simon/da/lockfree/src/tests/ipfixlolib /home/simon/da/lockfree/src/tests/ipfixlolib/CMakeFiles/example_code.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/tests/ipfixlolib/CMakeFiles/example_code.dir/depend
