cmake_path(SET include_dir "${CMAKE_CURRENT_SOURCE_DIR}/../include")

# Add the header files to the project
set(header_files 
	"${include_dir}/process.hpp"
	"${include_dir}/module_factory.hpp"
	"${include_dir}/memory_factory.hpp"

	"${include_dir}/memory/memory.hpp"
	"${include_dir}/memory/pointer.hpp"
	"${include_dir}/memory/region.hpp"
	"${include_dir}/memory/protection.hpp"
	"${include_dir}/memory/protection_operation.hpp"

	"${include_dir}/modules/module.hpp"
	"${include_dir}/modules/export.hpp"
	"${include_dir}/modules/section.hpp"

	"${include_dir}/patterns/scanner.hpp"
	"${include_dir}/patterns/pattern.hpp"

	"${include_dir}/core/win.hpp"
	"${include_dir}/core/error.hpp"
	"${include_dir}/core/snapshot.hpp"
	"${include_dir}/core/errors/win32.hpp"
)

# Add the library to the project
add_library(_wincpp_core INTERFACE)

# Add the include directory to the project
target_sources(_wincpp_core INTERFACE ${header_files})

# Add the include directories
target_include_directories(_wincpp_core SYSTEM INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Add the library to the project
add_library(wincpp STATIC)

# Link the library to the core
target_link_libraries(wincpp INTERFACE _wincpp_core)

# Add the source files to the project
target_sources(wincpp PRIVATE 	
	"process.cpp"

	"module_factory.cpp"

	"core/win.cpp"
	"core/error.cpp"
	"core/snapshot.cpp"
	
	"core/errors/win32.cpp"
 "modules/module.cpp" "memory_factory.cpp" "memory/pointer.cpp" "memory/region.cpp" "memory/protection.cpp" "modules/export.cpp" "memory/protection_operation.cpp" "modules/section.cpp"  "patterns/scanner.cpp" "patterns/pattern.cpp")

# Add the include directory to the project
target_include_directories(wincpp PRIVATE ${include_dir})