#include "gtest/gtest.h"
#include "packet.h"
#include "packet_general.h"


TEST(request_packet, general_size_test)
{
  EXPECT_EQ(sizeof(core::request_general), 1);
}

TEST(request_packet, general_serialize_test)
{
  core::request_packet<core::request_general> general;

  general.command = core::packet_commands::read;
  general.type = core::packet_types::general;
  general.payload.unknown = 0xB0;
  general.verification = general.verify();

  auto str = general.serialize();

  std::string expected = { 0x11, 0x51, 0x04, (char)0xB0, 0x05 };

  EXPECT_EQ(str, expected);
}

TEST(response_packet, general_size_test)
{
  EXPECT_EQ(sizeof(core::response_general), 16);
}

TEST(response_packet, general_serialize_test)
{
  core::response_packet<core::response_general> general;

  general.type = core::packet_types::general;
  memcpy(general.payload.manufacturer, "HZXT", sizeof(general.payload.manufacturer));
  memcpy(general.payload.model, "BBS3", sizeof(general.payload.model));
  general.payload.hardware_version = '31';
  general.payload.firmware_version = '1100';
  general.payload.nominal_voltage = 4;
  general.payload.limit_control = 30;

  auto actual = general.serialize();

  std::string expected = {
    0x51,
    18,
    'H', 'Z', 'X', 'T',
    'B', 'B', 'S', '3',
    '1', '3',
    '0', '0', '1', '1',
    4,
    30,
  };

  EXPECT_EQ(actual, expected);
}

TEST(request_packet, general_deserialize_test)
{
  auto test = []
    {
      std::string actual = { 0x11, 0x51, 0x04, (char)0xB0, 0x05 };

      core::request_packet<core::request_general> general;
      general.deserialize(actual);
    };

  EXPECT_NO_THROW(test());
}

TEST(response_packet, general_deserialize_test)
{
  auto test = []
    {
      std::string actual = {
        0x51,
        18,
        'H', 'Z', 'X', 'T',
        'B', 'B', 'S', '3',
        '1', '3',
        '0', '0', '1', '1',
        4,
        30,
      };

      core::response_packet<core::response_general> general;
      general.deserialize(actual);
    };

  EXPECT_NO_THROW(test());
}
