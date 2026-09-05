include(FetchContent)

set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
set(CATCH_INSTALL_EXTRAS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.15.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Catch2)

add_library(wincpp_export_target SHARED
    export_fixture.cpp
    export_target.def
)

add_library(wincpp_export_forwarders SHARED
    export_forwarders.cpp
)

add_executable(wincpp_tests
    core_tests.cpp
    macro_hygiene_tests.cpp
    memory_tests.cpp
    module_tests.cpp
    pattern_tests.cpp
    process_tests.cpp
    thread_tests.cpp
    window_tests.cpp
)

target_link_libraries(wincpp_tests PRIVATE
    wincpp::wincpp
    Catch2::Catch2WithMain
)

add_dependencies(wincpp_tests
    wincpp_export_target
    wincpp_export_forwarders
)

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(wincpp_tests)
