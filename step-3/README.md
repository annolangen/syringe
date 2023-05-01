# Step 3

Adds factory function matching and conversion matching. This is might no longer be just for toy programs.

## How it works

std::decay_t<C> for various callable types, C, normalizes to a pointer
function template parameter that breaks out the return type and argument
parameter pack. Specifically, he result of `std::decay_t<C>` can be matched
against `T (*)(A...)`. This allows us to match factory functions with the
correct signature.

The provider search now checks for conversion from the provided type to the
requested type. I am not sure why I could not just use
`std::enable_if_t<std::is_convertible_v<R,T>>`, but SFINAE works different
than I expected. I ended up writing a helper that continues the search if
the conversion fails.

## Remaining Problems

- No Scope. Executes factory function every time a value is needed. We
  would like singleton and default scope. In C++ it might make sense to
  infer the scope from the factory function argument declaration. An
  argument of type `std::unique_ptr<T>` requires an exclusive
  value. Arguments of type `const T&` might be stored for the life of the
  injector and re-used
- Terrible error messages. It is templates
