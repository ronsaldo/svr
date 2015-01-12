#include <stdio.h>
#include "SVR/LoadUtilities.hpp"

namespace SVR
{

bool loadTextFileInto(const std::string &path, std::vector<char> &dest)
{
	// Open the file.
	FILE *f = fopen(path.c_str(), "rb");
	if (!f)
		return false;

	// Get the file size.
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	dest.resize(size+1);

	// Read the data into the buffer
	fseek(f, 0, SEEK_SET);
	size_t result = fread(&dest[0], size, 1, f);
	if (result != 1)
	{
		dest.clear();
		return false;
	}

	return true;
}

} // namespace SVR
