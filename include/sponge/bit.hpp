#ifndef SPONGE_BIT_HPP
#define SPONGE_BIT_HPP
#include<sponge/core.hpp>
namespace sponge
{
#if __cplusplus<=201703L
	template<typename T>
	constexpr int bit_width(T x)
	{
		if constexpr(sizeof(T)<=sizeof(unsigned int))
			return x?sizeof(unsigned int)*8-__builtin_clz((unsigned int)x):0;
		else if constexpr(sizeof(T)<=sizeof(unsigned long))
			return x?sizeof(unsigned long)*8-__builtin_clzl((unsigned long)x):0;
		else
			return x?sizeof(unsigned long long)*8-__builtin_clzll((unsigned long long)x):0;
	}
	template<typename T>
	constexpr int countr_zero(T x)
	{
		if constexpr(sizeof(T)<=sizeof(unsigned int))
			return x?__builtin_ctz((unsigned int)x):sizeof(T)*8;
		else if constexpr(sizeof(T)<=sizeof(unsigned long))
			return x?__builtin_ctzl((unsigned long)x):sizeof(T)*8;
		else
			return x?__builtin_ctzll((unsigned long long)x):sizeof(T)*8;
	}
	template<typename T>
	constexpr T bit_ceil(T x)
	{
		if(x<=1)return 1;
		return T(1)<<(bit_width<T>(x-1));
	}
	template<typename To,typename From>
	To bit_cast(const From& x)noexcept
	{
		static_assert(sizeof(To)==sizeof(From),"bit_cast requires equal sizes");
		static_assert(is_trivially_copyable<To>::value,"To must be trivially copyable");
		static_assert(is_trivially_copyable<From>::value,"From must be trivially copyable");
		To res;
		memcpy(&res,&x,sizeof(To));
		return res;
	}
#endif
}
#endif