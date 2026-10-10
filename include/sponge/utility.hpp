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
	template<typename FIte,typename Stream=istream>
	void read_each(FIte first,FIte last,Stream& s=cin)
	{
		for(;first!=last;++first)s>>*first;
	}
	template<typename FIte,typename Separator=char,typename Stream=ostream>
	void write_each(FIte first,FIte last,const Separator& sep=' ',Stream& s=cout)
	{
		for(;first!=last;++first)s<<*first<<sep;
	}
	enum cmp_op{_ls,_le,_eq,_ge,_gt,_ne};
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
	template<cmp_op op=_ls,bool absolute=1,typename T>
	INLINE bool cmp(T x,T y)
	{
		int d;
		if constexpr(absolute)d=sgn(x-y);
		else d=sgnr(x,y);
		switch(op)
		{
		case _ls:return d<0;
		case _le:return d<=0;
		case _eq:return d==0;
		case _ge:return d>=0;
		case _gt:return d>0;
		case _ne:return d!=0;
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
	template<typename T,typename F>
	INLINE T binary_search_f0(T l,T r,F&& f)
	{
#if __cplusplus>=202002L
		return *ranges::partition_point(views::iota(l,r),f);
#else
		T ans=r;
		r--;
		while(l<=r)
		{
			T mid=(l+r)>>1;
			if(!f(mid))ans=mid,r=mid-1;
			else l=mid+1;
		}
		return ans;
#endif
	}
	template<typename T,typename F>
	INLINE T binary_search_f1(T l,T r,F&& f)
	{
		return binary_search_f0(l,r,not_fn<F>(f));
	}
	template<typename T,typename F>
	INLINE T binary_search_l0(T l,T r,F&& f)
	{
		return binary_search_f1(l,r,f)-1;
	}
	template<typename T,typename F>
	INLINE T binary_search_l1(T l,T r,F&& f)
	{
		return binary_search_f0(l,r,f)-1;
	}
}
#endif