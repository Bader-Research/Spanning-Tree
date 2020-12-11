#include "simple.h"
#include "graph.h"


/* spanning_tree. The vertex is grafted onto the smallest vertex that is adjacent to it. So there is no need for concurrent write*/

#define v_attribute parent

int spanning_tree_CREW(V* graph, int nVertices, V** pTree,THREADED)
{
  int *p_changed,*buffer,*D,*done,i,j,k,min,winner,changed,iteration=0;
  V* tree=*pTree;

#if DEBUG_TIMING
  hrtime_t start,end;
  double interval1=0,interval2=0,interval3=0;
#endif

#if DEBUG_CORRECTNESS
  int edge_count=0;
#endif

  p_changed=node_malloc(sizeof(int),TH);
  on_one_thread (*p_changed)=1;

  buffer=node_malloc(sizeof(int)*nVertices,TH);
  D=node_malloc(sizeof(int)*nVertices,TH); /*used for Di*/
  done=node_malloc(sizeof(int)*nVertices,TH);

  pardo(i,0,nVertices,1) /*initialize the D values, Jaja p217*/
    {     
      D[i]=i;
      done[i]=0;
      graph[i].v_attribute=i;
    }

  node_Barrier();
  while((*p_changed)==1)
    {
    iteration++;
    changed=0;
    node_Barrier();
      /*1. phase one*/

#if DEBUG_TIMING
    start= gethrtime();
#endif

     on_one_thread (*p_changed)=0;

      node_Barrier();
      pardo(i,0,nVertices,1)
	{
	 if(D[i]!=D[D[i]]) continue;

	 min = D[i];
	 winner=0;

	 for(k=0;k<graph[i].n_neighbors;k++)
	  {
	    j=graph[i].my_neighbors[k];
	    if(D[j]<D[i] && D[j]<min && done[D[i]]==0)
	      {
		min = D[j];
		winner=j;
	      }
	  }

	  if(min!=D[i]) {
		changed=1;
		done[D[i]]=1;
		graph[D[i]].v_attribute=D[winner];
		D[D[i]]=D[winner];
#if DEBUG_CORRECTNESS
		edge_count++;
#endif
	  }
	}

      node_Barrier();

#if DEBUG_TIMING
      end = gethrtime();
      interval1+=end-start;
#endif

	/*3.phase 3*/

#if DEBUG_GRAPH
      printf("phase 3:====\n");
#endif
#if DEBUG_TIMING
      start = gethrtime();
#endif

      node_Barrier();
      pardo(i,0,nVertices,1)
	{
	  while(D[D[i]]!=D[i]) D[i]=D[D[i]];
        }

      if(changed) (*p_changed)=1;
      node_Barrier();

#if DEBUG_TIMING
      end = gethrtime();
      interval3+=end-start;
#endif
      
    }/*while*/

    node_Barrier();

#if 0
    pardo(i,0,nVertices,1)
    {
	 if(D[i]!=D[0]) {
		printf("ERROR in SPANNING TREE\n");
		break;
	}
    }
#endif

#if DEBUG_TIMING
    on_one_thread {
      printf("total time for phase 1 :%f s\n", interval1/1000000000);
      printf("total time for phase 2 :%f s\n", interval2/1000000000);
      printf("total time for phase 3 :%f s\n", interval3/1000000000);
    } 
#endif

#if DEBUG_CORRECTNESS
    printf("Total edges got is %d\n",edge_count);
#endif

    on_one_thread printf("Number of iterations is %d\n",iteration);
    node_free(p_changed,TH);
    node_free(buffer, TH);
    node_free(D, TH);
    node_free(done, TH);

    (*pTree)=tree;
}

