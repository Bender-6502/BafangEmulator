/**
* @file trace.h
* Logging interface.
*
* Also benefits from the following:
*	Supports printf formatting
*	Logs source file and line number
*	thread safety
*/
#pragma once

namespace trace
{
  /**
  * @brief Supported trace operations, currently set to only file
  and console (stdout), although this can be extended
  sometime in the future to support serial (rs232) or
  maybe even sockets
  */
  enum flags : unsigned int
  {
    flag_file = 1,
    flag_console = 2,
    flag_reserved = 4,
  };

  /**
  Initialise the logging engine and specify which operations are valid

  To support more than one flag use the following:
  @code trace::init(trace::flag_file | trace::flag_console); @endcode

  @param[in] flags The trace flags.
  */
  void init(unsigned int flags);

  /**
  Initialise the logging engine and specify which operations are valid

  @param[in]  file The file to write logs to.
  */
  void filename(const char* file);

  /**
  Log formatted message

  @param[in]  file The source filename.
  @param[in]  line The source line number.
  @param[in]  format The message to log.
  */
  void message(const char* file, int line, const char* format, ...);

  /**
  Log binary array

  @param[in]  file The source filename.
  @param[in]  line The source line number.
  @param[in]  buffer Binary array to log.
  @param[in]  size Binary array size
  */
  void binary(const char* file, int line, const unsigned char* buffer, size_t size);

  /**
  Log binary array

  @param[in]  file The source filename.
  @param[in]  line The source line number.
  @param[in]  buffer Binary array to log.
  @param[in]  size Binary array size
  */
  void binary(const char* file, int line, const char* buffer, size_t size);
}

#define TRACE_INIT(flags) trace::init(flags)
#define TRACE_FILENAME(file) trace::filename(file)
#define TRACE_MESSAGE(...) trace::message(__FILE__, __LINE__, __VA_ARGS__)
#define TRACE_BINARY(buffer, size) trace::binary(__FILE__, __LINE__, buffer, size)
