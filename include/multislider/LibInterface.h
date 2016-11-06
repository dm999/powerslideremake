/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_LIB_INTERFACE_H_
#define _MULTI_SLIDER_LIB_INTERFACE_H_

#if !defined(__MINGW32__)
#ifdef _WIN32 
# ifdef _MULTISLIDER_EXPORT_DLL
#  define MULTISLIDER_EXPORT _declspec(dllexport)
# else
#  define MULTISLIDER_EXPORT _declspec(dllimport)
# endif
#else
# define MULTISLIDER_EXPORT
#endif
#else
# define MULTISLIDER_EXPORT
#endif

#endif
