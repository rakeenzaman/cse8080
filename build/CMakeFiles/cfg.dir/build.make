# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/ubuntu/Desktop/project test"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/ubuntu/Desktop/project test/build"

# Include any dependencies generated for this target.
include CMakeFiles/cfg.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/cfg.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/cfg.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cfg.dir/flags.make

CMakeFiles/cfg.dir/main.cpp.o: CMakeFiles/cfg.dir/flags.make
CMakeFiles/cfg.dir/main.cpp.o: ../main.cpp
CMakeFiles/cfg.dir/main.cpp.o: CMakeFiles/cfg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/ubuntu/Desktop/project test/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cfg.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/cfg.dir/main.cpp.o -MF CMakeFiles/cfg.dir/main.cpp.o.d -o CMakeFiles/cfg.dir/main.cpp.o -c "/home/ubuntu/Desktop/project test/main.cpp"

CMakeFiles/cfg.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cfg.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/ubuntu/Desktop/project test/main.cpp" > CMakeFiles/cfg.dir/main.cpp.i

CMakeFiles/cfg.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cfg.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/ubuntu/Desktop/project test/main.cpp" -o CMakeFiles/cfg.dir/main.cpp.s

# Object files for target cfg
cfg_OBJECTS = \
"CMakeFiles/cfg.dir/main.cpp.o"

# External object files for target cfg
cfg_EXTERNAL_OBJECTS =

cfg: CMakeFiles/cfg.dir/main.cpp.o
cfg: CMakeFiles/cfg.dir/build.make
cfg: /usr/lib/llvm-17/lib/libclangFrontend.a
cfg: /usr/lib/llvm-17/lib/libclangSerialization.a
cfg: /usr/lib/llvm-17/lib/libclangDriver.a
cfg: /usr/lib/llvm-17/lib/libclangParse.a
cfg: /usr/lib/llvm-17/lib/libclangSema.a
cfg: /usr/lib/llvm-17/lib/libclangAnalysis.a
cfg: /usr/lib/llvm-17/lib/libclangAST.a
cfg: /usr/lib/llvm-17/lib/libclangBasic.a
cfg: /usr/lib/llvm-17/lib/libclangEdit.a
cfg: /usr/lib/llvm-17/lib/libclangLex.a
cfg: /usr/lib/llvm-17/lib/libclangTooling.a
cfg: /usr/lib/llvm-17/lib/libclang-17.so.17
cfg: /usr/lib/llvm-17/lib/libclangFrontend.a
cfg: /usr/lib/llvm-17/lib/libclangParse.a
cfg: /usr/lib/llvm-17/lib/libclangSerialization.a
cfg: /usr/lib/llvm-17/lib/libclangSema.a
cfg: /usr/lib/llvm-17/lib/libclangAnalysis.a
cfg: /usr/lib/llvm-17/lib/libclangEdit.a
cfg: /usr/lib/llvm-17/lib/libclangSupport.a
cfg: /usr/lib/llvm-17/lib/libclangDriver.a
cfg: /usr/lib/llvm-17/lib/libclangASTMatchers.a
cfg: /usr/lib/llvm-17/lib/libclangAST.a
cfg: /usr/lib/llvm-17/lib/libclangFormat.a
cfg: /usr/lib/llvm-17/lib/libclangToolingInclusions.a
cfg: /usr/lib/llvm-17/lib/libclangToolingCore.a
cfg: /usr/lib/llvm-17/lib/libclangRewrite.a
cfg: /usr/lib/llvm-17/lib/libclangLex.a
cfg: /usr/lib/llvm-17/lib/libclangBasic.a
cfg: /usr/lib/llvm-17/lib/libLLVM-17.so.1
cfg: CMakeFiles/cfg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/ubuntu/Desktop/project test/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cfg"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cfg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cfg.dir/build: cfg
.PHONY : CMakeFiles/cfg.dir/build

CMakeFiles/cfg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cfg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cfg.dir/clean

CMakeFiles/cfg.dir/depend:
	cd "/home/ubuntu/Desktop/project test/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/ubuntu/Desktop/project test" "/home/ubuntu/Desktop/project test" "/home/ubuntu/Desktop/project test/build" "/home/ubuntu/Desktop/project test/build" "/home/ubuntu/Desktop/project test/build/CMakeFiles/cfg.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/cfg.dir/depend

