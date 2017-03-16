#ifndef H_COMPAR_H_
#define H_COMPAR_H_

#define NEW_ITEM 1
#define DUPLICATE_ITEM 0

int compStates(const void* item1, const void* item2);

int compStateIds(const void* item1, const void* item2);

int compChildren(const void* item1, const void* item2);

#endif /*H_COMPAR_H_*/
