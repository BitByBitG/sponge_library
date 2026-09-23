#ifndef SPONGE_CORE_HPP
#define SPONGE_CORE_HPP
#define SPONGELIB_VERSION "1.0.0"
#include<bits/stdc++.h>
#include<cassert>
#ifdef __GNUC__
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif
namespace sponge
{
	using namespace std;
	using uint=unsigned int;
	using ll=long long;
	using ull=unsigned long long;
	using ld=long double;
	class null_t{};
	namespace detail
	{
		template<typename T,enable_if_t<is_floating_point<T>::value,int> =0>
		constexpr T inf_impl()
		{ return numeric_limits<T>::infinity(); }
		template<typename T,enable_if_t<is_integral<T>::value,int> =0>
		constexpr T inf_impl()
		{ return numeric_limits<T>::max()>>1; }
		constexpr ld eps_impl()
		{ return 1e-15L; }
		template<typename T,enable_if_t<is_floating_point<T>::value,int> =0>
		constexpr T eps_impl()
		{ return 1e-9; }
		template<typename T>
		constexpr T eps_impl()
		{ return 0; }
	}
	template<typename T>
	constexpr T inf=detail::inf_impl<T>();
	template<typename T>
	constexpr T eps=detail::eps_impl<T>();
	template<typename _F>
	void multitest_n(int _n,_F&& _f)
	{
		for(int _i=1;_i<=_n;_i++)_f(_i);
	}
	template<typename _F,typename _S=istream>
	void multitest(_F&& _f,_S& _s=cin)
	{
		int _n;
		_s>>_n;
		multitest_n(_n,_f);
	}
}
#endif