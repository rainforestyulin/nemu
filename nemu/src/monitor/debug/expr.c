#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
word_t vaddr_read1(vaddr_t addr);


enum {
  TK_NOTYPE = 256, TK_EQ, TK_LPAR, TK_RPAR, TK_NUM, TK_R, TK_N_EQ, TK_AND, TK_PTR, TK_NEG_NUM

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"0[xX][0-9a-fA-F]+|[0-9]+",TK_NUM},
  {"\\(",TK_LPAR},
  {"\\)",TK_RPAR},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-",'-'},		// sub	
  {"\\*",'*'},		// mul
  {"\\/",'/'},		// div
  {"==", TK_EQ},        // equal
  {"!=",TK_N_EQ},	//not equal
  {"&&",TK_AND},        //and
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
			int j;
	      		for(j=0;j<substr_len;j++){
	          		tokens[nr_token].str[j]=*(substr_start+j);
	       		};
			tokens[nr_token].str[j]='\0';
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
			if(nr_token==0||(tokens[nr_token-1].type!=TK_NUM&&tokens[nr_token-1].type!=TK_R&&tokens[nr_token-1].type!=TK_RPAR)){
				tokens[nr_token].type=TK_NEG_NUM;
                        	tokens[nr_token].str[0]=*(substr_start);
				nr_token++;
			}else{
				tokens[nr_token].type=rules[i].token_type;
                        	tokens[nr_token].str[0]=*(substr_start);
                       		nr_token++;
			}
                        break;
		case '*':
			if(nr_token==0||(tokens[nr_token-1].type!=TK_NUM&&tokens[nr_token-1].type!=TK_R&&tokens[nr_token-1].type!=TK_RPAR)){
				tokens[nr_token].type=TK_PTR;
				tokens[nr_token].str[0]=*(substr_start);
	                        nr_token++;
				break;
			}else{
				tokens[nr_token].type=rules[i].token_type;
                        	tokens[nr_token].str[0]=*(substr_start);
                        	nr_token++;
                        	break;
			}
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
		case TK_N_EQ:
                        tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len;j++){
                                tokens[nr_token].str[j]=*(substr_start+j);
                        };
                        nr_token++;
                        break;
		case TK_AND:
                        tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len;j++){
                                tokens[nr_token].str[j]=*(substr_start+j);
                        };
                        nr_token++;
                        break;
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
	bool res=true;
	Log("in chech_%d_%d\n",p,q);
	if(tokens[p].type==TK_LPAR&&tokens[q].type==TK_RPAR){
		int h_flag=p+1;
		int t_flag=q-1;
		int par_num=0;
		while(h_flag<=t_flag){
			if(tokens[h_flag].type==TK_LPAR){
				par_num++;
				h_flag++;
				continue;
			}else if(tokens[h_flag].type==TK_RPAR){
				if(par_num>0){
					par_num--;
					h_flag++;
				}else{
					res=false;
					break;
					
				}
			}else{
				h_flag++;
			}
		}
	
	}else{
		res= false;
		Log("not_parenthesed");
	}
		return res;
}

long int get_num_val(int p){
	if(tokens[p].type==TK_NUM){
		int i=0;
		while(tokens[p].str[i]!=-'\0'){
			printf("%c",tokens[p].str[i]);
			i++;
		}
		if(tokens[p].str[1]=='x'||tokens[p].str[1]=='X'){
			int i=0;
        	        long int val=0;
               		while(tokens[p].str[i]!='\0'){
				 i++;
                	}
			i=i-2;
			printf("\ndeep of 0x%d\n",i);
               		for(int j=0;j<i;j++){
                        	int temp=1;
                        	for(int jj=i-j-1;jj>0;jj--){
                        		temp=temp*16;
                        	}
				if(tokens[p].str[j+2]>=48&&tokens[p].str[j+2]<=57){
                        		val=val+(tokens[p].str[j+2]-48)*temp;
				}else if(tokens[p].str[j+2]>=65&&tokens[p].str[j+2]<=70){
					val=val+(tokens[p].str[j+2]-55)*temp;
				}else if(tokens[p].str[j+2]>=97&&tokens[p].str[j+2]<=102){
					val=val+(tokens[p].str[j+2]-87)*temp;
				}else{
					assert(-1);
				}

               		}
                	return val;
		}else{
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
		return val;
		}		
	}else if(tokens[p].type==TK_R){
		long int val=0;
		bool issuccess=false;
		val=isa_reg_str2val(tokens[p].str,&issuccess);
		if(issuccess==true){
			return val;
		}
	}else{
		printf("invalid_value_expr\n");
		assert(-1);
	}
	return 0;
}


