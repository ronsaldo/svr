#include <stdio.h>
#include <stdlib.h>
#include "SVR/Logging.hpp"

namespace SVR
{
/**
* Logs a fatal error and aborts the execution of the program.
*/
void fatalError(const char *message)
{
	fprintf(stderr, "FATAL ERROR: %s\n", message);
	abort();
}

/**
* Logs an error.
*/
void logError(const char *message)
{
	fprintf(stderr, "ERROR: %s\n", message);
}

/**
* Logs a warning
*/
void logWarning(const char *message)
{
	fprintf(stderr, "WARNING: %s\n", message);
}

/**
* Logs a message
*/
void logMessage(const char *message)
{
	fprintf(stdout, "MESSAGE: %s\n", message);
}
}
