#include "serial_handler.h"
#include "packet.h"
#include "packet_types.h"
#include "packet_builder.h"
#include "exceptions.h"


namespace core
{
  serial_handler::serial_handler(const std::string& port, profile& general, profile& config)
    : general_(general)
    , config_(config)
  {
    s_.event(core::serial::events::connected, core::bind(&serial_handler::on_connected, this));
    s_.event(core::serial::events::disconnected, core::bind(&serial_handler::on_disconnected, this));
    s_.event(core::serial::events::data_available, core::bind(&serial_handler::on_data_available, this));

    s_.connect(port);
    s_.param("1200,n,8,1");
  }


  serial_handler::~serial_handler()
  {}


  void serial_handler::poll()
  {
    while (s_.is_connected())
    {
      s_.poll();
    }
  }


  void serial_handler::on_connected()
  {
    TRACE_MESSAGE("on_connected->port: %s", s_.port().c_str());
  }


  void serial_handler::on_disconnected()
  {
    TRACE_MESSAGE("on_disconnected->port: %s", s_.port().c_str());
  }


  void serial_handler::on_data_available()
  {
    std::lock_guard<std::mutex> lock(mutex_); // Lock access to profiles

    const std::string& data = s_.peek();

    TRACE_MESSAGE("on_data_available->");
    TRACE_BINARY(data.data(), data.length());

    if (data.size() >= 2)
    {
      try
      {
        packet_commands command = static_cast<packet_commands>(data[0]);
        switch (command)
        {
          case packet_commands::read:
          {
            packet_types type = static_cast<packet_types>(data[1]);
            switch (type)
            {
              case packet_types::general:
              {
                // Valid request
                request_packet<request_general> request;
                request.deserialize(data);
                s_.flush_all();

                TRACE_MESSAGE("on_data_available->request received: read general");
                TRACE_BINARY(request.data(), request.length());

                // Send response
                response_packet<response_general> response;
                packet_builder::build(response, general_);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response sent: read general");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              case packet_types::basic:
              {
                // Basic config requested
                TRACE_MESSAGE("on_data_available->request received: read basic");
                TRACE_BINARY(data.data(), data.length());
                
                s_.flush_all();

                // Send response
                response_packet<response_basic> response;
                packet_builder::build(response, config_);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response sent: read basic");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              case packet_types::pedal:
              {
                // Pedal assist config requested
                TRACE_MESSAGE("on_data_available->request received: read pedal assist");
                TRACE_BINARY(data.data(), data.length());

                s_.flush_all();

                // Send response
                response_packet<response_pedal> response;
                packet_builder::build(response, config_);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response sent: read pedal assist");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              case packet_types::throttle:
              {
                // Throttle handle config requested
                TRACE_MESSAGE("on_data_available->request received: read throttle handle");
                TRACE_BINARY(data.data(), data.length());

                s_.flush_all();

                // Send response
                response_packet<response_throttle> response;
                packet_builder::build(response, config_);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response sent: read throttle handle");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              default:
              {
                TRACE_MESSAGE("on_data_available->read type not supported: 0x%X", static_cast<int>(type));
                s_.flush_all();

                // Should we respond back?
              }
            };
          }
          break;

          case packet_commands::write:
          {
            packet_types type = static_cast<packet_types>(data[1]);
            switch (type)
            {
              case packet_types::basic:
              {
                // Basic write requested
                request_packet<request_basic> request;
                request.deserialize(data);
                s_.flush_all();

                TRACE_MESSAGE("on_data_available->request received: write basic");
                TRACE_BINARY(request.data(), request.length());

                // Send response
                response_status_basic result = packet_builder::parse(request, config_);
                response_status_packet<response_status_basic> response(packet_types::basic, result);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response status sent: write basic");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              case packet_types::pedal:
              {
                // Pedal assist write requested
                request_packet<request_pedal> request;
                request.deserialize(data);
                s_.flush_all();

                TRACE_MESSAGE("on_data_available->request received: write pedal assist");
                TRACE_BINARY(request.data(), request.length());

                // Send response
                response_status_pedal result = packet_builder::parse(request, config_);
                response_status_packet<response_status_pedal> response(packet_types::pedal, result);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response status sent: write pedal assist");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              case packet_types::throttle:
              {
                // Throttle handle write requested
                request_packet<request_throttle> request;
                request.deserialize(data);
                s_.flush_all();

                TRACE_MESSAGE("on_data_available->request received: write throttle handle");
                TRACE_BINARY(request.data(), request.length());

                // Send response
                response_status_throttle result = packet_builder::parse(request, config_);
                response_status_packet<response_status_throttle> response(packet_types::throttle, result);
                s_.write(response.serialize());

                TRACE_MESSAGE("on_data_available->response status sent: write throttle handle");
                TRACE_BINARY(response.data(), response.length());
              }
              break;

              default:
              {
                TRACE_MESSAGE("on_data_available->rwrite type not supported: 0x%X", static_cast<int>(type));
                s_.flush_all();

                // Should we respond back?
              }
            }
          }
          break;

          default:
          {
            TRACE_MESSAGE("on_data_available->write not supported (%d)", static_cast<int>(command));
            s_.flush_all();

            // We should really respond back?!?
          }
        }
      }
      catch (...)
      {
        exception_handler();
      }
    }
  }


  std::mutex serial_handler::mutex_;
}
