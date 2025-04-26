#include "System.h"

using namespace System;

Sys& System::instance() {
   static Sys instance;
   return instance;
}

Receiver Sys::onReceiving() {
   return {};
}

Policy Sys::onForwarding(int policy) {
   return Policy(policy);
}

size_t Sys::registerReceiver(Receiver&& receiver) {
   receivers.emplace_back(std::move(receiver));
   return receivers.size() - 1;
}

size_t Sys::registerPolicy(Policy&& policy) {
   int pol = policy.policy;
   policies.emplace(pol, std::move(policy));
   return policies.size() - 1;
}

void Sys::sendEvent(Event&& event) {
   for (auto& receiver: receivers) {
      if (canAccept(receiver, event)) {
         receiver.call(event.data);
      }
   }
}

bool Sys::canAccept(const Receiver& receiver, const Event& event) {
   // check group
   if (receiver.group != 0 && event.group != 0 && receiver.group != event.group) {
      return false;
   }

   // check category
   if (receiver.category != 0 && event.category != 0 && receiver.category != event.category) {
      return false;
   }

   // check policy
   if (receiver.policy != 0 && event.policy != 0 && receiver.policy != event.policy) {
      return false;
   }

   // validate policy if any
   if (const int pol = receiver.policy != 0 ? receiver.policy : event.policy;
      pol != 0 && policies.contains(receiver.policy)) {
      auto& policy = policies.at(pol);
      if (!policy.handle(receiver.data)) {
         return false;
      }
   }

   return true;
}
