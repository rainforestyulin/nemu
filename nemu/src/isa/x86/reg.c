#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include "local-include/reg.h"


const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  word_t sample[8];
  word_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));




  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
  
}

void isa_reg_display() {
  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    printf("%s\t0x%x\t%d\t\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
  }

 /*printf("eax%x\n",cpu.eax);
 printf("ECX-----%x\n",cpu.ecx);
 printf("EDX-----%x\n",cpu.edx);
 printf("EBX-----%x\n",cpu.ebx);
 printf("ESP-----%x\n",cpu.esp);
 printf("EBP-----%x\n",cpu.ebp);
 printf("ESI-----%x\n",cpu.esi);
 printf("EDI-----%x\n",cpu.edi);*/
/* printf("-----------------------------\n");
 printf("EAX-----%x\n",cpu.gpr[0]._32);
 printf("ECX-----%x\n",cpu.gpr[1]._32);
 printf("EDX-----%x\n",cpu.gpr[2]._32);
 printf("EBX-----%x\n",cpu.gpr[3]._32);
 printf("ESP-----%x\n",cpu.gpr[4]._32);
 printf("EBP-----%x\n",cpu.gpr[5]._32);
 printf("ESI-----%x\n",cpu.gpr[6]._32);
 printf("EDI-----%x\n",cpu.gpr[7]._32);*/

}

word_t isa_reg_str2val(const char *s, bool *success) {
	int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
	  printf("%s -- %s\n",s,regsl[i]);
    if(strcmp(s,regsl[i])==0){
      *success=true;
      return cpu.gpr[i]._32;
    }
  }
  return -1;
}
