#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_LPAR, TK_RPAR, TK_NUM, TK_R, TK_NUM_X

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"[0-9]+[^a-z]",TK_NUM},
  {"0[xX][0-9a-fA-F]+",TK_NUM_X},
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

static Token tokens[1000] __attribute__((used)) = {};
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
		case TK_EQ:
			tokens[nr_token].type=rules[i].token_type;
			for(int j=0;j<substr_len;j++){
				tokens[nr_token].str[j]=*(substr_start+j);
			};
			nr_token++;
			break;
		case TK_NUM_X:
			tokens[nr_token].type=rules[i].token_type;
			for(int j=0;j<substr_len-2;j++){
				tokens[nr_token].str[j]=*(substr_start+j+2);
			}
			nr_token++;
			break;
               default: 
			TODO();
			break;
        //Log("nr_buf_%d",nr_token);
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
	bool res=true;
	Log("in chech_%d_%d\n",p,q);
	if(tokens[p].type==TK_LPAR&&tokens[q].type==TK_RPAR){
	//	bool islegall=false;
		int h_flag=p+1;
		int t_flag=q-1;
                int par_num=0;
	//	int rec=0;
		while(h_flag<=t_flag){
			
			if(tokens[h_flag].type==TK_LPAR){
				par_num++;
				h_flag++;
				//Log("L");
				continue;
			}else if(tokens[h_flag].type==TK_RPAR){
				//Log("R");
				if(par_num>0){
					par_num--;
					h_flag++;
					//Log("R+");
				}else{
					//Log("break");
					res=false;
					break;
					
				}
			}else{
				h_flag++;
			//	Log("num");
			}
			//Log("ttttttt\n");
		}
	  ///legall
	
	}else{
		res= false;
		Log("false");
	}
		return res;
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
		//printf("%ld\t %lx\n",val,val);
		return val;		
	}else if(tokens[p].type==TK_R){
		long int val=0;
		bool issuccess=false;
		val=isa_reg_str2val(tokens[p].str,&issuccess);
		if(issuccess==true){
		printf("%ld\t %lx\n",val,val);
		return val;}
	}else{
		printf("invalid_expr\n");
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
	//  printf("got num\n");
     return  get_num_val(p);	  
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
	Log("legal");
    return eval(p + 1, q - 1);
  }
  else {
	  int op=-1;
	  int op_type=-1;
     	  for (int i=p;i<q;i++){
     		if(tokens[i].type==TK_LPAR){
			int j=i+1;
			int l_depth=1;
			while(l_depth>0){
				if(tokens[j].type==TK_LPAR){
					l_depth++;
					j++;
				}else if(tokens[j].type==TK_RPAR){
					l_depth--;
					if(l_depth==0){
						break;
					}else{
						j++;
					}

				}else{
					j++;
				}
			}
			i=j;
			Log("i==%d",i);
			continue;
		}
			else if(tokens[i].type==TK_NUM||tokens[i].type==TK_R){
			continue;
		}
			else if(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/'||tokens[i].type==TK_EQ)
		    {
				int j=i+1;
				bool  isfind=false;
				while(j<q){
					if(tokens[j].type==TK_LPAR){
                        	int jj=j+1;
                        	int l_depth=1;
                        	while(l_depth>0){
                                	if(tokens[jj].type==TK_LPAR){
                                        	l_depth++;
                                        	jj++;
                                	}else if(tokens[jj].type==TK_RPAR){
                                       		 l_depth--;
                                        	if(l_depth==0){
                                                break;
                                        	}else{
                                                	jj++;
                                        	}

                               		 }else{
                                        	jj++;
                                     }
                       		 }
							j=jj+1;
							continue;
					}
					else if (tokens[j].type == '+' || tokens[j].type == '-') {
						isfind = true;
						break;
					}
					else 
					{
						j++;
					}
					

			////
			    }
			
			
				if(isfind){
					i=j-1;
					continue;
				}else{
					op=i;
					op_type=tokens[i].type;
					break;
				}
			}
     	}	  
    /* We should do more things here. */
    //op = the position of 主运算符 in the token expression;
    long int val1=0,val2=0;
    if(op_type=='+'||op_type=='-'||op_type=='*'||op_type=='/'||op_type==TK_EQ){
   	     val1 = eval(p, op - 1);
         val2 = eval(op + 1, q);
    }
    switch (op_type) {
      case '+': return val1 + val2; break;
      case '-': return val1 - val2; break;
      case '*': return val1 * val2; break;
      case '/': if(val2!=0){
			return val1 / val2; break;
		}else{
			panic("Wdiv 0 err!");
			assert(0);
		}
      case TK_EQ: if(val1==val2){
		  return 1;
		  }else{
		  return 0;
		  }
		  break;
      default: Log("invalid_expr");
		assert(0); break;
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  };
  Log("--%d--",nr_token-1);
  long int a=eval(0,nr_token-1);
  printf("%ld\t%lx\n",a,a);
 // eval(0,nr_token-1);
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();

  return 0;
}
