#pragma once
#include "bind.h"
#include <memory>
#include <string>
#include <functional>

namespace core
{
  class serial
  {
  public:

    enum class events
    {
      connected,
      disconnected,
      data_available,
    };

    serial();
    serial(serial&&) = default;
    serial(const serial&) = delete;
    serial& operator=(serial&&) = default;
    serial& operator=(const serial&) = delete;
   ~serial();

    void event(events ev, core::bind&& func);

    void connect(const std::string& port);
    void disconnect();
    void poll();

    void param(const std::string& data = "9600,n,8,1");

    void flush(size_t size);
    std::string read(size_t size);

    void flush_all();
    std::string read_all();

    void write(const std::string& data);

    const std::string& peek() const;
    size_t size() const;
    const std::string& port() const;

    bool is_connected() const;

  private:

    struct impl;
    std::unique_ptr<impl> impl_;
  };
}
