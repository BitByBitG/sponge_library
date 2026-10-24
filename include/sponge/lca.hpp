#ifndef SPONGE_LCA_HPP
#define SPONGE_LCA_HPP
#include<sponge/core.hpp>
#include<sponge/sparse_table.hpp>
namespace sponge
{
	namespace detail
	{
		struct __dfn_cmp
		{
			vector<int>* dfn;
			int operator()(int x,int y)const
			{
				return (*dfn)[x]<(*dfn)[y]?x:y;
			}
		};
	}
	class lca_tree
	{
	public:
		int n;
		int root;
		vector<vector<int>> g;
		vector<int> dfn;
		sparse_table<int,default_id<int>,detail::__dfn_cmp> st;
		lca_tree():n(),root(){}
		lca_tree(int _n):n(_n),root(1)
		{
			g.resize(n+1);
			dfn.resize(n+1);
		}
		void link(int x,int y)
		{
			g[x].push_back(y);
		}
		void linku(int x,int y)
		{
			g[x].push_back(y);
			g[y].push_back(x);
		}
		void build()
		{
			int _dfn=0;
			vector<int> a(n+1);
			auto dfs=[&](auto&& self,int x,int f)->void
			{
				dfn[x]=++_dfn;
				a[dfn[x]]=f;
				for(auto y:g[x])if(y!=f)self(self,y,x);
			};
			dfs(dfs,root,0);
			using ST=decltype(st);
			using STM=ST::M;
			st=ST(n,[&](int x){return a[x];},STM({},{&dfn}));
		}
		int lca(int x,int y)
		{
			if(x==y)return x;
			x=dfn[x];y=dfn[y];
			if(x>y)swap(x,y);
			return st.query(x+1,y);
		}
	};
}
#endif