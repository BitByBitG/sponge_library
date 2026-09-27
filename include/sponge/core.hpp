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