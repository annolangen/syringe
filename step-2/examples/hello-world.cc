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

struct AddressKey {};
struct GreeteeKey {};

Greeting MakeGreeting(Keyed<const char *, AddressKey> address,
                      Keyed<const char *, GreeteeKey> greetee) {
  return {address.value, greetee.value};
}

std::ostream &operator<<(std::ostream &out, const Greeting &g) {
  return out << g.address << " " << g.greetee << "!" << std::endl;
}

int main() {
  std::function<Greeting(Keyed<const char *, AddressKey> address,
                         Keyed<const char *, GreeteeKey> greetee)>
      x = MakeGreeting;
  Greeting g = InjectorBuilder()
                   .bind<AddressKey>("Hello")
                   .bind<GreeteeKey>("world")
                   .addFactory(x)
                   .build()
                   .get<Greeting>();
  std::cout << g;
  return 0;
}
