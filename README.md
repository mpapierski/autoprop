# autoprop
C++17 library for compile-time reflection of class properties.

# Problem

At times you need to manage a large struct with many attributes, and perhaps associate their names to them.

Possible use cases:

- Maintaining a large SQL query to read and write class attributes from database.
- Serialization of class attributes.

# Example code

```cpp
class Person : public AutoProp<Person> {
public:
  AUTOPROP_BEGIN(Person);
  std::string AUTOPROP(first_name);
  std::string AUTOPROP(last_name);
  int AUTOPROP(age);
  AUTOPROP_END(Person);
};
```

`Person` is still the same class, just augmented with additional methods. There is literally zero runtime overhead to iterate over list of attributes to get their names and values.

# API

```cpp
AUTOPROP_BEGIN(Class);
```

A prolog which you have to write once at the beggining of a class presumably before first `AUTOPROP`.

```cpp
T AUTOPROP(name);
```

Defines an attribute `name` of type `T` and registers it into classes internal list of attributes.

```cpp
AUTOPROP_END(Class);
```

Defines an epilog of the `AutoProp<>` derivative class.

## Inherited from AutoProp<T>

### forEach

Example:

```cpp
person.forEach([](auto&& key, const auto& value) {
});
```

Calls functor with arguments `(key, value)` where `key` is attribute's name, and `value` is const reference to its value.

### get<T>(name)

Currently `get(name)` needs a type of the attribute to be known. If the attribute is not found by its `name`, the behaviour is undefined.

### set(name, value)

Set attribute value by its name. If specified attribute doesn't exists the behaviour is undefined.

## getAttributeList()

Returns a list of attributes. It should be considered an implementation detail. Returns a tree of nodes instead of something linear like `std::tuple`. This is just for performance reasons.

Example:

```cpp
Node<
  Node<
    Node<
      Empty, 
      Attribute<Person, std::string>
    >, 
    Attribute<Person, std::string>
    >, 
  Attribute<Person, int>
  >
>
```

# Author

- [Michał Papierski](michal@papierski.net)
