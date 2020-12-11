#ifndef _GRAPH_H_
#define _GRAPH_H_

#define DIRECTED 1
#define UNDIRECTED 0
#define DEBUG_BALANCE 1
#define THRESHOLD 50  /* when number of elements on stack is less than threshold, we will not steal work from it */

typedef struct vertex{
  int parent;    
  int n_neighbors;
  int *my_neighbors;
  int round_removed; /*at which round is it removed if it is a degree 2 vertex*/
} V; /*tree structure for graph and spanning tree. Parent shows the parent information for a vertex in the spanning tree. Yet if we need to eliminate the degree 2 vertices, this "parent" isn't a parent, it actually means there is a teree edge between "me" and "parent", but we don't know who is who's parent.*/

typedef struct edge{
	int v1,v2;
	int is_in_tree;
	int workspace; /* used to show v2's index in v1's neighbor list*/
	} E;
 
int initialize_graph(const char * file,V** graph, int *nVertices);
int delete_graph(V* graph, int nVertices);
#endif /*_GRAPH_H_*/
