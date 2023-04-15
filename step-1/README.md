# Step 1

The simplest implementation that is just barely useful.

## How it works

Injection is a compile-time operation. The injector is a struct that
derives from multiple provider structs. They are from two families
`_Binding` and `_Factory`. The injector's `get` implementation is a
template function that generates code to build the entire dependency
graph.

## Problems

- No Keys. Cannot inject different values of a given type. Guice and Dagger
  have a key concept to tag providers
- No Scope. Executes factory function every time a value is needed. We
  would like singleton and default scope. In C++ it might make sense to
  infer the scope from the factory function argument declaration. An
  argument of type `std::unique_ptr<T>` requires an exclusive
  value. Arguments of type `const T&` might be stored for the life of the
  injector and re-used
- Literal matching. A `const char*` provider should work for factory
  argument of types like `std::string` and `std::string_view` to which the
  value can be implicitly converted
- No factory function matching. The factory function must be a value of
  type `std::function`. We want references to functions and lambdas to work
- Terrible error messages. It is templates
