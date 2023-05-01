#include <iostream>
#include <string>

#include "syringe.h"

struct Greeting {
  std::string address;
  std::string greetee;
};

struct AddressKey {};

int main() {
  Greeting g = InjectorBuilder()
                   .bind<AddressKey>("Hello")
                   .bind("world")
                   .addFactory(+[](Keyed<std::string, AddressKey> address,
                                   std::string greetee) {
                     return Greeting{address.value, greetee};
                   })
                   .build()
                   .get<Greeting>();
  std::cout << g.address << " " << g.greetee << "!" << std::endl;
  return 0;
}
