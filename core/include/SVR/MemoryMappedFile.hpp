#ifndef _SVR_MEMORY_MAPPED_FILE_HPP_
#define _SVR_MEMORY_MAPPED_FILE_HPP_

// Memory mapped file definition is platform specific.
#if defined(_WIN32)
#   include "SVR/Win32MemoryMappedFile.hpp"
#elif (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#   include "SVR/UnixMemoryMappedFile.hpp"
#else
#   error unsupported operating system
#endif

#endif //_SVR_MEMORY_MAPPED_FILE_HPP_
