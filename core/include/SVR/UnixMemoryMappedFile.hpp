#ifndef _SVR_UNIX_MEMORY_MAPPED_FILE_HPP_
#define _SVR_UNIX_MEMORY_MAPPED_FILE_HPP_

namespace SVR
{

/**
 * This class is used to represent a memory mapped file.
 */
class MemoryMappedFile
{
public:
    MemoryMappedFile(int fd, char *data, size_t size);
    ~MemoryMappedFile();

    static MemoryMappedFile *create(const char *filename, size_t size);
    static MemoryMappedFile *open(const char *filename, bool canWrite);
    void close();

    size_t getSize();
    char *getData();

private:
    int fd;
    char *data;
    size_t size;
};

} // namespace ImageMapping

#endif //_SVR_MAPPED_FILE_HPP_

