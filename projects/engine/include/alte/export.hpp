#pragma once

// Macro to export/import symbols
#define ALTE_EXPORT __declspec(dllexport)
#define ALTE_IMPORT __declspec(dllimport)

// If the system is running on mac or linux
#if defined(__APPLE__) || defined(__LINUX__)

// Linux and Mac
#define ALTE_EXPORT [[gnu::visibility("default")]]
#define ALTE_IMPORT [[gnu::visibility("default")]]

// If the system is running on windows
#elif defined(_WIN32)

// Windows
#define ALTE_EXPORT __declspec(dllexport)
#define ALTE_IMPORT __declspec(dllimport)

#endif

// If the lib is built as a static library
#ifndef ALTE_STATIC

// If ALTE_BUILD is defined, then use the ALTE_EXPORT macro to export
// symbols
#ifdef ALTE_BUILD
#define ALTE_API ALTE_EXPORT

// otherwise, use the ALTE_IMPORT macro to import symbols
#else
#define ALTE_API ALTE_IMPORT
#endif

#endif