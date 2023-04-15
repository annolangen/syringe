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

Greeting MakeGreeting(const char *greetee) { return {"Hello", greetee}; }

template <class T, class P, class I>
T _provide(P &provider, I &injector);

template <class T>
struct _Binding {
  T value;
};

template <class T, class I>
T _provide(_Binding<T> &provider, I &) {
  return provider.value;
}

template <class C>
struct _Factory {
  C callable;
};

template <class T, class I, class... A>
T _provide(_Factory<std::function<T(A...)>> &f, I &injector) {
  return std::invoke(f.callable, injector.template get<A>()...);
}

template <class T, class... P>
struct _BindingType {};

template <class T, class... P>
struct _BindingType<T, _Binding<T>, P...> {
  using type = _Binding<T>;
};

template <class T, class... A, class... P>
struct _BindingType<T, _Factory<std::function<T(A...)>>, P...> {
  using type = _Factory<std::function<T(A...)>>;
};

template <class T, class H, class... P>
struct _BindingType<T, H, P...> {
  using type = typename _BindingType<T, P...>::type;
};

template <class... P>
struct _Injector : public P... {
  _Injector(std::tuple<P...> &providers) : P(std::get<P>(providers))... {}
  template <class T>
  T get() {
    return _provide((typename _BindingType<T, P...>::type &)*this, *this);
  }
};

template <class... P>
struct _InjectorBuilder {
  template <class T>
  _InjectorBuilder<_Binding<T>, P...> bind(T value) {
    return {std::make_tuple(_Binding<T>{value}, std::get<P>(providers)...)};
  }
  template <class T, class... A>
  _InjectorBuilder<_Factory<std::function<T(A...)>>, P...> addFactory(
      std::function<T(A...)> c) {
    return {std::make_tuple(_Factory<std::function<T(A...)>>{c},
                            std::get<P>(providers)...)};
  }
  _Injector<P...> build() { return {providers}; }
  std::tuple<P...> providers;
};

_InjectorBuilder<> InjectorBuilder() { return {}; }

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
