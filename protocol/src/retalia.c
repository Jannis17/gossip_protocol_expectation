/* needed for qsort*/
int comp_nodes (const void *p, const void *q) {
    graph x = *(const graph *)p;
    graph y = *(const graph *)q;

    /* Avoid return x - y, which can cause undefined behaviour
       because of signed integer overflow. */
    if (x < y)
        return -1;  // Return -1 if you want ascending, 1 if you want descending order. 
    else if (x > y)
        return 1;   // Return 1 if you want ascending, -1 if you want descending order. 

    return 0;
}


/* compares the ids of the args lexicographically */
int compStateIds(const void* item1, const void* item2)
{
	protocol_state_t* state1, *state2;
	
	state1 = (protocol_state_t *) item1;
	state2 = (protocol_state_t *) item2;
	
	if (state1->id < state2->id)
		return LESS;
	
	if (state1->id > state2->id)
		return GREATER;
	
	return EQUAL;
}
