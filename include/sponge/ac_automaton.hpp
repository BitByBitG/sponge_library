#ifndef SPONGE_AC_AUTOMATON_HPP
#define SPONGE_AC_AUTOMATON_HPP
#include<sponge/core.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	template<int diff>
	struct diff_mapping
	{
		int operator()(char ch)const{ return ch-diff; }
	};
	using lower_to_num=diff_mapping<97>;
	using upper_to_num=diff_mapping<65>;
	using digit_to_num=diff_mapping<48>;
	struct alpha_to_num
	{
		int operator()(char ch)const{ return ch>=97?ch-97:ch-65; }
	};
	template<
		typename Data,typename InitFn,typename InsertFn,typename BuildFn,
		typename String=string,int _sigma=26,typename Mapping=lower_to_num
	>
	class ac_automaton
	{
	public:
		using string_type=String;
		using value_type=Data;
		static constexpr int sigma=_sigma;
		static constexpr Mapping mapping{};
		static constexpr InitFn init_fn{};
		static constexpr InsertFn insert_fn{};
		static constexpr BuildFn build_fn{};
		class node_t
		{
		public:
			array<int,sigma> son;
			int fail;
			Data data;
			node_t():son{},fail(),data(init_fn()){}
		};
		vector<node_t> trie={node_t(),node_t()};
		INLINE node_t& operator[](int x)
		{
			return trie[x];
		}
		int ncnt=1,pcnt=0;
		vector<int> link;
		ac_automaton(){}
		ac_automaton(int n)
		{
			resize(n);
		}
		void resize(int n)
		{
			trie.resize(n+2);
			link.resize(n+2);
		}
		void clear()
		{
			*this=ac_automaton();
		}
		int alloc()
		{
			trie[++ncnt]=node_t();
			return ncnt;
		}
		int insert(const String& pattern)
		{
			pcnt++;
			int x=1;
			for(char ch:pattern)
			{
				int d=mapping(ch);
				if(!trie[x].son[d])trie[x].son[d]=alloc();
				x=trie[x].son[d];
			}
			trie[x].data=insert_fn(*this,x,pattern);
			return x;
		}
		void build()
		{
			for(int i=0;i<sigma;i++)trie[0].son[i]=1;
			trie[1].fail=0;
			deque<int> q;
			q.push_back(1);
			while(!q.empty())
			{
				int x=q.front();
				q.pop_front();
				int fail=trie[x].fail;
				for(int i=0;i<sigma;i++)
				{
					int y=trie[x].son[i];
					if(!y)trie[x].son[i]=trie[fail].son[i];
					else
					{
						int z=trie[y].fail=trie[fail].son[i];
						trie[y].data=build_fn(*this,y,z);
						q.push_back(y);
					}
				}
			}
		}
	};
	template<typename String=string,int _sigma=26,typename Mapping=lower_to_num>
	using ac_automaton_n=ac_automaton<null_t,null_id,null_auto,null_auto,String,_sigma,Mapping>;
}
#endif