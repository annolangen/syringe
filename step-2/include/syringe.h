#pragma once

#include <functional>

// Wrapper class for factory function parameters to accecpt a value associated
// with a specific key type.
template <class T, class K>
struct Keyed {
  /*implicit*/ Keyed(T &&v) : value(std::forward<T>(v)) {}
  T value;
};

// Internal names start with an underscore.

struct _DefaultKey {};

// A provider that returns a bound value.
template <class T, class K = _DefaultKey>
struct _Binding {
  T value;
};

template <class T, class K, class I>
T _provide(_Binding<T, K> &provider, I &) {
  return provider.value;
}

// A provider that returns a value created by a function
template <class C, class K = _DefaultKey>
struct _Factory {
  C callable;
};

// Standard C++ requires a wrapper, because only classes, not
// functions, allow partial specialization.
template <class T, class I>
struct _InjectorWrapper {
  static T get(I &injector) { return injector.template get<T>(); }
};
template <class T, class K, class I>
struct _InjectorWrapper<Keyed<T, K>, I> {
  static Keyed<T, K> get(I &injector) { return injector.template get<T, K>(); }
};

template <class T, class K, class I, class... A>
T _provide(_Factory<std::function<T(A...)>, K> &f, I &injector) {
  return std::invoke(f.callable, _InjectorWrapper<A, I>::get(injector)...);
}

// Holder of the provider type for a given type.
template <class T, class K, class... P>
struct _BindingType {};

template <class T, class K, class... P>
struct _BindingType<T, K, _Binding<T, K>, P...> {
  using type = _Binding<T, K>;
};

template <class T, class K, class... A, class... P>
struct _BindingType<T, K, _Factory<std::function<T(A...)>, K>, P...> {
  using type = _Factory<std::function<T(A...)>, K>;
};

template <class T, class K, class H, class... P>
struct _BindingType<T, K, H, P...> {
  using type = typename _BindingType<T, K, P...>::type;
};

// Injector that provides values using a given set of providers.
template <class... P>
struct _Injector : public P... {
  _Injector(std::tuple<P...> &providers) : P(std::get<P>(providers))... {}

  template <class T, class K = _DefaultKey>
  T get() {
    return _provide((typename _BindingType<T, K, P...>::type &)*this, *this);
  }
};

// Builder for an injector.
template <class... P>
struct _InjectorBuilder {
  template <class K, class T>
  _InjectorBuilder<_Binding<T, K>, P...> bind(T value) {
    return {std::make_tuple(_Binding<T, K>{value}, std::get<P>(providers)...)};
  }

  template <class T>
  _InjectorBuilder<_Binding<T, _DefaultKey>, P...> bind(T value) {
    return {std::make_tuple(_Binding<T, _DefaultKey>{value},
                            std::get<P>(providers)...)};
  }

  template <class T, class K = _DefaultKey, class... A>
  _InjectorBuilder<_Factory<std::function<T(A...)>, K>, P...> addFactory(
      std::function<T(A...)> c) {
    return {std::make_tuple(_Factory<std::function<T(A...)>, K>{c},
                            std::get<P>(providers)...)};
  }
  _Injector<P...> build() { return {providers}; }
  std::tuple<P...> providers;
};

// Exported function to create an empty injector builder.
_InjectorBuilder<> InjectorBuilder() { return {}; }
