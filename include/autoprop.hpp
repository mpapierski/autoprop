#include <boost/preprocessor.hpp>
#include <string>
#include <string_view>
#include <type_traits>

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
    auto &&names = static_cast<T *>(this)->getAttributeList();
    forEachImpl(names, [&](auto &&v) {
      auto && [ key, value ] = v;
      f(std::move(key), static_cast<T *>(this)->*value);
    });
  }

  template <typename Value> void set(std::string_view key, Value value) {
    auto &&names = static_cast<T *>(this)->getAttributeList();
    forEachImpl(names, [&](auto &&v) {
      auto && [ name, ptr ] = v;
      if (name == key) {
        static_cast<T *>(this)->*ptr = value;
      }
    });
  }

  template <typename R> constexpr R get(std::string_view key) {
    auto &&names = static_cast<T *>(this)->getAttributeList();
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

#define AUTOPROP_BEGIN_AUX(type, counter)                                      \
  using this_type = type;                                                      \
  static constexpr std::size_t CONCAT(attr, counter){0};                       \
  static constexpr std::string_view getClassName() noexcept { return #type; }  \
  static constexpr Empty CONCAT(names, counter)() noexcept { return {}; }

#define AUTOPROP_BEGIN(type) AUTOPROP_BEGIN_AUX(type, __COUNTER__)

#define AUTOPROP_AUX(name, counter, counter_prev)                              \
  name;                                                                        \
  static constexpr std::size_t CONCAT(attr, counter){                          \
      1 + CONCAT(attr, counter_prev)};                                         \
  static constexpr auto CONCAT(names, counter)() noexcept {                    \
    return makeNode(CONCAT(names, counter_prev)(),                             \
                    makeAttribute(#name, &this_type::name));                   \
  }

#define AUTOPROP_AUX_PREV(name, counter)                                       \
  AUTOPROP_AUX(name, counter, BOOST_PP_SUB(counter, 1))
#define AUTOPROP(name) AUTOPROP_AUX_PREV(name, __COUNTER__)

#define AUTOPROP_END_AUX_PREV(type, counter, counter_prev)                     \
  static constexpr std::size_t kTotalAttributes{CONCAT(attr, counter_prev)};   \
  static constexpr auto getAttributeList() noexcept {                          \
    return CONCAT(names, counter_prev)();                                      \
  }

#define AUTOPROP_END_AUX(type, counter)                                        \
  AUTOPROP_END_AUX_PREV(type, counter, BOOST_PP_SUB(counter, 1))
#define AUTOPROP_END(type) AUTOPROP_END_AUX(type, __COUNTER__)
}