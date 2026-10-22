#ifndef SPONGE_UTILITY_HPP
#define SPONGE_UTILITY_HPP
#include<sponge/core.hpp>
#include<sponge/type_traits.hpp>
namespace sponge
{
	template<typename T>
	INLINE void chkmin(T& x,const T& y)
	{
		x=min(x,y);
	}
	template<typename T>
	INLINE void chkmax(T& x,const T& y)
	{
		x=max(x,y);
	}
	enum cmp_op{cmp_lt,cmp_le,cmp_eq,cmp_ge,cmp_gt,cmp_ne};
	template<typename T>
	INLINE int sgn(T x)
	{
		return x<-eps<T>?-1:(x>eps<T>?1:0);
	}
	template<typename T>
	INLINE int sgnr(T x,T y)
	{
		T e=eps<T>*max(T(1),max(abs(x),abs(y)));
		T d=x-y;
		return d<-e?-1:(d>e?1:0);
	}
	template<cmp_op op=cmp_lt,bool absolute=1,typename T>
	INLINE bool cmp(T x,T y)
	{
		int d;
		if constexpr(absolute)d=sgn(x-y);
		else d=sgnr(x,y);
		switch(op)
		{
		case cmp_lt:return d<0;
		case cmp_le:return d<=0;
		case cmp_eq:return d==0;
		case cmp_ge:return d>=0;
		case cmp_gt:return d>0;
		case cmp_ne:return d!=0;
		}
	}
	template<typename T>
	INLINE T sqr(T x)
	{
		return x*x;
	}
#if __cplusplus<=201703L
	template<typename C>
	INLINE uint64_t size(const C& c)
	{
		return c.size();
	}
	template<typename C>
	INLINE int64_t ssize(const C& c)
	{
		return static_cast<int64_t>(c.size());
	}
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