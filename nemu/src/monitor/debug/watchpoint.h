#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>
#define MAX_EXPR_LEN 64
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[MAX_EXPR_LEN];
  /* TODO: Add more members if necessary */

} WP;


#endif
