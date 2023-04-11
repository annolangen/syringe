#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
// #include "syringe.h"

struct Greeting {
  std::string address;
  std::string greetee;
};

Greeting MakeGreeting(const char* greetee) { return {"Hello", greetee}; }

namespace internal {

template <class T, class P, class I>
T provide(P& provider, I& injector);

template <class T>
struct Binding {
  T value;
};

template <class T, class I>
T provide(Binding<T>& provider, I&) {
  return provider.value;
}

template <class C>
struct Factory {
  C callable;
};

template <class T, class I, class... A>
T provide(Factory<std::function<T(A...)>>& f, I& injector) {
  return std::invoke(f.callable, injector.template get<A>()...);
}

template <class... T>
struct Bindings {};

template <class... F>
struct Factories {};

template <class B, class F>
struct Injector {};

template <class T, class B, class F>
struct BindingType {};

template <class T, class... B, class F>
struct BindingType<T, Bindings<Binding<T>, B...>, F> {
  using type = Binding<T>;
};

template <class T, class B1, class... B, class F>
struct BindingType<T, Bindings<B1, B...>, F> {
  using type = typename BindingType<T, Bindings<B...>, F>::type;
};

template <class T, class... A, class... F>
struct BindingType<T, Bindings<>,
                   Factories<Factory<std::function<T(A...)>>, F...>> {
  using type = Factory<std::function<T(A...)>>;
};

template <class T, class F1, class... F>
struct BindingType<T, Bindings<>, Factories<F1, F...>> {
  using type = typename BindingType<T, Bindings<>, Factories<F...>>::type;
};

template <class... B, class... F>
struct Injector<Bindings<B...>, Factories<F...>> : public B..., public F... {
  Injector(std::tuple<B...>& b, std::tuple<F...> f)
      : B(std::get<B>(b))..., F(std::get<F>(f))... {}
  template <class T>
  T get() {
    return provide(
        (typename BindingType<T, Bindings<B...>, Factories<F...>>::type&)*this,
        *this);
  }
};

template <class B, class F>
struct InjectorBuilder {};

template <class... B, class... F>
struct InjectorBuilder<Bindings<B...>, Factories<F...>> {
  template <class T>
  InjectorBuilder<Bindings<Binding<T>, B...>, Factories<F...>> bind(T value) {
    return {std::make_tuple(Binding<T>{value}, std::get<B>(binding)...),
            factory};
  }
  template <class T, class... A>
  InjectorBuilder<Bindings<B...>,
                  Factories<Factory<std::function<T(A...)>>, F...>>
  addFactory(std::function<T(A...)> c) {
    return {binding, std::make_tuple(Factory<std::function<T(A...)>>{c},
                                     std::get<F>(factory)...)};
  }
  Injector<Bindings<B...>, Factories<F...>> build() {
    return {binding, factory};
  }
  std::tuple<B...> binding;
  std::tuple<F...> factory;
};

}  // namespace internal

internal::InjectorBuilder<internal::Bindings<>, internal::Factories<>>
InjectorBuilder() {
  return {};
}

std::ostream& operator<<(std::ostream& out, const Greeting& g) {
  return out << g.address << " " << g.greetee << "!" << std::endl;
}

int main() {
  std::function<Greeting(const char*)> x = MakeGreeting;
  Greeting g =
      InjectorBuilder().bind("world").addFactory(x).build().get<Greeting>();
  std::cout << g;
  return 0;
}
