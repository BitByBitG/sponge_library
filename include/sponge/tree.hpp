#ifndef SPONGE_TREE_HPP
#define SPONGE_TREE_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
#include<sponge/forest.hpp>
namespace sponge
{
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class tree
	{
	public:
		using Fr=forest<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using forest_type=Fr;
		inline static Fr fr;
		using Sz=Fr::Sz;
		using size_type=Sz;
		using Scr=Fr::Scr;
		using Tcr=Fr::Tcr;
		int root;
		tree():root(){}
		tree(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<tree,tree> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {tree(x),tree(y)};
		}
		array<tree,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {tree(x),tree(y),tree(z)};
		}
#ifndef SPONGE_TREE_NTAG
		void update(Sz l,Sz r,Tcr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,Scr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_TREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,Scr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		void derive_from(tree x){root=fr.derive(x.root);}
		static tree merge(tree x,tree y){return tree(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
	};
#define OT ordered_tree
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class OT
	{
	public:
		using Fr=ordered_forest<Compare,S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using forest_type=Fr;
		inline static Fr fr;
		using Sz=Fr::Sz;
		using size_type=Sz;
		using Scr=Fr::Scr;
		using Tcr=Fr::Tcr;
		int root;
		OT():root(){}
		OT(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<OT,OT> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {OT(x),OT(y)};
		}
		array<OT,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {OT(x),OT(y),OT(z)};
		}
#ifndef SPONGE_TREE_NTAG
		void update(Sz l,Sz r,Tcr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,Scr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_TREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,Scr v){fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		void derive_from(OT x){root=fr.derive(x.root);}
		static OT merge(OT x,OT y){return OT(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
		int find(Scr v){return fr.find(root,v);}
		int find_by_order(Sz k){return fr.find_by_order(root,k);}
		Sz order_of_key(Scr v){return fr.order_of_key(root,v);}
		int lower_bound(Scr v){return fr.lower_bound(root,v);}
		int upper_bound(Scr v){return fr.upper_bound(root,v);}
		int predecessor(Scr v){return fr.predecessor(root,v);}
		int successor(Scr v){return fr.successor(root,v);}
		int insert(Scr v){return fr.insert(root,v);}
		bool erase(Scr v){return fr.erase(root,v);}
		pair<OT,OT> vsplit(Scr k)
		{
			auto[x,y]=fr.vsplit(root,k);
			return {OT(x),OT(y)};
		}
		array<OT,3> vaccess(Scr l,Scr r)
		{
			auto[x,y,z]=fr.vaccess(root,l,r);
			return {OT(x),OT(y),OT(z)};
		}
#ifndef SPONGE_TREE_NTAG
		void vupdate(Scr l,Scr r,Tcr t){fr.vupdate(l,r,t);}
#endif
		S vquery(Scr l,Scr r){return fr.vquery(l,r);}
		static OT hmerge(OT x,OT y){return OT(fr.hmerge(x.root,y.root));}
 	};
#undef OT
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using tree_n=tree<
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S,SizeType>,null_op,SizeType
	>;
	template<typename S,typename SId,typename SizeType=int>
	using tree_nn=tree_n<S,SId,ordered_op<S>,SizeType>;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_tree_n=ordered_tree<
		Compare,
		S,null_t,SId,null_id,is_null_id,
		SOpSS,ignore_tag<S,SizeType>,null_op,SizeType
	>;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_tree_nn=ordered_tree_n<Compare,S,SId,ordered_op<S>,SizeType>;
	template<typename S,typename SId,typename SOpSS,typename SizeType=int>
	using forest_n=tree_n<S,SId,SOpSS,SizeType>::Fr;
	template<typename S,typename SId,typename SizeType=int>
	using forest_nn=tree_nn<S,SId,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_forest_n=ordered_tree_n<Compare,S,SId,SOpSS,SizeType>::OFr;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_forest_nn=ordered_tree_nn<Compare,S,SId,SizeType>::OFr;
}
#endif