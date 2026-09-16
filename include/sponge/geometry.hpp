#ifndef SPONGE_GEOMETRY_HPP
#define SPONGE_GEOMETRY_HPP
#include<sponge/core.hpp>
#include<sponge/utility.hpp>
#include<sponge/math.hpp>
namespace sponge
{
	template<typename T=ld,typename U=ld>
	class vec
	{
	public:
		T x,y;
		vec():x(),y(){}
		vec(T _x,T _y):x(_x),y(_y){}
		template<typename V,typename W>
		vec(vec<V,W> v):x(v.x),y(v.y){}
		vec operator+(const vec& v)const
		{ return vec(x+v.x,y+v.y); }
		vec operator-(const vec& v)const
		{ return vec(x-v.x,y-v.y); }
		vec operator-()const
		{ return vec(-x,-y); }
		U operator*(const vec& v)const
		{ return U(x)*v.x+U(y)*v.y; }
		U operator^(const vec& v)const
		{ return U(x)*v.y-U(y)*v.x; }
		U len2()const
		{ return sqr<U>(x)+sqr<U>(y); }
		ld len()const
		{ return sqrt(len2()); }
		ld arg()const
		{ return atan2(y,x); }
		vec<ld,ld> inv()const
		{
			const ld d=len2();
			return vec(x/d,-y/d);
		}
		vec<ld,ld> unit()const
		{
			const ld d=len();
			return vec(x/d,y/d);
		}
		vec operator*(const T& v)const
		{ return vec(x*v,y*v); }
		vec operator/(const T& v)const
		{ return vec(x/v,y/v); }
		vec& operator+=(const vec& v)
		{ return *this=*this+v; }
		vec& operator-=(const vec& v)
		{ return *this=*this-v; }
		vec& operator*=(const T& v)
		{ return *this=*this*v; }
		vec& operator/=(const T& v)
		{ return *this=*this/v; }
		friend int cmp(const vec& a,const vec& b)
		{
			int k=sgn(b.x-a.x);
			return k?k:sgn(b.y-a.y);
		}
		bool operator<(const vec& v)const
		{ return cmp(*this,v)>0; }
		bool operator>(const vec& v)const
		{ return cmp(*this,v)<0; }
		bool operator<=(const vec& v)const
		{ return cmp(*this,v)>=0; }
		bool operator>=(const vec& v)const
		{ return cmp(*this,v)<=0; }
		bool operator==(const vec& v)const
		{ return cmp(*this,v)==0; }
		bool operator!=(const vec& v)const
		{ return cmp(*this,v); }
		pair<T,T> to_pair()const
		{ return {x,y}; }
		template<typename Istream>
		friend Istream& operator>>(Istream& is,vec& v)
		{ return is>>v.x>>v.y; }
		template<typename Ostream>
		friend Ostream& operator<<(Ostream& os,const vec& v)
		{ return os<<'('<<v.x<<','<<v.y<<')'; }
	};
	template<typename T>
	using point=vec<T>;
	template<typename T=ld>
	using __line_base_t=pair<point<T>,point<T>>;
	template<typename T=ld>
	using __poly_base_t=vector<point<T>>;
	template<typename T=ld>
	class line:public __line_base_t<T>
	{
		using B=__line_base_t<T>;
	public:
		using B::pair;
		line(const B& b):B(b){}
	};
	template<typename T=ld>
	class segment:public __line_base_t<T>
	{
		using B=__line_base_t<T>;
	public:
		using B::pair;
		segment(const B& b):B(b){}
	};
	template<typename T=ld>
	class ray:public __line_base_t<T>
	{
		using B=__line_base_t<T>;
	public:
		using B::pair;
		ray(const B& b):B(b){}
	};
	template<typename T=ld>
	class polygon:public __poly_base_t<T>
	{
	public:
		using __poly_base_t<T>::vector;
		template<typename Istream>
		friend Istream& operator>>(Istream& is,polygon& p)
		{
			for(auto& i:p)is>>i;
			return is;
		}
		template<typename Ostream>
		friend Ostream& operator<<(Ostream& os,const polygon& p)
		{
			for(const auto& i:p)os<<i<<' ';
			return os;
		}
		polygon operator-()
		{
			polygon p(this->size());
			for(size_t i=0;i<this->size();i++)p[i]=-this->at(i);
			return p;
		}
	};
	template<typename T=ld>
	using piter=polygon<T>::const_iterator;
	template<typename T>
	class circle
	{
	public:
		vec<T> o;
		T r;
		circle():r(){}
		circle(const vec<T>& _o,const T _r):o(_o),r(_r){}
		circle(const vec<T>& a,const vec<T>& b):
			o((a+b)/2),r(len(o-a))
		{}
		static circle from_3_points(const vec<T>& a,const vec<T>& b,const vec<T>& c)
		{
			vec<T> _b=b-a,_c=c-a;
			ld e=_b*(a+b);
			ld f=_c*(a+c);
			ld g=2*(_b^_c);
			if(!sgn(g))
			{
				const auto dab=len2(b-a);
				const auto dac=len2(a-c);
				const auto dbc=len2(b-c);
				if(dab>=dac&&dab>=dbc)return circle(a,b);
				if(dac>=dbc) return circle(a,c);
				return circle(b,c);
			}
			vec o{(_c.y*e-_b.y*f)/g,(_b.x*f-_c.x*e)/g};
			return {o,len(o-a)};
		}
		circle(const vec<T>& a,const vec<T>& b,const vec<T>& c):
			circle(from_3_points(a,b,c))
		{}
	};
	using vecl=vec<ld>;
	using pointl=point<ld>;
	using linel=line<ld>;
	using segmentl=segment<ld>;
	using rayl=ray<ld>;
	using polygonl=polygon<ld>;
	using piterl=piter<ld>;
	using circlel=circle<ld>;
	constexpr auto infl=inf<ld>;
	constexpr auto epsl=eps<ld>;
	constexpr auto pil=numbers::pi_v<ld>;
	template<typename T>
	INLINE bool cw(const vec<T>& a,const vec<T>& b,const vec<T>& c)
	{
		return sgn((b-a)^(c-a))<0;
	}
	template<typename T>
	INLINE bool cwns(const vec<T>& a,const vec<T>& b,const vec<T>& c)
	{
		return sgn((b-a)^(c-a))<=0;
	}
	template<typename T=ld>
	INLINE ld len2(const vec<T>& v)
	{
		return v.len2();
	}
	template<typename T=ld>
	INLINE ld len(const vec<T>& v)
	{
		return v.len();
	}
	template<typename T=ld>
	INLINE ld arg(const vec<T>& v)
	{
		return v.arg();
	}
	template<typename T=ld>
	ld perimeter(const polygon<T>& p)
	{
		if(p.empty())return 0;
		ld ans=len(p.front()-p.back());
		for(int i=0;i<ssize(p)-1;i++)
			ans+=len(p[i]-p[i+1]);
		return ans;
	}
	template<typename T=ld>
	ld _2area(const polygon<T>& p)
	{
		if(p.empty())return 0;
		ld ans=(p.back()^p.front());
		for(int i=0;i<ssize(p)-1;i++)
			ans+=(p[i]^p[i+1]);
		return ans;
	}
	template<typename T=ld>
	ld area(const polygon<T>& p)
	{
		return _2area(p)/2;
	}
	template<typename T=ld>
	vec<T> rotate(const vec<T>& a,ld rad)
	{
		const vec<T> w(cos(rad),sin(rad));
		return vec<T>(a*w,w^a);
	}
	template<typename T=ld>
	vec<T> rotate(const vec<T>& a,const vec<T>& b,ld rad)
	{
		return rotate(a-b,rad)+b;
	}
	template<typename T>
	bool intersect(const vec<T>& p,const line<T>& l)
	{
		return !sgn((p-l.first)^(l.second-l.first));
	}
	template<typename T>
	bool intersect(const vec<T>& p,const segment<T>& s)
	{
		return intersect(p,static_cast<line<T>>(s))
			&&sgn((p-s.first)*(p-s.second))<=0;
	}
	template<typename T>
	bool intersect(const vec<T>& p,const ray<T>& r)
	{
		return intersect(p,static_cast<line<T>>(r))
			&&sgn((-(p-r.first))*(r.first-r.second))>=0;
	}
	template<typename T>
	optional<vecl> intersect_p(const line<T>& a,const line<T>& b)
	{
		const vecl x=a.second-a.first,y=b.second-b.first,z=a.first-b.first;
		if(sgn(x^y)==0)return nullopt;
		return vecl(a.first)+x*((y^z)/(x^y));
	}
	template<typename T,template<typename> class A,template<typename> class B>
	bool intersect(const A<T>& a,const B<T>& b)
	{
		auto p=intersect_p(static_cast<line<T>>(a),static_cast<line<T>>(b));
		if(p)
		{
			if constexpr(!is_same_v<A<T>,line<T>>)
				if(!intersect(*p,a))return false;
			if constexpr(!is_same_v<B<T>,line<T>>)
				if(!intersect(*p,b))return false;
			return true;
		}
		if(!intersect(a.first,static_cast<line<T>>(b)))return false;
		if constexpr(is_same_v<A<T>,line<T>>||is_same_v<B<T>,line<T>>)
			return true;
		if(intersect(a.first,b)||intersect(a.second,b))return true;
		if(intersect(b.first,a)||intersect(b.second,a))return true;
		return false;
	}
	template<typename T>
	vecl foot(const vec<T>& p,const line<T>& l)
	{
		const vecl x=p-l.first,y=p-l.second,z=l.second-l.first;
		const ld l1=(x*z)/len2(z),l2=-(y*z)/len2(z);
		return l.first+z*l1/(l1+l2);
	}
	template<typename T>
	vecl foot(const vec<T>& p,const segment<T>& s)
	{
		if(s.first==s.second) return s.first;
		const vec x=p-s.first,y=p-s.second,z=s.second-s.first;
		if(sgn(x*z)<0)return s.first;
		if(sgn(y*z)>0)return s.second;
		return foot(p,static_cast<line<T>>(s));
	}
	enum geo_state:int
	{
		geo_outside=0,
		geo_inside=1,
		geo_boundary=2
	};
	template<typename T>
	geo_state in_convex(const vec<T>& a,const polygon<T>& p)
	{
		int n=p.size();
		switch(n)
		{
		case 0:
			return geo_outside;
		case 1:
			return sgn(len2(p[0]-a))==0?geo_boundary:geo_outside;
		case 2:
			return intersect(a,segment<T>(p[0],p[1]))?geo_boundary:geo_outside;
		default:
			break;
		}
		if(cw(p[0],p[1],a)||cw(p[0],a,p.back()))return geo_outside;
		if(intersect(a,segment<T>(p[0],p[1]))||intersect(a,segment<T>(p[0],p.back())))return geo_boundary;
		int l=1,r=n-2;
		while(l<r)
		{
			int mid=(l+r+1)>>1;
			if(!cw(p[0],p[mid],a))l=mid;
			else r=mid-1;
		}
		if(cw(p[l],p[l+1],a))return geo_outside;
		if(intersect(a,segment<T>(p[l],p[l+1])))return geo_boundary;
		return geo_inside;
	}
	template<typename T>
	geo_state in_circle(const vec<T>& p,const circle<T>& c)
	{
		auto t=sgn(len(c.o-p)-c.r);
		return t==0?geo_boundary:t==-1?geo_inside:geo_outside;
	}
	template<typename T=ld>
	polygon<T> convex_hull(polygon<T> p)
	{
		if(ssize(p)<=1)return p;
		sort(p.begin(),p.end());
		polygon<T> stk;
		for(const auto& x:p)
		{
			while(ssize(stk)>1&&cwns(stk.end()[-2],stk.back(),x))
				stk.pop_back();
			stk.push_back(x);
		}
		const int sz=stk.size();
		for(int i=ssize(p)-2;i>=0;i--)
		{
			while(ssize(stk)>sz&&cwns(stk.end()[-2],stk.back(),p[i]))
				stk.pop_back();
			stk.push_back(p[i]);
		}
		stk.pop_back();
		return stk;
	}
	template<typename T>
	polygon<T> minkowski(const polygon<T>& a,const polygon<T>& b)
	{
		if(a.empty()||b.empty())return {};
		int n=ssize(a),m=ssize(b);
		int ia=min_element(a.begin(),a.end())-a.begin();
		int ib=min_element(b.begin(),b.end())-b.begin();
		polygon<T> c(n+1),d(m+1);
		for(int i=0;i<n;i++)c[i]=a[(ia+i)%n];
		for(int i=0;i<m;i++)d[i]=b[(ib+i)%m];
		c[n]=c[0];
		d[m]=d[0];
		polygon<T> e;
		e.reserve(n+m);
		e.push_back(c[0]+d[0]);
		int i=0,j=0;
		while(i<n&&j<m)
		{
			vec<T> va=c[i+1]-c[i];
			vec<T> vb=d[j+1]-d[j];
			auto cr=va^vb;
			if(sgn(cr)>0)e.push_back(e.back()+va),i++;
			else if(sgn(cr)<0)e.push_back(e.back()+vb),j++;
			else e.push_back(e.back()+va+vb),i++,j++;
		}
		while(i<n)e.push_back(e.back()+c[i+1]-c[i]),i++;
		while(j<m)e.push_back(e.back()+d[j+1]-d[j]),j++;
		if(ssize(e)>1&&e.front()==e.back())e.pop_back();
		return e;
	}
	template<typename T>
	pair<piter<T>,piter<T>> farthest_pair(const polygon<T>& p)
	{
		int n=ssize(p);
		if(n==1)return {p.begin(),p.begin()};
		if(n==2)return {p.begin(),p.begin()+1};
		ld max_d2=0;
		pair<int,int> ans{0,1};
		for(int i=0,j=1;i<n;i++)
		{
			int k=(i+1)%n;
			while(1)
			{
				int l=(j+1)%n;
				auto cur=abs((p[k]-p[i])^(p[j]-p[i]));
				auto nxt=abs((p[k]-p[i])^(p[l]-p[i]));
				if(nxt>cur)j=l;
				else break;
			}
			ld d2=len2(p[i]-p[j]);
			if(d2>max_d2)max_d2=d2,ans={i,j};
			d2=len2(p[k]-p[j]);
			if(d2>max_d2)max_d2=d2,ans={k,j};
		}
		return {p.begin()+ans.first,p.begin()+ans.second};
	}
	template<typename T>
	pair<piter<T>,piter<T>> closest_pair(const polygon<T>& p)
	{
		int n=ssize(p);
		if(n==1)return {p.begin(),p.begin()};
		if(n==2)return {p.begin(),p.begin()+1};
		vector<pair<vec<T>,int>> q;
		q.reserve(n);
		for(int i=0;i<n;i++)q.push_back({p[i],i});
		sort(q.begin(),q.end(),
			[](const auto& x,const auto& y)
			{
				return x.first.x==y.first.x?x.first.y<y.first.y:x.first.x<y.first.x;	
			});
		ld min_d2=inf<ld>;
		pair<int,int> ans={0,0};
		auto dc=[&](auto&& dc,int l,int r)->void
		{
			if(r-l<=1)return;
			int mid=(l+r)>>1;
			ld mx=q[mid].first.x;
			dc(dc,l,mid);
			dc(dc,mid,r);
			inplace_merge(q.begin()+l,q.begin()+mid,q.begin()+r,
				[](const auto& x,const auto& y)
				{
					return x.first.y<y.first.y;
				});
			vector<pair<vec<T>,int>> t;
			t.reserve(r-l);
			for(int i=l;i<r;i++)if(sqr(q[i].first.x-mx)<min_d2)t.push_back(q[i]);
			for(int i=0;i<ssize(t);i++)
			{
				for(int j=i+1;j<ssize(t)&&sqr(t[j].first.y-t[i].first.y)<min_d2;j++)
				{
					ld d2=len2(t[i].first-t[j].first);
					if(d2<min_d2)
					{
						min_d2=d2;
						ans={t[i].second,t[j].second};
					}
				}
			}
		};
		dc(dc,0,n);
		return {p.begin()+ans.first,p.begin()+ans.second};
	}
	template<typename T>
	circle<T> min_circle(polygon<T> p)
	{
		if(p.empty())return circle<T>(vec<T>(0,0),0);
		int n=ssize(p);
		shuffle(p.begin(),p.end(),rng);
		circle<T> c(p[0],0);
		for(int i=1;i<n;i++)
			if(!in_circle(p[i],c))
			{
				c=circle<T>(p[i],0);
				for(int j=0;j<i;j++)
					if(!in_circle(p[j],c))
					{
						c=circle<T>(p[i],p[j]);
						for(int k=0;k<j;k++)
							if(!in_circle(p[k],c))
								c=circle<T>(p[i],p[j],p[k]);
					}
			}
		return c;
	}
	template<typename T>
	polygon<ld> halfplane_intersect(vector<line<T>> h)
	{
		auto dir=[](const line<T>& l)->vecl
		{
			return vecl(l.second-l.first);
		};
		auto upper=[](const vecl& v)
		{
			return sgn(v.y)>0||(sgn(v.y)==0&&sgn(v.x)>0);
		};
		auto cmp=[&](const line<T>& a,const line<T>& b)
		{
			vecl u=dir(a),v=dir(b);
			bool hu=upper(u),hv=upper(v);
			if(hu!=hv)return hu<hv;
			int cr=sgn(u^v);
			if(cr)return cr>0;
			return false;
		};
		auto on_left=[](const line<T>& l,const vecl& p)
		{
			return sgn((vecl(l.second)-vecl(l.first))^(p-vecl(l.first)))>=0;
		};
		sort(h.begin(),h.end(),cmp);
		vector<line<T>> hs;
		hs.reserve(h.size());
		for(const auto& l:h)
		{
			if(!hs.empty())
			{
				vecl u=dir(hs.back()),v=dir(l);
				if(sgn(u^v)==0&&sgn(u*v)>0)
				{
					if(on_left(l,vecl(hs.back().first)))continue;
					hs.back()=l;
					continue;
				}
			}
			hs.push_back(l);
		}
		deque<line<T>> q;
		for(const auto& l:hs)
		{
			while(q.size()>=2)
			{
				auto p=intersect_p(q[q.size()-2],q.back());
				if(!p||!on_left(l,*p))q.pop_back();
				else break;
			}
			while(q.size()>=2)
			{
				auto p=intersect_p(q[0],q[1]);
				if(!p||!on_left(l,*p))q.pop_front();
				else break;
			}
			q.push_back(l);
		}
		while(q.size()>2)
		{
			auto p=intersect_p(q[q.size()-2],q.back());
			if(p&&on_left(q.front(),*p))break;
			q.pop_back();
		}
		while(q.size()>2)
		{
			auto p=intersect_p(q[0],q[1]);
			if(p&&on_left(q.back(),*p))break;
			q.pop_front();
		}
		if(q.size()<3)return {};
		polygon<ld> ans;
		ans.reserve(q.size());
		for(int i=0;i<ssize(q);i++)
		{
			auto p=intersect_p(q[i],q[(i+1)%q.size()]);
			if(!p)return {};
			ans.push_back(*p);
		}
		return ans;
	}
}
#endif