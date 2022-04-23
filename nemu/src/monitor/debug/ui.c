#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
void isa_reg_display(void);
void cpu_exec(uint64_t);
int is_batch_mode();
word_t vaddr_read1(vaddr_t addr);
word_t expr(char *e, bool *success);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_s(char *args){
  if(args!=NULL){
  cpu_exec(strtol(args,NULL,10));
  //printf("%ld\n",strtol(args,NULL,10));
  return 0;
  }else{
  cpu_exec(1);
  //printf("%ld\n",strtol(args,NULL,10));
  return 0;
  }
}

static int cmd_info(char *args){
  const char *temp=args;
  if(temp!=NULL){
    if(strcmp(temp,"r")==0||strcmp(temp,"w")==0){
     // printf("%s\n",temp);
      isa_reg_display();
      return 0;
    }else{
      printf("invalid argument");
      return -1;
    }
  }
  printf("invalid argument");
  return -1;
}
static int cmd_m_p(char *args){
  int r[2];
  r[0]=strtol(args,NULL,10);
  r[1]=strtol(args+2,NULL,16);
  for (int i=0;i<r[0];i++){
  	printf("0x%x\t0x%x\t\n",r[1]+i,vaddr_read1(r[1]+i));
  }
 // printf("%s____%d____%x",args,r[0],r[1]);
  return 0;
}

static int cmd_e_p(char *args){
	bool issuccess;
        expr(args, &issuccess);
      
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si", "Signal step execute,add [N] for n steps", cmd_s },
  {"info","\"info r\" for display regs state,\"info w\" for display watch_points", cmd_info },
  {"x","\"x N expr\" for print N memo from addr expr",cmd_m_p},
  {"p","\"x EXPR \" for caculate the result of the EXPR",cmd_e_p},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
