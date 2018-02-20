#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace mpapierski {

template <typename T> struct Value { T value; };

template <typename Head, typename Tail> struct Node {
  Head head;
  Tail tail;
};

template <typename T, typename R> struct Attribute {
  const char *name;
  R(T::*pointer);
};

template <typename T, typename R>
constexpr Attribute<T, R> makeAttribute(const char *name, R(T::*pointer)) {
  return {name, pointer};
}

template <typename Head, typename Tail>
constexpr Node<Head, Tail> makeNode(Head &&head, Tail &&tail) {
  return {std::forward<Head>(head), std::forward<Tail>(tail)};
};

struct Empty {};

template <typename T> void is_type(T);

template <typename T, typename Func>
constexpr void forEachImpl(T &&node, Func &&func) {
  forEachImpl(std::move(node.head), std::forward<Func>(func));
  forEachImpl(std::move(node.tail), std::forward<Func>(func));
}

template <typename Func>
constexpr void forEachImpl(Empty &&node, Func &&func) {}

template <typename T, typename R, typename Func>
constexpr void forEachImpl(Attribute<T, R> &&node, Func &&func) {
  func(std::move(node));
}

template <typename T> class AutoProp {
public:
  template <typename Function> void forEach(Function &&f) {
    auto &&names = static_cast<T *>(this)->all_names();
    forEachImpl(names, [&](auto &&v) {
      auto && [ key, value ] = v;
      f(std::move(key), static_cast<T *>(this)->*value);
    });
  }

  template <typename Value> void set(std::string_view key, Value value) {
    auto &&names = static_cast<T *>(this)->all_names();
    forEachImpl(names, [&](auto &&v) {
      auto && [ name, ptr ] = v;
      if (name == key) {
        static_cast<T *>(this)->*ptr = value;
      }
    });
  }

  template <typename R> constexpr R get(std::string_view key) {
    auto &&names = static_cast<T *>(this)->all_names();
    R result;
    forEachImpl(names, [&](auto &&v) {
      auto && [ name, ptr ] = v;
      using t =
          typename std::decay<decltype(static_cast<T *>(nullptr)->*ptr)>::type;
      if
        constexpr(std::is_same<R, t>{}) {
          if (name == key) {
            result = static_cast<T *>(this)->*ptr;
          }
        }
    });
    return result;
  }
};

#define CONCAT_(a, b) a##b
#define CONCAT3_(a, b, c) a##b##c
#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT3(a, b, c) CONCAT3_(a, b, c)

#define AUTOPROP_BEGIN(type)                                                   \
  using this_type = type;                                                      \
  static constexpr Empty CONCAT(names, __COUNTER__)() noexcept { return {}; }

#define AUTOPROP_AUX(name, counter)                                            \
  name;                                                                        \
  static constexpr auto CONCAT(names, counter)() noexcept {                    \
    return makeNode(CONCAT(names, BOOST_PP_SUB(counter, 1))(),                 \
                    makeAttribute(#name, &this_type::name));                   \
  }

#define AUTOPROP(name) AUTOPROP_AUX(name, __COUNTER__)

#define AUTOPROP_END_AUX(type, counter)                                        \
  static constexpr auto all_names() noexcept {                                 \
    return CONCAT(names, BOOST_PP_SUB(counter, 1))();                          \
  }

#define AUTOPROP_END(type) AUTOPROP_END_AUX(type, __COUNTER__)
}