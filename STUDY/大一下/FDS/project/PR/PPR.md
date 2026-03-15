```c
#include <stdio.h> 
int e[1001][1001],Nv,Ne,dis[1001];//e:edges dis:distances 
int seq[1001];//sequence 
int v[1001]; // visited or not 005 
const int inf=999999;//represent a infinite number 
int h[1001],pos[1001],size;//min_heap 
void init_e() 
{ 
	int i,j; 
	for(i=1;i<=Nv;i++) 
	for(j=1;j<=Nv;j++) 
		e[i][j]=inf; 
}//initial edges 
void init_dis(int source) 
{ 
	int i; 
	for(i=1;i<=Nv;i++) 
	{ 
		dis[i]=inf; 
		v[i]=0; 
	} 
	dis[source]=0; 
}//initial distance 
void swap(int x,int y) 
{ 
	int tmp=h[x]; 
	h[x]=h[y]; 
	h[y]=tmp; 
	pos[h[x]]=x; 
	pos[h[y]]=y; 
}//swap the position in heap 
void up(int x) 
{ 
	while(x>1&&dis[h[x]]<dis[h[x/2]]) 
		swap(x,x/2),x/=2; 
}//update a node in the direction-up 
void down(int x) 
{ 
	int s=x; 
	while(1) 
	{
		if(x*2<=size&&dis[h[x*2]]<dis[h[s]])s=x*2; 
		if(x*2+1<=size&&dis[h[x*2+1]]<dis[h[s]])s=x*2+1; 
		if(s!=x) 
		{ 
			swap(x,s); 
			x=s; 
		} 
		else break; 
	} 
}//update a node in the direction-down 
void push(int x) 
{ 
	h[++size]=x; 
	pos[x]=size; 
	up(size); 
}//add a node into the heap 
void pop() 
{ 
	swap(1, size--); 
	down(1); 
}//delete the head node 
void update(int x) 
{ 
	up(pos[x]); 
} 
void init_heap() 
{ 
	int i; 
	size=0; 
	for(i=0;i<=Nv;i++)pos[i]=0; 
	push(seq[1]); 
} 
int Dijkstra_heap() 
{ 
	int idx=1,i; 
	while(idx <= Nv) 
	{ 
		int u=seq[idx]; 
		if(v[u])return 0;//visited 
		if(dis[u]!=dis[h[1]])return 0; //dis of sequence current node isn't the shortest 
		v[u]=1;//marked visited 
		pop(); 
		for(i=1;i<=Nv;i++)
		{ 
			if(!v[i]&&e[u][i]!=inf) 
			{ 
				if(dis[i]>dis[u]+e[u][i]) 
				{ 
					dis[i]=dis[u]+e[u][i]; 
					if(pos[i])update(i); //has been in the heap 
					else push(i); 
				} 
			} 
		} 
		idx++; 
	} 
	return 1; 
}//main function of heap 
int main() 
{ 
	int i,x,y,w,K; 
	/*freopen("tree_sample.txt","r",stdin); 
	freopen("tree_output.txt","w",stdout);*/ 
	//optional,to read a big file 
	scanf("%d %d",&Nv,&Ne); 
	init_e(); 
	for(i=1;i<=Ne;i++) 
	{ 
		scanf("%d %d %d",&x,&y,&w); 
		if(w<e[x][y])e[x][y]=e[y][x]=w;//input the weight of edge 
	} 
	scanf("%d",&K); 
	while(K--) 
	{ 
		for(i=1;i<=Nv;i++)scanf("%d",&seq[i]); 
		init_dis(seq[1]); 
		init_heap(); 
		int f=Dijkstra_heap(); 
		if(f)printf("Yes\n"); 
		else printf("No\n"); 
	} 
	return 0; 
}
```