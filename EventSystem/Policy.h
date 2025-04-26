#pragma once
#include <any>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Event.h"

namespace System {
   struct Policy {
      explicit Policy(int policy);

      template<typename F>
      Policy&& validate(std::string key, F&& validator) &&;

      template<typename... Args>
      Policy&& validate(std::string key, std::function<bool(Args...)> validator) &&;

      Policy&& orCall(std::function<void()>&& func) &&;
      Policy&& orSend(Event&& event) &&;

      size_t finalize() &&;

      bool call(const std::string& key, const std::vector<std::any>& arguments);
      bool handle(const std::vector<std::tuple<std::string, std::vector<std::any>>>& arguments);

      int policy = 0;
      std::unordered_map<std::string, std::vector<std::function<bool(const std::vector<std::any>&)>>> validators;
      std::vector<std::function<void()>> or_call;
      std::vector<Event> or_events;
   };

   template<typename F>
   Policy&& Policy::validate(std::string key, F&& validator) && {
      return std::move(*this).validate(std::move(key), std::function(std::forward<F>(validator)));
   }

   template<typename... Args>
   Policy&& Policy::validate(std::string key, std::function<bool(Args...)> validator) && {
      validators[std::move(key)].emplace_back([validator = std::move(validator)](const std::vector<std::any>& args) {
         if (args.size() != sizeof...(Args)) {
            // we can't call the action: wrong number of arguments
            return false;
         }

         auto convert_args = []<size_t ... I>(std::index_sequence<I...>, const std::vector<std::any>& args) {
            return std::make_tuple(
               std::any_cast<std::decay_t<std::tuple_element_t<I, std::tuple<Args...> > > >(args[I])...
            );
         };

         try {
            std::tuple<Args...> tuple = convert_args(std::make_index_sequence<sizeof...(Args)>(), args);
            return std::apply(validator, tuple);
         } catch (const std::bad_any_cast& e) {
            // we can't call the action: wrong type of arguments
            return false;
         }
      });
      return std::move(*this);
   }
}
