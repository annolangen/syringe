#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

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
  using Fn = std::function<S>;
  Fn fn;
};

// Helper to convert a callable type to a _Factory type instance
template <class, class>
struct _FactoryFor {};
template <class K, class T, class... A>
struct _FactoryFor<K, T (*)(A...)> {
  using type = _Factory<T(A...), K>;
};
template <class K, class C>
using _FactoryFor_t = typename _FactoryFor<K, std::decay_t<C>>::type;

// Standard C++ requires a wrapper, because only classes, not
// functions, allow partial specialization.
template <class T, class I>
struct _InjectorWrapper {
  static T get(I &injector) { return injector.template get<T, _DefaultKey>(); }
};
template <class T, class K, class I>
struct _InjectorWrapper<Keyed<T, K>, I> {
  static Keyed<T, K> get(I &injector) { return injector.template get<T, K>(); }
};

template <class T, class K, class... A, class I>
T _provide(_Factory<T(A...), K> &f, I &injector) {
  return (f.fn)(_InjectorWrapper<A, I>::get(injector)...);
}

// _BindingType<R, K, P...>::type is the _Binding<T, K> or _Factory
// returning T with key K from the provider pack P, if T is convertible to R.
template <class R, class K, class... P>
struct _BindingType {};

// Helper to select the first _Binding or _Factory returning T with key K that
// is convertible to R.
template <bool isConvertible, class R, class K, class... P>
struct _BindingTypeHelper {};

template <class R, class K, class H, class... P>
struct _BindingTypeHelper<true, R, K, H, P...> {
  using type = H;
};

template <class R, class K, class H, class... P>
struct _BindingTypeHelper<false, R, K, H, P...> {
  using type = typename _BindingType<R, K, P...>::type;
};

template <bool isConvertible, class R, class K, class... P>
using _BindingTypeHelper_t =
    typename _BindingTypeHelper<isConvertible, R, K, P...>::type;

template <class R, class K, class T, class... P>
struct _BindingType<R, K, _Binding<T, K>, P...> {
  using type = _BindingTypeHelper_t<std::is_convertible_v<T, R>, R, K,
                                    _Binding<T, K>, P...>;
};

template <class R, class T, class... A, class K, class... P>
struct _BindingType<R, K, _Factory<T(A...), K>, P...> {
  using type = _BindingTypeHelper_t<std::is_convertible_v<T, R>, R, K,
                                    _Factory<T(A...), K>, P...>;
};

template <class R, class K, class H, class... P>
struct _BindingType<R, K, H, P...> {
  using type = typename _BindingType<R, K, P...>::type;
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
    return {std::make_tuple(_Binding<T, _DefaultKey>{value},
                            std::get<P>(providers)...)};
  }

  template <class C, class F = _FactoryFor_t<_DefaultKey, C>>
  _InjectorBuilder<F, P...> addFactory(C c) {
    F factory = {typename F::Fn(c)};
    return {std::make_tuple(factory, std::get<P>(providers)...)};
  }

  _Injector<P...> build() { return {providers}; }
  std::tuple<P...> providers;
};

// Exported function to create an empty injector builder.
_InjectorBuilder<> InjectorBuilder() { return {}; }
