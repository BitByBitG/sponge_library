#ifndef SPONGE_STRING_HASH_HPP
#define SPONGE_STRING_HASH_HPP
#include<sponge/core.hpp>
#include<sponge/modint.hpp>
namespace sponge
{
	template<typename... Args>
	class string_hash_t
	{
		using self=string_hash_t<Args...>;
		template<size_t... I>
		static void set_value(tuple<Args...>& x,long long v,index_sequence<I...>)
		{
			((get<I>(x)=v),...);
		}
		template<size_t... I>
		static void multiply(tuple<Args...>& x,const tuple<Args...>& y,index_sequence<I...>)
		{
			((get<I>(x)*=get<I>(y)),...);
		}
		template<size_t... I>
		void insert_impl(long long ch,index_sequence<I...>)
		{
			((get<I>(val)=get<I>(val)*get<I>(base)+ch),...);
		}
		template<size_t... I>
		static self query_impl(const self& l,const self& r,int n,index_sequence<I...>)
		{
			self res;
			((get<I>(res.val)=get<I>(r.val)-get<I>(l.val)*get<I>(power[n])),...);
			return res;
		}
		static constexpr int _size=sizeof...(Args);
	public:
		int size()const
		{
			return _size;
		}
		static tuple<Args...> base{};
		static vector<tuple<Args...>> power{};
		static void set_base(const tuple<Args...>& _base)
		{
			base=_base;
		}
		static void init_power(int n)
		{
			power.resize(n+1);
			set_value(power[0],1,index_sequence_for<Args...>{});
			for(int i=1;i<=n;i++)
			{
				power[i]=power[i-1];
				multiply(power[i],base,index_sequence_for<Args...>{});
			}
		}
		tuple<Args...> val{};
		template<typename Char>
		void insert(Char ch)
		{
			insert_impl(ch,index_sequence_for<Args...>{});
		}
		static self query(const self& l,const self& r,int n)
		{
			return query_impl(l,r,n,index_sequence_for<Args...>{});
		}
		template<typename RAIte>
		static self query(RAIte first,RAIte last)
		{
			return query(*prev(first),*prev(last),last-first);
		}
		static self query(const vector<self>& vec,int l,int r)
		{
			return query(vec[l-1],vec[r],r-l+1);
		}
	};
	template<int... Mods>
	struct make_hash
	{
		using type=string_hash_t<static_modint<Mods>...>;
	};
	template<int... Mods>
	using make_hash_t=typename make_hash<Mods...>::type;
}
#endif