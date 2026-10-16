#ifndef SPONGE_SEARCH_HPP
#define SPONGE_SEARCH_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T,typename F>
	INLINE T binary_search_f0(T l,T r,F&& f)
	{
		T ans=r+1;
		while(l<=r)
		{
			T mid=(l+r)>>1;
			if(!f(mid))ans=mid,r=mid-1;
			else l=mid+1;
		}
		return ans;
	}
	template<typename T,typename F>
	INLINE T binary_search_f1(T l,T r,F&& f)
	{
		return binary_search_f0(l,r,[&](T x){return !f(x);});
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
	template<typename T,typename F,std::enable_if_t<std::is_integral_v<T>,int> =0>
	auto ternary_search_max(T l,T r,F&& f)
	{
		ASSERT(l<=r);
		using U=std::make_unsigned_t<T>;
		using V=std::decay_t<decltype(f(l))>;
		while(l<r)
		{
			T mid=l+T((U(r)-U(l))/2);
			if(f(mid)<f(mid+1))l=mid+1;
			else r=mid;
		}
		return std::pair<T,V>{l,f(l)};
	}
	template<typename T,typename F,std::enable_if_t<std::is_integral_v<T>,int> =0>
	auto ternary_search_min(T l,T r,F&& f)
	{
		ASSERT(l<=r);
		using U=std::make_unsigned_t<T>;
		using V=std::decay_t<decltype(f(l))>;
		while(l<r)
		{
			T mid=l+T((U(r)-U(l))/2);
			if(f(mid)>f(mid+1))l=mid+1;
			else r=mid;
		}
		return std::pair<T,V>{l,f(l)};
	}
	template<typename T,typename F,std::enable_if_t<std::is_floating_point_v<T>,int> =0>
	auto ternary_search_max(T l,T r,F&& f,int cnt=100)
	{
		ASSERT(l<=r);
		using V=std::decay_t<decltype(f(l))>;
		while(cnt-->0&&l<r)
		{
			T ml,mr;
			if(l<0&&r>0)
			{
				ml=l*(T(2)/3)+r/3;
				mr=l/3+r*(T(2)/3);
			}
			else
			{
				T d=(r-l)/3;
				ml=l+d,mr=r-d;
			}
			if(!(l<ml&&ml<mr&&mr<r))break;
			V vl=f(ml),vr=f(mr);
			if(vl<vr)l=ml;
			else if(vr<vl)r=mr;
			else l=ml,r=mr;
		}
		T p=(l<0&&r>0)?l/2+r/2:l+(r-l)/2;
		return std::pair<T,V>{p,f(p)};
	}
	template<typename T,typename F,std::enable_if_t<std::is_floating_point_v<T>,int> =0>
	auto ternary_search_min(T l,T r,F&& f,int cnt=100)
	{
		ASSERT(l<=r);
		using V=std::decay_t<decltype(f(l))>;
		while(cnt-->0&&l<r)
		{
			T ml,mr;
			if(l<0&&r>0)
			{
				ml=l*(T(2)/3)+r/3;
				mr=l/3+r*(T(2)/3);
			}
			else
			{
				T d=(r-l)/3;
				ml=l+d,mr=r-d;
			}
			if(!(l<ml&&ml<mr&&mr<r))break;
			V vl=f(ml),vr=f(mr);
			if(vl>vr)l=ml;
			else if(vr>vl)r=mr;
			else l=ml,r=mr;
		}
		T p=(l<0&&r>0)?l/2+r/2:l+(r-l)/2;
		return std::pair<T,V>{p,f(p)};
	}
}
#endif