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
	template<typename T>
	constexpr T inf=numeric_limits<T>::max()/2;
	template<typename F>
	void multitest_n(int _n,F&& _f)
	{
		for(int _i=1;_i<=_n;_i++)_f(_i);
	}
	template<typename F,typename Stream=istream>
	void multitest(F&& _f,Stream& _s=cin)
	{
		int _n;
		_s>>_n;
		multitest_n(_n,_f);
	}
}
#endif