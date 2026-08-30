#ifndef SPONGE_SM64_HASH_HPP
#define SPONGE_SM64_HASH_HPP
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/hash_policy.hpp>
#include<sponge/core.hpp>
namespace sponge
{
	namespace detail
	{
		inline ull _sm64(ull x)
		{
            static mt19937_64 rng64(random_device{}());
			static const ull mask=rng64();
			x^=mask;
			x+=0x9e3779b97f4a7c15;
			x=(x^(x>>30))*0xbf58476d1ce4e5b9;
			x=(x^(x>>27))*0x94d049bb133111eb;
			return x^(x>>31);
		}
		inline void composite_hash(ull& h,ull x)
		{
			h^=_sm64(x+0x9e3779b97f4a7c15ULL+(h<<6)+(h>>2));
		}
		template<typename T>
		inline ull hashing_byte_by_byte(const T& x)
		{
			ull h=0;
			const unsigned char* p=reinterpret_cast<const unsigned char*>(&x);
			for(size_t i=0;i<sizeof(T);i++)composite_hash(h,p[i]);
			return h;
		}
		template<typename T>
			requires (is_trivial_v<T>&&!is_integral_v<T>&&!is_floating_point_v<T>)
		ull hashing(const T& x)
		{
			return hashing_byte_by_byte(x);
		}
		template<typename T>
			requires is_floating_point_v<T>
		ull hashing(const T& x)
		{
			if(x==0)return _sm64(0);
			else return hashing_byte_by_byte(x);
		}
		template<typename T>
			requires is_integral_v<T>
		ull hashing(T x)
		{
			if constexpr(sizeof(T)<=sizeof(ull))return _sm64((ull)x);
			else
			{
				ull a=(ull)x;
				ull b=(ull)(x>>64);
				composite_hash(a,b);
				return a;
			}
		}
		template<typename T1,typename T2>
		ull hashing(const pair<T1,T2>& p)
		{
			ull h=0;
			composite_hash(h,hashing(p.first));
			composite_hash(h,hashing(p.second));
			return h;
		}
		template<typename Tuple,size_t... I>
		ull tuple_hashing(const Tuple& t,index_sequence<I...>)
		{
			ull h=0;
			(composite_hash(h,hashing(get<I>(t))),...);
			return h;
		}
		template<typename... Args>
		ull hashing(const tuple<Args...>& t)
		{
			return tuple_hashing(t,index_sequence_for<Args...>{});
		}
		template<typename C>
			requires requires(C c)
			{
				c.begin();
				c.end();
			}
		ull hashing(const C& c)
		{
			ull h=0;
			for(const auto& i:c)composite_hash(h,hashing(i));
			return h;
		}
	}
	template<typename T>
	class sm64_hash
	{
	public:
		size_t operator()(const T& x)const
		{
			return detail::hashing(x);
		}
	};
	template<typename T>
	using uset=__gnu_pbds::gp_hash_table<T,__gnu_pbds::null_type,sm64_hash<T>>;
	template<typename T1,typename T2>
	using umap=__gnu_pbds::gp_hash_table<T1,T2,sm64_hash<T1>>;
}
#endif