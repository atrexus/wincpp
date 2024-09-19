# CMakeList.txt : CMake project for wincpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Set our C++ standard
set (CMAKE_CXX_STANDARD 23)

# Define our project
project (wincpp LANGUAGES CXX)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Define the library targets
add_subdirectory (src)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We're in the root, define additional targets for developers.
    option(BUILD_EXAMPLES   "whether or not examples should be built" ON)
    option(BUILD_PACKAGE    "whether or not to build a package" ON)

    if(BUILD_PACKAGE)
        set(package_files include/ src/ CMakeLists.txt LICENSE)
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip
            COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip --format=zip -- ${package_files}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${package_files})
        add_custom_target(${PROJECT_NAME}_package DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip)
    endif()

    if (BUILD_EXAMPLES)
		add_subdirectory(examples)
    endif()
endif()