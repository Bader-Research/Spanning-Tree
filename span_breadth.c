
#include "simple.h"
#include "graph.h"
#include "stack.h"

/*see if we can do a breadth-search by each thread, currently El and n_edges are not used*/
int spanning_tree_breadth(V* graph, int nVertices,THREADED)
{
#define S_POINTS (THREADS*THREADS*THREADS)
#define MYCOLOR (MYTHREAD+1)

  hrtime_t start,end;
  double interval;

  int * color, myroot, *stack,top,count,i,j,n,n_neighbors,seed=MYCOLOR,root,walks,r,counter=0,bottom;
  double power;
  int * count_M;
  int ** stack_M, **top_M, **bottom_M;

  count_M=node_malloc(THREADS*sizeof(int),TH);
  color=node_malloc(sizeof(int)*nVertices,TH);
  stack_M = node_malloc(THREADS*sizeof(int *),TH);
  top_M = node_malloc(THREADS*sizeof(int *),TH);
  bottom_M=node_malloc(THREADS*sizeof(int *),TH);

  stack_M[MYTHREAD]=malloc(nVertices*sizeof(int));
  stack=stack_M[MYTHREAD];
  top_M[MYTHREAD]=&top;
  bottom_M[MYTHREAD]=&bottom;

  pardo(i,0,nVertices,1){
    color[i]=0;
  }

  node_Barrier();

  top=-1;
  bottom=-1;
  count=0;

  /*lets select a point to start in the graph*/
  on_one_thread {
    root=(rand_r(&seed)%nVertices);
    power=1/(float)THREADS;
    color[root]=MYCOLOR;
  }

  root=node_Bcast_i(root,TH);
  printf("walks is %d, root is %d\n",walks,root);

  /*lets first generate some candidates for the threads to start their random walks*/
	       
  node_Barrier();
  on_one_thread {
    color[root]=MYCOLOR;
    graph[root].parent=root;
    myroot=root;
    j=0;
    push(myroot,stack_M[j],top_M[j]);
    j=(j+1)%THREADS;
    i=0;
    for(i=0;i<S_POINTS;i++)
      {
	n_neighbors=graph[myroot].n_neighbors;
	r=rand_r(&seed);
	if(r%2==0){
	  for(r=0;r<n_neighbors;r++)
	    {
	      n=graph[myroot].my_neighbors[r];
	      if(color[n]==0){
		graph[n].parent=myroot;
		color[n]=MYCOLOR;
		myroot=n;
		count++;
		push(myroot,stack_M[j],top_M[j]);
		/*printf("sub tree : %d \n",myroot);*/
		j=(j+1)%THREADS;
		break;
	      } ;
	    }
	  if(r==n_neighbors) {
	    r=(rand_r(&seed)%n_neighbors);
	    myroot=graph[myroot].my_neighbors[r];
	  }
	}
	else {
	  for(r=n_neighbors-1;r>=0;r--)
	    {
	      n=graph[myroot].my_neighbors[r];
	      if(color[n]==0){
		graph[n].parent=myroot;
		color[n]=MYCOLOR;
		myroot=n;
		count++;
		push(myroot,stack_M[j],top_M[j]);
		/*printf("sub tree : %d \n",myroot);*/
		j=(j+1)%THREADS;
		break;
	      }      
	    }
	  if(r<0){
	    r=(rand_r(&seed)%n_neighbors);
	    myroot=graph[myroot].my_neighbors[r];
	  }
	}		
      }
    end=gethrtime();
    interval=end-start;
    printf("generating sub tree done\n");
  }

  node_Barrier();
  while(!is_empty( stack, &top,&bottom))
    {
      n=pop(stack,&top,bottom);
      if(n==-1) {
	printf("stack overflow\n");
	bottom=-1;
	top=-1;
	break;
	}
      if(MYTHREAD%2==0)
	{
	  for(i=0;i<graph[n].n_neighbors;i++)
	    {
	      if(color[graph[n].my_neighbors[i]]==0) {/*found new frontier*/
		color[graph[n].my_neighbors[i]]=MYCOLOR;
		push(graph[n].my_neighbors[i],stack,&top);
		count++;
	      }
	  
	    }
	} else{
	  for(i=graph[n].n_neighbors-1;i>=0;i--)
	    {
	      if(color[graph[n].my_neighbors[i]]==0) {/*found new frontier*/
		color[graph[n].my_neighbors[i]]=MYCOLOR;
		push(graph[n].my_neighbors[i],stack,&top);
		count++;	      
	      }
	    }
      
	}
    }
  count_M[MYTHREAD]=count;
  node_Barrier();
  printf("Thread %d count is %d\n",MYTHREAD, count);

  on_one_thread{
    int max=0, min=nVertices;
    for(i=0;i<THREADS;i++)
      {
	if(count_M[i]>max) max=count_M[i];
	if(count_M[i]<min) min=count_M[i];
      }
    printf("METRICS:=====span_breadth:The difference between counts is %d\n",max-min);
  }
  node_Barrier();
  node_free(count_M,TH);
  node_free(stack_M,TH);
  node_free(top_M,TH);
  node_free(bottom_M, TH);
  node_free(color, TH);
  free(stack);
}





