#ifndef SPONGE_HLPP_HPP
#define SPONGE_HLPP_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename Flow=ll>
	class hlpp_graph
	{
	public:
		using flow_type=Flow;
		int freq=8;
		int v=0,e=1,s=0,t=0;
		Flow maxflow=0;
		int highest=0,max_gap=0,relabel_time=0;
		vector<int> head={0},cur={0},dis={0};
		vector<Flow> excess={0};
		vector<int> gap_head={0},gap_next={0},gap_prev={0},active_head={0},active_next={0};
		vector<int> next={0,0},son={0,0};
		vector<Flow> cap={0,0};
		hlpp_graph(){}
		hlpp_graph(int n)
		{
			resize(n);
		}
		hlpp_graph(int n,int m)
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
			excess.resize(n);
			gap_head.resize(n);
			gap_prev.resize(n);
			gap_next.resize(n);
			active_head.resize(n);
			active_next.resize(n);
		}
		void reserve_vtx(int n)
		{
			++n;
			head.reserve(n);
			cur.reserve(n);
			dis.reserve(n);
			excess.reserve(n);
			gap_head.reserve(n);
			gap_prev.reserve(n);
			gap_next.reserve(n);
			active_head.reserve(n);
			active_next.reserve(n);
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
			*this=hlpp_graph();
		}
		int alloc()
		{
			head.push_back(0);
			cur.push_back(0);
			dis.push_back(0);
			excess.push_back(0);
			gap_head.push_back(0);
			gap_prev.push_back(0);
			gap_next.push_back(0);
			active_head.push_back(0);
			active_next.push_back(0);
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
			link(x,y,_cap);
			link(y,x,_cap);
		}
		inline void push(int x,int y,int i,const Flow flow)
		{
			if(!excess[y]&&y!=t)
			{
				active_next[y]=active_head[dis[y]];
				active_head[dis[y]]=y;
			}
			cap[i]-=flow;
			cap[i^1]+=flow;
			excess[x]-=flow;
			excess[y]+=flow;
		}
		void relabel(int x,int label)
		{
			if(gap_prev[x])
			{
				if(gap_prev[x]==x)
				{
					gap_prev[gap_next[x]]=gap_next[x];
					gap_head[dis[x]]=gap_next[x];
				}
				else
				{
					gap_next[gap_prev[x]]=gap_next[x];
					if(gap_next[x])gap_prev[gap_next[x]]=gap_prev[x];
				}
			}
			if((dis[x]=label)>=v)return;
			max_gap=max(max_gap,dis[x]);
			if(excess[x]>0)
			{
				highest=max(highest,dis[x]);
				active_next[x]=active_head[dis[x]];
				active_head[dis[x]]=x;
			}
			if((gap_next[x]=gap_head[dis[x]]))gap_prev[gap_next[x]]=x;
			gap_head[dis[x]]=gap_prev[x]=x;
		}
		void discharge(int x)
		{
			int d=v;
			for(int i=cur[x];i;i=next[i])
			{
				int y=son[i];
				if(cap[i]>0)
				{
					if(dis[x]==dis[y]+1)
					{
						push(x,y,i,min(excess[x],cap[i]));
						if(!excess[x])return cur[x]=i,[]{}();
					}
					else d=min(d,dis[y]+1);
				}
			}
			for(int i=head[x];i!=cur[x];i=next[i])
			{
				int y=son[i];
				if(cap[i]>0)d=min(d,dis[y]+1);
			}
			cur[x]=head[x];
			relabel_time++;
			if(gap_next[gap_head[dis[x]]])relabel(x,d);
			else
			{
				int tmp=dis[x];
				for(int i=tmp;i<=max_gap;i++)
				{
					for(int j=gap_head[i];j;j=gap_next[j])dis[j]=v;
					gap_head[i]=0;
				}
				max_gap=tmp-1;
			}
		}
		void global_relabel()
		{
			relabel_time=highest=max_gap=0;
			fill(dis.begin(),dis.end(),v);
			fill(gap_head.begin(),gap_head.end(),0);
			fill(gap_prev.begin(),gap_prev.end(),0);
			fill(gap_next.begin(),gap_next.end(),0);
			fill(active_head.begin(),active_head.end(),0);
			fill(active_next.begin(),active_next.end(),0);
			copy(head.begin(),head.end(),cur.begin());
			deque<int>dq;
			dis[t]=0;
			dq.push_back(t);
			while(!dq.empty())
			{
				int x=dq.front();
				dq.pop_front();
				for(int i=head[x];i;i=next[i])
				{
					int y=son[i];
					if(cap[i^1]>0&&y!=s&&dis[y]==v)
					{
						relabel(y,dis[x]+1);
						dq.push_back(y);
					}
				}
			}
		}
		void highest_label_preflow_push()
		{
			global_relabel();
			for(int i=head[s];i;i=next[i])
				if(cap[i])
				{
					int y=son[i];
					push(s,y,i,cap[i]);
					highest=max(highest,dis[y]);
				}
			for(;highest>=0;highest--)
				while(active_head[highest])
				{
					int x=active_head[highest];
					active_head[highest]=active_next[x];
					discharge(x);
					if(relabel_time>v*freq)global_relabel();
				}
			maxflow=excess[t];
		}
		inline Flow mf()
		{
			highest_label_preflow_push();
			return maxflow;
		}
	};
}
#endif