#ifndef SPONGE_FOREST_HPP
#define SPONGE_FOREST_HPP
#include<sponge/core.hpp>
#include<sponge/functors.hpp>
#include<sponge/monoid.hpp>
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT
	>
	class forest:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>
	{
	public:
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		using s_cr=typename M::s_cr;
		using t_cr=typename M::t_cr;
		struct node_t
		{
			int fa;
			array<int,2> son;
#ifndef SPONGE_TREE_NREV
			bool rev;
#endif
			S val;
			T tag;
			node_t():
				fa(),son{},
#ifndef SPONGE_TREE_NREV
				rev(),
#endif
				val(s_id()),tag(t_id())
			{}
			node_t(s_cr v):
				fa(),son{},
#ifndef SPONGE_TREE_NREV
				rev(),
#endif
				val(v),tag(t_id())
			{}
		};
		vector<node_t> tr;
		vector<int> stk;
		tree()
		{
			tr.push_back(node_t());
		}
		tree(int n)
		{
			tr.push_back(node_t());
			build(n,[](int){return s_id();});
		}
		template<typename F>
		tree(int n,F&& f)
		{
			tr.push_back(node_t());
			build(n,f);
		}
		void reserve(int n)
		{
			tr.reserve(n*2+10);
		}
		void clear()
		{
			tr.resize(1);
			stk.clear();
		}
		node_t& operator[](int x)
		{
			return tr[x];
		}
		INLINE int& fa(const int x)
		{
			return tr[x].fa;
		}
		INLINE int& ls(const int x)
		{
			return tr[x].son[0];
		}
		INLINE int& rs(const int x)
		{
			return tr[x].son[1];
		}
		inline S& value(const int x)
		{
			return tr[x].val;
		}
		int new_node()
		{
			if(!stk.empty())
			{
				int x=stk.back();
				stk.pop_back();
				return x;
			}
			else
			{
				tr.push_back(node_t());
				return tr.size()-1;
			}
		}
		void delete_node(int x)
		{
			tr[x]=node_t();
			stk.push_back(x);
		}
		inline bool dir(const int x)
		{
			return x==rs(fa(x));
		}
	};
}
#endif