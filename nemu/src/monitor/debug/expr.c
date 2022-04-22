#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_LPAR, TK_RPAR, TK_NUM, TK_R_EAX, TK_R_ECX, TK_R_EDX, TK_R_EBX, TK_R_ESP, TK_R_EBP, TK_R_ESI, TK_R_EDI

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
  {"\%eax",TK_R_EAX},
  {"\%ecx",TK_R_ECX},
  {"\%edx",TK_R_EDX},
  {"\%ebx",TK_R_EBX},
  {"\%esp",TK_R_ESP},
  {"\%ebp",TK_R_EBP},
  {"\%esi",TK_R_ESI},
  {"\%edi",TK_R_EDI},
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
		case TK_R_EAX:
			break;
		case TK_R_ECX:
                        break;
		case TK_R_EDX:
                        break;
		case TK_R_EBX:
                        break;
		case TK_R_ESP:
                        break;
		case TK_R_EBP:
                        break;
		case TK_R_ESI:
                        break;
		case TK_R_EDI:
                        break;

               default: 
			printf("illegal EXPR\n");
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

bool make_tokens(char *c){
	return make_token(c);
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
