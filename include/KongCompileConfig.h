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

//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _KONG_MATERIAL_MAX_TEXTURES_ 4

//! Define _KONG_COMPILE_WITH_OPENGL_ to compile the Irrlicht engine with OpenGL.
/** If you do not wish the engine to be compiled with OpenGL, comment this
define out. */
#define _KONG_COMPILE_WITH_OPENGL_
#ifdef NO_KONG_COMPILE_WITH_OPENGL_
#undef _KONG_COMPILE_WITH_OPENGL_
#endif

//! Define _KONG_COMPILE_WITH_KONG_SCENE_LOADER_ if you want to be able to load
/** .irr scenes using ISceneManager::loadScene */
#define _KONG_COMPILE_WITH_KONG_SCENE_LOADER_
#ifdef NO_KONG_COMPILE_WITH_KONG_SCENE_LOADER_
#undef _KONG_COMPILE_WITH_KONG_SCENE_LOADER_
#endif

//! Define _KONG_COMPILE_WITH_JPEGLIB_ to enable compiling the engine using libjpeg.
/** This enables the engine to read jpeg images. If you comment this out,
the engine will no longer read .jpeg images. */
#define _KONG_COMPILE_WITH_LIBJPEG_
#ifdef NO_KONG_COMPILE_WITH_LIBJPEG_
#undef _KONG_COMPILE_WITH_LIBJPEG_
#endif

//! Define _KONG_USE_NON_SYSTEM_JPEG_LIB_ to let irrlicht use the jpeglib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the jpeg lib installed in the system.
This is only used when _KONG_COMPILE_WITH_LIBJPEG_ is defined. */
#define _KONG_USE_NON_SYSTEM_JPEG_LIB_
#ifdef NO_KONG_USE_NON_SYSTEM_JPEG_LIB_
#undef _KONG_USE_NON_SYSTEM_JPEG_LIB_
#endif

//! Define _KONG_COMPILE_WITH_LIBPNG_ to enable compiling the engine using libpng.
/** This enables the engine to read png images. If you comment this out,
the engine will no longer read .png images. */
#define _KONG_COMPILE_WITH_LIBPNG_
#ifdef NO_KONG_COMPILE_WITH_LIBPNG_
#undef _KONG_COMPILE_WITH_LIBPNG_
#endif

//! Define _KONG_USE_NON_SYSTEM_LIBPNG_ to let irrlicht use the libpng which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the libpng installed in the system.
This is only used when _KONG_COMPILE_WITH_LIBPNG_ is defined. */
#define _KONG_USE_NON_SYSTEM_LIB_PNG_
#ifdef NO_KONG_USE_NON_SYSTEM_LIB_PNG_
#undef _KONG_USE_NON_SYSTEM_LIB_PNG_
#endif

//! Define _KONG_COMPILE_WITH_SKINNED_MESH_SUPPORT_ if you want to use bone based
/** animated meshes. If you compile without this, you will be unable to load
B3D, MS3D or X meshes */
#define _KONG_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#ifdef NO_KONG_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#undef _KONG_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#endif

#ifdef _KONG_COMPILE_WITH_SKINNED_MESH_SUPPORT_
//! Define _KONG_COMPILE_WITH_B3D_LOADER_ if you want to use Blitz3D files
#define _KONG_COMPILE_WITH_B3D_LOADER_
#ifdef NO_KONG_COMPILE_WITH_B3D_LOADER_
#undef _KONG_COMPILE_WITH_B3D_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_MS3D_LOADER_ if you want to Milkshape files
#define _KONG_COMPILE_WITH_MS3D_LOADER_
#ifdef NO_KONG_COMPILE_WITH_MS3D_LOADER_
#undef _KONG_COMPILE_WITH_MS3D_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_X_LOADER_ if you want to use Microsoft X files
#define _KONG_COMPILE_WITH_X_LOADER_
#ifdef NO_KONG_COMPILE_WITH_X_LOADER_
#undef _KONG_COMPILE_WITH_X_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_OGRE_LOADER_ if you want to load Ogre 3D files
#define _KONG_COMPILE_WITH_OGRE_LOADER_
#ifdef NO_KONG_COMPILE_WITH_OGRE_LOADER_
#undef _KONG_COMPILE_WITH_OGRE_LOADER_
#endif
#endif	// _KONG_COMPILE_WITH_SKINNED_MESH_SUPPORT_

