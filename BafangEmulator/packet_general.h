#pragma once
#include "packet_types.h"
#include <cstdint>


namespace core
{
#pragma pack(push, 1)
  struct request_general
  {
    static const packet_types packet_type = packet_types::general;

    uint8_t unknown;
  };

  struct response_general
  {
    static const packet_types packet_type = packet_types::general;

    char manufacturer[4];
    char model[4];
    uint16_t hardware_version;
    uint32_t firmware_version;
    uint8_t nominal_voltage;
    uint8_t limit_control;
  };
#pragma pack(pop)
}
