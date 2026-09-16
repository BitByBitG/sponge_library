#ifndef SPONGE_PTREE_HPP
#define SPONGE_PTREE_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
#include<sponge/pforest.hpp>
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ptree
	{
	public:
		using Fr=pforest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using pforest_type=Fr;
		inline static Fr fr;
		using Sz=typename Fr::Sz;
		using size_type=Sz;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
		int root;
		ptree():root(){}
		ptree(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<ptree,ptree> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {ptree(x),ptree(y)};
		}
		array<ptree,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {ptree(x),ptree(y),ptree(z)};
		}
#ifndef SPONGE_PTREE_NTAG
		void update(Sz l,Sz r,t_cr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,s_cr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_PTREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,s_cr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		void derive_from(ptree x){root=fr.derive(x.root);}
		static ptree merge(ptree x,ptree y){return ptree(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
	};
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ordered_ptree
	{
	public:
		using Fr=ordered_pforest<Compare,S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using pforest_type=Fr;
		inline static Fr fr;
		using Sz=typename Fr::Sz;
		using size_type=Sz;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
		int root;
		ordered_ptree():root(){}
		ordered_ptree(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<ordered_ptree,ordered_ptree> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {ordered_ptree(x),ordered_ptree(y)};
		}
		array<ordered_ptree,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {ordered_ptree(x),ordered_ptree(y),ordered_ptree(z)};
		}
#ifndef SPONGE_PTREE_NTAG
		void update(Sz l,Sz r,t_cr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,s_cr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_PTREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,s_cr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		void derive_from(ordered_ptree x){root=fr.derive(x.root);}
		static ordered_ptree merge(ordered_ptree x,ordered_ptree y){return ordered_ptree(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
		int find(s_cr v){return fr.find(root,v);}
		int find_by_order(Sz k){return fr.find_by_order(root,k);}
		Sz order_of_key(s_cr v){return fr.order_of_key(root,v);}
		int lower_bound(s_cr v){return fr.lower_bound(root,v);}
		int upper_bound(s_cr v){return fr.upper_bound(root,v);}
		int predecessor(s_cr v){return fr.predecessor(root,v);}
		int successor(s_cr v){return fr.successor(root,v);}
		int insert(s_cr v){return fr.insert(root,v);}
		bool erase(s_cr v){return fr.erase(root,v);}
		pair<ordered_ptree,ordered_ptree> vsplit(s_cr k)
		{
			auto[x,y]=fr.vsplit(root,k);
			return {ordered_ptree(x),ordered_ptree(y)};
		}
		array<ordered_ptree,3> vaccess(s_cr l,s_cr r)
		{
			auto[x,y,z]=fr.vaccess(root,l,r);
			return {ordered_ptree(x),ordered_ptree(y),ordered_ptree(z)};
		}
#ifndef SPONGE_PTREE_NTAG
		void vupdate(s_cr l,s_cr r,t_cr t){fr.vupdate(root,l,r,t);}
#endif
		S vquery(s_cr l,s_cr r){return fr.vquery(root,l,r);}
		static ordered_ptree hmerge(ordered_ptree x,ordered_ptree y){return ordered_ptree(fr.hmerge(x.root,y.root));}
 	};
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ptree_n=ptree<
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S,SizeType>,null_op,SizeType
	>;
	template<typename S,typename SId,typename SizeType=int>
	using ptree_nn=ptree_n<S,SId,ordered_op<S>,SizeType>;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_ptree_n=ordered_ptree<
		Compare,
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S,SizeType>,null_op,SizeType
	>;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_ptree_nn=ordered_ptree_n<Compare,S,SId,ordered_op<S>,SizeType>;
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using pforest_n=typename ptree_n<S,SId,SOpSS,SizeType>::Fr;
	template<typename S,typename SId,typename SizeType=int>
	using pforest_nn=typename ptree_nn<S,SId,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_pforest_n=typename ordered_ptree_n<Compare,S,SId,SOpSS,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_pforest_nn=typename ordered_ptree_nn<Compare,S,SId,SizeType>::Fr;
}
#endif