//! Define _KONG_COMPILE_WITH_KONG_MESH_LOADER_ if you want to load Irrlicht Engine .irrmesh files
#define _KONG_COMPILE_WITH_KONG_MESH_LOADER_
#ifdef NO_KONG_COMPILE_WITH_KONG_MESH_LOADER_
#undef _KONG_COMPILE_WITH_KONG_MESH_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_HALFLIFE_LOADER_ if you want to load Halflife animated files
#define _KONG_COMPILE_WITH_HALFLIFE_LOADER_
#ifdef NO_KONG_COMPILE_WITH_HALFLIFE_LOADER_
#undef _KONG_COMPILE_WITH_HALFLIFE_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_MD2_LOADER_ if you want to load Quake 2 animated files
#define _KONG_COMPILE_WITH_MD2_LOADER_
#ifdef NO_KONG_COMPILE_WITH_MD2_LOADER_
#undef _KONG_COMPILE_WITH_MD2_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_MD3_LOADER_ if you want to load Quake 3 animated files
#define _KONG_COMPILE_WITH_MD3_LOADER_
#ifdef NO_KONG_COMPILE_WITH_MD3_LOADER_
#undef _KONG_COMPILE_WITH_MD3_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_3DS_LOADER_ if you want to load 3D Studio Max files
#define _KONG_COMPILE_WITH_3DS_LOADER_
#ifdef NO_KONG_COMPILE_WITH_3DS_LOADER_
#undef _KONG_COMPILE_WITH_3DS_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_COLLADA_LOADER_ if you want to load Collada files
#define _KONG_COMPILE_WITH_COLLADA_LOADER_
#ifdef NO_KONG_COMPILE_WITH_COLLADA_LOADER_
#undef _KONG_COMPILE_WITH_COLLADA_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_CSM_LOADER_ if you want to load Cartography Shop files
#define _KONG_COMPILE_WITH_CSM_LOADER_
#ifdef NO_KONG_COMPILE_WITH_CSM_LOADER_
#undef _KONG_COMPILE_WITH_CSM_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_BSP_LOADER_ if you want to load Quake 3 BSP files
#define _KONG_COMPILE_WITH_BSP_LOADER_
#ifdef NO_KONG_COMPILE_WITH_BSP_LOADER_
#undef _KONG_COMPILE_WITH_BSP_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_DMF_LOADER_ if you want to load DeleD files
#define _KONG_COMPILE_WITH_DMF_LOADER_
#ifdef NO_KONG_COMPILE_WITH_DMF_LOADER_
#undef _KONG_COMPILE_WITH_DMF_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_LMTS_LOADER_ if you want to load LMTools files
#define _KONG_COMPILE_WITH_LMTS_LOADER_
#ifdef NO_KONG_COMPILE_WITH_LMTS_LOADER_
#undef _KONG_COMPILE_WITH_LMTS_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_MY3D_LOADER_ if you want to load MY3D files
#define _KONG_COMPILE_WITH_MY3D_LOADER_
#ifdef NO_KONG_COMPILE_WITH_MY3D_LOADER_
#undef _KONG_COMPILE_WITH_MY3D_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_OBJ_LOADER_ if you want to load Wavefront OBJ files
#define _KONG_COMPILE_WITH_OBJ_LOADER_
#ifdef NO_KONG_COMPILE_WITH_OBJ_LOADER_
#undef _KONG_COMPILE_WITH_OBJ_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_OCT_LOADER_ if you want to load FSRad OCT files
#define _KONG_COMPILE_WITH_OCT_LOADER_
#ifdef NO_KONG_COMPILE_WITH_OCT_LOADER_
#undef _KONG_COMPILE_WITH_OCT_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_LWO_LOADER_ if you want to load Lightwave3D files
#define _KONG_COMPILE_WITH_LWO_LOADER_
#ifdef NO_KONG_COMPILE_WITH_LWO_LOADER_
#undef _KONG_COMPILE_WITH_LWO_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_STL_LOADER_ if you want to load stereolithography files
#define _KONG_COMPILE_WITH_STL_LOADER_
#ifdef NO_KONG_COMPILE_WITH_STL_LOADER_
#undef _KONG_COMPILE_WITH_STL_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_PLY_LOADER_ if you want to load Polygon (Stanford Triangle) files
#define _KONG_COMPILE_WITH_PLY_LOADER_
#ifdef NO_KONG_COMPILE_WITH_PLY_LOADER_
#undef _KONG_COMPILE_WITH_PLY_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_SMF_LOADER_ if you want to load 3D World Studio mesh files
#define _KONG_COMPILE_WITH_SMF_LOADER_
#ifdef NO_KONG_COMPILE_WITH_SMF_LOADER_
#undef _KONG_COMPILE_WITH_SMF_LOADER_
#endif

