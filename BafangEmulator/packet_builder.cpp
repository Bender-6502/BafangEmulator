#include "packet_builder.h"
#include "trace.h"
#include <algorithm>


namespace core
{
  namespace packet_builder
  {
    void build(response_packet<response_general>& response, profile& general)
    {
      response.type = packet_types::general;

      memcpy(response.payload.manufacturer, general.find("General", "MANUFACTURER", "HZXT").c_str(), sizeof(response.payload.manufacturer));
      memcpy(response.payload.model, general.find("General", "MODEL", "BBS3").c_str(), sizeof(response.payload.model));
      response.payload.hardware_version = general.find("General", "HARDWARD", '31');
      response.payload.firmware_version = general.find("General", "FIRMWARD", '1100');
      response.payload.nominal_voltage = general.find("General", "VOLTS", 4);
      response.payload.limit_control = general.find("General", "LIMIT", 30);

      if (!general.exists())
        general.save();
    }

    void build(response_packet<response_basic>& response, profile& config)
    {
      response.type = packet_types::basic;

      response.payload.low_battery     = config.find("Basic", "LBP", 20);
      response.payload.current_limit   = config.find("Basic", "LC", 25);

      response.payload.assist0_current = config.find("Basic", "ALC0", 10);
      response.payload.assist1_current = config.find("Basic", "ALC1", 20);
      response.payload.assist2_current = config.find("Basic", "ALC2", 30);
      response.payload.assist3_current = config.find("Basic", "ALC3", 40);
      response.payload.assist4_current = config.find("Basic", "ALC4", 50);
      response.payload.assist5_current = config.find("Basic", "ALC5", 60);
      response.payload.assist6_current = config.find("Basic", "ALC6", 70);
      response.payload.assist7_current = config.find("Basic", "ALC7", 80);
      response.payload.assist8_current = config.find("Basic", "ALC8", 90);
      response.payload.assist9_current = config.find("Basic", "ALC9", 100);

      response.payload.assist0_speed   = config.find("Basic", "ALBP0", 10);
      response.payload.assist1_speed   = config.find("Basic", "ALBP1", 20);
      response.payload.assist2_speed   = config.find("Basic", "ALBP2", 30);
      response.payload.assist3_speed   = config.find("Basic", "ALBP3", 40);
      response.payload.assist4_speed   = config.find("Basic", "ALBP4", 50);
      response.payload.assist5_speed   = config.find("Basic", "ALBP5", 60);
      response.payload.assist6_speed   = config.find("Basic", "ALBP6", 70);
      response.payload.assist7_speed   = config.find("Basic", "ALBP7", 80);
      response.payload.assist8_speed   = config.find("Basic", "ALBP8", 90);
      response.payload.assist9_speed   = config.find("Basic", "ALBP9", 100);

      int wheels[] = { 16<<1,17<<1,18<<1,19<<1,20<<1,21<<1,22<<1,23<<1,24<<1,25<<1,26<<1,27<<1,27<<1|1,28<<1,29<<1,30<<1 };
      response.payload.wheel_size      = wheels[config.find("Basic", "WD", 10)];

      response.payload.speed_meter     = (config.find("Basic", "SMM", 0) * 64) + config.find("Basic", "SMS", 1);
    }

    void build(response_packet<response_pedal>& response, profile& config)
    {
      response.type = packet_types::pedal;

      response.payload.sensor_type     = config.find("Pedal Assist", "PT", 3);
      response.payload.assist_level    = config.find("Pedal Assist", "DA", 0);
      response.payload.speed_limit     = config.find("Pedal Assist", "SL", 0);

      response.payload.start_current   = config.find("Pedal Assist", "SC", 20);
      response.payload.slow_start_mode = config.find("Pedal Assist", "SSM", 5);
      response.payload.start_deg       = config.find("Pedal Assist", "SDN", 20);
      response.payload.work_mode       = config.find("Pedal Assist", "WM", 0);

      response.payload.stop_delay      = config.find("Pedal Assist", "TS", 25);
      response.payload.current_decay   = config.find("Pedal Assist", "CD", 8);
      response.payload.stop_decay      = config.find("Pedal Assist", "SD", 20);
      response.payload.keep_current    = config.find("Pedal Assist", "KC", 20);
    }

    void build(response_packet<response_throttle>& response, profile& config)
    {
      response.type = packet_types::throttle;

      response.payload.start_volt    = config.find("Throttle Handle", "SV", 11);
      response.payload.end_volt      = config.find("Throttle Handle", "EV", 35);
      response.payload.mode          = config.find("Throttle Handle", "MODE", 0);
      response.payload.assist_level  = config.find("Throttle Handle", "DA", 4);
      response.payload.speed_limit   = config.find("Throttle Handle", "SL", 3);
      response.payload.start_current = config.find("Throttle Handle", "SC", 20);
    }

