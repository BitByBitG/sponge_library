#include<sponge/data_structure>
#include<cassert>
#include<random>
struct id
{
	long long operator()()const
	{
		return 0;
	}
};
struct isid
{
	bool operator()(long long x)const
	{
		return x==0;
	}
};
struct act
{
	long long operator()(long long s,long long t,int n)const
	{
		return s+t*n;
	}
};
using P=sponge::persistent_segtree<long long,long long,id,id,isid,std::plus<long long>,act,std::plus<long long>>;
int main()
{
	std::mt19937 rng(42);
	std::vector<P> trees(1,P(17,[](int i)
	{
		return (long long)i;
	}));
	std::vector<std::vector<long long>> a(1);
	for(int i=1;i<=17;++i)a[0].push_back(i);
	for(int k=0;k<1000;++k)
	{
		int v=rng()%trees.size();
		P t;
		t.derive_from(trees[v]);
		auto b=a[v];
		int l=rng()%17,r=rng()%17;
		if(l>r)std::swap(l,r);
		if(k%2)
		{
			int d=rng()%10;
			t.update(l+1,r+1,d);
			for(int i=l;i<=r;++i)b[i]+=d;
		}else
		{
			t.set(l+1,3);
			b[l]=3;
		}trees.push_back(t);
		a.push_back(b);
		for(int j=0;j<5;++j)
		{
			int z=rng()%trees.size();
			long long s=0;
			for(int i=l;i<=r;++i)s+=a[z][i];
			assert(trees[z].query(l+1,r+1)==s);
			long long lim=rng()%100;
			int right=l;
			long long sum=0;
			while(right<17&&sum+a[z][right]<=lim)sum+=a[z][right++];
			assert(trees[z].max_right(l+1,[&](long long x)
			{
				return x<=lim;
			})==right);
			int left=r;
			sum=0;
			while(left>=0&&sum+a[z][left]<=lim)sum+=a[z][left--];
			assert(trees[z].min_left(r+1,[&](long long x)
			{
				return x<=lim;
			})==left+2);
		}
	}
}
