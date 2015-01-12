#ifndef _SVR_LOAD_UTILITIES_HPP_
#define _SVR_LOAD_UTILITIES_HPP_

#include <string>
#include <vector>
#include "SVR/Common.hpp"

namespace SVR
{

/**
 * Loads a whole text file in memory.
 */
SVR_EXPORT bool loadTextFileInto(const std::string &path, std::vector<char> &dest);

}
#endif //_SVR_LOAD_UTILITIES_HPP_
