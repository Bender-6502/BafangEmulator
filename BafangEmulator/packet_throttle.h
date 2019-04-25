#pragma once
#include "packet_types.h"
#include <cstdint>


namespace core
{
#pragma pack(push, 1)
  struct response_throttle
  {
    static const packet_types packet_type = packet_types::throttle;

    uint8_t start_volt;
    uint8_t end_volt;
    uint8_t mode;
    uint8_t assist_level;
    uint8_t speed_limit;
    uint8_t start_current;
  };

  using request_throttle = response_throttle;

  enum class response_status_throttle : uint8_t
  {
    start_volt,
    end_volt,
    mode,
    assist_level,
    speed_limit,
    start_current,
    success,
  };
#pragma pack(pop)
}