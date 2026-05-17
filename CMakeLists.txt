cmake_minimum_required(VERSION 3.23)

project(wincpp VERSION 2.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(wincpp INTERFACE)
add_library(wincpp::wincpp ALIAS wincpp)

target_compile_features(wincpp INTERFACE cxx_std_23)

target_include_directories(wincpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS wincpp EXPORT wincppTargets)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/wincpp" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/wincppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/wincppConfig.cmake"
    "include(\${CMAKE_CURRENT_LIST_DIR}/wincppTargets.cmake)\n"
)

set(wincpp_CONFFILE_DEST "${CMAKE_INSTALL_DATAROOTDIR}/wincpp")

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/wincppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/wincppConfigVersion.cmake"
    DESTINATION "${wincpp_CONFFILE_DEST}"
)

install(EXPORT wincppTargets
    FILE wincppTargets.cmake
    NAMESPACE wincpp::
    DESTINATION "${wincpp_CONFFILE_DEST}"
)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(BUILD_EXAMPLES "whether or not examples should be built" ON)
    option(BUILD_PACKAGE "whether or not to build a package" ON)
    option(BUILD_TESTS "whether or not tests should be built" ON)

    if(BUILD_PACKAGE)
        set(package_files include/ examples/ tests/ CMakeLists.txt LICENSE README.md)
        file(GLOB_RECURSE package_dependencies CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*"
            "${CMAKE_CURRENT_SOURCE_DIR}/examples/*"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*"
        )
        list(APPEND package_dependencies
            "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
            "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        )
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip
            COMMAND ${CMAKE_COMMAND} -E tar c ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip --format=zip -- ${package_files}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${package_dependencies})
        add_custom_target(${PROJECT_NAME}_package DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-src.zip)
    endif()

    if(BUILD_EXAMPLES AND WIN32)
        add_subdirectory(examples)
    endif()

    if(BUILD_TESTS AND WIN32)
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()
