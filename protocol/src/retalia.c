

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
