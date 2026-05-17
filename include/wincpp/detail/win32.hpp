#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// clang-format off
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
// clang-format on

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