int find_op_and(int p,int q){
	bool isfind=false;
	int i=p;
	int index=-1;
	while(i<=q){
		int k=i;
                assert(k<=q);
                if(tokens[k].type==TK_LPAR){
                	int j=k+1;
                        int l_depth=1;
                        while(l_depth>0&&j<=q){
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
				continue;
		}
		else if(tokens[i].type==TK_AND){
			index=i;
			isfind=true;
			i++;
		}else{
			i++;
		}
	}
	if(isfind==true){
		Log("find op and %d",index);
		return index;
	}else{
		Log("find op and %d",index);
		return index;
	}
}
int find_op_eq(int p,int q){
	bool isfind=false;
	int i=p;
	int index=-1;
	while(i<=q){
		int k=i;
                assert(k<=q);
                if(tokens[k].type==TK_LPAR){
                        int j=k+1;
                        int l_depth=1;
                        while(l_depth>0&&j<=q){
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
                                continue;
                }

		else	if(tokens[i].type==TK_EQ||tokens[i].type==TK_N_EQ){
			index=i;
			isfind=true;
			i++;
		}else{
			i++;
		}
	}
	if(isfind==true){
		Log("find op eq%d",index);
                return index;
        }else{
		Log("find op eq%d",index);
                return index;
        };
	Log("find op eq%d",index);
}
int find_op_plus(int p,int q){
        bool isfind=false;
        int i=p;
        int index=-1;
        while(i<=q){
		int k=i;
                assert(k<=q);
                if(tokens[k].type==TK_LPAR){
                        int j=k+1;
                        int l_depth=1;
                        while(l_depth>0&&j<=q){
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
                                //Log("i of (=%d",k);
                                continue;
                                //Log("i of (=%d",k);
                }
		else    if(tokens[i].type=='+'||tokens[i].type=='-'){
                        index=i;
                        isfind=true;
                        i++;
                }else{
                        i++;
                }
        }
        if(isfind==true){
		Log("find plus %d",index);
                return index;
        }else{
		Log("find plus %d",index);
                return index;
        }
	Log("find plus %d",index);
}
int find_op_mul(int p,int q){
        bool isfind=false;
        int i=p;
        int index=-1;
        while(i<=q){
		int k=i;
                assert(k<=q);
                if(tokens[k].type==TK_LPAR){
                        int j=k+1;
                        int l_depth=1;
                        while(l_depth>0&&j<=q){
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
                                //Log("i of (=%d",k);
                                continue;
                                //Log("i of (=%d",k);
                }
		else   if(tokens[i].type=='*'||tokens[i].type=='/'){
                        index=i;
                        isfind=true;
                        i++;
                }else{
                        i++;
                }
        }
        if(isfind==true){
		Log("find * / result %d",index);
                return index;
        }else{
		Log("find * / result %d",index);
                return index;
        }
	Log("find * / result %d",index);
}

long int eval(int p,int q){
  if (p > q) {
    /* Bad expression */
	assert(-1);
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
	Log("parenthesed");
    return eval(p + 1, q - 1);
  }
  else { 
	  long int val1=0;  
	  int op=-1;
	  int op_type=-1;
		for (int i=p;i<q;i++){		//表达式分解
     			if(tokens[i].type==TK_LPAR){	//若匹配到左括号，则找到其对应的右括号
				int j=i+1;
				int l_depth=1;
				while(l_depth>0){
					assert(j <= q);              //bad expr
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
				Log("index of )_%d",i);
				continue;
		   	}
			else if(tokens[i].type==TK_NUM||tokens[i].type==TK_R){
				continue;
		    	}
			else if(tokens[i].type==TK_PTR||tokens[i].type==TK_NEG_NUM){	
				int k=i+1;
				assert(k<=q);
				if(tokens[k].type==TK_LPAR){
					int j=k+1;
        				int l_depth=1;
					while(l_depth>0&&j<=q){
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
					k=j;
					Log("k of (=%d",k);
				}else if(tokens[k].type==TK_NUM||tokens[k].type==TK_R){	
					
				}else {
					assert(0);
					Log("err");
				}
				if(tokens[i].type==TK_PTR){
					long int addr=eval(i+1,k);
					Log("addr is%lx",addr);
					val1= vaddr_read1(addr);
					i=k;
					Log("ptr_end_%d",i);
				}else{
					val1=-eval(i+1,k);
					Log("neg_val1%ld",val1);
					i=k;
					Log("neg_end%d",i);
				}
				//Log("q is%d",q);
				if(i<q&&tokens[i+1].type!=TK_R&&tokens[i+1].type!=TK_NUM&&tokens[i+1].type!=TK_LPAR&&tokens[i+1].type!=TK_RPAR&&tokens[i+1].type!=TK_PTR) {
					Log("detetc");
					continue;
				}else {
					Log("return ptr/neg");
					return val1;	
				}
			
		    }
			else if(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/'||tokens[i].type==TK_EQ||tokens[i].type==TK_N_EQ||tokens[i].type==TK_AND);
		    {	
		    	    	op=i;
				op_type=tokens[i].type;
			    	Log("find real op %c",tokens[i].type);
				switch (op_type){
					case TK_AND:
						if(find_op_and(i+1,q)!=-1){
                                        		op=find_op_and(i+1,q);
                                        		op_type=tokens[op].type;
                                		};break;
					case TK_EQ:
						if(find_op_and(i+1,q)!=-1){
                                        		op=find_op_and(i+1,q);
                                        		op_type=tokens[op].type;
                                		}else if(find_op_eq(i+1,q)!=-1){
                                        		op=find_op_eq(i+1,q);
                                        		op_type=tokens[op].type;
                                		};break;
					case TK_N_EQ:
                                                if(find_op_and(i+1,q)!=-1){
                                                        op=find_op_and(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_eq(i+1,q)!=-1){
                                                        op=find_op_eq(i+1,q);
                                                        op_type=tokens[op].type;
                                                };break;
					case '+':
						if(find_op_and(i+1,q)!=-1){
                                                        op=find_op_and(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_eq(i+1,q)!=-1){
                                                        op=find_op_eq(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_plus(i+1,q)!=-1){
                                       			op=find_op_plus(i+1,q);
                                        		op_type=tokens[op].type;
                                		};break;
					case '-':
                                                if(find_op_and(i+1,q)!=-1){
                                                        op=find_op_and(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_eq(i+1,q)!=-1){
                                                        op=find_op_eq(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_plus(i+1,q)!=-1){
                                                        op=find_op_plus(i+1,q);
                                                        op_type=tokens[op].type;
                                                };break;
					case '*':
						if(find_op_and(i+1,q)!=-1){
                                                        op=find_op_and(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_eq(i+1,q)!=-1){
                                                        op=find_op_eq(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_plus(i+1,q)!=-1){
                                                        op=find_op_plus(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_mul(i+1,q)!=-1){
                                        		op=find_op_mul(i+1,q);
                                        		op_type=tokens[op].type;
                                		};break;
					case '/':
                                                if(find_op_and(i+1,q)!=-1){
                                                        op=find_op_and(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_eq(i+1,q)!=-1){
                                                        op=find_op_eq(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_plus(i+1,q)!=-1){
                                                        op=find_op_plus(i+1,q);
                                                        op_type=tokens[op].type;
                                                }else if(find_op_mul(i+1,q)!=-1){
                                                        op=find_op_mul(i+1,q);
                                                        op_type=tokens[op].type;
                                                };break;
					}
			}
			    Log("detemied op%d_____%c",op,tokens[op].type);
 			    assert(op!=-1);
			    switch (op_type) {
      		    		case '+': return eval(p,op-1)+eval(op+1,q);break;    
      				case '-': return eval(p,op-1)-eval(op+1,q);break;
      				case '*': return eval(p,op-1)*eval(op+1,q);break;
      				case '/': if(eval(op+1,q)!=0){
                        		return eval(p,op-1)/eval(op+1,q);break;
                		}else{
                        		panic("Wdiv 0 err!");
                        		assert(0);
                		}
      				case TK_EQ: if(eval(p,op-1) == eval(op+1,q)) {
                        		return 1;
                  		}else{
                        		return 0;
                  		}
                  		break;
      				case TK_N_EQ: if(eval(p,op-1) != eval(op+1,q)) {
                        		return 1;
                  		}else{
                        		return 0;
                  		}
                  		break;
      				case TK_AND: if(eval(p,op-1) * eval(op+1,q) != 0) {
                        		return 1;
                   		}else{
                        		return 0;
                   		}
                  		break;
      				default: Log("invalid_expr");
                	assert(0); break;
    	       		}		
     	}
  }
  return 0;
}

long int expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  };
  Log("--%d--",nr_token-1);
  long int a=eval(0,nr_token-1);
  printf("%ld\t0x%lx\n",a,a);
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();

  return 0;
}
