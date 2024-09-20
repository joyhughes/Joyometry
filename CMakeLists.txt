# Created by the script cgal_create_cmake_script_with_options
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.23)
project(Joyometry)

# CGAL and its components
find_package(CGAL REQUIRED)

create_single_source_cgal_program("joyom.cpp")
