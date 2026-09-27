#ifndef SPONGE_STRING_ALGO_HPP
#define SPONGE_STRING_ALGO_HPP
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
}
#endif