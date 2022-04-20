#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static int buf_nr=0;
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
void   val_deal(int val,char *c){
        int depth=0;
	int tempp=10;
	int temp=val;
	printf("val_is_%d\n",val);
	while(temp>10){
		temp=temp/10;
		depth++;
	}
	*(c+buf_nr)=temp+48;
	buf_nr++;
	printf("add_%c\n",temp+48);
	for(int i=0;i<depth-1;i++){
		tempp=tempp*tempp;
	}
	if((val-temp*tempp)>0){
		val_deal(val-temp*tempp,c);
	}
};
int choose(int val){
      int p=rand()%(val-1);
      printf("choose is %d\n",p);
      return p;
};
void gen_num(){
        int temp=0;
        srand((int)time(0));
        temp=rand()%1000;
	printf("temp_is_%d \n",temp);
        val_deal(temp,buf);
	/*for(int i=0;i<buf_nr;i++){
	printf("%c\n",buf[i]);
	}*/
};
void gen(int a){
        buf[buf_nr++]=a;
	printf("add_%c\n",a);
};
void gen_rand_op(){
        switch(choose(4)){
                case 0: buf[buf_nr++]='+';
			printf("add_op%c\n",'+');
                case 1: buf[buf_nr++]='-';
			printf("add_op%c\n",'-');
                case 2: buf[buf_nr++]='*';
			printf("add__op%c\n",'*');
                case 3: buf[buf_nr++]='/';
			printf("add_op_%c\n",'/');
        }
};


static void gen_rand_expr() {
	switch (choose(3)) {
  	case 0: gen_num(); break;
    	case 1: gen('('); gen_rand_expr(); gen(')'); break;
    	default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  };
	//  buf[0] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    assert(fscanf(fp, "%d", &result));
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
