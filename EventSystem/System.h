#pragma once

#include <map>

#include "Event.h"
#include "Policy.h"
#include "Receiver.h"

namespace System {
   struct Sys;

   Sys& instance();

   static constexpr int Any = 0;
   static constexpr int Data = 1;
   static constexpr int Info = 100;
   static constexpr int Secure = 200;

   struct Sys {
   public:
      Receiver onReceiving();
      Policy onForwarding(int policy);

   public:
      size_t registerReceiver(Receiver&& receiver);
      size_t registerPolicy(Policy&& policy);
      void sendEvent(Event&& event);

   private:
      friend Sys& System::instance();
      Sys() = default;

      bool canAccept(const Receiver& receiver, const Event& event);

      std::vector<Receiver> receivers;
      std::map<size_t, Policy> policies;
   };
}
