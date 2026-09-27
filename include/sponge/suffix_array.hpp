#ifndef SPONGE_SUFFIX_ARRAY_HPP
#define SPONGE_SUFFIX_ARRAY_HPP
#include<sponge/core.hpp>
namespace sponge
{
	class suffix_array
	{
	private:
		vector<int> rk2,_cnt,id;
	public:
		vector<int> sa,rk,ht;
		template<typename String>
		void suffix_sort(const String& str)
		{
			int n=str.size(),v=*max_element(str.begin(),str.end());
			sa.resize(n+1);
			rk.resize(n+1);
			rk2.resize(n+1);
			_cnt.resize(max(n,v)+1);
			id.resize(max(n,v)+1);
			ht.resize(n+1);
			for(int i=1;i<=n;i++)rk[i]=str[i-1];
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
				if(rk[i]==1)continue;
				if(j)j--;
				int p=sa[rk[i]-1];
				while(i+j<=n&&p+j<=n&&str[i+j-1]==str[p+j-1])j++;
				ht[rk[i]]=j;
			}
		}
	};
}
#endif