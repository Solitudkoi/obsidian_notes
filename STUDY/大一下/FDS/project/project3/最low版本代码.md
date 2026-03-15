```c
#include<stdio.h>
#include<stdlib.h>
#define MAXWEIGHT 200
int judge(int **weight,int *s,int nv)		//determine whether it is Dijkstra Sequence
{
	int i;
	int **list=(int **)malloc(sizeof(int *)*(nv+1));		//list:from 1 to nv
	for(i=0;i<nv+1;i++)  
	{
		list[i]=(int *)malloc(sizeof(int)*2);		//Build a two-dimensional array to record the path
	}				
	for(i=0;i<nv+1;i++)
	{
		list[i][0]=MAXWEIGHT;
		list[i][1]=0;				//Initialize the array
	}
	
	int pm=MAXWEIGHT;  		//record the min path to choose next vertice
	for(i=1;i<nv+1;i++)
	{
		int min=MAXWEIGHT;
		if(i==1)
		{
			list[s[i]][0]=0;	
		}
		else if(list[s[i]][0]>pm) 		//Determine whether this vertice is the one with the shortest path
		{
			for(i=0;i<nv+1;i++) free(list[i]);
			free(list); return 0;
		}
		
		for(int j=1;j<nv+1;j++)
		{
			if((weight[s[i]][j]!=0)&&(weight[s[i]][j]+list[s[i]][0]<list[j][0])&&(list[j][1]==0))
				list[j][0]=weight[s[i]][j]+list[s[i]][0];   //Update the shortest paths of the nodes connected to this node
		}
		
		for(int k=1;k<nv+1;k++)
		{
			if(list[k][0]<iimin&&list[k][1]==0&&k!=s[i])			//Find the minimum path vertice,
			{
				min=list[k][0];
			}
		}
		pm=min;    		//record 
		list[s[i]][1]=1;		//record this vertice has been used
	}
	
	for(i=0;i<nv+1;i++) free(list[i]);
	free(list);			//free memory
	return 1;
}
int main()
{
	int nv,ne,i;
	scanf("%d %d",&nv,&ne);		//input
	int **weight=(int **)malloc(sizeof(int *)*(nv+1));
	for(i=0;i<nv+1;i++)  
		weight[i]=(int *)malloc(sizeof(int)*(nv+1));	//Build a two-dimensional array
	for(i=0;i<nv+1;i++)
		for(int j=0;j<nv+1;j++)
			weight[i][j]=0;		//Initialize the array
	for(i=0;i<ne;i++)
	{
		int r,c,w;
		scanf("%d %d %d",&r,&c,&w);
		weight[r][c]=w;			//w:from 1 to nv
		weight[c][r]=w;
	}		//input vertices and weights
	
	int num;
	scanf("%d",&num);
	for(i=0;i<num;i++)
	{
		int *s=(int *)malloc(sizeof(int)*(nv+1));
		for(int j=1;j<nv+1;j++)  		//s: from 1 to nv
		{
			scanf("%d",&s[j]);	      //input the sequence 
		}
		if(judge(weight,s,nv)==1)	printf("Yes\n");		//output
		else	printf("No\n");
		free(s);
	}
	for(i=0;i<nv+1;i++)  free(weight[i]);  //free memory
	free(weight);
	return 0;
} 
```