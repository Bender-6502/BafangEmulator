#pragma once
#include "packet_types.h"
#include <cstdio>
#include <cstdint>
#include <string>


namespace core
{
  void deserialize(const std::string& data, uint8_t* ptr, size_t size) noexcept(false);
  std::string serialize(const uint8_t* ptr, size_t size) noexcept;

#pragma pack(push, 1)
  template<class T>
  struct request_packet
  {
    typedef T valuetype;

    packet_commands command;
    packet_types type;
    length_t size;
    valuetype payload;
    verify_t verification;

    request_packet()
      : command(packet_commands::read)
      , type(valuetype::packet_type)
      , size(packet_size() - 1)
      , verification(0)
    {}

    request_packet(packet_commands command, packet_types type)
      : command(command)
      , type(type)
      , size(packet_size() - 1)
      , verification(0)
    {}

   ~request_packet()
    {}

    void deserialize(const std::string& data) noexcept(false)
    {
      core::deserialize(data, reinterpret_cast<uint8_t*>(this), packet_size());

      if (verification != verify())
        throw std::runtime_error("verification failure");
    }

    std::string serialize() const noexcept
    {
      return core::serialize(reinterpret_cast<const uint8_t*>(this), packet_size());
    }

    verify_t verify() const noexcept
    {
      verify_t checksum = static_cast<uint8_t>(type) + size;

      const uint8_t* data = reinterpret_cast<const uint8_t*>(&payload);
      for (uint32_t i = 0; i < sizeof(payload); i++)
      {
        checksum += *data++;
      }

      return checksum % 256;
    }

    const char* data() const
    {
      return reinterpret_cast<const char*>(this);
    }

    size_t length() const
    {
      return packet_size();
    }

    static length_t packet_size()
    {
      return static_cast<length_t>(sizeof(request_packet<valuetype>));
    }
  };

  template<class T>
  struct response_packet
  {
    typedef T valuetype;

    packet_types type;
    length_t size;
    valuetype payload;

    response_packet()
      : type(valuetype::packet_type)
      , size(packet_size())
    {}

    response_packet(packet_types type)
      : type(type)
      , size(packet_size())
    {}

   ~response_packet()
    {}

    void deserialize(const std::string& data) noexcept(false)
    {
      core::deserialize(data, reinterpret_cast<uint8_t*>(this), packet_size());
    }

    std::string serialize() const noexcept
    {
      return core::serialize(reinterpret_cast<const uint8_t*>(this), packet_size());
    }

    const char* data() const
    {
      return reinterpret_cast<const char*>(this);
    }

    size_t length() const
    {
      return packet_size();
    }

    static length_t packet_size()
    {
      return static_cast<length_t>(sizeof(response_packet<valuetype>));
    }
  };

  template<class T>
  struct response_status_packet
  {
    typedef T valuetype;

    packet_types type;
    valuetype status;

    response_status_packet()
      : type(packet_types::general)
      , status(0)
    {}

    response_status_packet(packet_types type, valuetype status)
      : type(type)
      , status(status)
    {}

   ~response_status_packet()
    {}

    void deserialize(const std::string& data) noexcept(false)
    {
      core::deserialize(data, reinterpret_cast<uint8_t*>(this), packet_size());
    }

    std::string serialize() const noexcept
    {
      return core::serialize(reinterpret_cast<const uint8_t*>(this), packet_size());
    }

    const char* data() const
    {
      return reinterpret_cast<const char*>(this);
    }

    size_t length() const
    {
      return packet_size();
    }

    static length_t packet_size()
    {
      return static_cast<length_t>(sizeof(response_status_packet));
    }
  };
#pragma pack(pop)
}
