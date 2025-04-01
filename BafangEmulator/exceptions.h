#pragma once
#include <system_error>


namespace core
{
  std::error_code get_last_error();
  void exception_handler();
}
