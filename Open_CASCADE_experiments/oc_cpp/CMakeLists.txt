cmake_minimum_required(VERSION 3.10)
project(MyOCCTProject)

# Ensure we are using at least C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCASCADE
find_package(OpenCASCADE REQUIRED)

# Add executable
add_executable(my_occt_app main.cpp)
add_executable(oc_test_app oc_test.cpp)

# Link the OpenCASCADE libraries
target_link_libraries(my_occt_app PRIVATE ${OpenCASCADE_LIBRARIES})

# Include OpenCASCADE headers
target_include_directories(my_occt_app PRIVATE ${OpenCASCADE_INCLUDE_DIR})
