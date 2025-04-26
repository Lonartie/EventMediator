#include "Policy.h"

#include "System.h"

using namespace System;

Policy::Policy(int policy) {
   this->policy = policy;
}

Policy&& Policy::orCall(std::function<void()>&& func) && {
   or_call.emplace_back(std::move(func));
   return std::move(*this);
}

Policy&& Policy::orSend(Event&& event) && {
   or_events.push_back(std::move(event));
   return std::move(*this);
}

bool Policy::call(const std::string& key, const std::vector<std::any>& arguments) {
   for (auto& validator: validators[key]) {
      if (validator(arguments)) {
         return true;
      }
   }
   return false;
}

bool Policy::handle(const std::vector<std::tuple<std::string, std::vector<std::any> > >& arguments) {
   for (auto&& [key, args]: arguments) {
      if (call(key, args)) {
         return true;
      }
   }

   for (auto&& func: std::move(or_call)) {
      func();
   }
   or_call.clear();

   for (auto&& event: std::move(or_events)) {
      System::instance().sendEvent(std::move(event));
   }
   or_events.clear();

   return false;
}

size_t Policy::finalize() && {
   return System::instance().registerPolicy(std::move(*this));
}
