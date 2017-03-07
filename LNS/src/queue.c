/* queue.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libshar"
#include "queue.h"


/*
 * reset_queue: resets a queue
 *   hd: queue (this)
 *
 * returns queue length
 */
unsigned long
reset_queue(struct queue_t *hd)
{
	unsigned long i;

	if(!hd)
		return 0;

	i=0;
	while(!QUEUE_IS_EMPTY(hd)) {
		dequeue_from_queue(hd, NULL);
		i++;
	}
	hd->head=NULL;
	hd->count=0;

	return i;
}

/*
 * new_queue: create a queue (ctor)
 *   max: maximum number of nodes (optional)
 *   compar: comparison function for node data
 *   print_data_fun: node data pretty printing function (optional)
 *
 * returns the queue created, NULL on error
 */
struct queue_t *
new_queue(const unsigned long max,
	int (*compar)(const void *, const void *),
	void (*print_data_fun)(FILE *fp, const void *))
{
	struct queue_t *hd;

	MALLOC_ZERO(hd, sizeof(struct queue_t));
	hd->compar=compar?compar:default_compar_fun;
	hd->print_data_fun=print_data_fun;
	QUEUE_SET_MAX(hd, max);

	return hd;
}

/*
 * new_queue_from_array: create a queue from an array (ctor)
 *   data: input array
 *   numdata: array length
 *   max: maximum number of nodes (optional)
 *   compar: comparison function for node data
 *   print_data_fun: node data pretty printing function (optional)
 *
 * returns the queue created, NULL on error
 */
struct queue_t *
new_queue_from_array(void **data, const unsigned long numdata,
	const unsigned long max,
	int (*compar)(const void *, const void *),
	void (*print_data_fun)(FILE *fp, const void *))
{
	struct queue_t *hd;
	unsigned long i;

	if(!data||0==numdata)
		return NULL;

	hd=new_queue(max, compar, print_data_fun);
	for(i=0; i<numdata; i++)
		if(!QUEUE_IS_FULL(hd))
			enqueue_to_queue(hd, data[i]);
		else
			break;

	return hd;
}

/*
 * delete_queue: destroys a queue
 *   hd: queue (this)
 */
void
delete_queue(struct queue_t *hd)
{
	reset_queue(hd);
	FREE(hd);
}

/*
 * queue_to_array: export a queue to an array
 *   hd: queue (this)
 *   siz: array length returned
 *
 * returns the array from the queue, NULL on error
 */
void **
queue_to_array(const struct queue_t *hd, unsigned long *siz)
{
	void **qv;
	struct queue_node_t *p;
	unsigned long i;

	if(!hd||!siz)
		return NULL;

	i=hd->count;
	if(!i) {
		*siz=0;

		return NULL;
	}

	MALLOC_SAFE(qv, i*sizeof(void *));

	i=0;
	QUEUE_FOREACH(p, hd) {
		qv[i]=p->data;
		i++;
	}

	*siz=i;

	return qv;
}

/*
 * sizeof_queue: calculate a queue's memory footprint
 *   hd: queue (this)
 */
size_t
sizeof_queue(const struct queue_t *hd)
{
	struct queue_node_t *p;
	size_t l;

	if(!hd)
		return 0;

	l=sizeof(struct queue_t);
	QUEUE_FOREACH(p, hd)
		l+=sizeof(struct queue_node_t);

	return l;
}

/*
 * fprint_hash: queue pretty printing
 *   fp: output file stream
 *   hd: queue (this)
 */
int
fprint_queue(FILE *fp, const struct queue_t *hd)
{
	struct queue_node_t *p;
	int i;

	if(!fp||!hd)
		return 0;

	i=0;
	fprintf(fp, "QUEUE %p [%lu/%lu]\t#<data> {this = (next, data)}\n",
		(void *) hd, hd->count, hd->max
	);
	fprintf(fp, "mem: %lu bytes\n", (unsigned long) sizeof_queue(hd));
	fprintf(fp, "================================\n");
	QUEUE_FOREACH(p, hd) {
		fprintf(fp, "<");
		if(hd->print_data_fun)
			hd->print_data_fun(fp, p->data);
		fprintf(fp, "> {%p = (%p, %p)}\n",
			(void *) p, (void *) p->next, p->data
		);
		i++;
	}
	fprintf(fp, "================================\n");

	return i;
}


