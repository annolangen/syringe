# Step 2

Adds key to bindings and a special struct `Keyed` to use for factory
function arguments.

## How it works

Each Provider now has an additional type parameter, which defaults to some
struct `_DefaultKey`. Specialization for `Keyed` parameters find the correct
provider. Note that the C++ standard does not allow partial specialization of function templates. So we have to use a wrapper struct template with a static member function to implement partial specialization.

## Remaining Problems

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
