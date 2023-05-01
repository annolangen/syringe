# Step 1

The simplest implementation that is just barely useful.

## How it works

The injector is a class with a tuple member of providers. The providers are
from two families `_Binding` and `_Factory`. The injector's template
function `get` returns a value of the template paramter type. The type
parameter is used to find the correct provider in the tuple.

Injection is a compile-time operation. All templates are instantiated at
compile time. The entire dependency graph is constructed at compile time.

## Problems

- No Keys. Cannot inject different values of a given type. Guice and Dagger
  have a key concept to tag providers. Boost.DI has a `named` annotation.
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
