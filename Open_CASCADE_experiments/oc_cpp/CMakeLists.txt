cmake_minimum_required(VERSION 3.10)
project(MyOCCTProject)

# Ensure we are using at least C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCASCADE
find_package(OpenCASCADE REQUIRED)

# Add executables
add_executable(my_occt_app minimal.cpp)
add_executable(torc_app torc.cpp)

# Link the OpenCASCADE libraries to both executables
target_link_libraries(my_occt_app PRIVATE ${OpenCASCADE_LIBRARIES})
target_link_libraries(torc_app PRIVATE ${OpenCASCADE_LIBRARIES})

# Include OpenCASCADE headers for both executables
target_include_directories(my_occt_app PRIVATE ${OpenCASCADE_INCLUDE_DIR})
target_include_directories(torc_app PRIVATE ${OpenCASCADE_INCLUDE_DIR})

