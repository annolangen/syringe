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
template <class S, class K = _DefaultKey>
struct _Factory {
  std::function<S> fn;
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

template <class T, class K, class... A, class I>
T _provide(_Factory<T(A...), K> &f, I &injector) {
  return (f.fn)(_InjectorWrapper<A, I>::get(injector)...);
}

// _BindingType<T, K, P...>::type is the _Binding<T, K> or _Factory
// returning T with key K from the provider pack P.
template <class T, class K, class... P>
struct _BindingType {};

template <class T, class K, class... P>
struct _BindingType<T, K, _Binding<T, K>, P...> {
  using type = _Binding<T, K>;
};

template <class T, class K, class... A, class... P>
struct _BindingType<T, K, _Factory<T(A...), K>, P...> {
  using type = _Factory<T(A...), K>;
};

template <class T, class K, class H, class... P>
struct _BindingType<T, K, H, P...> {
  using type = typename _BindingType<T, K, P...>::type;
};

// Injector that provides values using a given set of providers.
template <class... P>
class _Injector {
 public:
  _Injector(std::tuple<P...> &providers) : providers_(providers) {}

  template <class T, class K = _DefaultKey>
  T get() {
    return _provide(
        std::get<typename _BindingType<T, K, P...>::type>(providers_), *this);
  }

 private:
  std::tuple<P...> providers_;
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
    return bind<_DefaultKey, T>(value);
  }

  template <class T, class K = _DefaultKey, class... A>
  _InjectorBuilder<_Factory<T(A...), K>, P...> addFactory(
      std::function<T(A...)> c) {
    return {
        std::make_tuple(_Factory<T(A...), K>{c}, std::get<P>(providers)...)};
  }

  _Injector<P...> build() { return {providers}; }
  std::tuple<P...> providers;
};

// Exported function to create an empty injector builder.
_InjectorBuilder<> InjectorBuilder() { return {}; }
