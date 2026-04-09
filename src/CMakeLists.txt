cmake_path(SET include_dir "${CMAKE_CURRENT_SOURCE_DIR}/../include")

# Add the header files to the project
set(header_files 
	"${include_dir}/wincpp/process.hpp"
	"${include_dir}/wincpp/module_factory.hpp"
	"${include_dir}/wincpp/memory_factory.hpp"
	"${include_dir}/wincpp/window_factory.hpp"
	"${include_dir}/wincpp/thread_factory.hpp"

	"${include_dir}/wincpp/memory/memory.hpp"
	"${include_dir}/wincpp/memory/allocation.hpp"
	"${include_dir}/wincpp/memory/pointer.hpp"
	"${include_dir}/wincpp/memory/region.hpp"
	"${include_dir}/wincpp/memory/protection.hpp"
	"${include_dir}/wincpp/memory/protection_operation.hpp"

	"${include_dir}/wincpp/modules/module.hpp"
	"${include_dir}/wincpp/modules/export.hpp"
	"${include_dir}/wincpp/modules/section.hpp"
	"${include_dir}/wincpp/modules/object.hpp"

	"${include_dir}/wincpp/patterns/scanner.hpp"
	"${include_dir}/wincpp/patterns/pattern.hpp"

	"${include_dir}/wincpp/windows/window.hpp"

	"${include_dir}/wincpp/threads/thread.hpp"

	"${include_dir}/wincpp/core/win.hpp"
	"${include_dir}/wincpp/core/error.hpp"
	"${include_dir}/wincpp/core/snapshot.hpp"
	"${include_dir}/wincpp/core/errors/win32.hpp"
	"${include_dir}/wincpp/core/errors/user.hpp"
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
	"memory_factory.cpp"
	"window_factory.cpp"
	"thread_factory.cpp"

	"memory/allocation.cpp"
	"memory/pointer.cpp"
	"memory/region.cpp"
	"memory/protection.cpp"
	"memory/protection_operation.cpp"
	"memory/memory.cpp"

	"modules/module.cpp"
	"modules/export.cpp"
	"modules/section.cpp"
	"modules/object.cpp"

	"patterns/scanner.cpp"
	"patterns/pattern.cpp"

	"windows/window.cpp"

	"threads/thread.cpp"

	"core/win.cpp"
	"core/error.cpp"
	"core/snapshot.cpp"
	
	"core/errors/win32.cpp"
	"core/errors/user.cpp"
 "memory/allocation.cpp")

# Add the include directory to the project
target_include_directories(wincpp PRIVATE ${include_dir})

install(TARGETS wincpp EXPORT wincpp
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
