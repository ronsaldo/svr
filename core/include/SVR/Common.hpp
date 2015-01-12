#ifndef _SVR_COMMON_HPP_
#define _SVR_COMMON_HPP_

#if defined(_MSC_VER)
#pragma warning( disable : 4251) // I have full control of the toolchain for my code.
#endif

#ifdef _WIN32
#	ifdef BUILD_SVR_CORE
#	define SVR_EXPORT __declspec(dllexport)
#	else
#	define SVR_EXPORT __declspec(dllimport)
#	endif
#else
#	define SVR_EXPORT
#endif //

#if defined(_MSC_VER)
#define SVR_NORETURN __declspec(noreturn)
#else
#define SVR_NORETURN [[noreturn]]
#endif


#endif //_LENG_COMMON_HPP_
