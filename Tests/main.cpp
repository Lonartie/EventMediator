#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "EventSystem/System.h"

TEST_SUITE("EventSystem") {
   TEST_CASE("Creating callable Receiver") {
      int received_int;
      std::string received_string;

      auto receiver = System::instance()
            .onReceiving()
            .ofGroup(System::Any)
            .ofCategory(System::Info)
            .withPolicy(System::Secure)
            .withAction([&](const int i) {
               received_int = i;
            })
            .withAction([&](const std::string& msg) {
               received_string = msg;
            })
            .withAction([&](const int i, const std::string& msg) {
               received_int = i;
               received_string = msg;
            });

      received_int = 0;
      received_string = "";
      receiver.call({42});
      DOCTEST_CHECK(received_int == 42);

      received_int = 0;
      received_string = "";
      receiver.call({std::string("Hello World!")});
      DOCTEST_CHECK(received_string == "Hello World!");

      received_int = 0;
      received_string = "";
      receiver.call({42, std::string("Hello World!")});
      DOCTEST_CHECK(received_int == 42);
      DOCTEST_CHECK(received_string == "Hello World!");

      received_int = 0;
      received_string = "";
      receiver.call({true});
      DOCTEST_CHECK(received_int == 0);
      DOCTEST_CHECK(received_string == "");
   }

   TEST_CASE("Creating Event") {
      auto event = System::Event()
            .ofGroup(System::Any)
            .ofCategory(System::Info)
            .withPolicy(System::Secure)
            .withData(43, std::string("Hello World!"));

      DOCTEST_CHECK(event.group == System::Any);
      DOCTEST_CHECK(event.category == System::Info);
      DOCTEST_CHECK(event.policy == System::Secure);
      DOCTEST_CHECK(event.data.size() == 2);
      DOCTEST_CHECK(std::any_cast<int>(event.data[0]) == 43);
      DOCTEST_CHECK(std::any_cast<std::string>(event.data[1]) == "Hello World!");
   }

   TEST_CASE("Creating validatable Policy") {
      auto policy = System::instance()
            .onForwarding(System::Secure)
            .validate("api_key", [](int key) {
               return key == 42;
            })
            .orCall([] { throw std::runtime_error("NO!"); });

      DOCTEST_CHECK_THROWS_WITH_AS(policy.handle({{"hello", {"world"}}}), "NO!", std::runtime_error);
      DOCTEST_CHECK_THROWS_WITH_AS(policy.handle({{"api_key", {"world"}}}), "NO!", std::runtime_error);
      DOCTEST_CHECK(policy.handle({{"api_key", {42}}}));
   }

   TEST_CASE("Scenario #1") {
      System::instance()
            .onForwarding(System::Secure)
            .validate("SecurityKey", [](std::string data) {
               return data == "1234";
            })
            .validate("SecurityKey", [](int data) {
               return data == 1234;
            })
            .orCall([] { throw std::runtime_error("Invalid Key!"); })
            .finalize();

      std::optional<std::variant<std::string, int>> result;

      System::instance()
            .onReceiving()
            .ofGroup(System::Data)
            // .ofCategory(System::Any)
            .withPolicy(System::Secure)
            // .withData("SecurityKey", std::string("1234"))
            .withData("SecurityKey", 1234)
            .withAction([&](std::string message) {
               result = message;
            })
            .withAction([&](int number) {
               result = number;
            })
            .finalize();

      System::Event()
            // .ofGroup(System::Any)
            .ofCategory(System::Data)
            .withPolicy(System::Secure)
            .withData(std::string("SUCCESS"))
            //.withData(42)
            .send();

      DOCTEST_CHECK(std::get<std::string>(*result) == "SUCCESS");
      result = {};

      System::Event()
            // .ofGroup(System::Any)
            .ofCategory(System::Data)
            .withPolicy(System::Secure)
            //.withData(std::string("SUCCESS"))
            .withData(42)
            .send();

      DOCTEST_CHECK(std::get<int>(*result) == 42);
      result = {};

      System::Event()
            .ofGroup(System::Info)
            .ofCategory(System::Info)
            .withPolicy(System::Secure)
            //.withData(std::string("SUCCESS"))
            .withData(42)
            .send();

      DOCTEST_CHECK(!result.has_value());
   }
}
