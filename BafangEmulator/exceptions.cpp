#include "exceptions.h"
#include "trace.h"
#include <Windows.h>


namespace core
{
  std::error_code get_last_error()
  {
    return std::make_error_code(static_cast<std::errc>(GetLastError()));
  }

  void exception_handler()
  {
    try
    {
      throw;
    }
    catch (std::system_error& e)
    {
      TRACE_MESSAGE("System error: %s (%d)", e.what(), e.code().value());
    }
    catch (std::exception& e)
    {
      TRACE_MESSAGE("Standard error: %s", e.what());
    }
    catch (...)
    {
      TRACE_MESSAGE("Unknown error");
    }
  }
}
