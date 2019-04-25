#include "exceptions.h"
#include "trace.h"
#include <system_error>

namespace core
{
  void exception_handler()
  {
    try
    {
      throw;
    }
    catch (std::system_error& e)
    {
      TRACE_MESSAGE("exception: system_error, code: %d, what: %s", e.code().value(), e.what());
    }
    catch (std::exception& e)
    {
      TRACE_MESSAGE("exception: standard, what: %s", e.what());
    }
    catch (...)
    {
      TRACE_MESSAGE("exception: catch-all");
    }
  }
}