/*
 * count_queue_data: counts data's appearances in queue
 *   hd: queue (this)
 *   data: data to lookup
 *
 * returns data's appearance count
 */
unsigned long
count_queue_data(const struct queue_t *hd, const void *data)
{
	struct queue_node_t *p;
	unsigned long i;

	i=0;
	QUEUE_FOREACH(p, hd)
		if(!hd->compar(p->data, data))
			i++;

	return i;
}

/*
 * search_queue: search data in queue
 *   hd: queue (this)
 *   found: data's queue node (optional)
 *   prev: data's previous queue node (optional)
 *   all: all queue nodes with this data (optional)
 *   where: data queue node's offset in queue (optional)
 *   data: data to lookup
 *
 * returns 1 if found, 0 otherwise
 */
int
search_queue(struct queue_t *hd,
	struct queue_node_t **found, struct queue_node_t **prev,
	struct queue_t **all, int *where,
	const void *data)
{
	struct queue_t *q;
	struct queue_node_t *p, *pp;
	int px;

	if(QUEUE_IS_EMPTY(hd)) {
		if(all)
			*all=NULL;

		return 0;
	}

	px=0;
	p=hd->head;
	if(!hd->compar(p->data, data)) {
		if(prev)
			*prev=NULL;
		if(where)
			*where=px;
		if(found)
			*found=p;

		if(!all)
			return 1;
		else
			goto sunexeia;
	}
	QUEUE_FOREACH(p, hd) {
		px++;
		if(p->next)
			if(!hd->compar(p->next->data, data)) {
				if(prev)
					*prev=p;
				if(where)
					*where=px;
				p=p->next;
				if(found)
					*found=p;

				if(!all)
					return 1;
				else
					goto sunexeia;
			}
	}

	return 0;

sunexeia:
	q=new_queue(hd->max, hd->compar, hd->print_data_fun);
	QUEUE_FOREACH_START(pp, p)
		if(pp->next)
			if(!hd->compar(data, pp->next->data))
				enqueue_to_queue(q, pp->next->data);
	*all=q;

	return 1;
}


/*
 * enqueue_to_queue: enqueues an item to queue
 *   hd: queue (this)
 *   data: item to enqueue
 *
 * returns 1
 */
int
enqueue_to_queue(struct queue_t *hd, void *data)
{
	struct queue_node_t *p;

	if(QUEUE_IS_FULL(hd))
		return 0;

	MALLOC_SAFE(p, sizeof(struct queue_node_t));

	p->data=data;
	p->next=NULL;
	if(!hd->head)
		hd->head=hd->tail=p;
	else {
		hd->tail->next=p;
		hd->tail=p;
	}

	hd->count++;

	return 1;
}

/*
 * enqueue_to_queue_unique: enqueues a unique item to queue
 *   hd: queue (this)
 *   data: item to enqueue
 *
 * returns 0 if item is already in queue, 1 otherwise
 */
int
enqueue_to_queue_unique(struct queue_t *hd, void *data)
{
	if(QUEUE_IS_FULL(hd))
		return 0;

	if(search_queue(hd, NULL, NULL, NULL, NULL, data))
		return 0;

	enqueue_to_queue(hd, data);

	return 1;
}

/*
 * dequeue_from_queue: dequeues an item from queue
 *   hd: queue (this)
 *   data: dequeued item (optional)
 *
 * returns 1 if successful, 0 otherwise
 */
int
dequeue_from_queue(struct queue_t *hd, void **data)
{
	struct queue_node_t *p;
	void *n;

	if(QUEUE_IS_EMPTY(hd)) {
		if(data)
			*data=NULL;

		return 0;
	}

	n=hd->head->data;
	p=hd->head;
	hd->head=hd->head->next;
	FREE_SAFE(p);

	hd->count--;

	if(data)
		*data=n;

	return 1;
}


