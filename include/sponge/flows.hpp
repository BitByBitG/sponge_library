#ifndef SPONGE_FLOWS_HPP
#define SPONGE_FLOWS_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename Flow=ll>
	class dinitz_graph
	{
	public:
		using flow_type=Flow;
		int v=0,e=1,s=0,t=0;
		Flow maxflow=0;
		vector<int> head={0},cur={0},dis={0};
		vector<int> son={0,0},next={0,0};
		vector<Flow> cap={0,0};
		dinitz_graph(){}
		dinitz_graph(int n)
		{
			resize(n);
		}
		dinitz_graph(int n,int m)
		{
			resize(n);
			reserve_edge(m);
		}
		void resize(int n)
		{
			v=n++;
			head.resize(n);
			cur.resize(n);
			dis.resize(n);
		}
		void reserve_vtx(int n)
		{
			++n;
			head.reserve(n);
			cur.reserve(n);
			dis.reserve(n);
		}
		void reserve_edge(int m)
		{
			(m<<=1)+=2;
			son.reserve(m);
			next.reserve(m);
			cap.reserve(m);
		}
		void clear()
		{
			*this=dinitz_graph();
		}
		int alloc()
		{
			head.push_back(0);
			cur.push_back(0);
			dis.push_back(0);
			return ++v;
		}
		void add_edge(int x,int y,const Flow _cap)
		{
			son.push_back(y);
			cap.push_back(_cap);
			next.push_back(head[x]);
			head[x]=++e;
		}
		void link(int x,int y,const Flow _cap)
		{
			add_edge(x,y,_cap);
			add_edge(y,x,0);
		}
		void linku(int x,int y,const Flow _cap)
		{
			add_edge(x,y,_cap);
			add_edge(y,x,_cap);
		}
		bool bfs()
		{
			fill(dis.begin(),dis.end(),inf<int>);
			cur=head;
			deque<int>dq;
			dq.push_back(s);
			dis[s]=0;
			while(!dq.empty())
			{
				int x=dq.front();
				dq.pop_front();
				for(int i=head[x];i;i=next[i])
				{
					int y=son[i];
					if(cap[i]>0&&dis[y]==inf<int>)
					{
						dq.push_back(y);
						dis[y]=dis[x]+1;
						if(y==t)return 1;
					}
				}
			}
			return 0;
		}
		Flow dfs(int x,Flow flow)
		{
			if(x==t)return flow;
			Flow res=0;
			for(int i=cur[x];i&&flow;i=next[i])
			{
				int y=son[i];
				cur[x]=i;
				if(cap[i]>0&&dis[y]==dis[x]+1)
				{
					Flow k=dfs(y,min(flow,cap[i]));
					if(k==0)dis[y]=inf<int>;
					cap[i]-=k;
					cap[i^1]+=k;
					res+=k;
					flow-=k;
				}
			}
			return res;
		}
		void dinitz()
		{
			while(bfs())maxflow+=dfs(s,inf<Flow>);
		}
		Flow mf()
		{
			dinitz();
			return maxflow;
		}
	};
	template<typename Flow=ll,typename Cost=ll>
	class pd_graph
	{
	public:
		using flow_type=Flow;
		using cost_type=Cost;
		int v=0,e=1,s=0,t=0;
		Flow maxflow=0;
		Cost mincost=0;
		vector<int> head={0},cur={0},pre_v={0},pre_e={0};
		vector<char> vis={0};
		vector<Cost> dis={0},h={0};
		vector<int> son={0,0},next={0,0};
		vector<Flow> cap={0,0};
		vector<Cost> cost={0,0};
		pd_graph(){}
		pd_graph(int n)
		{
			resize(n);
		}
		pd_graph(int n,int m)
		{
			resize(n);
			reserve_edge(m);
		}
		void resize(int n)
		{
			v=n++;
			head.resize(n);
			cur.resize(n);
			pre_v.resize(n);
			pre_e.resize(n);
			vis.resize(n);
			dis.resize(n);
			h.resize(n);
		}
		void reserve_vtx(int n)
		{
			++n;
			head.reserve(n);
			cur.reserve(n);
			pre_v.reserve(n);
			pre_e.reserve(n);
			vis.reserve(n);
			dis.reserve(n);
			h.reserve(n);
		}
		void reserve_edge(int m)
		{
			(m<<=1)+=2;
			son.reserve(m);
			next.reserve(m);
			cap.reserve(m);
			cost.reserve(m);
		}
		void clear()
		{
			*this=pd_graph();
		}
		int alloc()
		{
			head.push_back(0);
			cur.push_back(0);
			pre_v.push_back(0);
			pre_e.push_back(0);
			vis.push_back(0);
			dis.push_back(0);
			h.push_back(0);
			return ++v;
		}
		void add_edge(int x,int y,const Flow _cap,const Cost _cost)
		{
			son.push_back(y);
			cap.push_back(_cap);
			cost.push_back(_cost);
			next.push_back(head[x]);
			head[x]=++e;
		}
		void link(int x,int y,const Flow _cap,const Cost _cost)
		{
			add_edge(x,y,_cap,_cost);
			add_edge(y,x,0,-_cost);
		}
		void linku(int x,int y,const Flow _cap,const Cost _cost)
		{
			link(x,y,_cap,_cost);
			link(y,x,_cap,_cost);
		}
		void spfa()
		{
			deque<int>dq;
			fill(h.begin(),h.end(),inf<Cost>);
			fill(vis.begin(),vis.end(),0);
			vis[s]=1;
			h[s]=0;
			dq.push_back(s);
			while(!dq.empty())
			{
				int x=dq.front();
				dq.pop_front();
				vis[x]=0;
				for(int i=head[x];i;i=next[i])
				{
					int y=son[i];
					if(cap[i]>0&&h[y]>h[x]+cost[i])
					{
						h[y]=h[x]+cost[i];
						if(!vis[y])
						{
							dq.push_back(y);
							vis[y]=1;
						}
					}
				}
			}
		}
		bool dijkstra()
		{
			priority_queue<pair<Cost,int>,vector<pair<Cost,int>>,greater<pair<Cost,int>>> pq;
			fill(dis.begin(),dis.end(),inf<Cost>);
			fill(vis.begin(),vis.end(),0);
			dis[s]=0;
			pq.push({0,s});
			while(!pq.empty())
			{
				auto[d,x]=pq.top();
				pq.pop();
				if(vis[x])continue;
				vis[x]=1;
				for(int i=head[x];i;i=next[i])
				{
					int y=son[i];
					Cost w=cost[i]+h[x]-h[y];
					if(cap[i]>0&&dis[y]>dis[x]+w)
					{
						dis[y]=dis[x]+w;
						pre_v[y]=x;
						pre_e[y]=i;
						if(!vis[y])pq.push({dis[y],y});
					}
				}
			}
			return dis[t]!=inf<Cost>;
		}
		void augment()
		{
			Flow flow=inf<Flow>;
			for(int i=1;i<=v;i++)
				if(dis[i]!=inf<Cost>)
					h[i]+=dis[i];
			for(int i=t;i!=s;i=pre_v[i])flow=min(flow,cap[pre_e[i]]);
			for(int i=t;i!=s;i=pre_v[i])
			{
				cap[pre_e[i]]-=flow;
				cap[pre_e[i]^1]+=flow;
			}
			maxflow+=flow;
			mincost+=flow*h[t];
		}
		void primal_dual()
		{
			spfa();
			while(dijkstra())augment();
		}
		pair<Flow,Cost> mcmf()
		{
			primal_dual();
			return make_pair(maxflow,mincost);
		}
		pair<Flow,Cost> mcf()
		{
			spfa();
			while(dijkstra())
			{
				if(h[t]+dis[t]>=0)break;
				augment();
			}
			return make_pair(maxflow,mincost);
		}
	};
	template<typename Flow=ll>
	using mf_graph=dinitz_graph<Flow>;
	template<typename Flow=ll,typename Cost=ll>
	using mcmf_graph=pd_graph<Flow,Cost>;
	template<typename Flow=ll,typename Cost=ll>
	using mcf_graph=pd_graph<Flow,Cost>;
}
#endif