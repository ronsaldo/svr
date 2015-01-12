#ifndef _WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "SVR/UnixMemoryMappedFile.hpp"

namespace SVR
{
MemoryMappedFile::MemoryMappedFile(int fd, char *data, size_t size)
    : fd(fd), data(data), size(size)
{
}

MemoryMappedFile::~MemoryMappedFile()
{
}

MemoryMappedFile *MemoryMappedFile::create(const char *filename, size_t size)
{
    // Open the file.
    int fd = ::open(filename, O_RDWR | O_CREAT, 0644);
    if(fd < 0)
    {
        perror("Failed to open file");
        abort();
    }

    // Set the file size
    if(ftruncate(fd, size) < 0)
    {
        perror("Failed to reserve file size");
        abort();
    }

    // Perform memory mapping
    char *data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(!data)
    {
        perror("Failed to memory map file");
        abort();
    }

    return new MemoryMappedFile(fd, data, size);
}

MemoryMappedFile *MemoryMappedFile::open(const char *filename, bool canWrite)
{
    // Open the file.
    int fd = ::open(filename, canWrite ? O_RDONLY : O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open file");
        abort();
    }

    // Get the file size
    size_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // Perform memory mapping
    char *data = (char*)mmap(NULL, size, canWrite ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0);
    if(!data)
    {
        perror("Failed to memory map file");
        abort();
    }

    return new MemoryMappedFile(fd, data, size);
}

void MemoryMappedFile::close()
{
    munmap(data, size);
    ::close(fd);
}

size_t MemoryMappedFile::getSize()
{
    return size;
}

char *MemoryMappedFile::getData()
{
    return data;
}

} // namespace ImageMapping

#endif // _WIN32