/*
 * swap_queue_data: exchange data between two queue nodes
 *   hd: queue (this)
 *   node1: first queue node
 *   node2: second queue node
 *
 * if an item for either keys does not exist return 0,
 *   else return 2 if nodes are the same, else return 1
 */

int
swap_queue_data(struct queue_t *hd,
	struct queue_node_t *node1, struct queue_node_t *node2)
{
	struct queue_node_t *p, *pp1, *pp2, *temp;
	int ip1, ip2, datap, px;

	if(QUEUE_IS_EMPTY(hd))
		return 0;

	px=0;
	ip1=-1;
	ip2=-1;
	/* only one node can be queue head - they are different */
	if(hd->head==node1)
		ip1=px;
	else if(hd->head==node2)
		ip2=px;
	pp1=NULL;
	pp2=NULL;
	QUEUE_FOREACH(p, hd) {
		px++;
		if(p->next) {
			if(node1==p->next) {
				pp1=p;
				ip1=px;
			} else if(node2==p->next) {
				pp2=p;
				ip2=px;
			}
		}
		if(-1!=ip1&&-1!=ip2)
			break;
	}
	if(-1==ip1||-1==ip2)
		return 0;
	if(node1==node2)
		return 2;

	if(ip2<ip1) {
		VAR_SWAP(node1, node2, temp);
		VAR_SWAP(pp1, pp2, temp);
		VAR_SWAP(ip1, ip2, datap);
	}

	if(pp1)
		pp1->next=node2;
	else
		hd->head=node2;
	pp2->next=node1;
	if(!node2->next)
		hd->tail=node1;
	VAR_SWAP(node1->next, node2->next, temp);

	return 1;
}

/*
 * run_queue_prog: run a programme for every queue node datum
 *   hd: queue (this)
 *   prog: programme
 *   param: programme's parameters
 *
 * returns the number of queue nodes the programme ran for
 * NOTE: programme's parameters are:
 *   1: current queue node datum
 *   2: param parameter of this function
 */
unsigned long
run_queue_prog(struct queue_t *hd,
	void (*prog)(void **, void *), void *param)
{
	struct queue_node_t *p;
	unsigned long j;

	j=0;
	QUEUE_FOREACH(p, hd) {
		prog(&p->data, param);
		j++;
	}

	return j;
}


#ifdef T_UTST
#include <time.h>

/* hash operations */
enum {
	HD_ENQUEUE=0,		/* arg1 = item */
	HD_DEQUEUE,		/* arg1 = item */
	HD_SEARCH,		/* arg1 = item */
	HD_SWAP,		/* arg1 = item1, arg2 = item2 */
	HD_ENQUEUE_UNIQUE,	/* arg1 = item */
	HD_COUNT_ITEM,		/* arg1 = item */
	ALL_HD_OPS		/* marker */
};

/* queue workload structure */
struct queue_work_t {
	struct queue_t *q;
	unsigned long maxkey;
	unsigned int nops;	/* number of operations */
};

int		compar_fn(const void *item1, const void *item2);
void		do_work(struct queue_work_t *work);
void		print_data_fn(FILE *fp, const void *item);
void		usage(const char *argv0);

int
compar_fn(const void *item1, const void *item2)
{
	unsigned long a, b;

	a=(unsigned long) item1;
	b=(unsigned long) item2;

	return a-b;
}

void
print_data_fn(FILE *fp, const void *item)
{
	fprintf(fp, "%lu", (unsigned long) item);
}

