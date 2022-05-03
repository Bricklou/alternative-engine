#pragma once

// Macro to export/import symbols
#define ASSETSLIB_EXPORT __declspec(dllexport)
#define ASSETSLIB_IMPORT __declspec(dllimport)

// If the system is running on mac or linux
#if defined(__APPLE__) || defined(__LINUX__)

// Linux and Mac
#define ASSETSLIB_EXPORT [[gnu::visibility("default")]]
#define ASSETSLIB_IMPORT [[gnu::visibility("default")]]

// If the system is running on windows
#elif defined(_WIN32)

// Windows
#define ASSETSLIB_EXPORT __declspec(dllexport)
#define ASSETSLIB_IMPORT __declspec(dllimport)

#endif

// If the lib is built as a static library
#ifndef ASSETSLIB_STATIC

// If ASSETSLIB_BUILD is defined, then use the ASSETSLIB_EXPORT macro to export
// symbols
#ifdef ASSETSLIB_BUILD
#define ASSETSLIB_API ASSETSLIB_EXPORT

// otherwise, use the ASSETSLIB_IMPORT macro to import symbols
#else
#define ASSETSLIB_API ASSETSLIB_IMPORT
#endif

#endif