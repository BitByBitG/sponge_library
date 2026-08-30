#ifndef SPONGE_FLOW3_HPP
#define SPONGE_FLOW3_HPP
#include<sponge/core.hpp>
#include<sponge/flows.hpp>
namespace sponge
{
	template<typename Flow=ll,template<typename>class Base=mf_graph>
	class bounded_flow:public Base<Flow>
	{
	public:
		using base_type=Base<Flow>;
		using base_type::v,base_type::e,base_type::s,base_type::t;
		vector<Flow>excess={0};
		bounded_flow():base_type(){}
		bounded_flow(int n):base_type(n)
		{
			excess.assign(n+1,0);
		}
		bounded_flow(int n,int m):base_type(n,m)
		{
			excess.assign(n+1,0);
		}
		void resize(int n)
		{
			base_type::resize(n);
			excess.assign(n+1,0);
		}
		void clear()
		{
			*this=bounded_flow();
		}
		int alloc()
		{
			excess.push_back(0);
			return base_type::alloc();
		}
		void link(int u,int v,Flow l,Flow r)
		{
			if(r-l>0)base_type::link(u,v,r-l);
			excess[u]-=l;
			excess[v]+=l;
		}
		bool feasible()
		{
			int s0=this->alloc();
			int t0=this->alloc();
			Flow sum=0;
			for(int i=1;i<=v;i++)
			{
				if(excess[i]>0)base_type::link(s0,i,excess[i]),sum+=excess[i];
				else if(excess[i]<0)base_type::link(i,t0,-excess[i]);
			}
			s=s0,t=t0;
			return this->mf()==sum;
		}
		Flow maxf(int s0,int t0)
		{
			base_type::link(t0,s0,inf<Flow>);
			if(!feasible())return -1;
			s=s0,t=t0;
			base_type::maxflow=0;
			return this->mf();
		}
		Flow minf(int s0,int t0)
		{
			base_type::link(t0,s0,inf<Flow>);
			int id=e;
			if(!feasible())return -1;
			Flow tmp=base_type::cap[id];
			base_type::cap[id]=base_type::cap[id-1]=0;
			s=t0,t=s0;
			base_type::maxflow=0;
			return tmp-this->mf();
		}
	};
	template<typename Flow=ll,typename Cost=ll,template<typename,typename>class Base=mcmf_graph>
	class bounded_mcf:public Base<Flow,Cost>
	{
	public:
		using base_type=Base<Flow,Cost>;
		using base_type::v,base_type::e,base_type::s,base_type::t;
		vector<Flow>excess={0};
		Cost precost;
		bounded_mcf():base_type(),precost(){}
		bounded_mcf(int n):base_type(n),precost()
		{
			excess.assign(n+1,0);
		}
		bounded_mcf(int n,int m):base_type(n,m),precost()
		{
			excess.assign(n+1,0);
		}
		void resize(int n)
		{
			base_type::resize(n);
			excess.assign(n+1,0);
		}
		void clear()
		{
			*this=bounded_mcf();
		}
		int alloc()
		{
			excess.push_back(0);
			return base_type::alloc();
		}
		void link(int u,int v,Flow l,Flow r,Cost c)
		{
			if(r-l>0)base_type::link(u,v,r-l,c);
			precost+=l*c;
			excess[u]-=l;
			excess[v]+=l;
		}
		bool feasible()
		{
			int s0=this->alloc();
			int t0=this->alloc();
			Flow sum=0;
			for(int i=1;i<=v;i++)
			{
				if(excess[i]>0)base_type::link(s0,i,excess[i],0),sum+=excess[i];
				else if(excess[i]<0)base_type::link(i,t0,-excess[i],0);
			}
			s=s0,t=t0;
			return this->mcmf().first==sum;
		}
		Cost mcf(int s0,int t0)
		{
			base_type::link(t0,s0,inf<Flow>,0);
			if(!feasible())return -inf<Cost>;
			return precost+base_type::mincost;
		}
	};
}
#endif