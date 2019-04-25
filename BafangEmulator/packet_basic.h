#pragma once
#include "packet_types.h"
#include <cstdint>


namespace core
{
#pragma pack(push, 1)
  struct response_basic
  {
    static const packet_types packet_type = packet_types::basic;

    uint8_t low_battery;
    uint8_t current_limit;

    uint8_t assist0_current;
    uint8_t assist1_current;
    uint8_t assist2_current;
    uint8_t assist3_current;
    uint8_t assist4_current;
    uint8_t assist5_current;
    uint8_t assist6_current;
    uint8_t assist7_current;
    uint8_t assist8_current;
    uint8_t assist9_current;

    uint8_t assist0_speed;
    uint8_t assist1_speed;
    uint8_t assist2_speed;
    uint8_t assist3_speed;
    uint8_t assist4_speed;
    uint8_t assist5_speed;
    uint8_t assist6_speed;
    uint8_t assist7_speed;
    uint8_t assist8_speed;
    uint8_t assist9_speed;

    uint8_t wheel_size;

    uint8_t speed_meter;
  };

  using request_basic = response_basic;

  enum class response_status_basic : uint8_t
  {
    low_battery,
    current_limit,

    assist0_current,
    assist0_speed,
    assist1_current,
    assist1_speed,
    assist2_current,
    assist2_speed,
    assist3_current,
    assist3_speed,
    assist4_current,
    assist4_speed,
    assist5_current,
    assist5_speed,
    assist6_current,
    assist6_speed,
    assist7_current,
    assist7_speed,
    assist8_current,
    assist8_speed,
    assist9_current,
    assist9_speed,

    wheel_size,

    speed_meter,

    success,
  };
#pragma pack(pop)
}