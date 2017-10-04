#include "simulations.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "graph.h"
#include "memory.h"
#include "state.h"
#include "queue.h"
#include "compar.h"
#include "test.h"
#include "../../nauty26r7/nauty.h"

void simulate (int agents_min, int agents_max) {
	int n;
	
	graph secrets [MAXN*MAXM];
	graph avail_calls [MAXN*MAXM];
	
	for (n=agents_min; n<= agents_max; n++) {
		init_secrets_graph(secrets, n);
		init_avail_calls_graph(avail_calls,n);
	}
}
