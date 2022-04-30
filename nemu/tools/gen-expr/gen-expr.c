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
"  signed result = %s; "
"  printf(\"%%d\", result); "
"  return 0; "
"}";
void val_deal(int val,char *c,int *maxdepth_p,int *buf_nr_p){
        int depth=0;
	int tempp=10;
	int temp=val;
	while(temp>=10){
		temp=temp/10;
		depth++;
	}
	if(*maxdepth_p==-1){
		(*maxdepth_p)=depth;
		for(int i=*buf_nr_p;i<=(*buf_nr_p)+depth;i++){
			*(c+i)='0';
		}
	}
	*(c+*(buf_nr_p)+*maxdepth_p-depth)=temp+48;
	for(int i=0;i<depth-1;i++){
                tempp=tempp*tempp;
        }
	if((val-temp*tempp)>0){
                val_deal(val-temp*tempp,c,maxdepth_p,buf_nr_p);
	}
};
void val_deal_tool(int val,char *c,int *buf_nr_p){
	int t_maxdepth=-1;
	if(val>=0){
		val_deal(val,c,&t_maxdepth,buf_nr_p);
		*buf_nr_p=*buf_nr_p+t_maxdepth+1;
	}else{
		*(c+buf_nr)='-';
		(*buf_nr_p)++;
		val_deal(-val,c,&t_maxdepth,buf_nr_p);
        	*buf_nr_p=*buf_nr_p+t_maxdepth+2;
	}
	
}

int choose(int val){
	srand((int)(time(0) + rand()));
	int p=rand()%val;
	return p;
};
void gen_num(){
        int temp=0;
        srand((int)(time(0) + rand()));
        temp=rand()%1000;
	//printf("%d\n",temp);
	//printf("temp_is_%d \n",temp);
        val_deal_tool(temp,buf,&buf_nr);
	/*for(int i=0;i<buf_nr;i++){
	printf("%c\n",buf[i]);
	};*/
};
void gen(int a){
        buf[buf_nr++]=a;
};
void gen_rand_op(){
        switch(choose(4)){
                case 0: buf[buf_nr++]='+';break;
                case 1: buf[buf_nr++]='-';break;
                case 2: buf[buf_nr++]='*';break;
                case 3: buf[buf_nr++]='/';break;
        }
};

static void gen_rand_expr() {
	assert(buf_nr<65530);
	switch (choose(3)) {
  	case 0: gen_num(); break;
    	case 1: gen('('); gen_rand_expr(); gen(')'); break;
    	default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  };
	  buf[buf_nr] = '\0';
}

int main(int argc, char *argv[]) {
/*	for (int i=0;i<2;i++){
		buf_nr=0;
		gen_rand_expr();
	
	for(int i=0;i<buf_nr;i++){
        printf("%c",buf[i]);
        };
	printf("\n");
	}
  printf("\n");
  return 0;
*/
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_nr=0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/code.c -o /tmp/expr");
    if (ret != 0) continue;

    fp = popen("/tmp/expr", "r");
    assert(fp != NULL);

    int result;
    assert(fscanf(fp, "%d", &result));
    pclose(fp);

    printf("%d---- %s\n", result, buf);
    //buf[0] = '\0';
  }
  for(int i=0;i<buf_nr;i++){
        printf("%c",buf[i]);
        };
  printf("\n");
  return 0;
}
