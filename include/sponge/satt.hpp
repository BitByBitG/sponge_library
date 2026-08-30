#ifndef SPONGE_SATT_HPP
#define SPONGE_SATT_HPP
#include<sponge/core.hpp>
#include<sponge/monoid.hpp>
#include<sponge/functors.hpp>
namespace sponge
{
	enum cluster_type:bool
	{
		compress=1,
		rake=0
	};
	template<
		typename S,typename T,typename SId,typename TId,typename IsTId,
		typename SOpSS,typename SOpST,typename TOpTT
	>
	class satt:
		public double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>
	{
	public:
		using M=double_monoid<S,T,SId,TId,IsTId,SOpSS,SOpST,TOpTT>;
		using M::s_id,M::t_id,M::is_t_id,M::s_op_s_s,M::s_op_s_t,M::t_op_t_t;
		struct node_t
		{
			int fa;
			array<int,3> son;
			bool rev;
			int path_size,subtree_size;
			S val,path_val,subtree_val;
			T path_tag,subtree_tag;
			node_t():
				fa(0),son{0,0,0},
				rev(0),path_size(0),subtree_size(0),
				val(s_id()),path_val(s_id()),subtree_val(s_id()),
				path_tag(t_id()),subtree_tag(t_id()){}
		};
		vector<node_t> tr;
		vector<int> stk;
		satt()
		{
			tr.push_back(node_t());
		}
		satt(int n)
		{
			tr.push_back(node_t());
			resize(n);
		}
		void resize(int n)
		{
			tr.resize(n+1);
		}
		void reserve(int n)
		{
			tr.reserve(n+1);
		}
		void clear()
		{
			tr.resize(1);
			stk.clear();
		}
		node_t& operator[](int x)
		{
			return tr[x];
		}
		INLINE int& fa(const int x)
		{
			return tr[x].fa;
		}
		INLINE int& ls(const int x)
		{
			return tr[x].son[0];
		}
		INLINE int& rs(const int x)
		{
			return tr[x].son[1];
		}
		INLINE int& ms(const int x)
		{
			return tr[x].son[2];
		}
		inline S& value(const int x)
		{
			return tr[x].val;
		}
		int new_node()
		{
			if(!stk.empty())
			{
				int x=stk.back();
				stk.pop_back();
				return x;
			}
			else
			{
				tr.push_back(node_t());
				return tr.size()-1;
			}
		}
		void delete_node(int x)
		{
			tr[x]=node_t();
			stk.push_back(x);
		}
		inline bool dir(const int x)
		{
			return x==rs(fa(x));
		}
		inline bool is_root(const int x)
		{
			return ls(fa(x))!=x&&rs(fa(x))!=x;
		}
		void setf(const int x,const int f,const int d)
		{
			if(x)fa(x)=f;
			if(f)tr[f].son[d]=x;
		}
		void reverse(const int x)
		{
			if(!x)return;
			swap(ls(x),rs(x));
			tr[x].rev^=1;
		}
		void apply_path_tag(int x,const T& t)
		{
			if(!x)return;
			tr[x].val=s_op_s_t(tr[x].val,t,1);
			tr[x].path_val=s_op_s_t(tr[x].path_val,t,tr[x].path_size);
			tr[x].path_tag=t_op_t_t(tr[x].path_tag,t);
		}
		void apply_subtree_tag(int x,const T& t)
		{
			if(!x)return;
			tr[x].subtree_val=s_op_s_t(tr[x].subtree_val,t,tr[x].subtree_size);
			tr[x].subtree_tag=t_op_t_t(tr[x].subtree_tag,t);
		}
		template<cluster_type type>
		void push_up(int x)
		{
			switch(type)
			{
				case compress:
					tr[x].path_size=tr[ls(x)].path_size+1+tr[rs(x)].path_size;
					tr[x].subtree_size=tr[ls(x)].subtree_size+tr[rs(x)].subtree_size+tr[ms(x)].subtree_size;
					tr[x].path_val=s_op_s_s(s_op_s_s(tr[ls(x)].path_val,tr[x].val),tr[rs(x)].path_val);
					tr[x].subtree_val=s_op_s_s(s_op_s_s(tr[ls(x)].subtree_val,tr[rs(x)].subtree_val),tr[ms(x)].subtree_val);
					break;
				case rake:
					tr[x].subtree_size=tr[ls(x)].subtree_size+tr[rs(x)].subtree_size+tr[ms(x)].subtree_size+tr[ms(x)].path_size;
					tr[x].subtree_val=s_op_s_s(s_op_s_s(s_op_s_s(tr[ls(x)].subtree_val,tr[rs(x)].subtree_val),tr[ms(x)].subtree_val),tr[ms(x)].path_val);
			}
		}
		template<cluster_type type>
		void push_down(int x)
		{
			switch(type)
			{
				case compress:
					if(tr[x].rev)
					{
						reverse(ls(x));
						reverse(rs(x));
						tr[x].rev=0;
					}
					if(!is_t_id(tr[x].path_tag))
					{
						apply_path_tag(ls(x),tr[x].path_tag);
						apply_path_tag(rs(x),tr[x].path_tag);
						tr[x].path_tag=t_id();
					}
					if(!is_t_id(tr[x].subtree_tag))
					{
						apply_subtree_tag(ls(x),tr[x].subtree_tag);
						apply_subtree_tag(rs(x),tr[x].subtree_tag);
						apply_subtree_tag(ms(x),tr[x].subtree_tag);
						tr[x].subtree_tag=t_id();
					}
					break;
				case rake:
					if(!is_t_id(tr[x].subtree_tag))
					{
						apply_subtree_tag(ls(x),tr[x].subtree_tag);
						apply_subtree_tag(rs(x),tr[x].subtree_tag);
						apply_subtree_tag(ms(x),tr[x].subtree_tag);
						apply_path_tag(ms(x),tr[x].subtree_tag);
						tr[x].subtree_tag=t_id();
					}
			}
		}
		template<cluster_type type>
		void push_path(int x)
		{
			if(!is_root(x))push_path<type>(fa(x));
			push_down<type>(x);
		}
		template<cluster_type type>
		void rotate(int x)
		{
			int f=fa(x),g=fa(f),d=dir(x),y=tr[x].son[!d];
			if(g)tr[g].son[ms(g)==f?2:dir(f)]=x;
			if(y)fa(y)=f;
			fa(x)=g,tr[x].son[!d]=f;
			fa(f)=x,tr[f].son[d]=y;
			push_up<type>(f);push_up<type>(x);
		}
		template<cluster_type type>
		void splay(int x,int guard=0)
		{
			push_path<type>(x);
			for(int y;y=fa(x),!is_root(x)&&y!=guard;rotate<type>(x))
				if(fa(y)!=guard&&!is_root(y))
					rotate<type>(dir(y)==dir(x)?y:x);
		}
		void erase(int x)
		{
			setf(ms(x),fa(x),1);
			if(ls(x))
			{
				int y=ls(x);
				push_down<rake>(y);
				while(rs(y))y=rs(y),push_down<rake>(y);
				splay<rake>(y,x);
				setf(rs(x),y,1);
				setf(y,fa(x),2);
				push_up<rake>(y);
				push_up<compress>(fa(x));
			}
			else setf(rs(x),fa(x),2);
			delete_node(x);
		}
		void splice(int x)
		{
			splay<rake>(x);
			int y=fa(x);
			splay<compress>(y);
			push_down<rake>(x);
			if(rs(y))swap(fa(ms(x)),fa(rs(y))),swap(ms(x),rs(y));
			else erase(x);
			push_up<rake>(x);
			push_up<compress>(y);
		}
		int access(int x)
		{
			splay<compress>(x);
			int z=x;
			if(rs(x))
			{
				int y=new_node();
				setf(ms(x),y,0);
				setf(rs(x),y,2);
				rs(x)=0;
				setf(y,x,2);
				push_up<rake>(y);
				push_up<compress>(x);
			}
			while(fa(x))splice(fa(x)),x=fa(x),push_up<compress>(x);
			splay<compress>(z);
            return x;
		}
		void set_root(int x)
		{
			access(x);
			reverse(x);
		}
		void set_value(int x,const S& v=s_id())
		{
			value(x)=v;
			push_up<compress>(x);
		}
		int find(int x)
		{
			access(x);
			while(ls(x))push_down<compress>(x),x=ls(x);
			splay<compress>(x);
			return x;
		}
		bool connected(int x,int y)
		{
			return find(x)==find(y);
		}
		void expose(int x,int y)
		{
			set_root(x);
			access(y);
		}
		void link(int x,int y)
		{
			access(x);
			set_root(y);
			setf(y,x,1);
			push_up<compress>(x);
		}
		int cut(int x)
		{
			access(x);
			int y=ls(x);
			while(rs(y))push_down<compress>(y),y=rs(y);
			ls(x)=fa(ls(x))=0;
			push_up<compress>(x);
			return y;
		}
		bool cut(int x,int y)
		{
			int root=find(x);
			expose(x,y);
			if(ls(y)!=x||rs(x))return 0;
			fa(x)=ls(y)=0;
			push_up<compress>(y);
			set_root(x);
			set_root(y);
			set_root(root);
			return 1;
		}
		void change(int x,int y)
		{
			if(x==y)return;
			int root=find(x);
			if(x==root)return;
			int z=cut(x);
			if(find(x)==find(y))link(x,z);
			else link(x,y);
			set_root(root);
		}
		void update_path(int x,int y,const T& t)
		{
			int root=find(x);
			expose(x,y);
			apply_path_tag(y,t);
			set_root(root);
		}
		void update_vertex(int x,const T& t)
		{
			update_path(x,x,t);
		}
		void update_subtree(int x,const T& t)
		{
			int root=find(x);
			access(x);
			apply_subtree_tag(ms(x),t);
			tr[x].val=s_op_s_t(tr[x].val,t,1);
			push_up<compress>(x);
			set_root(root);
		}
		S query_path(int x,int y)
		{
			int root=find(x);
			expose(x,y);
			S res=tr[y].path_val;
			set_root(root);
			return res;
		}
		S query_vertex(int x)
		{
			return query_path(x,x);
		}
		S query_subtree(int x)
		{
			int root=find(x);
			access(x);
			S res=s_op_s_s(tr[ms(x)].subtree_val,tr[x].val);
			set_root(root);
			return res;
		}
        int lca(int x,int y)
		{
			int root=find(x);
			if(root!=find(y))return 0;
			access(x);
			int z=access(y);
			set_root(root);
			return z;
		}
	};
	template<typename S,typename SId,typename SOpSS>
	using satt_n=satt<
		S,
		null_t,
		SId,
		null_id,
		is_null_id,
		SOpSS,
		ignore_tag<S>,
		null_op
	>;
	using satt_nn=satt_n<
		null_t,
		null_id,
		null_op
	>;
}
#endif