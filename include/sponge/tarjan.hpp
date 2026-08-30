#ifndef SPONGE_TARJAN_HPP
#define SPONGE_TARJAN_HPP
#include<sponge/core.hpp>
namespace sponge
{
	class scc_graph
	{
	public:
		int v=0;
		vector<basic_string<int>> g;
		scc_graph(){}
		scc_graph(int n)
		{
			resize(n);
		}
		void resize(int n)
		{
			v=n;
			g.resize(n+1);
		}
		void link(int x,int y)
		{
			g[x].push_back(y);
		}
		void linku(int x,int y)
		{
			link(x,y);
			link(y,x);
		}
		vector<int> scc()
		{
			int scc_cnt=0,_dfn=0;
			vector<int> _scc(v+1),dfn(v+1),low(v+1),stk;
			vector<char> vis(v+1);
			auto dfs=[&](auto&& dfs,int x)->void
			{
				dfn[x]=low[x]=++_dfn;
				stk.push_back(x);
				vis[x]=1;
				for(int y:g[x])
				{
					if(!dfn[y])dfs(dfs,y),low[x]=min(low[x],low[y]);
					else if(vis[y])low[x]=min(low[x],dfn[y]);
				}
				if(dfn[x]==low[x])
				{
					scc_cnt++;
					int y;
					do
					{
						y=stk.back();
						stk.pop_back();
						_scc[y]=scc_cnt;
						vis[y]=0;
					}
					while(y!=x);
				}
			};
			for(int i=1;i<=v;i++)if(!dfn[i])dfs(dfs,i);
			return _scc;
		}
	};
	class e_bcc_graph
	{
	public:
		int v=0,e=0;
		vector<vector<pair<int,int>>> g;
		e_bcc_graph(){}
		e_bcc_graph(int n)
		{
			resize(n);
		}
		void resize(int n)
		{
			v=n;
			g.resize(n+1);
		}
		void linku(int x,int y)
		{
			++e;
			g[x].push_back({y,e});
			g[y].push_back({x,e});
		}
		vector<vector<int>> e_bcc()
		{
			int bcc_cnt=0,_dfn=0;
			vector<vector<int>> _bcc;
			vector<int> dfn(v+1),low(v+1),stk;
			vector<char> vis(v+1);
			auto dfs=[&](auto&& dfs,int x,int e)->void
			{
				dfn[x]=low[x]=++_dfn;
				stk.push_back(x);
				for(auto[y,i]:g[x])
				{
					if(i==e)continue;
					if(!dfn[y])
					{
						dfs(dfs,y,i);
						low[x]=min(low[x],low[y]);
					}
					else low[x]=min(low[x],dfn[y]);
				}
				if(dfn[x]==low[x])
				{
					bcc_cnt++;
					_bcc.push_back(vector<int>());
					int y;
					do
					{
						y=stk.back();
						stk.pop_back();
						_bcc[bcc_cnt-1].push_back(y);
					}
					while(y!=x);
				}
			};
			for(int i=1;i<=v;i++)if(!dfn[i])dfs(dfs,i,0);
			return _bcc;
		}
	};
	class v_bcc_graph
	{
	public:
		int v=0;
		vector<basic_string<int>> g;
		v_bcc_graph(){}
		v_bcc_graph(int n)
		{
			resize(n);
		}
		void resize(int n)
		{
			v=n;
			g.resize(n+1);
		}
		void linku(int x,int y)
		{
			if(x==y)return;
			g[x].push_back(y);
			g[y].push_back(x);
		}
		vector<vector<int>> v_bcc()
		{
			int bcc_cnt=0,_dfn=0,root=0;
			vector<vector<int>> _bcc;
			vector<int> dfn(v+1),low(v+1),stk;
			vector<char> vis(v+1);
			auto dfs=[&](auto&& dfs,int x)->void
			{
				dfn[x]=low[x]=++_dfn;
				stk.push_back(x);
				if (x==root&&g[x].empty())
				{
					bcc_cnt++;
					_bcc.push_back(vector<int>());
					_bcc[bcc_cnt-1].push_back(x);
					return;
				}
				for (int y:g[x])
				{
					if(!dfn[y])
					{
						dfs(dfs,y);
						low[x]=min(low[x],low[y]);
						if(low[y]>=dfn[x])
						{
							bcc_cnt++;
							_bcc.push_back(vector<int>());
							int z;
							do
							{
								z=stk.back();
								stk.pop_back();
								_bcc[bcc_cnt-1].push_back(z);
							}
							while(z!=y);
							_bcc[bcc_cnt-1].push_back(x);
						}
					}
					else low[x]=min(low[x],dfn[y]);
				}
			};
			for(int i=1;i<=v;i++)
			{
				if(!dfn[i])
				{
					root=i;
					dfs(dfs,i);
				}
			}
			return _bcc;
		}
	};
}
#endif