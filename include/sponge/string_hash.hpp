#ifndef SPONGE_STRING_HASH_HPP
#define SPONGE_STRING_HASH_HPP
#include<sponge/core.hpp>
#include<sponge/modint.hpp>
#include<sponge/utility.hpp>
namespace sponge
{
	template<typename T1=m9,typename T2=m10>
	class string_hash
	{
	public:
		static T1 b1;
		static T2 b2;
		static void set_base(T1 _b1,T2 _b2)
		{
			b1=_b1,b2=_b2;
		}
		static vector<T1> p1;
		static vector<T2> p2;
		static int pow_max=0;
		static void init_power(int n)
		{
			pow_max=n;
			p1.resize(n+1);
			p1[0]=1;
			for(int i=1;i<=n;i++)p1[i]=p1[i-1]*b1;
			p2.resize(n+1);
			p2[0]=1;
			for(int i=1;i<=n;i++)p2[i]=p2[i-1]*b2;
		}
		static pair<T1,T2> power_checked(int n)
		{
			if(pow_max<n)init_power(n);
			return make_pair(p1[n],p2[n]);
		}
		template<typename String>
		static string_hash from_string(const String& s)
		{
			string_hash res;
			for(auto i:s)res.append(i);
			return res;
		}
		template<typename String>
		static vector<string_hash> to_vector(const String& s)
		{
			int n=ssize(s);
			vector<string_hash> res(n);
			res[0].append(s[0]);
			for(int i=1;i<n;i++)
			{
				res[i]=res[i-1];
				res[i].append(s[i]);
			}
		}
		static string_hash query(const vector<string_hash>& v,int l,int r)
		{
			auto hl=l>0?v[l-1]:0;
			auto hr=v[r];
			return string_hash(hr.first-hl.first*p1[r-l+1],hr.second-hl.second*p2[r-l+1]);
		}
		static string_hash query_checked(const vector<string_hash>& v,int l,int r)
		{
			auto hl=l>0?v[l-1]:0;
			auto hr=v[r];
			auto[_p1,_p2]=power_checked(r-l+1);
			return string_hash(hr.first-hl.first*_p1,hr.second-hl.second*_p2);
		}
		T1 h1;
		T2 h2;
		string_hash():h1(),h2(){}
		string_hash(T1 _h1,T2 _h2):h1(_h1),h2(_h2){}
		template<typename String>
		string_hash(const String& s):
		{
			*this=from_string(s);
		}
		template<typename Char>
		void append(Char c)
		{
			h1=h1*b1+c;
			h2=h2*b2+c;
		}
	};
	template<uint prm1,uint prm2>
	using make_hash_t=string_hash<static_modint<uint,prm1,1>,static_modint<uint,prm2,0>>;
}
#endif