#include "trace.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <cctype>
#include <mutex>

#pragma warning (disable : 4996)

namespace trace
{
  namespace
  {
    unsigned int current_flags = 0;
    char current_file[FILENAME_MAX] = { 0 };

    /**
    * @brief Retrieve the date and time including the current tick count
    */
    void date_time(char* out, size_t size)
    {
      time_t rawtime = { 0 };
      time(&rawtime);
      struct tm* timeinfo = localtime(&rawtime);

      size_t written = strftime(out, size, "%d/%m/%y %X", timeinfo);
      sprintf(out + written, ".%03u", (unsigned int)clock() % 1000U);
    }

    /**
    * @brief Retrieve the filename from the path
    */
    const char* leaf(const char* in)
    {
#if defined (_WIN32)
      const char* found = strrchr(in, '\\');
#else
      const char* found = strrchr(in, '/');
#endif
      if (found)
        return found + 1;
      else
        return in;
    }
  }

  void init(unsigned int flags)
  {
    current_flags = flags;
  }

  void filename(const char* f)
  {
    if (f)
      strcpy(current_file, f);
  }

  void message(const char* file, int line, const char* format, ...)
  {
    /* Mutex lock */
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    FILE* current_output = NULL;
    char current_date[50] = { 0 };
    unsigned int operations = current_flags;

    /* Get the current date and time */
    date_time(current_date, sizeof(current_date));

    while (operations)
    {
      /* Acquire next operation */
      if (operations & flag_file)
      {
        /* Log to file */
        current_output = fopen(current_file, "a+");
      }
      else if (operations & flag_console)
      {
        /* Log to console */
        current_output = stdout;
      }
      else
      {
        /* Ignore other operations or we loop infinitely */
        operations = 0;
      }

      if (current_output)
      {
        /* Log the source file, line number, and data/time */
        fprintf(current_output, "%s|L:%d|%s|", leaf(file), line, current_date);

        /* Log the message */
        va_list args;
        va_start(args, format);
        vfprintf(current_output, format, args);
        va_end(args);

        /* Line ending */
        fprintf(current_output, "\r\n");
      }

      /* Housekeeping by closing the handle if necessary
      and clearing the current operation flag */
      if (operations & flag_file)
      {
        if (current_output)
        {
          fclose(current_output);
        }
        operations &= ~flag_file;
      }
      else if (operations & flag_console)
      {
        operations &= ~flag_console;
      }

      current_output = NULL;
    }
  }

  void binary(const char* file, int line, const unsigned char* buffer, size_t size)
  {
    while (size)
    {
      int offset = 0;
      char data[256] = { 0 };

      size_t count = 16;
      if (count > size)
        count = size;

      for (size_t i = 0; i < count; i++)
      {
        offset += sprintf(data + offset, "%02X ", buffer[i]);
      }

      for (size_t i = 0; i < 16 - count; i++)
      {
        offset += sprintf(data + offset, "   ");
      }

      for (size_t i = 0; i < count; i++)
      {
        if (isprint(buffer[i]))
          offset += sprintf(data + offset, "%c", buffer[i]);
        else
          offset += sprintf(data + offset, ".");
      }

      message(file, line, data);

      buffer += count;
      size -= count;
    }
  }


  void binary(const char* file, int line, const char* buffer, size_t size)
  {
    binary(file, line, reinterpret_cast<const unsigned char*>(buffer), size);
  }
}
