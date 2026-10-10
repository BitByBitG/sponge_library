#ifndef SPONGE_EULER_PATH_HPP
#define SPONGE_EULER_PATH_HPP
#include<sponge/core.hpp>
#include<sponge/utility.hpp>
namespace sponge
{
	class hierholzer_graph
	{
		int v=0,e=0;
		vector<vector<pair<int,int>>> g;
		hierholzer_graph(){}
		hierholzer_graph(int n)
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
		vector<int> hierholzer(int s)
		{
			vector<uint8_t> vis(e+1);
			vector<int> ptr(v+1);
			for(int i=1;i<=v;i++)ptr[i]=g[i].size()-1;
			vector<int> path;
			auto dfs=[&](auto&& self,int x)->void
			{
				while(ptr[x]>=0)
				{
					auto[y,id]=g[x][ptr[x]];
					ptr[x]--;
					if(vis[id])continue;
					vis[id]=1;
					self(self,y);
				}
				path.push_back(x);
			};
			dfs(dfs,s);
			reverse(path.begin(),path.end());
			return path;
		}
		optional<vector<int>> find_euler_path()
		{
			int odd_cnt=0,s=1;
			for(int i=1;i<=v;i++)if(ssize(g[i])&1)odd_cnt++,s=i;
			if(odd_cnt!=0&&odd_cnt!=2)return nullopt;
			auto path=hierholzer(s);
			if(ssize(path)!=e+1)return nullopt;
			return path;
		}
		optional<vector<int>> find_euler_cycle()
		{
			int odd_cnt=0;
			for(int i=1;i<=v;i++)if(ssize(g[i])&1)return nullopt;
			auto path=hierholzer(1);
			if(ssize(path)!=e+1)return nullopt;
			return path;
		}
	};
	class directed_hierholzer_graph
	{
		int v=0,e=0;
		vector<vector<int>> g;
		vector<int> in;
	public:
		directed_hierholzer_graph(){}
		directed_hierholzer_graph(int n)
		{
			resize(n);
		}
		void resize(int n)
		{
			v=n;
			g.resize(n+1);
			in.resize(n+1);
		}
		void link(int x,int y)
		{
			++e;
			g[x].push_back(y);
			in[y]++;
		}
		vector<int> hierholzer(int s)
		{
			vector<int> ptr(v+1);
			for(int i=1;i<=v;i++)ptr[i]=ssize(g[i])-1;
			vector<int> path;
			auto dfs=[&](auto&& self,int x)->void
			{
				while(ptr[x]>=0)
				{
					int y=g[x][ptr[x]];
					ptr[x]--;
					self(self,y);
				}
				path.push_back(x);
			};
			dfs(dfs,s);
			reverse(path.begin(),path.end());
			return path;
		}
		optional<vector<int>> find_euler_path()
		{
			if(!v)return nullopt;
			int s=0,t=0;
			for(int i=1;i<=v;i++)
			{
				int d=ssize(g[i])-in[i];
				if(d==1)
				{
					if(s)return nullopt;
					s=i;
				}
				else if(d==-1)
				{
					if(t)return nullopt;
					t=i;
				}
				else if(d!=0)return nullopt;
			}
			if(bool(s)!=bool(t))return nullopt;
			if(!s)
			{
				s=1;
				for(int i=1;i<=v;i++)if(!g[i].empty())
				{
					s=i;
					break;
				}
			}
			auto path=hierholzer(s);
			if(ssize(path)!=e+1)return nullopt;
			return path;
		}
		optional<vector<int>> find_euler_cycle()
		{
			if(!v)return nullopt;
			int s=1;
			for(int i=1;i<=v;i++)
			{
				if(ssize(g[i])!=in[i])return nullopt;
				if(!g[i].empty())s=i;
			}
			auto path=hierholzer(s);
			if(ssize(path)!=e+1)return nullopt;
			return path;
		}
	};
}
#endif