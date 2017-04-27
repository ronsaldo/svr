#include <set>
#include <stdio.h>
#include <string.h>
#include "SVR/FitsFile.hpp"

namespace SVR
{

inline bool isWhite(int c)
{
    return c <= ' ';
}

inline size_t computeHeaderSize(size_t numberOfProperties)
{
    return ((numberOfProperties + 1) * 80 + 2880) / 2880 * 2880;
}

FitsFile::FitsFile(MemoryMappedFile *memoryFile)
    : memoryFile(memoryFile)
{
    position = memoryFile->getData();
}

FitsFile::~FitsFile()
{
    delete memoryFile;
}

FitsFormat FitsFile::getFormat() const
{
    return format;
}

size_t FitsFile::getAxisCount() const
{
    return axis.size();
}

size_t FitsFile::getAxis(size_t index) const
{
    return index < getAxisCount() ? axis[index] : 1;
}

size_t FitsFile::getNumberOfElements() const
{
    size_t result = 1;
    for(auto axisValue : axis)
        result *= axisValue;
    return result;
}

size_t FitsFile::getWidth() const
{
    return getAxis(0);
}

size_t FitsFile::getHeight() const
{
    return getAxis(1);
}

size_t FitsFile::getDepth() const
{
    return getAxis(2);
}

const FitsHeaderProperties &FitsFile::getHeaderProperties() const
{
    return headerProperties;
}

char *FitsFile::getImageData()
{
    return imageData;
}

FitsFile *FitsFile::open(const char *fileName, bool canWrite)
{
    auto memoryFile = MemoryMappedFile::open(fileName, canWrite);
    if(!memoryFile)
        return nullptr;

    auto fits = new FitsFile(memoryFile);
    fits->readHeader();
    // If we do not have an axis, read the next header.
    if(fits->getPropertyIfAbsent("NAXIS", "0") == "0")
    {
        fits->headerProperties.clear();
        fits->readHeader();
    }

    fits->loadHeaderData();
    return fits;
}

FitsFile *FitsFile::create(const char *fileName, FitsHeaderProperties properties, size_t dataSize)
{
    size_t headerSize = computeHeaderSize(properties.size());

    auto memoryFile = MemoryMappedFile::create(fileName, headerSize + dataSize);
    if(!memoryFile)
        return nullptr;

    auto fits = new FitsFile(memoryFile);
    fits->headerProperties = properties;
    fits->loadHeaderData();
    fits->writeHeader();
    fits->imageData = memoryFile->getData() + headerSize;
    return fits;
}

std::string FitsFile::getPropertyIfAbsent(const std::string &name, const std::string &absentValue)
{
    auto it = headerProperties.find(name);
    if(it == headerProperties.end())
        return absentValue;
    return it->second;
}

void FitsFile::close()
{
    memoryFile->close();
}

void FitsFile::readHeader()
{
    char *headerStart = position;
    position = headerStart;

    for(;;)
    {
        auto keyValue = readHeaderLine();
        if(keyValue.first == "END")
            break;
        headerProperties.insert(keyValue);
    }

    auto headerSize = position - headerStart;
    auto remainingHeader = 2880 - (headerSize % 2880);
    position += remainingHeader;
    imageData = position;
}

void FitsFile::writeHeader()
{
    char buffer[32];
    std::set<std::string> specialProperties;

    // Clean the header.
    auto headerSize = computeHeaderSize(headerProperties.size());
    position = memoryFile->getData();
    memset(position, ' ', headerSize);

    // Write the special properties first.
    specialProperties.insert("SIMPLE");
    writeHeaderLine("SIMPLE", headerProperties["SIMPLE"]);

    specialProperties.insert("NAXIS");
    writeHeaderLine("NAXIS", headerProperties["NAXIS"]);

    for(size_t i = 1; i <= axis.size(); ++i)
    {
        sprintf(buffer, "NAXIS%d", int(i));
        specialProperties.insert(buffer);
        writeHeaderLine(buffer, headerProperties[buffer]);
    }

    specialProperties.insert("BITPIX");
    writeHeaderLine("BITPIX", headerProperties["BITPIX"]);

    // Write the normal properties.
    for(auto &prop : headerProperties)
    {
        auto it = specialProperties.find(prop.first);
        if(it == specialProperties.end())
            writeHeaderLine(prop.first, prop.second);
    }

    writeHeaderLine("END", std::string());
}

void FitsFile::writeHeaderLine(const std::string &key, const std::string &value)
{
    if(value.empty())
        sprintf(position, "%-8s", key.c_str());
    else
        sprintf(position, "%-8s= %s", key.c_str(), value.c_str());
    position += 80;
}

std::pair<std::string, std::string> FitsFile::readHeaderLine()
{
    std::string key, value;

    // Read the key
    for(int i = 0; i < 8; ++i)
    {
        if(!isWhite(position[i]))
            key.push_back(position[i]);
    }

    // Read the value.
    if(position[8] == '=' && position[9] == ' ')
    {
        for(int i = 10; i < 80; ++i)
        {
            char c = position[i];
            if(c == '/')
                break;
            if(!isWhite(c))
                value.push_back(c);
        }
    }

    position += 80;
    return std::make_pair(key, value);
}

void FitsFile::loadHeaderData()
{
    char buffer[32];
    size_t numberOfAxis = atoi(headerProperties["NAXIS"].c_str());
    axis.reserve(numberOfAxis);

    for(size_t i = 1; i <= numberOfAxis; ++i)
    {
        sprintf(buffer, "NAXIS%d", int(i));
        axis.push_back(atoi(headerProperties[buffer].c_str()));
    }

    format = (FitsFormat)atoi(headerProperties["BITPIX"].c_str());
}


} // namespace SVR
