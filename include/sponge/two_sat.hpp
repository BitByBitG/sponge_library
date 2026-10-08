#ifndef SPONGE_TWO_SAT_HPP
#define SPONGE_TWO_SAT_HPP
#include<sponge/core.hpp>
#include<sponge/tarjan.hpp>
namespace sponge
{
	enum ts_ret:bool
	{
		ts_sat=1,ts_unsat=0
	};
	enum ts_op
	{
		ts_or,ts_nand,ts_imply,ts_equal,
		ts_xor,ts_true,ts_false,ts_and
	};
	class two_sat
	{
	public:
		int n=0;
		scc_graph g;
		vector<int> scc;
		two_sat(){}
		two_sat(int _n)
		{
			resize(_n);
		}
		void resize(int _n)
		{
			n=_n;
			g.resize(2*n+2);
		}
		INLINE int neg(int x)
		{
			return x^1;
		}
		INLINE int lit(int x,bool value)
		{
			return x<<1|!value;
		}
		INLINE void eit(int x,int y)
		{
			g.link(neg(x),y);
			g.link(neg(y),x);
		}
		INLINE void imp(int x,int y)
		{
			eit(neg(x),y);
		}
		template<ts_op op=ts_nand>
		void constraint(int x,int y=0)
		{
			x<<=1;
			y<<=1;
			switch(op)
			{
			case ts_or:
				eit(x,y);
				break;
			case ts_nand:
				eit(neg(x),neg(y));
				break;
			case ts_imply:
				imp(x,y);
				break;
			case ts_equal:
				imp(x,y);
				imp(y,x);
				break;
			case ts_xor:
				eit(x,y);
				eit(neg(x),neg(y));
				break;
			case ts_true:
				eit(x,x);
				break;
			case ts_false:
				eit(neg(x),neg(x));
				break;
			case ts_and:
				eit(x,x);
				eit(y,y);
				break;
			}
		}
		ts_ret satisfy()
		{
			scc=g.scc();
			for(int i=1;i<=n;i++)
			{
				int x=i<<1;
				if(scc[x]==scc[neg(x)])return ts_unsat;
			}
			return ts_sat;
		}
		vector<uint8_t> solution()
		{
			vector<uint8_t> ans(n+1);
			for(int i=1;i<=n;i++)
			{
				int x=i<<1;
				ans[i]=(scc[x]<scc[neg(x)]);
			}
			return ans;
		}
	};
}
#endif