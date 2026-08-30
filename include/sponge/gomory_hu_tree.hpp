#ifndef SPONGE_GOMORY_HU_TREE_HPP
#define SPONGE_GOMORY_HU_TREE_HPP
#include<sponge/core.hpp>
#include<sponge/flows.hpp>
namespace sponge
{
	template<typename Flow=ll,template<typename>class Base=mf_graph>
	class gomory_hu_tree:public Base<Flow>
	{
	public:
		using base_type=Base<Flow>;
		vector<vector<pair<int,Flow>>> tr={vector<pair<int,Flow>>()};
		vector<int> dep={0};
		vector<vector<int>> fa={vector<int>()};
		vector<vector<Flow>> mn={vector<Flow>()};
		gomory_hu_tree():base_type(){}
		gomory_hu_tree(int n):base_type(n)
		{
			tr.resize(n+1);
			dep.resize(n+1);
			fa.assign(n+1,vector<int>(__lg(n)+1));
			mn.assign(n+1,vector<Flow>(__lg(n)+1,inf<Flow>));
		}
		gomory_hu_tree(int n,int m):base_type(n,m)
		{
			tr.resize(n+1);
			dep.resize(n+1);
			fa.assign(n+1,vector<int>(__lg(n)+1));
			mn.assign(n+1,vector<Flow>(__lg(n)+1,inf<Flow>));
		}
		void resize(int n)
		{
			base_type::resize(n);
			tr.resize(n+1);
			dep.resize(n+1);
			fa.assign(n+1,vector<int>(__lg(n)+1));
			mn.assign(n+1,vector<Flow>(__lg(n)+1,inf<Flow>));
		}
		void clear()
		{
			*this=gomory_hu_tree();
		}
		int alloc()=delete;
		void dfs(int x,int f)
		{
			dep[x]=dep[f]+1;
			fa[x][0]=f;
			for(auto[y,w]:tr[x])
				if(y!=f)
				{
					mn[y][0]=w;
					dfs(y,x);
				}
		}
		Flow query(int x,int y)
		{
			Flow res=inf<Flow>;
			if(dep[x]<dep[y])swap(x,y);
			int d=dep[x]-dep[y];
			for(int k=__lg(d);k>=0;k--)
			{
				if(d>>k&1)
				{
					res=min(res,mn[x][k]);
					x=fa[x][k];
				}
			}
			if(x==y)return res;
			for(int k=__lg(dep[x]);k>=0;k--)
			{
				if(fa[x][k]!=fa[y][k])
				{
					res=min(res,mn[x][k]);
					res=min(res,mn[y][k]);
					x=fa[x][k];
					y=fa[y][k];
				}
			}
			res=min(res,mn[x][0]);
			res=min(res,mn[y][0]);
			return res;
		}
		void build()
		{
			auto dnc=[&](auto&& dnc,const vector<int>& v)->void
			{
				if(ssize(v)==1)return;
				int s=v[0],t=v[1];
				base_type copy(static_cast<base_type&>(*this));
				copy.s=s;
				copy.t=t;
				Flow min_cut=copy.mf();
				tr[s].push_back({t,min_cut});
				tr[t].push_back({s,min_cut});
				vector<char> vis(base_type::v+1);
				auto dfs=[&](auto&& dfs,int x)->void
				{
					vis[x]=1;
					for(int i=copy.head[x];i;i=copy.next[i])
					{
						int y=copy.son[i];
						if(!vis[y]&&copy.cap[i]>0)dfs(dfs,y);
					}
				};
				dfs(dfs,s);
				vector<int> l,r;
				for(int i:v)(vis[i]?l:r).push_back(i);
				dnc(dnc,l),dnc(dnc,r);
			};
			vector<int> v(base_type::v);
			iota(v.begin(),v.end(),1);
			dnc(dnc,v);
			dep[0]=0;
			dfs(1,0);
			for(int x=1;x<=base_type::v;x++)
			{
				for(int i=1;i<=__lg(dep[x]);i++)
				{
					fa[x][i]=fa[fa[x][i-1]][i-1];
					mn[x][i]=min(mn[x][i-1],mn[fa[x][i-1]][i-1]);
				}
			}
		}
	};
}
#endif