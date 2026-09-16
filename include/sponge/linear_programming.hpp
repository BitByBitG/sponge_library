#ifndef SPONGE_LINEAR_PROGRAMMING_HPP
#define SPONGE_LINEAR_PROGRAMMING_HPP
#include<sponge/core.hpp>
namespace sponge
{
	enum lp_ret
	{
		lp_infeasible,
		lp_unbounded,
		lp_optimal
	};
	enum lp_op
	{
		lp_le,
		lp_eq,
		lp_ge
	};
	template<typename T=ld>
	class lp
	{
	public:
		int n;
		vector<T> c,b;
		vector<vector<T>> a;
		lp():n(){}
		lp(int _n)
		{
			resize(_n);
		}
		lp(int _n,int _m)
		{
			resize(_n);
			reserve(_m);
		}
		void resize(int _n)
		{
			n=_n;
			c.resize(n);
		}
		void reserve(int _m)
		{
			a.reserve(_m);
			b.reserve(_m);
		}
		template<lp_op op=lp_le>
		void constraint(vector<T> _a,T _b)
		{
			if constexpr(op==lp_le||op==lp_eq)
			{
				a.push_back(_a);
				b.push_back(_b);
			}
			if constexpr(op==lp_ge||op==lp_eq)
			{
				for(auto& i:_a)i=-i;
				_b=-_b;
				a.push_back(_a);
				b.push_back(_b);
			}
		}
	};
	template<typename T=ld>
	class simplex
	{
	public:
		int n,m;
		vector<vector<T>> t;
		vector<int> b,nb;
		simplex():n(),m(){}
		template<typename U>
		simplex(const vector<U>& c,const vector<vector<U>>& a,const vector<U>& b):
			n(c.size()),m(b.size()),t(n+1,vector<T>(m+2))
		{
			for(int i=0;i<n;i++)t[i][m]=-c[i];
			for(int j=0;j<m;j++)
			{
				for(int i=0;i<n;i++)t[i][j]=a[j][i];
				t[n][j]=b[j];
			}
		}
		template<typename U>
		simplex(lp<U> p):
			simplex(p.c,p.a,p.b)
		{}
		void pivot(int x,int y)
		{
			swap(nb[x],b[y]);
			T v=-1/t[x][y];
			for(int j=0;j<=m+1;j++)t[x][j]=j==y?-v:v*t[x][j];
			for(int i=0;i<=n;i++)
			{
				if(i==x)continue;
				v=t[i][y];
				t[i][y]=0;
				for(int j=0;j<=m+1;j++)t[i][j]+=v*t[x][j];
			}
		}
		bool phase1()
		{
			int cnt=0;
			for(int j=0;j<m;j++)
			{
				if(t[n][j]<-eps<T>)
				{
					for(int i=0;i<=n;i++)t[i][m+1]+=t[i][j];
					b[j]=~b[j];
					cnt++;
				}
			}
			while(cnt)
			{
				int x=-1;
				T mi=-eps<T>;
				for(int i=0;i<n;i++)
				{
					if(t[i][m+1]<mi)
					{
						x=i;
						mi=t[i][m+1];
					}
				}
				if(x==-1)return 0;
				int y=-1;
				mi=inf<T>;
				for(int j=0;j<m;j++)
				{
					if((b[j]<0&&t[x][j]<-eps<T>)||(b[j]>=0&&t[x][j]>eps<T>))
					{
						T tmp=t[n][j]/t[x][j]+(b[j]<0?-eps<T>:eps<T>);
						if(tmp<mi)y=j,mi=tmp;
					}
				}
				if(y==-1)return 0;
				if(b[y]<0)
				{
					cnt--;
					b[y]=~b[y];
					pivot(x,y);
					t[x][m+1]+=1;
				}
				else pivot(x,y);
			}
			return 1;
		}
		bool phase2()
		{
			while(1)
			{
				int x=-1;
				T mi=-eps<T>;
				for(int i=0;i<n;i++)
				{
					if(t[i][m]<mi)
					{
						x=i;
						mi=t[i][m];
					}
				}
				if(x==-1)break;
				int y=-1;
				mi=inf<T>;
				for(int j=0;j<m;j++)
				{
					if(t[x][j]<=eps<T>)continue;
					T tmp=t[n][j]/t[x][j];
					if(tmp<mi)
					{
						y=j;
						mi=tmp;
					}
				}
				if(y==-1)return 0;
				pivot(x,y);
			}
			return 1;
		}
		lp_ret lp()
		{
			b.resize(m);
			nb.resize(n);
			iota(b.begin(),b.end(),n);
			iota(nb.begin(),nb.end(),0);
			return phase1()?phase2()?lp_optimal:lp_unbounded:lp_infeasible;
		}
		T maximize()
		{
			return t[n][m];
		}
		vector<T> solution()
		{
			vector<T> x(n);
			for(int j=0;j<m;j++)if(b[j]<n)x[b[j]]=t[n][j];
			return x;
		}
	};
}
#endif