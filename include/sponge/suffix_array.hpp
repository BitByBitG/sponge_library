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
			int n=str.size(),v=n?int(*max_element(str.begin(),str.end()))+1:0;
			sa.resize(n+1);
			rk.resize(n+1);
			rk2.resize(n+1);
			_cnt.resize(max(n,v)+1);
			id.resize(max(n,v)+1);
			ht.resize(n+1);
			fill(_cnt.begin(),_cnt.end(),0);
			fill(rk.begin(),rk.end(),0);
			if(!n)return;
			for(int i=1;i<=n;i++)rk[i]=int(str[i-1])+1;
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
					if(i>1&&rk2[sa[i]]==rk2[sa[i-1]]&&(sa[i]+k<=n?rk2[sa[i]+k]:0)==(sa[i-1]+k<=n?rk2[sa[i-1]+k]:0))rk[sa[i]]=tmp;
					else rk[sa[i]]=++tmp;
				}
				if(tmp==n)break;
				v=tmp;
			}
			get_height(str);
		}
		template<typename String>
		void get_height(const String& str)
		{
			int n=str.size();
			ht.assign(n+1,0);
			for(int i=1,j=0;i<=n;i++)
			{
				if(rk[i]==1){j=0;continue;}
				if(j)j--;
				int p=sa[rk[i]-1];
				while(i+j<=n&&p+j<=n&&str[i+j-1]==str[p+j-1])j++;
				ht[rk[i]]=j;
			}
		}
	};
	class sa_is:public suffix_array
	{
	private:
		// Zero-based SA-IS; the last symbol is a unique, smallest sentinel.
		static vector<int> solve(const vector<int>& s,int upper)
		{
			int n=s.size();
			if(n==1)return vector<int>(1,0);
			vector<int> sa(n,-1),cnt(upper+1),head(upper+1),tail(upper+1);
			vector<bool> is_s(n);
			is_s[n-1]=true;
			for(int i=n-2;i>=0;i--)is_s[i]=s[i]==s[i+1]?is_s[i+1]:s[i]<s[i+1];
			for(int c:s)cnt[c]++;
			for(int c=0,sum=0;c<=upper;c++)
			{
				head[c]=sum;
				sum+=cnt[c];
				tail[c]=sum;
			}
			auto induce=[&](const vector<int>& lms)
			{
				fill(sa.begin(),sa.end(),-1);
				vector<int> pos=tail;
				for(int i=(int)lms.size()-1;i>=0;i--)sa[--pos[s[lms[i]]]]=lms[i];
				pos=head;
				for(int i=0;i<n;i++)
					if(sa[i]>0&&!is_s[sa[i]-1]){int p=sa[i]-1;sa[pos[s[p]]++]=p;}
				pos=tail;
				for(int i=n-1;i>=0;i--)
					if(sa[i]>0&&is_s[sa[i]-1]){int p=sa[i]-1;sa[--pos[s[p]]]=p;}
			};
			vector<int> lms,index(n,-1);
			for(int i=1;i<n;i++)if(is_s[i]&&!is_s[i-1])
			{
				index[i]=lms.size();
				lms.push_back(i);
			}
			induce(lms);
			vector<int> sorted;
			for(int p:sa)if(p>=0&&index[p]!=-1)sorted.push_back(p);
			int m=lms.size(),name=0;
			vector<int> reduced(m);
			reduced[index[sorted[0]]]=0;
			for(int i=1;i<m;i++)
			{
				int a=sorted[i-1],b=sorted[i];
				bool same=true;
				for(int d=0;;d++)
				{
					if(a+d>=n||b+d>=n){same=false;break;}
					bool al=d>0&&index[a+d]!=-1,bl=d>0&&index[b+d]!=-1;
					if(s[a+d]!=s[b+d]||is_s[a+d]!=is_s[b+d]||al!=bl){same=false;break;}
					if(al)break;
				}
				if(!same)name++;
				reduced[index[b]]=name;
			}
			vector<int> order(m);
			if(name+1==m)for(int i=0;i<m;i++)order[reduced[i]]=i;
			else order=solve(reduced,name);
			for(int i=0;i<m;i++)sorted[i]=lms[order[i]];
			induce(sorted);
			return sa;
		}
	public:
		template<typename String>
		void suffix_sort(const String& str)
		{
			int n=str.size(),upper=0;
			vector<int> s(n+1);
			for(int i=0;i<n;i++){s[i]=int(str[i])+1;upper=max(upper,s[i]);}
			vector<int> order=solve(s,upper);
			sa.assign(n+1,0);
			rk.assign(n+1,0);
			for(int i=1;i<=n;i++){sa[i]=order[i]+1;rk[sa[i]]=i;}
			get_height(str);
		}
	};
}
#endif
