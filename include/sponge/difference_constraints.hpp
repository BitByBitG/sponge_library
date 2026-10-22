#ifndef SPONGE_DIFFERENCE_CONSTRAINTS_HPP
#define SPONGE_DIFFERENCE_CONSTRAINTS_HPP
#include<sponge/core.hpp>
#include<sponge/utility.hpp>
#include<sponge/shortest_path.hpp>
namespace sponge
{
	template<typename T=ll>
	class difference_constraints:
		public sparse_shortest_path_graph<T>
	{
	public:
		using base_type=sparse_shortest_path_graph<T>;
		using base_type::link,base_type::spfa;
		difference_constraints():base_type(){}
		difference_constraints(int _n):base_type(_n){}
		void clear()
		{
			*this=difference_constraints();
		}
		template<cmp_op op=cmp_le>
		void constraint(int x,int y,const T& w)
		{
			switch(op)
			{
			case cmp_le:
				link(x,y,w);
				break;
			case cmp_ge:
				link(y,x,-w);
				break;
			case cmp_eq:
				link(x,y,w);
				link(y,x,-w);
			}
		}
		optional<vector<T>> solve()
		{
			return spfa();
		}
	};
}
#endif