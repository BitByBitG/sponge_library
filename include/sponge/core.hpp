#ifndef SPONGE_CORE_HPP
#define SPONGE_CORE_HPP
#define SPONGE_VERSION "1.0.0"
#include<bits/stdc++.h>
#include<cassert>
namespace sponge
{
	using namespace std;
	using uint=unsigned int;
	using ll=long long;
	using ull=unsigned long long;
	using ld=long double;
	class null_t{};
#ifdef __GNUC__
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif
#ifdef SPONGE_LOCAL
#define ASSERT(condition)\
	[&](){\
		if(!(condition))\
		{\
			cerr<<"Assertion failed: " #condition ", file "<<__FILE__<<", line "<< __LINE__<<"\n";\
			throw runtime_error("Assertion failed");\
		}\
	}()
#define DEBUG(message)\
	[&](){\
		cerr<<(message)<<'\n';\
	}()
#else
#define ASSERT(condition) []{}()
#define DEBUG(message) []{}()
#endif
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
}
#endif