//! Define _KONG_COMPILE_WITH_KONG_WRITER_ if you want to write static .irrMesh files
#define _KONG_COMPILE_WITH_KONG_WRITER_
#ifdef NO_KONG_COMPILE_WITH_KONG_WRITER_
#undef _KONG_COMPILE_WITH_KONG_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_COLLADA_WRITER_ if you want to write Collada files
#define _KONG_COMPILE_WITH_COLLADA_WRITER_
#ifdef NO_KONG_COMPILE_WITH_COLLADA_WRITER_
#undef _KONG_COMPILE_WITH_COLLADA_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_STL_WRITER_ if you want to write .stl files
#define _KONG_COMPILE_WITH_STL_WRITER_
#ifdef NO_KONG_COMPILE_WITH_STL_WRITER_
#undef _KONG_COMPILE_WITH_STL_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_OBJ_WRITER_ if you want to write .obj files
#define _KONG_COMPILE_WITH_OBJ_WRITER_
#ifdef NO_KONG_COMPILE_WITH_OBJ_WRITER_
#undef _KONG_COMPILE_WITH_OBJ_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_PLY_WRITER_ if you want to write .ply files
#define _KONG_COMPILE_WITH_PLY_WRITER_
#ifdef NO_KONG_COMPILE_WITH_PLY_WRITER_
#undef _KONG_COMPILE_WITH_PLY_WRITER_
#endif

//! Define _KONG_COMPILE_WITH_BMP_LOADER_ if you want to load .bmp files
//! Disabling this loader will also disable the built-in font
#define _KONG_COMPILE_WITH_BMP_LOADER_
#ifdef NO_KONG_COMPILE_WITH_BMP_LOADER_
#undef _KONG_COMPILE_WITH_BMP_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_JPG_LOADER_ if you want to load .jpg files
#define _KONG_COMPILE_WITH_JPG_LOADER_
#ifdef NO_KONG_COMPILE_WITH_JPG_LOADER_
#undef _KONG_COMPILE_WITH_JPG_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_PCX_LOADER_ if you want to load .pcx files
#define _KONG_COMPILE_WITH_PCX_LOADER_
#ifdef NO_KONG_COMPILE_WITH_PCX_LOADER_
#undef _KONG_COMPILE_WITH_PCX_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_PNG_LOADER_ if you want to load .png files
#define _KONG_COMPILE_WITH_PNG_LOADER_
#ifdef NO_KONG_COMPILE_WITH_PNG_LOADER_
#undef _KONG_COMPILE_WITH_PNG_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_PPM_LOADER_ if you want to load .ppm/.pgm/.pbm files
#define _KONG_COMPILE_WITH_PPM_LOADER_
#ifdef NO_KONG_COMPILE_WITH_PPM_LOADER_
#undef _KONG_COMPILE_WITH_PPM_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_PSD_LOADER_ if you want to load .psd files
#define _KONG_COMPILE_WITH_PSD_LOADER_
#ifdef NO_KONG_COMPILE_WITH_PSD_LOADER_
#undef _KONG_COMPILE_WITH_PSD_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_DDS_LOADER_ if you want to load .dds files
// Outcommented because
// a) it doesn't compile on 64-bit currently
// b) anyone enabling it should be aware that S3TC compression algorithm which might be used in that loader
// is patented in the US by S3 and they do collect license fees when it's used in applications.
// So if you are unfortunate enough to develop applications for US market and their broken patent system be careful.
// #define _KONG_COMPILE_WITH_DDS_LOADER_
#ifdef NO_KONG_COMPILE_WITH_DDS_LOADER_
#undef _KONG_COMPILE_WITH_DDS_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_TGA_LOADER_ if you want to load .tga files
#define _KONG_COMPILE_WITH_TGA_LOADER_
#ifdef NO_KONG_COMPILE_WITH_TGA_LOADER_
#undef _KONG_COMPILE_WITH_TGA_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_WAL_LOADER_ if you want to load .wal files
#define _KONG_COMPILE_WITH_WAL_LOADER_
#ifdef NO_KONG_COMPILE_WITH_WAL_LOADER_
#undef _KONG_COMPILE_WITH_WAL_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_LMP_LOADER_ if you want to load .lmp files
#define _KONG_COMPILE_WITH_LMP_LOADER_
#ifdef NO_KONG_COMPILE_WITH_LMP_LOADER_
#undef _KONG_COMPILE_WITH_LMP_LOADER_
#endif
//! Define _KONG_COMPILE_WITH_RGB_LOADER_ if you want to load Silicon Graphics .rgb/.rgba/.sgi/.int/.inta/.bw files
#define _KONG_COMPILE_WITH_RGB_LOADER_
#ifdef NO_KONG_COMPILE_WITH_RGB_LOADER_
#undef _KONG_COMPILE_WITH_RGB_LOADER_
#endif

