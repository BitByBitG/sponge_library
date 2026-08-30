#ifndef SPONGE_SHORTEST_PATH_HPP
#define SPONGE_SHORTEST_PATH_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename T>
	class dense_shortest_path_graph
	{
	public:
		int v=0;
		vector<vector<T>> g;
		dense_shortest_path_graph(){}
		dense_shortest_path_graph(int n):v(n)
		{
			resize(n);
		}
		void resize(int n)
		{
			v=n;
			g.assign(n+1,vector<int>(n+1,inf<T>));
		}
		void clear()
		{
			*this=dense_shortest_path_graph();
		}
		void link(int x,int y,const T& v)
		{
			g[x][y]=min(g[x][y],v);
		}
		void linku(int x,int y,const T& v)
		{
			link(x,y,v);
			link(y,x,v);
		}
		vector<vector<T>> floyd()
		{
			auto dis=g;
			for(int k=1;k<=v;k++)
				for(int i=1;i<=v;i++)
					for(int j=1;j<=v;j++)
						dis[i][j]=min(dis[i][j],dis[i][k]+dis[k][j]);
			return dis;
		}
		vector<T> dijkstra(const vector<int>& s)
		{
			vector<T> dis(v+1,inf<T>);
			vector<char> vis(v+1);
			for(int x:s)dis[x]=0;
			for(int i=1;i<=v;i++)
			{
				int x=0;
				T m=inf<T>;
				for(int y=1;y<=v;y++)if(!vis[y]&&dis[y]<m)x=y,m=dis[y];
				vis[x]=1;
				for(int y=1;y<=v;y++)dis[y]=min(dis[y],dis[x]+g[x][y]);
			}
			return dis;
		}
		vector<T> dijkstra(int s)
		{
			return dijkstra({s});
		}
	};
	template<typename T>
	class sparse_shortest_path_graph
	{
	public:
		int v=0;
		vector<vector<pair<int,T>>> g;
		sparse_shortest_path_graph(){}
		sparse_shortest_path_graph(int n):v(n)
		{
			resize(n);
		}
		void resize(int n)
		{
			v=n;
			g.resize(n+1);
		}
		void clear()
		{
			*this=sparse_shortest_path_graph();
		}
		void link(int x,int y,const T& v)
		{
			g[x].push_back({y,v});
		}
		void linku(int x,int y,const T& v)
		{
			link(x,y);
			link(y,x);
		}
		vector<T> dijkstra(const vector<int>& s)
		{
			vector<T> dis(v+1,inf<T>);
			priority_queue<pair<T,int>,vector<pair<T,int>>,greater<>> pq;
			vector<char> vis(v+1);
			for(int x:s)pq.push({dis[x]=0,x});
			while(!pq.empty())
			{
				auto[dis_x,x]=pq.top();pq.pop();
				if(vis[x])continue;
				vis[x]=1;
				for(auto[y,w]:g[x])
				{
					T dis_y=dis_x+w;
					if(dis[y]>dis_y)pq.push({dis[y]=dis_y,y});
				}
			}
			return dis;
		}
		vector<T> dijkstra(int s)
		{
			return dijkstra({s});
		}
		vector<T> spfa(const vector<int>& s)
		{
			vector<T> dis(v+1,inf<T>);
			vector<char> vis(v+1);
			deque<int> dq;
			for(int x:s)
			{
				dis[x]=0;
				vis[x]=1;
				dq.push_back(x);
			}
			while(!dq.empty())
			{
				int x=dq.front();
				dq.pop_front();
				vis[x]=0;
				for(auto[y,w]:g[x])
				{
					if(dis[y]>dis[x]+w)
					{
						dis[y]=dis[x]+w;
						if(!vis[y])
						{
							dq.push_back(y);
							vis[y]=1;
						}
					}
				}
			}
			return dis;
		}
		vector<T> spfa(int s)
		{
			return spfa({s});
		}
	};
}
#endif