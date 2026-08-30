#ifndef SPONGE_MAX_INDEPENDENT_SET_HPP
#define SPONGE_MAX_INDEPENDENT_SET_HPP
#include<sponge/core.hpp>
namespace sponge
{
	class max_independent_set_graph
	{
	public:
		static ull ctz(const ull x)
		{
			return __builtin_ctzll(x);
		}
		static ull cnt(const ull x)
		{
			return __builtin_popcountll(x);
		}
		static int chain(int x)
		{
			return x&1?1:(x>>1)+1;
		}
		static int cycle(int x)
		{
			return x&1?x:2;
		}
		static vector<int> vertices(ull x)
		{
			vector<int> v;
			for(ull i=x;i;i&=i-1)v.push_back(ctz(i)+1);
			return v;
		}
		int v=0;
		ull e[64];
		max_independent_set_graph()
		{
			memset(e,0,sizeof(e));
		}
		max_independent_set_graph(int n)
		{
			memset(e,0,sizeof(e));
			resize(n);
		}
		void resize(int n)
		{
			v=n;
		}
		void clear()
		{
			*this=max_independent_set_graph();
		}
		void linku(int x,int y)
		{
			--x,--y;
			e[x]|=1ull<<y;
			e[y]|=1ull<<x;
		}
		void inverse()
		{
			ull u=(1ull<<v)-1;
			for(int i=0;i<v;i++)e[i]=(~e[i]&u)^(1ull<<i);
		}
		pair<ull,int> dfs(ull s)
		{
			if(!s)return {0,1};
			int d=-1,p=0;
			for(ull i=s;i;i&=i-1)
			{
				int x=ctz(i),c=cnt(e[x]&s);
				if(c>d)d=c,p=x;
			}
			if(d<=2)
			{
				ull v=s,x=0,s1,s2;
				int y=1;
				bool b2;
				auto dfs=[&](int x,bool b,auto &&dfs)->void
				{
					v^=1ull<<x;
					(b?s2:s1)|=1ull<<x;
					b2&=(cnt(e[x]&s)==2);
					while(v&e[x])dfs(ctz(v&e[x]),b^1,dfs);
				};
				while(v)
				{
					s1=s2=0,b2=1;
					dfs(ctz(v),0,dfs);
					int a=cnt(s1),b=cnt(s2),c=a+b;
					b2&&(c&1)?x|=(a<b?s1:s2):x|=(a>b?s1:s2);
					y*=(b2?cycle(c):chain(c));
				}
				return {x,y};
			}
			ull bp=1ull<<p;
			pair<ull,int>ans=dfs(s^bp);
			auto[x,y]=dfs(s&~(e[p]|bp));
			x|=bp;
			if(cnt(x)>cnt(ans.first))ans={x,y};
			else if(cnt(x)==cnt(ans.first))ans.second+=y;
			return ans;
		}
		pair<ull,int> max_independent_set()
		{
			return dfs((1ull<<v)-1);
		}
	};
}
#endif