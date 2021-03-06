//==========================================================================
// enum operator [enum_operator.hpp]
// author: MasyoLab
//==========================================================================
#pragma once
#include <type_traits>
#include "mdx.h"

_MDX_COMMON_BEGIN

//==========================================================================
// enum機能拡張
//==========================================================================
namespace enum_operator {
    // 演算
    template<typename T>
    struct calculation : std::false_type {};
    // Bit演算
    template<typename T>
    struct has_bitwise_operators : std::false_type {};
    // AND/OR演算
    template<typename T>
    struct has_and_or_operators : has_bitwise_operators<T> {};
}

//==========================================================================
// 基底型取得
//==========================================================================
namespace type_traits {
    template<bool con> using concept_t = typename std::enable_if<con, std::nullptr_t>::type;
    template<typename T> using underlying_type_t = typename std::underlying_type<T>::type;//C++11にはない
}

//==========================================================================
// 基底型取得
//==========================================================================
namespace detail {
    using namespace type_traits;
    template<typename T, concept_t<std::is_enum<T>::value> = nullptr>
    constexpr underlying_type_t<T> underlying_cast(T e) { return static_cast<underlying_type_t<T>>(e); }
}

_MDX_COMMON_END

//==========================================================================
// AND/OR演算 ここから
//==========================================================================
template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_and_or_operators<T>::value> = nullptr>
constexpr T operator&(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) & mdx::common::detail::underlying_cast((T)r));
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_and_or_operators<T>::value> = nullptr>
T& operator&=(T& l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) & mdx::common::detail::underlying_cast((T)r));
    return l;
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_and_or_operators<T>::value> = nullptr>
constexpr T operator|(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) | mdx::common::detail::underlying_cast((T)r));
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_and_or_operators<T>::value> = nullptr>
T& operator|=(T& l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) | mdx::common::detail::underlying_cast((T)r));
    return l;
}

//==========================================================================
// AND/OR演算 ここまで
//==========================================================================

//==========================================================================
// Bit演算 ここから
//==========================================================================
template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_bitwise_operators<T>::value> = nullptr>
constexpr T operator^(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) ^ mdx::common::detail::underlying_cast((T)r));
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_bitwise_operators<T>::value> = nullptr>
T& operator^=(T& l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) ^ mdx::common::detail::underlying_cast((T)r));
    return l;
}

template<typename T, mdx::common::type_traits::concept_t<mdx::common::enum_operator::has_bitwise_operators<T>::value> = nullptr>
constexpr T operator~(T op) {
    return static_cast<T>(~mdx::common::detail::underlying_cast(op));
}

//==========================================================================
// Bit演算 ここまで
//==========================================================================

//==========================================================================
// 演算 ここから
//==========================================================================
template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator+(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) + mdx::common::detail::underlying_cast((T)r));
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator-(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) - mdx::common::detail::underlying_cast((T)r));
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator/(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) / mdx::common::detail::underlying_cast((T)r));
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator*(T l, S r) {
    return static_cast<T>(mdx::common::detail::underlying_cast((T)l) * mdx::common::detail::underlying_cast((T)r));
}

template<typename T, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator++(T & l) {
    l = static_cast<T>(mdx::common::detail::underlying_cast(l) + mdx::common::detail::underlying_cast((T)1));
    return l;
}

template<typename T, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator--(T & l) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) - mdx::common::detail::underlying_cast((T)1));
    return l;
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator+=(T &l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) + mdx::common::detail::underlying_cast((T)r));
    return l;
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator-=(T &l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) - mdx::common::detail::underlying_cast((T)r));
    return l;
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator*=(T &l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) * mdx::common::detail::underlying_cast((T)r));
    return l;
}

template<typename T, typename S, mdx::common::type_traits::concept_t<mdx::common::enum_operator::calculation<T>::value> = nullptr>
constexpr T operator/=(T &l, S r) {
    l = static_cast<T>(mdx::common::detail::underlying_cast((T)l) / mdx::common::detail::underlying_cast((T)r));
    return l;
}

//==========================================================================
// 演算 ここまで
//==========================================================================

#if 0
/*

enum、enum class の型名を<T>に入れるとこにより数学的処理を可能にするclassです

template<> struct enum_operator::calculation<T> : std::true_type {};
template<> struct enum_operator::has_and_or_operators<T> : std::true_type {};
template<> struct enum_operator::has_bitwise_operators<T> : std::true_type {};

*/
#endif