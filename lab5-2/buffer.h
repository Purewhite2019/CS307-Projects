#ifndef _BUFFER_H
#define _BUFFER_H

typedef int buffer_item;
#define BUFFER_SIZE 5

void init(void);
int insert_item(buffer_item item);
int remove_item(buffer_item *item);

#endif