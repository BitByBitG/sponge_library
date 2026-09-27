#ifndef SPONGE_TYPE_TRAITS_HPP
#define SPONGE_TYPE_TRAITS_HPP
#include<sponge/core.hpp>
namespace sponge
{
	namespace detail
	{
		template<typename T> struct __inf { static constexpr T value=numeric_limits<T>::max()>>1; };
		template<> struct __inf<__int128_t> { static constexpr __int128_t value=(__int128_t(1)<<126)-1; };
		template<> struct __inf<__uint128_t> { static constexpr __uint128_t value=(__uint128_t(1)<<127)-1; };
		template<> struct __inf<float> { static constexpr float value=numeric_limits<float>::infinity(); };
		template<> struct __inf<double> { static constexpr double value=numeric_limits<double>::infinity(); };
		template<> struct __inf<long double> { static constexpr long double value=numeric_limits<long double>::infinity(); };
		template<typename T> struct __eps { static constexpr T value=0; };
		template<> struct __eps<float> { static constexpr float value=1e-6F; };
		template<> struct __eps<double> { static constexpr double value=1e-9; };
		template<> struct __eps<long double> { static constexpr long double value=1e-15L; };
	}
	template<typename T>
	constexpr T inf=detail::__inf<T>::value;
	template<typename T>
	constexpr T eps=detail::__eps<T>::value;
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