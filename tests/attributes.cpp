#include "autoprop.hpp"
#include <string>
#include <vector>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace mpapierski;
using namespace std::string_literals;

class Person : public AutoProp<Person> {
public:
  AUTOPROP_BEGIN(Person);
  std::string AUTOPROP(first_name);
  std::string AUTOPROP(last_name);
  int AUTOPROP(age);
  AUTOPROP_END(Person);
};

TEST_CASE("Person has attributes") {
  Person person;
  person.first_name = "Jan";
  person.last_name = "Kowalski";
  person.age = 42;

  SECTION("Name of a class") { REQUIRE(Person::getClassName() == "Person"); }
  SECTION("Getting an attribute") {
    REQUIRE(person.get<std::string>("first_name") == "Jan");
    REQUIRE(person.get<std::string>("last_name") == "Kowalski");
    REQUIRE(person.get<int>("age") == 42);
  }
  SECTION("Setting an attribute") {
    REQUIRE(person.get<int>("age") == 42);
    person.set("age", 1234);
    REQUIRE(person.get<int>("age") == 1234);
  }
  SECTION("Total attributes") {
    static_assert(person.kTotalAttributes == 3);
    REQUIRE(person.kTotalAttributes == 3);
  }
  SECTION("All attributes") {
    std::vector<std::string> vec;
    person.forEach([&](auto &&key, auto &&value) { vec.emplace_back(key); });
    REQUIRE(vec == std::vector<std::string>{"first_name", "last_name", "age"});
  }
}