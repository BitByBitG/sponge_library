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
	template<typename _T>
	constexpr _T inf=[]()->_T
	{
		if constexpr(is_floating_point_v<_T>)return numeric_limits<_T>::infinity();
		else return numeric_limits<_T>::max()/2;
	}();
	template<typename _T>
	constexpr _T eps=[]()->_T
	{
		if constexpr(is_same_v<_T,ld>)return 1e-15;
		else if constexpr(is_floating_point_v<_T>)return 1e-8;
		else return 0;
	}();
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