#pragma once
#include "profile.h"
#include "packet.h"
#include "packet_general.h"
#include "packet_basic.h"
#include "packet_pedal.h"
#include "packet_throttle.h"


namespace core
{
  namespace packet_builder
  {
    void build(response_packet<response_general>& response, profile& general);
    void build(response_packet<response_basic>& response, profile& config);
    void build(response_packet<response_pedal>& response, profile& config);
    void build(response_packet<response_throttle>& response, profile& config);

    response_status_basic parse(const request_packet<request_basic>& request, profile& config);
    response_status_pedal parse(const request_packet<request_pedal>& request, profile& config);
    response_status_throttle parse(const request_packet<request_throttle>& request, profile& config);
  }
}
