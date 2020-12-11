#include <sys/types.h>
#include "simple.h"
#include "graph.h"
#include "lock.h"

V* G;
V* T,*T1,*T2;
E* El;
int n_edge,max_d_node;

#define NANO 1000000000
#define DEBUG_BICONN 0 
#define DEBUG_VERIFY 0

void *SIMPLE_main(THREADED)
{
  int i,t,j, n_vertices,N,k,max=0;
  hrtime_t start,end;
  double interval,total=0;
  char * input_file;
  int * D;

  /*initialize graph from input file */  

  input_file = THARGV[0];

  start = gethrtime();
  on_one_thread{
	t = initialize_graph(input_file,&G,&n_vertices);
	if(t!=0) exit(0);
  }
  n_vertices=node_Bcast_i(n_vertices,TH);
  end = gethrtime();
  interval=end-start;
   printf("Time for initialization is %f\n",interval/NANO);
  node_Barrier();

  on_one_thread{
   max=0;
   for(i=0; i<n_vertices; i++)
   {
     if(G[i].n_neighbors>max){
	 max=G[i].n_neighbors; 
	 max_d_node=i;
     }
   }
  }
  node_Barrier();
 
#if 0
  start = gethrtime();
  initialize_graph_edgelist(G, n_vertices,&El,&n_edge, TH);
  end = gethrtime();
  interval=end-start;
  printf("Time for initialization(graph edge list) is %f\n",interval/NANO);
  node_Barrier();

  node_Barrier();
  start = gethrtime();
  spanning_tree_CRCW(G,El,n_vertices,n_edge,TH);
  end = gethrtime();
  interval=end-start;
  total+=interval;
  printf("Time for spanning_tree_CRCW is %f\n",interval/NANO);
  node_Barrier();

  pardo(i,0,n_edge,1)
  {
     El[i].workspace=0;
  }

  node_Barrier();
  start = gethrtime();
  spanning_tree_random01(G,El,n_vertices,n_edge,TH);
  end = gethrtime();
  interval=end-start;
  total+=interval;
  printf("Time for spanning_tree_random01 is %f\n",interval/NANO);
  node_Barrier();
#endif

  on_one_thread printf("METRICS:%d THREADS, file %s\n", THREADS, input_file);
  node_Barrier();
  start = gethrtime();
  spanning_tree_CREW(G,n_vertices,&T,TH);
  end = gethrtime();
  interval=end-start;
  total+=interval;
  node_Barrier();
  on_one_thread printf("METRICS:Time for spanning_tree_CREW is %f\n",interval/NANO);

  node_Barrier();
  start = gethrtime();
  spanning_tree_breadth(G,n_vertices,TH);
  end = gethrtime();
  interval=end-start;
  total+=interval;
  node_Barrier(); 
  on_one_thread printf("METRICS:Time for spanning_tree_breadth is %f\n",interval/NANO);

  node_Barrier();
  start = gethrtime();
  spanning_tree_breadth_B2(G,n_vertices,max_d_node,TH);
  end = gethrtime();
  interval=end-start;
  total+=interval;
  node_Barrier();
  on_one_thread printf("METRICS:Time for spanning_tree_breadth_B2 is %f\n",interval/NANO);

  node_Barrier();
  start = gethrtime();
  spanning_tree_e2d(G,n_vertices,TH);
  end = gethrtime();
  interval=end-start;
  total+=interval;
  node_Barrier();
  on_one_thread printf("METRICS:Time for spanning_tree_e2d is %f\n",interval/NANO);

#if DEBUG_VERIFY
  D=node_malloc(sizeof(int)*n_vertices,TH);
  pardo(i,0,n_vertices,1) D[i]=G[i].parent;
  node_Barrier();
  pardo(i,0,n_vertices,1)
    {
      while(D[i]!=D[D[i]]){
	D[i]=D[D[i]];
      }
    } 
  node_Barrier();
  printf("pointer jumping done\n");


  pardo(i,0,n_vertices,1)
    {
      if(D[i]!=D[0]) {
	printf("error\n");
	break;
      }
    }
  printf("done verifying\n");
  node_free(D,TH);
#endif

  on_one_thread{
	delete_graph(G,n_vertices);
	if(El) free(El);
  } 

  node_Barrier();
  SIMPLE_done(TH);
}






