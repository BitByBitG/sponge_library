#ifndef SPONGE_CONTAINER_HPP
#define SPONGE_CONTAINER_HPP
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
	template<typename T,typename... Args>
	auto make_vector(size_t n,Args... args)
	{
		if constexpr(sizeof...(args)==1)return vector<T>(n,T(args...));
		else return vector<decltype(make_vector<T>(args...))>(n,make_vector<T>(args...));
	}
	template<typename T,size_t N>
	constexpr auto make_array(const T& x)
	{
		array<T,N> a;
		a.fill(x);
		return a;
	}
	template<typename T,size_t N,size_t... Ns>
	constexpr auto make_array(const T& x)
	{
		auto y=make_array<T,Ns...>(x);
		array<decltype(y),N> a;
		a.fill(y);
		return a;
	}
	template<typename T,size_t N,bool is_const,size_t D>
	struct multidimension_proxy
	{
		using ptr_t=conditional_t<is_const,const T*,T*>;
		ptr_t p;
		const array<size_t,N>& stride;
		decltype(auto) operator[](size_t x)const
		{
			if constexpr(D+1==N)return p[x];
			else return multidimension_proxy<T,N,is_const,D+1>{p+x*stride[D],stride};
		}
	};
	template<typename T,size_t N>
	class multidimension
	{
	public:
		vector<T> a;
		array<size_t,N> sz{},stride{};
		template<typename... Args>
		multidimension(Args... args)
		{
			static_assert(sizeof...(Args)==N);
			resize(args...);
		}
		multidimension()=default;
		template<typename... Args>
		void resize(Args... args)
		{
			static_assert(sizeof...(Args)==N);
			sz={size_t(args)...};
			stride[N-1]=1;
			for(size_t i=N-1;i--;)stride[i]=stride[i+1]*sz[i+1];
			a.resize(sz[0]*stride[0]);
		}
		template<typename U>
		void fill(const U& x)
		{
			std::fill(a.begin(),a.end(),x);
		}
		size_t size(size_t d)const
		{
			return sz[d];
		}
		size_t size()const
		{
			return a.size();
		}
		T* data()
		{
			return a.data();
		}
		const T* data() const
		{
			return a.data();
		}
		decltype(auto) operator[](size_t x)
		{
			if constexpr(N==1)return a[x];
			else return multidimension_proxy<T,N,0,1>{a.data()+x*stride[0],stride};
		}
		decltype(auto) operator[](size_t x)const
		{
			if constexpr(N==1)return a[x];
			else return multidimension_proxy<T,N,1,1>{a.data()+x*stride[0],stride};
		}
	};
}
#endif