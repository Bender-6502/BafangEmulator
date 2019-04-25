#pragma once
#include <cstdint>


namespace core
{
  enum class packet_commands : uint8_t
  {
    read  = 0x11,  // Read flash
    write = 0x16,  // Write flash
  };

  enum class packet_types : uint8_t
  {
    general  = 0x51, 
    basic    = 0x52,   
    pedal    = 0x53, 
    throttle = 0x54,
  };

  enum class marker_types : uint8_t
  {
    rdIgnore = 0,  // marker - ignore further received data
    rdSingle = 1,  // marker - single block read
    wrSingle = 2,  // marker - single block write
    rdAll = 3,     // marker - full flash read
    wrAll = 4,     // marker - full flash write
    rdGEN = 5,     // marker - General data block read
  };

  using length_t = uint8_t;

  using verify_t = uint8_t;
}
