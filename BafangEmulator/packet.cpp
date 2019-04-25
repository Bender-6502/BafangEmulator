#include "packet.h"


namespace core
{
  void deserialize(const std::string& data, uint8_t* ptr, size_t size) noexcept(false)
  {
    if (data.size() < size)
    {
      throw std::runtime_error("packet truncation");
    }

    for (size_t i = 0; i < size; i++)
    {
      *ptr++ = data[i];
    }
  }

  std::string serialize(const uint8_t* ptr, size_t size) noexcept
  {
    std::string temp;

    for (size_t i = 0; i < size; i++)
    {
      temp += *ptr++;
    }

    return temp;
  }
}