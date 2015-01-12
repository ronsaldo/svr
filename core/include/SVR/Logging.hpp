#ifndef _SVR_LOGGING_HPP_
#define _SVR_LOGGING_HPP_

#include "SVR/Common.hpp"

namespace SVR
{

/**
 * Logs a fatal error and aborts the execution of the program.
 */
SVR_EXPORT SVR_NORETURN void fatalError(const char *message);

/**
* Logs an error.
*/
SVR_EXPORT void logError(const char *message);

/**
* Logs a warning
*/
SVR_EXPORT void logWarning(const char *message);

/**
* Logs a message
*/
SVR_EXPORT void logMessage(const char *message);


}
#endif //_SVR_LOGGING_HPP_
