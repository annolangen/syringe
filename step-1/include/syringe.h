#pragma once

#include <functional>
// Internal names start with an underscore.

// A provider that returns a bound value.
template <class T>
struct _Binding {
  T value;
};

// templates _provide extract value of type T using a specific provider.
template <class T, class I>
T _provide(_Binding<T> &provider, I &) {
  return provider.value;
}

// A provider that returns a value created by a function
template <class C>
struct _Factory {
  std::function<C> callable;
};

template <class T, class I, class... A>
T _provide(_Factory<T(A...)> &f, I &injector) {
  return std::invoke(f.callable, injector.template get<A>()...);
}

// _BindingType<T, P...>::type is the _Binding<T> or _Factory
// returning T from the provider pack P.
template <class T, class... P>
struct _BindingType {};

template <class T, class... P>
struct _BindingType<T, _Binding<T>, P...> {
  using type = _Binding<T>;
};

template <class T, class... A, class... P>
struct _BindingType<T, _Factory<T(A...)>, P...> {
  using type = _Factory<T(A...)>;
};

template <class T, class H, class... P>
struct _BindingType<T, H, P...> {
  using type = typename _BindingType<T, P...>::type;
};

// Injector that provides values using a given set of providers.
template <class... P>
class _Injector {
 public:
  _Injector(std::tuple<P...> &providers) : providers_(providers) {}

  template <class T>
  T get() {
    return getWithProvider<T, typename _BindingType<T, P...>::type>();
  }

 private:
  std::tuple<P...> providers_;

  template <class T, class ProviderForT>
  T getWithProvider() {
    return _provide(std::get<ProviderForT>(providers_), *this);
  }
};

// Builder for an injector.
template <class... P>
struct _InjectorBuilder {
  template <class T>
  _InjectorBuilder<_Binding<T>, P...> bind(T value) {
    return {std::make_tuple(_Binding<T>{value}, std::get<P>(providers)...)};
  }

  template <class T, class... A>
  _InjectorBuilder<_Factory<T(A...)>, P...> addFactory(
      std::function<T(A...)> c) {
    return {std::make_tuple(_Factory<T(A...)>{c}, std::get<P>(providers)...)};
  }
  _Injector<P...> build() { return {providers}; }
  std::tuple<P...> providers;
};

// Exported function to create an empty injector builder.
_InjectorBuilder<> InjectorBuilder() { return {}; }
