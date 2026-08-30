#ifndef SPONGELIB_K_SHORTEST_WALK_HPP
#define SPONGELIB_K_SHORTEST_WALK_HPP
#include<sponge/core.hpp>
namespace sponge
{
	namespace detail
	{
		template<typename T>
		class ksw_pheap
		{
		public:
			vector<int> rt;
			struct node_t
			{
				int ls,rs;
				int to;
				T delta;
			};
			vector<node_t> tr;
			int ncnt=0;
			void resize(int n,int m)
			{
				rt.resize(n+1);
				m=max(n,m);
				tr.resize(m*__lg(m)*2+100);
			}
			int alloc(T _delta,int _to)
			{
				tr[++ncnt]={0,0,_to,_delta};
				return ncnt;
			}
			int copy(int x)
			{
				tr[++ncnt]=tr[x];
				return ncnt;
			}
			int meld(int x,int y)
			{
				static mt19937_64 rng64(random_device{}());
				ull r=rng64();
				auto recurse=[&](auto&& recurse,int x,int y)->int
				{
					if(!x||!y)return x|y;
					if(tr[x].delta>tr[y].delta)swap(x,y);
					x=copy(x);
					if(r&1)swap(tr[x].ls,tr[x].rs);
					r>>=1;
					tr[x].rs=recurse(recurse,tr[x].rs,y);
					return x;
				};
				return recurse(recurse,x,y);
			}
			void merge(int x,int y)
			{
				rt[x]=meld(rt[x],rt[y]);
			}
			void insert(int i,T _delta,int _to)
			{
				rt[i]=meld(rt[i],alloc(_delta,_to));
			}
		};
	}
	template<typename T>
	class k_shortest_walk_graph
	{
	public:
		int v,e,s,t;
		vector<vector<int>> g,rg;
		vector<tuple<int,int,T>> ed;
		priority_queue<pair<T,int>,vector<pair<T,int>>,greater<>> pq;
		vector<T> dis_t;
		vector<int> out;
		detail::ksw_pheap<T> h;
		k_shortest_walk_graph():v(),e(),s(),t(){}
		k_shortest_walk_graph(int _v):v(_v),e(),s(),t()
		{
			g.resize(v+1);
			rg.resize(v+1);
		}
		void resize(int _v)
		{
			v=_v;
			g.resize(v+1);
			rg.resize(v+1);
		}
		void link(int x,int y,const T& v)
		{
			ed.push_back({x,y,v});
			g[x].push_back(e);
			rg[y].push_back(e);
			e++;
		}
		void linku(int x,int y,const T& v)
		{
			link(x,y,v);
			link(y,x,v);
		}
		void dijkstra()
		{
			dis_t.assign(v+1,inf<T>);
			dis_t[t]=0;
			pq.push({0,t});
			out.assign(v+1,-1);
			while(!pq.empty())
			{
				auto[dis_x,x]=pq.top();
				pq.pop();
				if(dis_x>dis_t[x])continue;
				for(int i:rg[x])
				{
					int y=get<0>(ed[i]);
					T dis_y=dis_x+get<2>(ed[i]);
					if(dis_y<dis_t[y])
					{
						dis_t[y]=dis_y;
						out[y]=i;
						pq.push({dis_y,y});
					}
				}
			}
		}
		void build_sidetracks()
		{
			h.resize(v,e);
			for(int i=0;i<e;i++)
			{
				auto[x,y,v]=ed[i];
				if(out[x]!=i&&dis_t[x]<inf<T>&&dis_t[y]<inf<T>)
					h.insert(x,v+dis_t[y]-dis_t[x],y);
			}
			deque<int> q;
			q.push_back(t);
			while(!q.empty())
			{
				int x=q.front();
				q.pop_front();
				for(int i:rg[x])
				{
					int y=get<0>(ed[i]);
					if(out[y]==i)
					{
						h.merge(y,x);
						q.push_back(y);
					}
				}
			}
		}
		void insert(int x,const T& v)
		{
			if(x)pq.push({v+h.tr[x].delta,x});
		}
		vector<T> ans;
		void eppstein(int k)
		{
			int cnt=0;
			ans.assign(k+1,-1);
			if(dis_t[s]>=inf<T>)return;
			ans[++cnt]=dis_t[s];
			insert(h.rt[s],dis_t[s]);
			while(!pq.empty()&&cnt<k)
			{
				auto[dis_x,x]=pq.top();
				pq.pop();
				ans[++cnt]=dis_x;
				insert(h.tr[x].ls,dis_x-h.tr[x].delta);
				insert(h.tr[x].rs,dis_x-h.tr[x].delta);
				insert(h.rt[h.tr[x].to],dis_x);
			}
		}
		vector<T>& k_shortest_walk(int k)
		{
			dijkstra();
			build_sidetracks();
			eppstein(k);
			return ans;
		}
	};
}
#endif