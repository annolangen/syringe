#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

#include "syringe.h"

struct Greeting {
  std::string address;
  std::string greetee;
};

Greeting MakeGreeting(const char *greetee) { return {"Hello", greetee}; }

std::ostream &operator<<(std::ostream &out, const Greeting &g) {
  return out << g.address << " " << g.greetee << "!" << std::endl;
}

int main() {
  std::function<Greeting(const char *)> x = MakeGreeting;
  Greeting g =
      InjectorBuilder().bind("world").addFactory(x).build().get<Greeting>();
  std::cout << g;
  return 0;
}
