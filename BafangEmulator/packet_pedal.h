#pragma once
#include "packet_types.h"
#include <cstdint>


namespace core
{
#pragma pack(push, 1)
  struct response_pedal
  {
    static const packet_types packet_type = packet_types::pedal;

    uint8_t sensor_type;
    uint8_t assist_level;
    uint8_t speed_limit;
    uint8_t start_current;

    uint8_t slow_start_mode;
    uint8_t start_deg;
    uint8_t work_mode;

    uint8_t stop_delay;
    uint8_t current_decay;
    uint8_t stop_decay;
    uint8_t keep_current;
  };

  using request_pedal = response_pedal;

  enum class response_status_pedal : uint8_t
  {
    sensor_type,
    assist_level,
    speed_limit,
    start_current,
    
    slow_start_mode,
    start_deg,
    work_mode,
    
    stop_delay,
    current_decay,
    stop_decay,
    keep_current,
    success,
  };
#pragma pack(pop)
}