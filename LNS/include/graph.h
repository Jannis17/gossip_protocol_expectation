#ifndef H_GRAPH_H_
#define H_GRAPH_H_

#include "../../nauty26r7/nauty.h"

#define LESS -1
#define EQUAL 0
#define GREATER 1

int edgesOf(graph*, int);

void addOnlySelfLoops(graph*, int);

void copyGraph(graph*, graph*, int);

void makeCall(graph*, int, int);

void printGraph(graph*, int);

void graphTest(int);

int possibleCalls(graph*, int, int);

int compGraphs(graph*, graph*, int);

void findCanonicalLabeling (graph* , graph* , int);

#endif /* H_GRAPH_H_ */