void
do_work(struct queue_work_t *work)
{
	struct queue_t *q;
	struct queue_node_t *p1, *p2;
	unsigned long y;
	int x, a, b;
	unsigned long arg1, arg2, maxkey;
	unsigned char op;
	unsigned int i, nops;
	uint64_t ms;

	q=work->q;
	nops=work->nops;
	maxkey=work->maxkey;

	srand(time(NULL));

	ms=get_sys_time();

	for(i=0; i<nops; i++) {
		op=rand()%ALL_HD_OPS;
		arg1=rand()%maxkey;
		arg2=rand()%maxkey;

		switch(op) {
		case HD_ENQUEUE:
			x=enqueue_to_queue(q, (void *) arg1);
#ifdef PRINTOUT
			if(!x)
				printf("\t\t#");
			printf("%u\tenqueue \t%lu", i, arg1);
			printf("\n");
#endif /* PRINTOUT */
		break;

		case HD_ENQUEUE_UNIQUE:
			x=enqueue_to_queue_unique(q, (void *) arg1);
#ifdef PRINTOUT
			if(!x)
				printf("\t\t#");
			printf("%u\tenqueue_unique\t%lu", i, arg1);
			printf("\n");
#endif /* PRINTOUT */
		break;

		case HD_DEQUEUE:
			x=dequeue_from_queue(q, (void **) &y);
#ifdef PRINTOUT
			if(!x)
				printf("\t\t#");
			printf("%u\tdequeue      \t%lu\n", i, y);
#endif /* PRINTOUT */
		break;

		case HD_SEARCH:
			x=search_queue(q, NULL, NULL, NULL, NULL,
				(void *) arg1
			);
#ifdef PRINTOUT
			if(!x)
				printf("\t\t#");
			printf("%u\tsearch        \t%lu\n", i, arg1);
#endif /* PRINTOUT */
		break;

		case HD_SWAP:
			a=search_queue(q, &p1, NULL, NULL, NULL,
				(void *) arg1
			);
			b=search_queue(q, &p2, NULL, NULL, NULL,
				(void *) arg2
			);
			if(!a||!b)
				x=0;
			else
				x=swap_queue_data(q, p1, p2);
#ifdef PRINTOUT
			if(!x)
				printf("\t\t#");
			printf("%u\tswap          \t%lu, %lu\n", i,
				arg1, arg2
			);
#endif /* PRINTOUT */
		break;

		case HD_COUNT_ITEM:
			x=count_queue_data(q, (void *) arg1);
#ifdef PRINTOUT
			if(!x)
				printf("\t\t#");
			printf("%u\tcount_item    \t%lu (%d)\n", i, arg1, x);
#endif /* PRINTOUT */
		break;

		default:
			FLOG_ERROR("queue_test: unknown queue opcode");
		}
	}

	ms=get_sys_time()-ms;
	fprintf(stderr, "done in %lu ms\n", (unsigned long) ms);
}

void
usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s INIT QUEUEMAX NOPS MAXKEY\n", argv0);
}

int
main(int argc, const char *argv[])
{
	const unsigned long init[]={5, 1, 3, 9, 7, 10, 2};
	struct queue_t *q;
	unsigned long max;
	unsigned long maxkey;
	int init0, nops;
	struct queue_work_t work;
	unsigned long *array;
#ifdef PRINTOUT
	unsigned long i;
#endif /* PRINTOUT */
	unsigned long siz;

	if(5!=argc) {
		usage(argv[0]);

		return 0;
	}

	init0=atoi(argv[1]);
	max=atoi(argv[2]);
	nops=atoi(argv[3]);
	maxkey=atol(argv[4]);

	if(init0) {
		q=new_queue_from_array(
			(void **) &init, sizeof(init)/sizeof(unsigned long),
			max, compar_fn, print_data_fn
		);
#ifdef PRINTOUT
		printf("\n");
		fprint_queue(stdout, q);
		printf("\n");
#endif /* PRINTOUT */
	} else
		q=new_queue(max, compar_fn, print_data_fn);

	work.q=q;
	work.nops=nops;
	work.maxkey=maxkey;
	do_work(&work);

#ifdef PRINTOUT
	printf("\n");
	fprint_queue(stdout, q);
	printf("\n");
#endif /* PRINTOUT */

	array=(unsigned long *) queue_to_array(q, &siz);
#ifdef PRINTOUT
	printf("QUEUE VALUES TO ARRAY (%lu):\n", siz);
	for(i=0; i<siz; i++)
		printf("%lu\t", array[i]);
	printf("\n\n");
#endif /* PRINTOUT */
	FREE_SAFE(array);

	DELETE_QUEUE(q);

	return 0;
}
#endif /* T_UTST */
