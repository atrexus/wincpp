#pragma once

#define WINCPP_SUPPRESS_AUTO_INL

#include "wincpp/core/error.hpp"
#include "wincpp/core/errors/user.hpp"
#include "wincpp/core/errors/win32.hpp"
#include "wincpp/core/snapshot.hpp"
#include "wincpp/core/win.hpp"
#include "wincpp/memory/allocation.hpp"
#include "wincpp/memory/memory.hpp"
#include "wincpp/memory/pointer.hpp"
#include "wincpp/memory/protection.hpp"
#include "wincpp/memory/protection_operation.hpp"
#include "wincpp/memory/region.hpp"
#include "wincpp/memory_factory.hpp"
#include "wincpp/module_factory.hpp"
#include "wincpp/modules/export.hpp"
#include "wincpp/modules/module.hpp"
#include "wincpp/modules/object.hpp"
#include "wincpp/modules/section.hpp"
#include "wincpp/patterns/pattern.hpp"
#include "wincpp/patterns/scanner.hpp"
#include "wincpp/process.hpp"
#include "wincpp/thread_factory.hpp"
#include "wincpp/threads/thread.hpp"
#include "wincpp/window_factory.hpp"
#include "wincpp/windows/window.hpp"

#undef WINCPP_SUPPRESS_AUTO_INL

#include "wincpp/wincpp.inl"
