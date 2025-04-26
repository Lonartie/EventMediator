#include "Receiver.h"
#include "System.h"

using namespace System;

Receiver&& Receiver::ofGroup(int group) && {
   this->group = group;
   return std::move(*this);
}

Receiver&& Receiver::ofCategory(int category) && {
   this->category = category;
   return std::move(*this);
}

Receiver&& Receiver::withPolicy(int policy) && {
   this->policy = policy;
   return std::move(*this);
}

size_t Receiver::finalize() && {
   return System::instance()
         .registerReceiver(std::move(*this));
}

void Receiver::call(const std::vector<std::any>& args) {
   for (auto& action: actions) {
      action(args);
   }
}
