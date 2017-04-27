#ifndef _SVR_FITS_FILE_HPP_
#define _SVR_FITS_FILE_HPP_

#include <map>
#include <string>
#include <vector>
#include <assert.h>

#include "SVR/MemoryMappedFile.hpp"

namespace SVR
{

/**
 * FITS format constants
 */
enum class FitsFormat
{
    UInt8 = 8,
    Int16 = 16,
    Int32 = 32,
    Int64 = 64,
    Float = -32,
    Double = -64,
};

struct SliceRange
{
    SliceRange(int start=-1, int size=-1)
        : start(start), size(size) {}

    bool isValid() const
    {
        return start >= 0 && size >= 0;
    }

    void setWholeSize(int wholeSize)
    {
        start = 0;
        size = wholeSize;
    }

    void clampToRange(int rangeStart, int rangeEnd)
    {
        if(start < rangeStart)
            start = rangeStart;
        if(start + size > rangeEnd)
            size -= start + size - rangeEnd;
    }
    
    int start;
    int size;
};

typedef std::map<std::string, std::string> FitsHeaderProperties;

/**
 * Fits file
 */
class FitsFile
{
public:
    FitsFile(MemoryMappedFile *memoryFile);
    ~FitsFile();

    size_t getAxisCount() const;
    size_t getAxis(size_t index) const;

    size_t getWidth() const;
    size_t getHeight() const;
    size_t getDepth() const;
    size_t getNumberOfElements() const;
    FitsFormat getFormat() const;

    const FitsHeaderProperties &getHeaderProperties() const;
    char *getImageData();

    static FitsFile *open(const char *fileName, bool canWrite=false);
    static FitsFile *create(const char *fileName, FitsHeaderProperties properties, size_t dataSize);
    void close();

    std::string getPropertyIfAbsent(const std::string &name, const std::string &absentValue);

private:
    void readHeader();
    void loadHeaderData();

    void writeHeader();
    void writeHeaderLine(const std::string &key, const std::string &value);

    std::pair<std::string, std::string> readHeaderLine();

    MemoryMappedFile *memoryFile;
    char *imageData;
    char *position;
    FitsHeaderProperties headerProperties;
    std::vector<size_t> axis;
    FitsFormat format;
};

} // namespace SVR

#endif // _SVR_FITS_FILE_HPP_