//! Define _KONG_COMPILE_WITH_BMP_WRITER_ if you want to write .bmp files
#define _KONG_COMPILE_WITH_BMP_WRITER_
#ifdef NO_KONG_COMPILE_WITH_BMP_WRITER_
#undef _KONG_COMPILE_WITH_BMP_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_JPG_WRITER_ if you want to write .jpg files
#define _KONG_COMPILE_WITH_JPG_WRITER_
#ifdef NO_KONG_COMPILE_WITH_JPG_WRITER_
#undef _KONG_COMPILE_WITH_JPG_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_PCX_WRITER_ if you want to write .pcx files
#define _KONG_COMPILE_WITH_PCX_WRITER_
#ifdef NO_KONG_COMPILE_WITH_PCX_WRITER_
#undef _KONG_COMPILE_WITH_PCX_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_PNG_WRITER_ if you want to write .png files
#define _KONG_COMPILE_WITH_PNG_WRITER_
#ifdef NO_KONG_COMPILE_WITH_PNG_WRITER_
#undef _KONG_COMPILE_WITH_PNG_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_PPM_WRITER_ if you want to write .ppm files
#define _KONG_COMPILE_WITH_PPM_WRITER_
#ifdef NO_KONG_COMPILE_WITH_PPM_WRITER_
#undef _KONG_COMPILE_WITH_PPM_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_PSD_WRITER_ if you want to write .psd files
#define _KONG_COMPILE_WITH_PSD_WRITER_
#ifdef NO_KONG_COMPILE_WITH_PSD_WRITER_
#undef _KONG_COMPILE_WITH_PSD_WRITER_
#endif
//! Define _KONG_COMPILE_WITH_TGA_WRITER_ if you want to write .tga files
#define _KONG_COMPILE_WITH_TGA_WRITER_
#ifdef NO_KONG_COMPILE_WITH_TGA_WRITER_
#undef _KONG_COMPILE_WITH_TGA_WRITER_
#endif

