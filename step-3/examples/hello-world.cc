#include <iostream>
#include <string>

#include "syringe.h"

struct Greeting {
  std::string address;
  std::string greetee;
};

struct AddressKey {};

Greeting MakeGreeting(Keyed<std::string, AddressKey> address,
                      std::string greetee) {
  return {address.value, greetee};
}

int main() {
  Greeting g = InjectorBuilder()
                   .bind<AddressKey>("Hello")
                   .bind("world")
                   .addFactory(MakeGreeting)
                   .build()
                   .get<Greeting>();
  std::cout << g.address << " " << g.greetee << "!" << std::endl;
  return 0;
}
