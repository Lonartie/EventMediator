#include "Event.h"
#include "System.h"

using namespace System;

Event&& Event::ofGroup(int group) && {
   this->group = group;
   return std::move(*this);
}

Event&& Event::ofCategory(int category) && {
   this->category = category;
   return std::move(*this);
}

Event&& Event::withPolicy(int policy) && {
   this->policy = policy;
   return std::move(*this);
}

void Event::send() && {
   System::instance().sendEvent(std::move(*this));
}
