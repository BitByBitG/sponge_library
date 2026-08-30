#ifndef SPONGE_SUPPORT_HPP
#define SPONGE_SUPPORT_HPP
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
#endif
#if __cplusplus<=201703L
	uint bit_ceil(uint x)
	{
		if (x<=1)return 1;
		return 1u<<(32-__builtin_clz(x-1));
	}
	ull bit_ceil(ull x)
	{
		if(x<=1)return 1;
		return 1ull<<(64-__builtin_clzll(x-1));
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
	namespace numbers
	{
		template<typename T>
		constexpr T pi_v=T(3.141592653589793238462643383279502884L);
		constexpr double pi=pi_v<double>;
	}
	template<class C>
	ll ssize(const C& c)
	{
		return static_cast<ll>(c.size());
	}
#endif
#if __cplusplus<=202002L
	template<typename T>
	constexpr T* to_address(T* p)noexcept
	{
		return p;
	}
	template<typename Ptr>
	auto to_address(const Ptr& p)
	{
		return to_address(p.operator->());
	}
#endif
}
#endif