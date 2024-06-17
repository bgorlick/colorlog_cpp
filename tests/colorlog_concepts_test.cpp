#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <exception>
#include <typeinfo>
#include <fstream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <optional>
#include <execinfo.h>
#include <concepts>
#include <type_traits>

// Define Arithmetic concept first
/// Concept to check if a type is arithmetic (e.g., int, float).
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// Helper traits for type checks

/// Trait to check if a type can be printed to an ostream.
template <typename T, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};

template <typename T>
inline constexpr bool is_printable_v = is_printable<T>::value;

/// Trait to check if a type is iterable.
template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {};

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

/// Trait to check if a type has a value_type member.
template <typename T, typename = void>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

template <typename T>
inline constexpr bool has_value_type_v = has_value_type<T>::value;

/// Trait to check if a type is a string.
template <typename T>
struct is_string : std::is_same<T, std::string> {};

template <typename T>
inline constexpr bool is_string_v = is_string<T>::value;

/// Concept to check if a type can be printed to an ostream.
template <typename T>
concept Printable = is_printable_v<T>;

/// Concept to check if a type is iterable.
template <typename T>
concept Iterable = is_iterable_v<T>;

/// Concept to check if a type is a string or an iterable of printable elements.
template <typename T>
concept StringOrIterableOfPrintable = is_string_v<T> || 
                                      (is_iterable_v<T> && has_value_type_v<T> && Printable<typename T::value_type>);

/// Concept to check if a type is printable, a string, or an iterable of printable elements.
template <typename T>
concept PrintableStringOrIterable = (Printable<T> && !Arithmetic<T>) || StringOrIterableOfPrintable<T>;

/// Concept to check if a type is an optional of printable elements.
template <typename T>
concept PrintableOptional = requires(T t) {
    { t.value() } -> Printable;
};

/// Concept to check if a type is printable, a string, an iterable of printable elements, or an optional of printable elements.
template <typename T>
concept PrintableStringOrIterableOrOptional = PrintableStringOrIterable<T> || PrintableOptional<T>;

/// Concept to check if a type is printable or an exception.
template <typename T>
concept PrintableOrException = (Printable<T> && !Arithmetic<T>) || std::is_base_of_v<std::exception, T>;

void test_concepts() {
    static_assert(PrintableStringOrIterable<std::string>);
    static_assert(PrintableStringOrIterable<std::vector<std::string>>);
    static_assert(PrintableStringOrIterable<std::vector<int>>);
    static_assert(!PrintableStringOrIterable<std::optional<std::vector<int>>>); 
    static_assert(PrintableOrException<std::string>);
    static_assert(!PrintableOrException<std::vector<int>>);
    static_assert(!PrintableStringOrIterable<int>); // Fixed now
    static_assert(PrintableStringOrIterableOrOptional<std::optional<std::string>>); 
    static_assert(PrintableStringOrIterableOrOptional<std::optional<int>>);
    static_assert(PrintableOrException<std::runtime_error>);
    static_assert(Arithmetic<int>);

    std::cout << "Concept tests passed!" << std::endl;
}

int main() {
    test_concepts();
    return 0;
}
