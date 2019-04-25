// Thread safe
#pragma once
#include "trace.h"
#include "serial.h"
#include "profile.h"
#include <string>
#include <mutex>


namespace core
{
  class serial_handler
  {
  public:

    serial_handler(const std::string& port, profile& general, profile& config);
   ~serial_handler();

    void poll();

  protected:

    void on_connected();
    void on_disconnected();
    void on_data_available();

  private:

    serial s_;
    profile& general_;
    profile& config_;
    static std::mutex mutex_;
  };
}
