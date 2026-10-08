#ifndef SPONGE_SUFFIX_BST_HPP
#define SPONGE_SUFFIX_BST_HPP
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/tree_policy.hpp>
#include<sponge/core.hpp>
#include<sponge/functor.hpp>
namespace sponge
{
	namespace detail
	{
		template<typename Key,typename Cmp>
		using sbst_pbds_rbt_t=__gnu_pbds::tree<
			Key,__gnu_pbds::null_type,Cmp,
			__gnu_pbds::rb_tree_tag,__gnu_pbds::tree_order_statistics_node_update
		>;
		struct sbst_fa_t
		{
			constexpr int operator()(int x) const
			{
				return x-1;
			}
		};
	}
	template<typename String=string,typename Inf=val_fn<char,127>,typename Fa=detail::sbst_fa_t,template<typename...> class Bst=detail::sbst_pbds_rbt_t>
	class suffix_bst
	{
	public:
		Inf _inf;
		Fa fa;
		using Ch=typename String::value_type;
		String s;
		String v;
		vector<ll> tag;
		bool __cmp(int x,int y)
		{
			if(x==y)return 0;
			if(x>=0&&y>=0)
			{
				if(s[x]!=s[y])return s[x]<s[y];
				if(tag[fa(x)]!=tag[fa(y)])return tag[fa(x)]<tag[fa(y)];
				return x<y;
			}
			else
			{
				int i=max(x,y),j=0;
				while(i&&j<ssize(v)&&s[i]==v[j])i--,j++;
				if(x<0)
				{
					if(j==ssize(v))return i!=0;
					if(!i)return 0;
					return v[j]<s[i];
				}
				else
				{
					if(!i)return j!=ssize(v);
					if(j==ssize(v))return 0;
					return s[i]<v[j];
				}
			}
		}
		struct __cmp_t
		{
			suffix_bst* p;
			bool operator()(int x,int y) const
			{
				return p->__cmp(x,y);
			}
		};
		Bst<int,__cmp_t> tr;
		suffix_bst():_inf{},fa{},tr(__cmp_t{this})
		{
			tr.insert(0);
			s.push_back(Ch{});
			tag.push_back(0);
		}
		suffix_bst(Fa&& _fa):_inf{},fa{_fa},tr(__cmp_t{this})
		{
			tr.insert(0);
			s.push_back(Ch{});
			tag.push_back(0);
		}
		int push(Ch c)
		{
			int i=s.size();
			s.push_back(c);
			tag.push_back(0);
			auto x=tr.insert(i).first;
			auto y=next(x);
			ll o=tag[*prev(x)];
			ll k=1;
			while(y!=tr.end()&&tag[*y]-o<=k*k)y++,k++;
			tag[i]=o+1;
			for(auto j=next(x);j!=y;j++)o+=k,tag[*j]=o;
			return i;
		}
		void pop()
		{
			tr.erase(s.size()-1);
			s.pop_back();
			tag.pop_back();
		}
		int count(const String& _v)
		{
			v=_v;
			reverse(v.begin(),v.end());
			int ans=tr.order_of_key(-1);
			v.push_back(_inf());
			ans=tr.order_of_key(-1)-ans;
			return ans;
		}
		int sa(int i)
		{
			return *tr.find_by_order(i);
		}
		int rk(int i)
		{
			return tr.order_of_key(i);
		}
	};
}
#endif