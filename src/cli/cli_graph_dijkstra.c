#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "cli.h"

/*
  Find index for the element in vertex array *arr that has value of v
*/
int
find_index(vertexid_t *arr, vertexid_t v)
{
  int idx = 0;
  while(arr[idx] != v)
    idx++;
  return idx;
}

/*
  A utility function to find the vertex with minimum distance value, from
    the set of vertices not yet included in shortest path tree
    len: number of vertices
*/
int
minDistance(int dist[], bool sptSet[], int len)
{
   // Initialize min value
   int min = INT_MAX, min_index;

   for (int v = 0; v < len; v++)
     if (sptSet[v] == false && dist[v] <= min)
         min = dist[v], min_index = v;

   return min_index;
}

/*
  Function to print shortest path from source to j using parent array
*/
void
printPath(vertexid_t *parent, vertexid_t *v_arr, int j)
{
    // Base Case : If j is source
    if (parent[j]==-1)
        return;

    printPath(parent, v_arr, parent[j]);

    printf(" -> %llu", v_arr[j]);
}

/*
  Dijkstra algorithm implementation
*/
void
cli_graph_dijkstra(char *cmdline, int *pos)
{
  graph_t g;
  vertex_t v;
  vertex_t x;
  edge_t e;

  char s[BUFSIZE];
	int graph_num, vertex_num, cnt;
  int i, j;
  int len = 0;
  int idx;

  memset(s, 0, BUFSIZE);
  nextarg(cmdline, pos, " ", s);
  if (strlen(s) == 0) {
		printf("Missing graph id\n");
		return;
	}
	graph_num = atoi(s);

  memset(s, 0, BUFSIZE);
	nextarg(cmdline, pos, " ", s);
	if (strlen(s) == 0) {
		printf("Missing vertex id\n");
		return;
	}
	vertex_num = atoi(s);

	// Change current graph
	for (g = graphs, cnt = 0; g != NULL; g = g->next, cnt++)
		if (cnt == graph_num)
			current = g;

  // Find the vertex to be read
  v = graph_find_vertex_by_id(current, vertex_num);
  if (v == NULL) {
		printf("Vertex %d doesn't exist in graph %d!\n", vertex_num, graph_num);
		return;
	}

  printf("Running Dijkstra on [graph %d] for [vertex %d]...\n", graph_num, vertex_num);

  // Vertex array that holds the input order of vertices
  vertexid_t *vertex_arr = (vertexid_t*)malloc(sizeof(vertexid_t)*INT_MAX);
  // Parent array to store shortest path tree
  vertexid_t *parent = (vertexid_t*)malloc(sizeof(vertexid_t)*INT_MAX);

	/* Vertices */
	for (x = current->v; x != NULL; x = x->next) {
    vertex_arr[len] = x->id;
    len++;
	}

	/* Edges */
  int graph_matrix[len][len];
  // Initialize graph matrix w/ zeros
  for(i = 0; i < len; i++){
    for(j = 0; j < len; j++){
        graph_matrix[i][j] = 0;
    }
  }
	for (e = current->e; e != NULL; e = e->next) {
    // Find all neighbors for vertex m & put them into graph matrix
    for (int m = 0; m < len; m++){
      if (vertex_arr[m] == e->id1){
        idx = find_index(vertex_arr,e->id2);
        // Assign a weight of 1 by default if an edge exists
        // Otherwise just keep 0.
        graph_matrix[m][idx] = 1;
        graph_matrix[idx][m] = 1;
      }
      else if (vertex_arr[m] == e->id2){
        idx = find_index(vertex_arr,e->id1);
        graph_matrix[m][idx] = 1;
        graph_matrix[idx][m] = 1;
      }
    }
	}

  // Store shortest distance from src to all vertices in a distance array dist[]
  int dist[len];
  int src = find_index(vertex_arr, vertex_num);
  // sptSet[i] = true if vertex i is included in shortest
  // path tree or shortest distance from src to i is finalized
  bool sptSet[len];

  // Initialize all distances as INFINITE and stpSet[] as false
  for (int m = 0; m < len; m++)
     dist[m] = INT_MAX, sptSet[m] = false;

  // Set parent of source vertex to -1 (no parent)
  parent[src] = -1;

  // Distance of source vertex from itself = 0
  dist[src] = 0;

  // Find shortest path ...
  for (int count = 0; count < len-1; count++)
  {
    // Pick the vertex from the set of vertices not yet processed w/ min distance.
    int u = minDistance(dist, sptSet, len);

    // Mark the vertex as processed
    sptSet[u] = true;

    // Update dist value of the adjacent vertices of the picked vertex.
    for (int v = 0; v < len; v++)

      // Update dist[v] only when:
      //  1) v is not in sptSet,
      //  2) Edge u-v
      //  3) Total weight of path from src to v through u < dist[v]
      if (!sptSet[v] && graph_matrix[u][v] && dist[u] != INT_MAX && dist[u]+graph_matrix[u][v] < dist[v])
      {
        parent[v]  = u;
        dist[v] = dist[u] + graph_matrix[u][v];
      }
  }

  // Print dijkstra result for destination, distance and path
  printf("Destination\tDistance from vertex %d\tPath\n", vertex_num);
  for (int m = 0; m < len; m++)
  {
    printf("%llu\t\t%d\t\t\t%llu", vertex_arr[m], dist[m], vertex_arr[src]);
    printPath(parent, vertex_arr, m);
    printf("\n");
  }

  // Free allocated memory
  free(vertex_arr);
  free(parent);
}
