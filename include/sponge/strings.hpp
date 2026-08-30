#ifndef SPONGE_STRINGS_HPP
#define SPONGE_STRINGS_HPP
#include<sponge/core.hpp>
namespace sponge
{
	template<typename RAIte1,typename RAIte2>
	void prefix_function_n(int n,RAIte1 s,RAIte2 pi)
	{
		for(int i=1;i<n;i++)
		{
			int j=pi[i-1];
			while(j>0&&s[i]!=s[j])j=pi[j-1];
			if(s[i]==s[j])j++;
			pi[i]=j;
		}
	}
	template<typename RAIte1,typename RAIte2>
	void prefix_function(RAIte1 first1,RAIte1 last1,RAIte2 first2)
	{
		prefix_function_n(last1-first1,first1,first2);
	}
	template<typename RAIte1,typename RAIte2>
	void z_algorithm_n(int n,RAIte1 s,RAIte2 z)
	{
		for(int i=1,l=0,r=0;i<n;i++)
		{
			if(i<=r&&z[i-l]<r-i+1)z[i]=z[i-l];
			else
			{
				z[i]=max(0,r-i+1);
				while(i+z[i]<n&&s[z[i]]==s[i+z[i]])++z[i];
			}
			if(i+z[i]-1>r)l=i,r=i+z[i]-1;
		}
	}
	template<typename RAIte1,typename RAIte2>
	void z_algorithm(RAIte1 first1,RAIte1 last1,RAIte2 first2)
	{
		z_algorithm_n(last1-first1,first1,first2);
	}
	template<typename String=string>
	vector<int> kmp(const String& text,const String& pattern,const typename String::value_type& sep='#')
	{
		String s=pattern;
		s.push_back(sep);
		for(auto i:text)s.push_back(i);
		vector<int> pi(ssize(s));
		prefix_function(s.begin(),s.end(),pi.begin());
		vector<int> pos;
		for(int i=ssize(pattern)+1;i<ssize(text)+ssize(pattern)+1;i++)
			if(pi[i]==ssize(pattern))
				pos.push_back(i-2*ssize(pattern));
		return pos;
	}
	class suffix_array
	{
	private:
		vector<int> rk2,_cnt,id;
	public:
		vector<int> sa,rk,ht;
		template<typename String>
		void suffix_sort(const String& str)
		{
			int n=str.size()-1,v=*max_element(str.begin()+1,str.end());
			sa.resize(n+1);
			rk.resize(n+1);
			rk2.resize(n+1);
			_cnt.resize(max(n,v)+1);
			id.resize(max(n,v)+1);
			ht.resize(n+1);
			for(int i=1;i<=n;i++)rk[i]=str[i];
			for(int i=1;i<=n;i++)_cnt[rk[i]]++;
			for(int i=1;i<=v;i++)_cnt[i]+=_cnt[i-1];
			for(int i=n;i>=1;i--)sa[_cnt[rk[i]]--]=i;
			for(int k=1;k<=n;k<<=1)
			{
				int tmp=0,cnt=0;
				fill(_cnt.begin(),_cnt.end(),0);
				rk2=rk;
				for(int i=n-k+1;i<=n;i++)id[++cnt]=i;
				for(int i=1;i<=n;i++)if(sa[i]>k)id[++cnt]=sa[i]-k;
				for(int i=1;i<=n;i++)_cnt[rk[i]]++;
				for(int i=1;i<=v;i++)_cnt[i]+=_cnt[i-1];
				for(int i=n;i>=1;i--)sa[_cnt[rk[id[i]]]--]=id[i];
				for(int i=1;i<=n;i++)
				{
					if(rk2[sa[i]]==rk2[sa[i-1]]&&rk2[sa[i]+k]==rk2[sa[i-1]+k])rk[sa[i]]=tmp;
					else rk[sa[i]]=++tmp;
				}
				if(tmp==n)break;
				v=tmp;
			}
			for(int i=1,j=0;i<=n;i++)
			{
				if(j)j--;
				while(str[i+j]==str[sa[rk[i]-1]+j])j++;
				ht[rk[i]]=j;
			}
		}
	};
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
	template<typename String=string,int _sigma=26,typename Mapping=lower_to_num,bool maintain_link=0>
	class ac_automaton
	{
	public:
		using string_type=String;
		static constexpr int sigma=_sigma;
		static constexpr Mapping mapping{};
		class node_t
		{
		public:
			array<int,sigma> son;
			int flag;
			int fail;
			node_t():son{},flag(),fail(){}
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
			if constexpr(maintain_link)link.resize(n+2);
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
			if(!trie[x].flag)trie[x].flag=pcnt;
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
					if(!y)
					{
						trie[x].son[i]=trie[fail].son[i];
						continue;
					}
					int z=trie[y].fail=trie[fail].son[i];
					if constexpr(maintain_link)link[y]=trie[z].flag?z:link[z];
					q.push_back(y);
				}
			}
		}
	};
}
#endif