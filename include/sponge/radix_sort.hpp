#ifndef SPONGE_RADIX_SORT_HPP
#define SPONGE_RADIX_SORT_HPP
#include<sponge/core.hpp>
namespace sponge
{
	namespace detail
	{
		template<int sh,typename T>
		void radix_pass(int n,uint32_t* cnt,T* src,T* dst)
		{
			memset(cnt,0,1024);
			for(int i=0;i<n;++i)++cnt[(src[i]>>sh)&255];
			uint32_t sum=0;
			for(int i=0;i<256;++i)
			{
				uint32_t t=cnt[i];
				cnt[i]=sum;
				sum+=t;
			}
			for(int i=0;i<n;++i)dst[cnt[(src[i]>>sh)&255]++]=src[i];
		}
		template<typename T>
		inline void radix_sort_32(T* a,int n)
		{
			alignas(64) uint32_t cnt[256];
			vector<T> buf(n);
			T* b=buf.data();
			radix_pass<0>(n,cnt,a,b);
			radix_pass<8>(n,cnt,b,a);
			radix_pass<16>(n,cnt,a,b);
			radix_pass<24>(n,cnt,b,a);
		}
		template<typename T>
		inline void radix_sort_64(T* a,int n)
		{
			alignas(64) uint32_t cnt[256];
			vector<T> buf(n);
			T* b=buf.data();
			radix_pass<0>(n,cnt,a,b);
			radix_pass<8>(n,cnt,b,a);
			radix_pass<16>(n,cnt,a,b);
			radix_pass<24>(n,cnt,b,a);
			radix_pass<32>(n,cnt,a,b);
			radix_pass<40>(n,cnt,b,a);
			radix_pass<48>(n,cnt,a,b);
			radix_pass<56>(n,cnt,b,a);
		}
	}
	template<typename RAIte>
		requires is_same_v<typename iterator_traits<RAIte>::value_type,uint32_t>
	void radix_sort(RAIte first,RAIte last)
	{
		const int n=last-first;
		if(n<=1)return;
		detail::radix_sort_32(&*first,n);
	}
	template<typename RAIte>
		requires is_same_v<typename iterator_traits<RAIte>::value_type,int32_t>
	void radix_sort(RAIte first,RAIte last)
	{
		const int n=last-first;
		if(n<=1)return;
		for(RAIte i=first;i!=last;++i)*i^=1u<<31;
		detail::radix_sort_32(&*first,n);
		for(RAIte i=first;i!=last;++i)*i^=1u<<31;
	}
	template<typename RAIte>
		requires is_same_v<typename iterator_traits<RAIte>::value_type,float>
	void radix_sort(RAIte first,RAIte last)
	{
		const int n=last-first;
		if(n<=1)return;
		vector<uint32_t> buf(n);
		RAIte a=first;
		uint32_t* b=buf.data();
		for(int i=0;i<n;i++)
		{
			uint32_t u=bit_cast<uint32_t>(a[i]);
			u=(u>>31)?~u:(u^0x80000000u);
			b[i]=u;
		}
		detail::radix_sort_32(b,n);
		for(int i=0;i<n;i++)
		{
			uint32_t u=b[i];
			u=(u>>31)?(u^0x80000000u):~u;
			a[i]=bit_cast<float>(u);
		}
	}
	template<typename RAIte>
		requires is_same_v<typename iterator_traits<RAIte>::value_type,uint64_t>
	void radix_sort(RAIte first,RAIte last)
	{
		const int n=last-first;
		if(n<=1)return;
		detail::radix_sort_64(&*first,n);
	}
	template<typename RAIte>
		requires is_same_v<typename iterator_traits<RAIte>::value_type,int64_t>
	void radix_sort(RAIte first,RAIte last)
	{
		const int n=last-first;
		if(n<=1)return;
		for(RAIte i=first;i!=last;++i)*i^=1ull<<63;
		detail::radix_sort_64(&*first,n);
		for(RAIte i=first;i!=last;++i)*i^=1ull<<63;
	}
	template<typename RAIte>
		requires is_same_v<typename iterator_traits<RAIte>::value_type,double>
	void radix_sort(RAIte first,RAIte last)
	{
		const int n=last-first;
		if(n<=1)return;
		vector<uint64_t> buf(n);
		RAIte a=first;
		uint64_t* b=buf.data();
		for(int i=0;i<n;i++)
		{
			uint64_t u=bit_cast<uint64_t>(a[i]);
			u=(u>>63)?~u:(u^0x8000000000000000ull);
			b[i]=u;
		}
		detail::radix_sort_64(b,n);
		for(int i=0;i<n;i++)
		{
			uint64_t u=b[i];
			u=(u>>63)?(u^0x8000000000000000ull):~u;
			a[i]=bit_cast<double>(u);
		}
	}
}
#endif