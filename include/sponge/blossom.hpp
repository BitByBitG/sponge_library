#ifndef SPONGE_BLOSSOM_HPP
#define SPONGE_BLOSSOM_HPP
#include<sponge/core.hpp>
namespace sponge
{
	class blossom
	{
	public:
		int n=0,dfn=0,ans=0;
		vector<vector<int>> g;
		vector<int> fa,mat,pre,dfn_,color;
		deque<int> que;
		blossom()=default;
		blossom(int _n)
		{
			resize(_n);
		}
		void resize(int _n)
		{
			n=_n;
			g.resize(n+1);
			fa.resize(n+1);
			mat.resize(n+1);
			pre.resize(n+1);
			dfn_.resize(n+1);
			color.resize(n+1);
		}
		void clear()
		{
			*this=blossom();
		}
		void link(int x,int y)
		{
			g[x].push_back(y);
			g[y].push_back(x);
		}
		INLINE void match(int x,int y)
		{
			mat[x]=y;mat[y]=x;
		}
		int find(int x)
		{
			while(x!=fa[x])x=fa[x]=fa[fa[x]];
			return x;
		}
		int lca(int x,int y)
		{
			dfn++;
			x=find(x),y=find(y);
			while(dfn_[x]!=dfn)
			{
				dfn_[x]=dfn;
				x=find(pre[mat[x]]);
				if(y)swap(x,y);
			}
			return x;
		}
		void shrink(int x,int y,int z)
		{
			while(find(x)!=z)
			{
				pre[x]=y,y=mat[x];
				fa[x]=fa[y]=z;
				if(color[y]==2)color[y]=1,que.push_back(y);
				x=pre[y];
			}
		}
		inline void rev(int x)
		{
			if(x)rev(mat[pre[x]]),match(x,pre[x]);
		}
		int augment(int s)
		{
			if((ans+1)<<1>n)return 0;
			iota(fa.begin(),fa.end(),0);
			fill(color.begin(),color.end(),0);
			fill(pre.begin(),pre.end(),0);
			que.clear();
			que.push_back(s);
			color[s]=1;
			while(!que.empty())
			{
				int x=que.front();
				que.pop_front();
				for(int y:g[x])
				{
					if(find(x)==find(y))continue;
					if(color[y]==1)shrink(x,y,lca(x,y)),shrink(y,x,lca(x,y));
					else if(color[y]==0)
					{
						color[y]=2,pre[y]=x;
						if(!mat[y])return rev(y),1;
						color[mat[y]]=1;
						que.push_back(mat[y]);
					}
				}
			}
			return 0;
		}
		void blossom_algorithm()
		{
			dfn=0;
			ans=0;
			fill(mat.begin(),mat.end(),0);
			fill(dfn_.begin(),dfn_.end(),0);
			for(int i=1;i<=n;i++)
				for(int j:g[i])
					if(!mat[i]&&!mat[j])
					{
						match(i,j);
						ans++;
						break;
					}
			for(int i=1;i<=n;i++)
				if(!mat[i])
					ans+=augment(i);
		}
		int matching()
		{
			blossom_algorithm();
			return ans;
		}
	};
	template<typename Value=ll>
	class weighted_blossom
	{
	public:
		using value_type=Value;
		weighted_blossom(){}
		weighted_blossom(int n)
		{
			resize(n);
		}
		struct edge_t
		{
			int u,v;
			Value w;
			edge_t(){}
			edge_t(int u,int v,Value w):u(u),v(v),w(w){}
		};
		int n,n_x;
		vector<vector<edge_t>> g;
		vector<vector<int>> flower;
		vector<int> root;
		vector<vector<int>> flower_from;
		vector<Value> label;
		vector<int> col;
		vector<int> slack;
		vector<int> mat;
		vector<int> fa;
		vector<int> vis;
		queue<int> que;
		vector<pair<Value,pair<int,int>>> edges;
		void link(int x,int y,const Value &w)
		{
			edges.push_back({w,{x,y}});
		}
		void allocate(int n)
		{
			++n;
			if((int)g.size()>=n)return;
			g.resize(n);
			for(auto &v:g)v.resize(n);
			flower.resize(n);
			root.resize(n);
			flower_from.resize(n);
			for(auto &v:flower_from)v.resize(n+1);
			label.resize(n);
			col.resize(n);
			slack.resize(n);
			mat.resize(n);
			fa.resize(n);
			vis.resize(n);
		}
		inline Value calc_slack(edge_t e)
		{
			return label[e.u]+label[e.v]-e.w;
		}
		inline void update_slack(int u,int v)
		{
			if(!slack[v]||calc_slack(g[u][v])<calc_slack(g[slack[v]][v]))slack[v]=u;
		}
		inline void recalc_slack(int u)
		{
			slack[u]=0;
			for(int i=1;i<=n;i++)if(g[i][u].w>0&&root[i]!=u&&col[root[i]]==1)update_slack(i,u);
		}
		void q_push(int x)
		{
			if(x<=n)que.push(x);
			else for(auto p:flower[x])q_push(p);
		}
		void set_root(int x,int r)
		{
			root[x]=r;
			if(x>n)for(auto p:flower[x])set_root(p,r);
		}
		int get_even_path_in_flower(int b,int x)
		{
			int pr=find(flower[b].begin(),flower[b].end(),x)-flower[b].begin();
			if(!(pr&1))return pr;
			reverse(flower[b].begin()+1,flower[b].end());
			return flower[b].size()-pr;
		}
		void set_match(int u,int v)
		{
			mat[u]=g[u][v].v;
			if(u>n)
			{
				edge_t e=g[u][v];
				int xr=flower_from[u][e.u];
				int pr=get_even_path_in_flower(u,xr);
				for(int i=0;i<pr;i++)set_match(flower[u][i],flower[u][i^1]);
				set_match(xr,v);
				rotate(flower[u].begin(),flower[u].begin()+pr,flower[u].end());
			}
		}
		void side_augment(int u,int v)
		{
			int nv=root[mat[u]],nu=root[fa[nv]];
			while(1)
			{
				set_match(u,v);
				u=nu,v=nv;
				if(!nv)break;
				set_match(nv,nu);
				nv=root[mat[u]],nu=root[fa[nv]];
			}
		}
		void linkss(int u,int v)
		{
			side_augment(u,v); 
			side_augment(v,u);
		}
		int get_lca(int u,int v)
		{
			static int t=0;
			++t;
			while(u||v)
			{
				if(vis[u]==t)return u;
				vis[u]=t;
				u=root[mat[u]];
				if(u)u=root[fa[u]];
				if(!u)swap(u,v);
			}
			return 0;
		}
		void add_blossom(int u,int v,int r)
		{
			int b=n+1;
			while(b<=n_x&&root[b])b++;
			if(b>n_x)++n_x;
			col[b]=1;label[b]=0;mat[b]=mat[r];flower[b].clear();
			for(int i=1;i<=n_x;i++)g[i][b].w=g[b][i].w=0;
			for(int i=1;i<=n;i++)flower_from[b][i]=0;
			while(u!=r)
			{
				flower[b].push_back(u);u=root[mat[u]];q_push(u);
				flower[b].push_back(u);u=root[fa[u]];
			}
			flower[b].push_back(r);
			reverse(flower[b].begin(),flower[b].end());
			while(v!=r)
			{
				flower[b].push_back(v);v=root[mat[v]];q_push(v);
				flower[b].push_back(v);v=root[fa[v]];
			}
			set_root(b,b);
			for(auto p:flower[b])
			{
				for(int i=1;i<=n_x;i++)
					if(!g[b][i].w||calc_slack(g[p][i])<calc_slack(g[b][i]))
					{
						g[b][i]=g[p][i];
						g[i][b]=g[i][p];
					}
				for(int i=1;i<=n;i++)if(flower_from[p][i])flower_from[b][i]=p;
			}
			recalc_slack(b);
		}
		void expand_blossom(int b)
		{
			int x;
			for(auto p:flower[b])set_root(p,p);
			x=flower_from[b][g[b][fa[b]].u];
			int pr=get_even_path_in_flower(b,x);
			col[x]=2;fa[x]=fa[b];
			for(int i=0;i<pr;i+=2){
				int white=flower[b][i];
				int black=flower[b][i+1];
				col[black]=1;col[white]=2;
				fa[white]=g[black][white].u;
				slack[black]=slack[white]=0;
				q_push(black);
			}
			for(int i=pr+1;i<(int)flower[b].size();i++)
			{
				col[flower[b][i]]=0;
				recalc_slack(flower[b][i]);
			}
			root[b]=0;
			flower[b].clear();
		}
		int augment_path(edge_t e)
		{
			int u=root[e.u],v=root[e.v];
			if(!col[v])
			{
				assert(mat[v]);
				fa[v]=e.u;
				col[v]=2;
				int nu=root[mat[v]];
				slack[nu]=slack[v]=0;
				col[nu]=1;
				q_push(nu);
			}
			else if(col[v]==1)
			{
				int r=get_lca(u,v);
				if(r)add_blossom(u,v,r);
				else return linkss(u,v),1;
			}
			return 0;
		}
		int augment()
		{
			fill(col.begin(), col.begin() + (n_x+1), 0);
			fill(slack.begin(), slack.begin() + (n_x+1), 0);
			fill(fa.begin(), fa.begin() + (n_x+1), 0);
			que=queue<int>();
			for(int i=1;i<=n_x;i++)
				if(root[i]==i&&!mat[i])
				{
					col[i]=1;
					q_push(i);
				}
			if(que.empty())return 0;
			while(1)
			{
				while(!que.empty())
				{
					int p=que.front();que.pop();
					assert(col[root[p]]==1);
					for(int i=1;i<=n;i++)
					{
						if(g[p][i].w==0||root[i]==root[p])continue;
						Value d=calc_slack(g[p][i]);
						if(!d){if(augment_path(g[p][i]))return 1;}
						else if(col[root[i]]!=2)update_slack(p,root[i]);
					}
				}
				Value delta=inf<Value>;
				for(int i=1;i<=n;i++)if(col[root[i]]==1)delta=min(delta,label[i]);
				for(int i=n+1;i<=n_x;i++)if(root[i]==i&&col[i]==2)delta=min(delta,label[i]/2);
				for(int i=1;i<=n_x;i++)
				{
					if(root[i]!=i||!slack[i])continue;
					if(!col[i])delta=min(delta,calc_slack(g[slack[i]][i]));
					else if(col[i]==1)delta=min(delta,calc_slack(g[slack[i]][i])/2);
				}
				for(int i=1;i<=n;i++)
				{
					if(col[root[i]]==1)label[i]-=delta;
					else if(col[root[i]]==2)label[i]+=delta;
				}
				for(int i=n+1;i<=n_x;i++)
				{
					if(root[i]!=i)continue;
					if(col[i]==1)label[i]+=2*delta;
					else if(col[i]==2)label[i]-=2*delta;
				}
				for(int i=1;i<=n;i++)if(label[i]<=0)return 0;
				for(int i=1;i<=n_x;i++)
				{
					if(root[i]!=i||!slack[i]||root[slack[i]]==i)continue;
					if(calc_slack(g[slack[i]][i])==0&&augment_path(g[slack[i]][i]))return 1;
				}
				for(int i=n+1;i<=n_x;i++)
					if(root[i]==i&&col[i]==2&&label[i]==0)
						expand_blossom(i);
			}
			return 0;
		}
		void resize(int _n)
		{
			n=n_x=_n;
			allocate(2*n+5);
		}
		void init()
		{
			fill(mat.begin(), mat.begin() + (n_x+1), 0);
			for(int i=0;i<=n;i++)
			{
				root[i]=i;
				flower[i].clear();
				for(int j=0;j<=n;j++)
				{
					flower_from[i][j]=(i==j)?i:0;
					g[i][j]=edge_t(i,j,0);
				}
			}
			Value w_max=0;
			for(auto pr:edges)
			{
				int u=pr.second.first,v=pr.second.second;
				Value w=pr.first;
				g[u][v]=edge_t(u,v,w*2);
				g[v][u]=edge_t(v,u,w*2);
				w_max=max(w_max,w);
			}
			for(int i=1;i<=n;i++)label[i]=w_max;
		}
		void clear()
		{
			*this=weighted_blossom();
		}
		pair<int,Value>matching()
		{
			init();
			int cnt=0;
			Value s=0;
			while(augment())++cnt;
			for(int i=1;i<=n;i++)if(mat[i]>i)s+=g[i][mat[i]].w/2;
			return make_pair(cnt,s);
		}
	};
}
#endif