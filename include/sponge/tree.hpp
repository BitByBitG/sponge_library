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
		using Sz=typename Fr::Sz;
		using size_type=Sz;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
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
		void update(Sz l,Sz r,t_cr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,s_cr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_TREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,s_cr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		static tree merge(tree x,tree y){return tree(fr.merge(x.root,y.root));}
		static S deref(int x){return fr.deref(x);}
	};
	template<
		typename Compare,
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT,typename SizeType=int
	>
	class ordered_tree
	{
	public:
		using Fr=ordered_forest<Compare,S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT,SizeType>;
		using forest_type=Fr;
		inline static Fr fr;
		using Sz=typename Fr::Sz;
		using size_type=Sz;
		using s_cr=typename Fr::s_cr;
		using t_cr=typename Fr::t_cr;
		int root;
		ordered_tree():root(){}
		ordered_tree(int _root):root(_root){}
		template<typename F>
		void build(int n,F&& f){root=fr.build(n,f);}
		template<typename RAIter>
		void build(RAIter first,RAIter last){root=fr.build(first,last);}
		pair<ordered_tree,ordered_tree> split(int k)
		{
			auto[x,y]=fr.split(root,k);
			return {ordered_tree(x),ordered_tree(y)};
		}
		array<ordered_tree,3> access(Sz l,Sz r)
		{
			auto[x,y,z]=fr.access(root,l,r);
			return {ordered_tree(x),ordered_tree(y),ordered_tree(z)};
		}
#ifndef SPONGE_TREE_NTAG
		void update(Sz l,Sz r,t_cr t){fr.update(root,l,r,t);}
#endif
		void set_pos(Sz p,s_cr v){fr.set_pos(root,p,v);}
#ifndef SPONGE_TREE_NREV
		void reverse(Sz l,Sz r){fr.reverse(root,l,r);}
#endif
		S query(Sz p){return pos(p);}
		S query(Sz l,Sz r){return fr.query(root,l,r);}
		S pos(Sz p){return fr.pos(root,p);}
		vector<S> range(Sz l,Sz r){return fr.range(root,l,r);}
		vector<S> seq(){return fr.seq(root);}
		int insert_after(Sz p,s_cr v){return fr.insert_after(root,p,v);}
		void erase_pos(Sz p){fr.erase_pos(root,p);}
		static ordered_tree merge(ordered_tree x,ordered_tree y){return ordered_tree(fr.merge(x.root,y.root));}
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
		pair<ordered_tree,ordered_tree> vsplit(s_cr k)
		{
			auto[x,y]=fr.vsplit(root,k);
			return {ordered_tree(x),ordered_tree(y)};
		}
		array<ordered_tree,3> vaccess(s_cr l,s_cr r)
		{
			auto[x,y,z]=fr.vaccess(root,l,r);
			return {ordered_tree(x),ordered_tree(y),ordered_tree(z)};
		}
#ifndef SPONGE_TREE_NTAG
		void vupdate(s_cr l,s_cr r,t_cr t){fr.vupdate(root,l,r,t);}
#endif
		S vquery(s_cr l,s_cr r){return fr.vquery(root,l,r);}
		static ordered_tree hmerge(ordered_tree x,ordered_tree y){return ordered_tree(fr.hmerge(x.root,y.root));}
 	};
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
	using forest_n=typename tree_n<S,SId,SOpSS,SizeType>::Fr;
	template<typename S,typename SId,typename SizeType=int>
	using forest_nn=typename tree_nn<S,SId,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SOpSS,typename SizeType=int>
	using ordered_forest_n=typename ordered_tree_n<Compare,S,SId,SOpSS,SizeType>::Fr;
	template<typename Compare,typename S,typename SId,typename SizeType=int>
	using ordered_forest_nn=typename ordered_tree_nn<Compare,S,SId,SizeType>::Fr;
}
#endif