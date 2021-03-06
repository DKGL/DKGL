#ifndef CONFIG_H
#define CONFIG_H

#define ALSOFT_VERSION "1.15.1"

#ifdef _WIN32
#ifdef AL_LIBTYPE_STATIC
#define AL_API
#define ALC_API
#else
#define AL_API  __declspec(dllexport)
#define ALC_API __declspec(dllexport)
#endif

#define ALIGN(x) __declspec(align(x))

#define SIZEOF_LONG			4
#define SIZEOF_LONG_LONG	8

#define restrict
#define inline __inline
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define strcasecmp	_stricmp
#define strncasecmp	_strnicmp
#if _MSC_VER < 1900
#define snprintf	_snprintf
#endif
/* #define isfinite	_finite */
/* #define isnan	_isnan */

#define HAVE__ALIGNED_MALLOC
#define HAVE_SSE
#define HAVE_MMDEVAPI
#define HAVE_DSOUND
#define HAVE_WINMM
#define HAVE_WAVE
#define HAVE_STAT
#define HAVE_STDINT_H
#define HAVE_WINDOWS_H
#define HAVE_XMMINTRIN_H
#define HAVE_MALLOC_H
#define HAVE_GUIDDEF_H
#define HAVE_FLOAT_H
#define HAVE__CONTROLFP
#endif	/*fdef _WIN32*/

#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE		// iOS
#error OpenAL-soft not support iOS.
#endif

#define AL_API  __attribute__((visibility("default")))
#define ALC_API __attribute__((visibility("default")))

/* Define any available alignment declaration */
#define ALIGN(x) __attribute__((aligned(x)))

#if __LP64__
#define SIZEOF_LONG			8
#define SIZEOF_LONG_LONG	8
#else
#define SIZEOF_LONG			4
#define SIZEOF_LONG_LONG	8
#endif

#define restrict __restrict
#define HAVE_POSIX_MEMALIGN
#define HAVE_SSE
#define HAVE_COREAUDIO
#define HAVE_WAVE
#define HAVE_STAT
#define HAVE_LRINTF
#define HAVE_STRTOF
#define HAVE_GCC_DESTRUCTOR
#define HAVE_GCC_FORMAT
#define HAVE_STDINT_H
#define HAVE_DLFCN_H
#define HAVE_XMMINTRIN_H
#define HAVE_CPUID_H
#define HAVE_FLOAT_H
#define HAVE_FENV_H
#define HAVE_PTHREAD_SETSCHEDPARAM
#endif	/*if defined(__APPLE__) && defined(__MACH__)*/

#ifdef __ANDROID__
#define AL_API  __attribute__((visibility("default")))
#define ALC_API __attribute__((visibility("default")))

/* Define any available alignment declaration */
#define ALIGN(x) __attribute__((aligned(x)))

#if __LP64__
#define SIZEOF_LONG			8
#define SIZEOF_LONG_LONG	8
#else
#define SIZEOF_LONG			4
#define SIZEOF_LONG_LONG	8
#endif

#define restrict __restrict
#define HAVE_ARM_NEON_H
#define HAVE_OPENSL
#define HAVE_WAVE
#define HAVE_STAT
#define HAVE_LRINTF
#define HAVE_STRTOF
#define HAVE_GCC_DESTRUCTOR
#define HAVE_GCC_FORMAT
#define HAVE_STDINT_H
#define HAVE_DLFCN_H
#define HAVE_XMMINTRIN_H
#define HAVE_FLOAT_H
#define HAVE_FENV_H
#define HAVE_PTHREAD_SETSCHEDPARAM
#endif /*ifdef __ANDROID__*/

#endif /*ifndef CONFIG_H*/