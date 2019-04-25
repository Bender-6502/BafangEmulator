#include "serial.h"
#include <Windows.h>
#include <stdexcept>
#include <thread>

namespace core
{
  struct serial::impl
  {
    impl()
      : handle_(INVALID_HANDLE_VALUE)
    {}

   ~impl()
    {
      disconnect();
    }

    void event(events ev, core::bind&& func)
    {
      switch (ev)
      {
        case events::connected:      connected_      = std::move(func); break;
        case events::disconnected:   disconnected_   = std::move(func); break;
        case events::data_available: data_available_ = std::move(func); break;
      }
    }

    void connect(const std::string& port)
    {
      if (is_connected())
        CloseHandle(handle_);

      port_   = port;
      handle_ = CreateFileA(port_.c_str(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            0,
                            OPEN_EXISTING,
                            0 /*FILE_FLAG_OVERLAPPED*/,
                            0);

      if (!is_connected())
        throw std::system_error(GetLastError(), std::system_category(), "open port failure");

      if (connected_)
        connected_();
    }

    void disconnect()
    {
      if (is_connected())
      {
        HANDLE temp = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        CloseHandle(temp);

        if (disconnected_)
          disconnected_();
      }
    }


    void poll()
    {
      if (is_connected())
      {
        char buffer[8092] = { 0 };

        DWORD length = 0;
        if (ReadFile(handle_, buffer, sizeof(buffer), &length, 0 /*&osReader*/))
        {
          buffer_ += std::string(buffer, length);
          if (data_available_)
            data_available_();
        }
      }
      std::this_thread::yield();
    }

    void param(const std::string& data)
    {
      DCB dcb;
      FillMemory(&dcb, sizeof(dcb), 0);
      dcb.DCBlength = sizeof(dcb);

      if (!BuildCommDCBA(data.c_str(), &dcb))
        throw std::system_error(GetLastError(), std::system_category(), "set param failure from string");

      if (!SetCommState(handle_, &dcb))
        throw std::system_error(GetLastError(), std::system_category(), "set param failure");

      COMMTIMEOUTS timeouts;
      timeouts.ReadIntervalTimeout = 20;
      timeouts.ReadTotalTimeoutMultiplier = 10;
      timeouts.ReadTotalTimeoutConstant = 100;
      timeouts.WriteTotalTimeoutMultiplier = 10;
      timeouts.WriteTotalTimeoutConstant = 100;
      if (!SetCommTimeouts(handle_, &timeouts))
        throw std::system_error(GetLastError(), std::system_category(), "set comms timeout failure");
    }

    void flush(size_t size)
    {
      buffer_.erase(0, size);
    }

    std::string read(size_t size)
    {
      std::string temp = buffer_.substr(0, size);
      buffer_ = buffer_.substr(size);
      return temp;
    }

    void flush_all()
    {
      buffer_.clear();
    }

    std::string read_all()
    {
      std::string temp = std::move(buffer_);
      flush_all();
      return temp;
    }

    void write(const std::string& data)
    {
      DWORD dwWritten = 0;
      if (!WriteFile(handle_, data.data(), (DWORD)data.length(), &dwWritten, nullptr))
      {
        throw std::system_error(GetLastError(), std::system_category(), "comms write failure");
      }
    }

    const std::string& peek() const
    {
      return buffer_;
    }

    size_t size() const
    {
      return buffer_.size();
    }

    const std::string& port() const
    {
      return port_;
    }

    bool is_connected() const
    {
      return handle_ != INVALID_HANDLE_VALUE;
    }

  protected:

    HANDLE handle_;
    bind connected_, disconnected_, data_available_;
    std::string buffer_, port_;
  };


  serial::serial()
    : impl_(new impl)
  {}


  serial::~serial()
  {}


  void serial::event(events ev, core::bind&& func)
  {
    if (impl_)
      impl_->event(ev, std::move(func));
    else
      throw std::runtime_error("no state");
  }


  void serial::connect(const std::string& port)
  {
    if (impl_)
      impl_->connect(port);
    else
      throw std::runtime_error("no state");
  }


  void serial::disconnect()
  {
    if (impl_)
      impl_->disconnect();
    else
      throw std::runtime_error("no state");
  }

  void serial::poll()
  {
    if (impl_)
      impl_->poll();
    else
      throw std::runtime_error("no state");
  }


  void serial::param(const std::string& data /*= "9600,n,8,1"*/)
  {
    if (impl_)
      impl_->param(data);
    else
      throw std::runtime_error("no state");
  }


  void serial::flush(size_t size)
  {
    if (impl_)
      impl_->flush(size);
    else
      throw std::runtime_error("no state");
  }


  std::string serial::read(size_t size)
  {
    if (impl_)
      return impl_->read(size);
    else
      throw std::runtime_error("no state");
  }


  void serial::flush_all()
  {
    if (impl_)
      impl_->flush_all();
    else
      throw std::runtime_error("no state");
  }


  std::string serial::read_all()
  {
    if (impl_)
      return impl_->read_all();
    else
      throw std::runtime_error("no state");
  }


  void serial::write(const std::string& data)
  {
    if (impl_)
      impl_->write(data);
    else
      throw std::runtime_error("no state");
  }


  const std::string& serial::peek() const
  {
    if (impl_)
      return impl_->peek();
    else
      throw std::runtime_error("no state");
  }


  size_t serial::size() const
  {
    if (impl_)
      return impl_->size();
    else
      throw std::runtime_error("no state");
  }


  const std::string& serial::port() const
  {
    if (impl_)
      return impl_->port();
    else
      throw std::runtime_error("no state");
  }


  bool serial::is_connected() const
  {
    return impl_ && impl_->is_connected();
  }
}
