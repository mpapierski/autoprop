#include "autoprop.hpp"
#include "json.hpp"
#include <iostream>

using namespace mpapierski;
using json = nlohmann::json;

class Person : public AutoProp<Person> {
public:
  AUTOPROP_BEGIN(Person);
  std::string AUTOPROP(first_name);
  std::string AUTOPROP(last_name);
  int AUTOPROP(age);
  AUTOPROP_END(Person);
};

template <typename T> json to_json(AutoProp<T> &obj) {
  json j;
  obj.forEach([&](auto &&key, auto &&value) { j[key] = value; });
  return j;
}

template <typename T> T from_json(json j) {
  T obj;
  obj.forEach([&](auto &&key, auto &&value) { obj.set(key, j[key]); });
  return obj;
}

int main() {
  Person person;
  person.first_name = "Jan";
  person.last_name = "Kowalski";
  person.age = 30;
  auto serialized = to_json(person);
  std::cout << serialized << '\n';
  Person person2 = from_json<Person>(serialized);
  std::cout << person2.first_name << "," << person2.last_name << ","
            << person2.age << '\n';
}