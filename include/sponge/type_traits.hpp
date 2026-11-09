#ifndef SPONGE_TYPE_TRAITS_HPP
#define SPONGE_TYPE_TRAITS_HPP
#include<sponge/core.hpp>
namespace sponge
{
#if __cplusplus<=201402L
	template<typename T>
	constexpr bool is_integral_v=is_integral<T>::value;
	template<typename T>
	constexpr bool is_floating_point_v=is_floating_point<T>::value;
	template<typename T,typename U>
	constexpr bool is_same_v=is_same<T,U>::value;
	template<typename T>
	constexpr bool is_trivial_v=is_trivial<T>::value;
	template<typename...>
	using void_t=void;
#endif
	template<typename T>
	struct promote;
	template<> struct promote<signed char> { using type=int; };
	template<> struct promote<unsigned char> { using type=uint; };
	template<> struct promote<short> { using type=int; };
	template<> struct promote<unsigned short> { using type=uint; };
	template<> struct promote<int> { using type=ll; };
	template<> struct promote<uint> { using type=ull; };
	template<> struct promote<ll> {using type=__int128_t;};
	template<> struct promote<ull> { using type=__uint128_t; };
	template<typename T>
	using promote_t=typename promote<T>::type;
}
#endif