//! Define __KONG_COMPILE_WITH_ZIP_ARCHIVE_LOADER_ if you want to open ZIP and GZIP archives
/** ZIP reading has several more options below to configure. */
#define __KONG_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#ifdef NO__KONG_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#undef __KONG_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#endif
#ifdef __KONG_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
//! Define _KONG_COMPILE_WITH_ZLIB_ to enable compiling the engine using zlib.
/** This enables the engine to read from compressed .zip archives. If you
disable this feature, the engine can still read archives, but only uncompressed
ones. */
#define _KONG_COMPILE_WITH_ZLIB_
#ifdef NO_KONG_COMPILE_WITH_ZLIB_
#undef _KONG_COMPILE_WITH_ZLIB_
#endif
//! Define _KONG_USE_NON_SYSTEM_ZLIB_ to let irrlicht use the zlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the zlib
installed on the system. This is only used when _KONG_COMPILE_WITH_ZLIB_ is
defined. */
#define _KONG_USE_NON_SYSTEM_ZLIB_
#ifdef NO_KONG_USE_NON_SYSTEM_ZLIB_
#undef _KONG_USE_NON_SYSTEM_ZLIB_
#endif
//! Define _KONG_COMPILE_WITH_ZIP_ENCRYPTION_ if you want to read AES-encrypted ZIP archives
#define _KONG_COMPILE_WITH_ZIP_ENCRYPTION_
#ifdef NO_KONG_COMPILE_WITH_ZIP_ENCRYPTION_
#undef _KONG_COMPILE_WITH_ZIP_ENCRYPTION_
#endif
//! Define _KONG_COMPILE_WITH_BZIP2_ if you want to support bzip2 compressed zip archives
/** bzip2 is superior to the original zip file compression modes, but requires
a certain amount of memory for decompression and adds several files to the
library. */
#define _KONG_COMPILE_WITH_BZIP2_
#ifdef NO_KONG_COMPILE_WITH_BZIP2_
#undef _KONG_COMPILE_WITH_BZIP2_
#endif
//! Define _KONG_USE_NON_SYSTEM_BZLIB_ to let irrlicht use the bzlib which comes with irrlicht.
/** If this is commented out, Irrlicht will try to compile using the bzlib
installed on the system. This is only used when _KONG_COMPILE_WITH_BZLIB_ is
defined. */
#define _KONG_USE_NON_SYSTEM_BZLIB_
#ifdef NO_KONG_USE_NON_SYSTEM_BZLIB_
#undef _KONG_USE_NON_SYSTEM_BZLIB_
#endif
//! Define _KONG_COMPILE_WITH_LZMA_ if you want to use LZMA compressed zip files.
/** LZMA is a very efficient compression code, known from 7zip. Irrlicht
currently only supports zip archives, though. */
#define _KONG_COMPILE_WITH_LZMA_
#ifdef NO_KONG_COMPILE_WITH_LZMA_
#undef _KONG_COMPILE_WITH_LZMA_
#endif
#endif

//! Define __KONG_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_ if you want to mount folders as archives
#define __KONG_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#ifdef NO__KONG_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#undef __KONG_COMPILE_WITH_MOUNT_ARCHIVE_LOADER_
#endif
//! Define __KONG_COMPILE_WITH_PAK_ARCHIVE_LOADER_ if you want to open ID software PAK archives
#define __KONG_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#ifdef NO__KONG_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#undef __KONG_COMPILE_WITH_PAK_ARCHIVE_LOADER_
#endif
//! Define __KONG_COMPILE_WITH_NPK_ARCHIVE_LOADER_ if you want to open Nebula Device NPK archives
#define __KONG_COMPILE_WITH_NPK_ARCHIVE_LOADER_
#ifdef NO__KONG_COMPILE_WITH_NPK_ARCHIVE_LOADER_
#undef __KONG_COMPILE_WITH_NPK_ARCHIVE_LOADER_
#endif
//! Define __KONG_COMPILE_WITH_TAR_ARCHIVE_LOADER_ if you want to open TAR archives
#define __KONG_COMPILE_WITH_TAR_ARCHIVE_LOADER_
#ifdef NO__KONG_COMPILE_WITH_TAR_ARCHIVE_LOADER_
#undef __KONG_COMPILE_WITH_TAR_ARCHIVE_LOADER_
#endif
//! Define __KONG_COMPILE_WITH_WAD_ARCHIVE_LOADER_ if you want to open WAD archives
#define __KONG_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#ifdef NO__KONG_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#undef __KONG_COMPILE_WITH_WAD_ARCHIVE_LOADER_
#endif

#endif
