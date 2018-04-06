// Copyright (C) 2018 Lyu Luan
// This file is part of the "Kong Engine".

#ifndef _KONG_COMPILE_CONFIG_H_
#define _KONG_CONPILE_CONFIG_H_

#define KONG_SDK_VERSION "1.0.0"

//! WIN32 for Windows32
//! WIN64 for Windows64
// The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define _KONG_WINDOWS_
#define _KONG_WINDOWS_API_
#define _KONG_COMPILE_WITH_WINDOWS_DEVICE_
#endif

#ifdef _KONG_WINDOWS_API_

// To build KongEngine as a static library, you must define _KONG_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifndef _KONG_STATIC_LIB_
#ifdef KONG_EXPORTS
#define KONG_API __declspec(dllexport)
#else
#define KONG_API __declspec(dllimport)
#endif // KONG_EXPORT
#else
#define KONG_API
#endif // _KONG_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define KONGCALLCONV __stdcall
#else
#define KONGCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // _KONG_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_KONG_STATIC_LIB_) && defined(KONG_EXPORTS)
#define KONG_API __attribute__ ((visibility("default")))
#else
#define KONG_API
#endif

#define KONGCALLCONV

#endif // _KONG_WINDOWS_API_


//! Define _IRR_COMPILE_WITH_OPENGL_ to compile the Irrlicht engine with OpenGL.
/** If you do not wish the engine to be compiled with OpenGL, comment this
define out. */
#define _KONG_COMPILE_WITH_OPENGL_
#ifdef NO_KONG_COMPILE_WITH_OPENGL_
#undef _KONG_COMPILE_WITH_OPENGL_
#endif

#endif
