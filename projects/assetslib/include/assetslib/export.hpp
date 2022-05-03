#pragma once

#define ASSETSLIB_EXPORT __declspec(dllexport)
#define ASSETSLIB_IMPORT __declspec(dllimport)

#if defined(__APPLE__) || defined(__LINUX__)

// Linux and Mac
#define ASSETSLIB_EXPORT [[gnu::visibility("default")]]
#define ASSETSLIB_IMPORT [[gnu::visibility("default")]]

#elif defined(_WIN32)

// Windows
#define ASSETSLIB_EXPORT __declspec(dllexport)
#define ASSETSLIB_IMPORT __declspec(dllimport)

#endif

#ifndef ASSETSLIB_STATIC

#ifdef ASSETSLIB_BUILD
#define ASSETSLIB_API ASSETSLIB_EXPORT
#else
#define ASSETSLIB_API ASSETSLIB_IMPORT
#endif

#endif