    response_status_basic parse(const request_packet<request_basic>& request, profile& config)
    {
      profile temp = config; // Make a copy of the config

      if (request.payload.low_battery > 55 && request.payload.low_battery < 18)
        return response_status_basic::low_battery;
      else
        temp.add("Basic", "LBP",  request.payload.low_battery);

      if (request.payload.current_limit == 0)
        return response_status_basic::current_limit;
      else
        temp.add("Basic", "LC",   request.payload.current_limit);

      if (request.payload.assist0_current > 100 || request.payload.assist0_current == 0)
        return response_status_basic::assist0_current;
      else
        temp.add("Basic", "ALC0", request.payload.assist0_current);

      if (request.payload.assist1_current > 100 || request.payload.assist1_current == 0)
        return response_status_basic::assist1_current;
      else
        temp.add("Basic", "ALC1", request.payload.assist1_current);

      if (request.payload.assist2_current > 100 || request.payload.assist2_current == 0)
        return response_status_basic::assist2_current;
      else
        temp.add("Basic", "ALC2", request.payload.assist2_current);
 
      if (request.payload.assist3_current > 100 || request.payload.assist3_current == 0)
        return response_status_basic::assist3_current;
      else
        temp.add("Basic", "ALC3", request.payload.assist3_current);
 
      if (request.payload.assist4_current > 100 || request.payload.assist4_current == 0)
        return response_status_basic::assist4_current;
      else
        temp.add("Basic", "ALC4", request.payload.assist4_current);

      if (request.payload.assist5_current > 100 || request.payload.assist5_current == 0)
        return response_status_basic::assist5_current;
      else
        temp.add("Basic", "ALC5", request.payload.assist5_current);

      if (request.payload.assist6_current > 100 || request.payload.assist6_current == 0)
        return response_status_basic::assist6_current;
      else
        temp.add("Basic", "ALC6", request.payload.assist6_current);

      if (request.payload.assist7_current > 100 || request.payload.assist7_current == 0)
        return response_status_basic::assist7_current;
      else
        temp.add("Basic", "ALC7", request.payload.assist7_current);
 
      if (request.payload.assist8_current > 100 || request.payload.assist8_current == 0)
        return response_status_basic::assist8_current;
      else
        temp.add("Basic", "ALC8", request.payload.assist8_current);
 
      if (request.payload.assist9_current > 100 || request.payload.assist9_current == 0)
        return response_status_basic::assist9_current;
      else
        temp.add("Basic", "ALC9", request.payload.assist9_current);

      if (request.payload.assist0_speed > 100 || request.payload.assist0_speed == 0)
        return response_status_basic::assist0_speed;
      else
        temp.add("Basic", "ALBP0", request.payload.assist0_speed);

      if (request.payload.assist1_speed > 100 || request.payload.assist1_speed == 0)
        return response_status_basic::assist1_speed;
      else
        temp.add("Basic", "ALBP1", request.payload.assist1_speed);
 
      if (request.payload.assist2_speed > 100 || request.payload.assist2_speed == 0)
        return response_status_basic::assist2_speed;
      else
        temp.add("Basic", "ALBP2", request.payload.assist2_speed);
 
      if (request.payload.assist3_speed > 100 || request.payload.assist3_speed == 0)
        return response_status_basic::assist3_speed;
      else
        temp.add("Basic", "ALBP3", request.payload.assist3_speed);

      if (request.payload.assist4_speed > 100 || request.payload.assist4_speed == 0)
        return response_status_basic::assist4_speed;
      else
        temp.add("Basic", "ALBP4", request.payload.assist4_speed);

      if (request.payload.assist5_speed > 100 || request.payload.assist5_speed == 0)
        return response_status_basic::assist5_speed;
      else
        temp.add("Basic", "ALBP5", request.payload.assist5_speed);
 
      if (request.payload.assist6_speed > 100 || request.payload.assist6_speed == 0)
        return response_status_basic::assist6_speed;
      else
        temp.add("Basic", "ALBP6", request.payload.assist6_speed);

      if (request.payload.assist7_speed > 100 || request.payload.assist7_speed == 0)
        return response_status_basic::assist7_speed;
      else
        temp.add("Basic", "ALBP7", request.payload.assist7_speed);

      if (request.payload.assist8_speed > 100 || request.payload.assist8_speed == 0)
        return response_status_basic::assist8_speed;
      else
        temp.add("Basic", "ALBP8", request.payload.assist8_speed);

      if (request.payload.assist9_speed > 100 || request.payload.assist9_speed == 0)
        return response_status_basic::assist9_speed;
      else
        temp.add("Basic", "ALBP9", request.payload.assist9_speed);

      auto find = [](uint8_t ws)
      {
        int wheels[] = { 16 << 1,17 << 1,18 << 1,19 << 1,20 << 1,21 << 1,22 << 1,23 << 1,24 << 1,25 << 1,26 * 2,27 << 1,27 << 1 | 1,28 << 1,29 << 1,30 << 1 };
        for (int i = 0; i < sizeof(wheels) / sizeof(int); i++)
        {
          if (ws == wheels[i])
          {
            return i;
          }
        }
        return -1;
      };

      auto found = find(request.payload.wheel_size);
      if (found == -1)
          return response_status_basic::wheel_size;
        else
          temp.add("Basic", "WD", found);

      TRACE_MESSAGE("WS %d", request.payload.wheel_size);
      TRACE_MESSAGE("WS %d", found);

      if (request.payload.speed_meter / 64 > 2)
        return response_status_basic::speed_meter;
      else
        temp.add("Basic", "SMM", request.payload.speed_meter / 64);

      if (request.payload.speed_meter % 64 > 36 || request.payload.speed_meter % 64 == 0)
        return response_status_basic::speed_meter;
      else
        temp.add("Basic", "SMS", request.payload.speed_meter % 64);

      config = temp; // Store changes to config
      config.save();

      return response_status_basic::success;
    }

