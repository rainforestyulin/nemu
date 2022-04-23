#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_LPAR, TK_RPAR, TK_NUM, TK_R

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"[0-9]+",TK_NUM},
  {"\\(",TK_LPAR},
  {"\\)",TK_RPAR},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-",'-'},		// sub	
  {"\\*",'*'},		// mul
  {"\\/",'/'},		// div
  {"==", TK_EQ},        // equal
  {"\%eax",TK_R},
  {"\%ecx",TK_R},
  {"\%edx",TK_R},
  {"\%ebx",TK_R},
  {"\%esp",TK_R},
  {"\%ebp",TK_R},
  {"\%esi",TK_R},
  {"\%edi",TK_R},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		case TK_NUM:
			tokens[nr_token].type=rules[i].token_type;
	      		for(int j=0;j<substr_len;j++){
	          		tokens[nr_token].str[j]=*(substr_start+j);
	       		};
			nr_token++;
			break;
		case TK_RPAR:
			tokens[nr_token].type=rules[i].token_type;
			tokens[nr_token].str[0]=*(substr_start);
			nr_token++;
			break;
		case TK_LPAR:
                        tokens[nr_token].type=rules[i].token_type;
                        tokens[nr_token].str[0]=*(substr_start);
			nr_token++;
			break;
		case TK_NOTYPE:
			break;
		case '+':
			tokens[nr_token].type=rules[i].token_type;
                        tokens[nr_token].str[0]=*(substr_start);
                        nr_token++;
                        break;
		case '-':
			tokens[nr_token].type=rules[i].token_type;
                        tokens[nr_token].str[0]=*(substr_start);
                        nr_token++;
                        break;
		case '*':
			tokens[nr_token].type=rules[i].token_type;
                        tokens[nr_token].str[0]=*(substr_start);
                        nr_token++;
                        break;
		case '/':
			tokens[nr_token].type=rules[i].token_type;
                        tokens[nr_token].str[0]=*(substr_start);
                        nr_token++;
                        break;
		case TK_R:
			tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len-1;j++){
                                tokens[nr_token].str[j]=*(substr_start+j+1);
                        };
                        nr_token++;
			break;
			/*
		case TK_R_ECX:
			tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len-1;j++){
                                tokens[nr_token].str[j]=*(substr_start+j+1);
                        };
                        nr_token++;
                        break;
		case TK_R_EDX:
			tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len-1;j++){
                                tokens[nr_token].str[j]=*(substr_start+j+1);
                        };
                        nr_token++;
                        break;
		case TK_R_EBX:
			tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len-1;j++){
                                tokens[nr_token].str[j]=*(substr_start+j+1);
                        };
                       */

               default: 
			TODO();
		        break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p,int  q){
	if(tokens[p].type==TK_LPAR&&tokens[q].type==TK_RPAR){
	return true;
	}
	return false;
}

long int get_num_val(int p){
	if(tokens[p].type==TK_NUM){
		int i=0;
		long int val=0;
		while(tokens[p].str[i]!='\0'){
		 i++;
		}
		for(int j=0;j<i;j++){
			int temp=1;
			for(int jj=i-j-1;jj>0;jj--){
			temp=temp*10;
			}
			val=val+(tokens[p].str[j]-48)*temp;

		}
		printf("%ld\t %lx\n",val,val);
		return val;		
	}else if(tokens[p].type==TK_R){
		printf("dadadada\n");
		long int val=0;
		bool issuccess=false;
		val=isa_reg_str2val(tokens[p].str,&issuccess);
		if(issuccess==true){
		printf("%ld\t %lx\n",val,val);
		return val;}else{
		printf("sdasdasdas\n");
		}

	}
	return 0;
}

word_t eval(int p,int q){
if (p > q) {
    /* Bad expression */
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
	  if(tokens[p].type==TK_NUM){
	   get_num_val(p);
      return 0;	
	  }
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    /* We should do more things here. */
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  eval(0,0);
  
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();

  return 0;
}
