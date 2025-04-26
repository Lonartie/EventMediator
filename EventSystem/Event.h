#pragma once
#include <any>
#include <vector>

namespace System {
   class Event {
   public:
      Event&& ofGroup(int group) &&;
      Event&& ofCategory(int category) &&;
      Event&& withPolicy(int policy) &&;

      template<typename... Args>
      Event&& withData(Args&&... args) &&;

      void send() &&;

      int group = 0;
      int category = 0;
      int policy = 0;
      std::vector<std::any> data;
   };

   template<typename... Args> Event&& Event::withData(Args&&... args) && {
      data.clear();
      data.reserve(sizeof...(Args));
      (data.emplace_back(std::forward<Args>(args)), ...);
      return std::move(*this);
   }
}