    response_status_pedal parse(const request_packet<request_pedal>& request, profile& config)
    {
      profile temp = config; // Make a copy of the config

      if (request.payload.sensor_type > 4)
        return response_status_pedal::sensor_type;
      else
        temp.add("Pedal Assist", "PT", request.payload.sensor_type);

      if (request.payload.assist_level > 9 && request.payload.assist_level < 255)
        return response_status_pedal::assist_level;
      else
        temp.add("Pedal Assist", "DA", request.payload.assist_level);

      if (request.payload.speed_limit > 99 && request.payload.speed_limit < 255)
        return response_status_pedal::speed_limit;
      else
        temp.add("Pedal Assist", "SL", request.payload.speed_limit);

      if (request.payload.start_current > 20 || request.payload.start_current == 0)
        return response_status_pedal::start_current;
      else
        temp.add("Pedal Assist", "SC", request.payload.start_current);

      if (request.payload.slow_start_mode > 8 || request.payload.slow_start_mode == 0)
        return response_status_pedal::slow_start_mode;
      else
        temp.add("Pedal Assist", "SSM", request.payload.slow_start_mode);

      if (request.payload.start_deg > 100 || request.payload.start_deg == 0)
        return response_status_pedal::start_deg;
      else
        temp.add("Pedal Assist", "SDN", request.payload.start_deg);

      if (request.payload.work_mode > 80 && request.payload.work_mode < 10 && request.payload.work_mode != 255)
        return response_status_pedal::work_mode;
      else
        temp.add("Pedal Assist", "WM", request.payload.work_mode);

      temp.add("Pedal Assist", "TS", request.payload.stop_delay);

      if (request.payload.current_decay > 8 || request.payload.current_decay == 0)
        return response_status_pedal::current_decay;
      else
        temp.add("Pedal Assist", "CD", request.payload.current_decay);

      temp.add("Pedal Assist", "SD", request.payload.stop_decay);

      if (request.payload.keep_current > 100 || request.payload.keep_current == 0)
        return response_status_pedal::keep_current;
      else
        temp.add("Pedal Assist", "KC", request.payload.keep_current);

      config = temp; // Store changes to config
      config.save();

      return response_status_pedal::success;
    }

    response_status_throttle parse(const request_packet<request_throttle>& request, profile& config)
    {
      profile temp = config; // Make a copy of the config

      if (request.payload.start_volt > 50)
        return response_status_throttle::start_volt;
      else
        temp.add("Throttle Handle", "SV", request.payload.start_volt);

      if (request.payload.end_volt > 50)
        return response_status_throttle::end_volt;
      else
        temp.add("Throttle Handle", "EV", request.payload.end_volt);

      if (request.payload.mode > 1)
        return response_status_throttle::mode;
      else
        temp.add("Throttle Handle", "MODE", request.payload.mode);

      if (request.payload.assist_level > 9 && request.payload.assist_level < 255)
        return response_status_throttle::assist_level;
      else
        temp.add("Throttle Handle", "DA", request.payload.assist_level);

      if (request.payload.speed_limit > 99 && request.payload.speed_limit < 255)
        return response_status_throttle::speed_limit;
      else
        temp.add("Throttle Handle", "SL", request.payload.speed_limit);
 
      if (request.payload.start_current > 100 || request.payload.start_current == 0)
        return response_status_throttle::start_current;
      else
        temp.add("Throttle Handle", "SC", request.payload.start_current);

      config = temp; // Store changes to config
      config.save();

      return response_status_throttle::success;
    }
  }
}
