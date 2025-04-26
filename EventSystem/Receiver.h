#pragma once
#include <functional>
#include <any>

namespace System {
   struct Receiver {
      Receiver&& ofGroup(int group) &&;
      Receiver&& ofCategory(int category) &&;
      Receiver&& withPolicy(int policy) &&;

      template<typename F>
      Receiver&& withAction(F&& action) &&;

      template<typename... Args>
      Receiver&& withAction(std::function<void(Args...)>&& action) &&;

      template<typename... Args>
      Receiver&& withData(std::string key, Args&&... args);

      size_t finalize() &&;

      void call(const std::vector<std::any>& args);

      int group = 0;
      int category = 0;
      int policy = 0;
      std::vector<std::function<void(const std::vector<std::any>&)> > actions;
      std::vector<std::tuple<std::string, std::vector<std::any> > > data;
   };

   template<typename F>
   Receiver&& Receiver::withAction(F&& action) && {
      return std::move(*this).withAction(std::function(std::forward<F>(action)));
   }

   template<typename... Args>
   Receiver&& Receiver::withAction(std::function<void(Args...)>&& action) && {
      actions.emplace_back([action = std::move(action)](const std::vector<std::any>& args) {
         if (args.size() != sizeof...(Args)) {
            // we can't call the action: wrong number of arguments
            return;
         }

         auto convert_args = []<size_t ... I>(std::index_sequence<I...>, const std::vector<std::any>& args) {
            return std::make_tuple(
               std::any_cast<std::decay_t<std::tuple_element_t<I, std::tuple<Args...> > > >(args[I])...
            );
         };

         try {
            std::tuple<Args...> tuple = convert_args(std::make_index_sequence<sizeof...(Args)>(), args);
            std::apply(action, tuple);
         } catch (const std::bad_any_cast& e) {
            // we can't call the action: wrong type of arguments
            return;
         }
      });

      return std::move(*this);
   }

   template<typename... Args>
   Receiver&& Receiver::withData(std::string key, Args&&... args) {
      data.emplace_back(std::make_tuple(key, std::vector{std::any(std::forward<Args>(args))...}));
      return std::move(*this);
   }
}
