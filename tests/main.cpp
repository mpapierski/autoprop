#include "autoprop.hpp"

using namespace mpapierski;

class Test : public AutoProp<Test> {
public:
  AUTOPROP_BEGIN(Test);
  std::string AUTOPROP(first_name);
  std::string AUTOPROP(last_name);
  int AUTOPROP(age);
  AUTOPROP_END(Test);
};

int main() {
  Test test1;
  test1.first_name = "Jan";
  test1.last_name = "Kowalski";
  test1.age = 42;
  test1.set("age", 43);

  test1.forEach([](auto &&name, auto &&value) {
    std::cout << name << '\t' << value << '\n';
  });
}
