/*
 * logging.h
 *
 * Logging facilities for printf-debugging on ESP8266.
 * Shame there is no good GDB for this chip.
 *
 *  Created on: 3 Jan 2018
 *      Author: m2
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include "Arduino.h"
#include "stdarg.h"
extern "C" {
#include "user_interface.h"
}

static void LOG_NAMED(const char* colorescape, const char* severity, const char* name, const char* format, va_list argptr);

static void LOG_DEBUG_NAMED(const char* name, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	LOG_NAMED("\033[1;32m","DEBUG",name,format,argptr);
}

static void LOG_INFO_NAMED(const char* name, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	LOG_NAMED("\033[1;37m","INFO",name,format,argptr);
}

static void LOG_WARN_NAMED(const char* name, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	LOG_NAMED("\033[1;33m","WARN",name,format,argptr);
}

static void LOG_ERROR_NAMED(const char* name, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	LOG_NAMED("\033[1;31m","ERROR",name,format,argptr);
}

static void LOG_FATAL_NAMED(const char* name, const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	LOG_NAMED("\033[1;31m","FATAL",name,format,argptr);
}

/**
 * Pretty-prints a message to the serial port.
 *
 * @param colorescape Escape code printed first for setting colors in terminal output
 * @param severity The string printed inside square brackets
 * @param name The string printed after square brackets
 * @param format Format string for printf
 * @param argptr Remaining arguments for printf
 */

static void LOG_NAMED(const char* colorescape, const char* severity, const char* name, const char* format, va_list argptr) {
	Serial.printf("%s%10d [%s] %s: ", colorescape, system_get_time(), severity, name);
	char buffer[128];
	vsprintf(buffer, format, argptr);
	Serial.print(buffer);
	Serial.print("\033[1;37m\n");
}


#endif /* LOGGING_H_ */
