#ifndef SPONGE_UTILITY_HPP
#define SPONGE_UTILITY_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T>
	class discretizer:public vector<T>
	{
	public:
		using vector<T>::vector;
		void discretize()
		{
			sort(this->begin(),this->end());
			this->erase(unique(this->begin(),this->end()),this->end());
		}
		int index(const T& x)
		{
			return lower_bound(this->begin(),this->end(),x)-this->begin()+1;
		}
		T value(int x)
		{
			return this->operator[](x-1);
		}
	};
	template<typename T,int n,typename Cmp=less<T>>
	class priority_array:public array<T,n>
	{
	public:
		static constexpr Cmp cmp{};
		using array<T,n>::array;
		void insert(const T& v)
		{
			if(cmp(v,this->operator[](n-1)))
			{
				this->operator[](n-1)=v;
				int k=n-1;
				while(k>0&&cmp(this->operator[](k),this->operator[](k-1)))
				{
					swap(this->operator[](k),this->operator[](k-1));
					k--;
				}
			}
		}
	};
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
	template<cmp_op op=_ls,typename T>
	INLINE bool cmp(T x,T y)
	{
		switch(op)
		{
		case _ls:return sgn(x-y)<0;
		case _le:return sgn(x-y)<=0;
		case _eq:return sgn(x-y)==0;
		case _ge:return sgn(x-y)>=0;
		case _gt:return sgn(x-y)>0;
		case _ne:return sgn(x-y)!=0;
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