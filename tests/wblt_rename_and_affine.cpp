#include<sponge/wblt.hpp>
#include<sponge/wblt_set.hpp>
#include<sponge/persistent_wblt.hpp>
#include<sponge/persistent_segtree.hpp>
#include<cassert>
struct id
{
	long long operator()()const
	{
		return 0;
	}
};
struct tag
{
	long long a,b;
};
struct ti
{
	tag operator()()const
	{
		return
		{
			1,0
		};
	}
};
struct eq
{
	bool operator()(tag t)const
	{
		return t.a==1&&t.b==0;
	}
};
struct apply
{
	long long operator()(long long s,tag t,int n)const
	{
		return s*t.a+t.b*n;
	}
};
struct compose
{
	tag operator()(tag x,tag y)const
	{
		return
		{
			x.a*y.a,x.b*y.a+y.b
		};
	}
};
int main()
{
	using P=sponge::persistent_segtree<long long,tag,id,ti,eq,std::plus<long long>,apply,compose>;
	P x(5,[](int i)
	{
		return i;
	});
	P y;
	y.derive_from(x);
	y.update(1,5,
	{
		2,1
	});
	P z;
	z.derive_from(y);
	z.update(2,4,
	{
		3,2
	});
	assert(x.query()==15);
	assert(y.query()==35);
	assert(z.query(2,4)==69);
	assert(z.query()==83);
	z.set(3,0);
	assert(z.query()==60);
	assert(y.query(3)==7);
	sponge::wblt_set<int> s;
	s.insert(2);
	assert(s.size()==1);
	using W=sponge::wblt_n<long long,id,std::plus<long long>>;
	W::fr.reserve(100);
	W w;
	w.build(3,[](int i)
	{
		return i;
	});
	assert(w.query(1,3)==6);
	using PW=sponge::persistent_wblt_n<long long,id,std::plus<long long>>;
	PW::fr.reserve(100);
	PW p;
	p.build(3,[](int i)
	{
		return i;
	});
	PW q;
	q.derive_from(p);
	q.set_pos(2,9);
	assert(p.query(1,3)==6);
	assert(q.query(1,3)==13);
	P empty;
	assert(empty.query()==0);
	assert(empty.max_right(1,[](long long)
	{
		return true;
	})==0);
}
