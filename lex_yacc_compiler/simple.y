%token  <string_val> WORD

%token  NOTOKEN LPARENT RPARENT LBRACE RBRACE LCURLY RCURLY COMA SEMICOLON EQUAL STRING_CONST LONG LONGSTAR VOID CHARSTAR CHARSTARSTAR INTEGER_CONST AMPERSAND OROR ANDAND EQUALEQUAL NOTEQUAL LESS GREAT LESSEQUAL GREATEQUAL PLUS MINUS TIMES DIVIDE PERCENT IF ELSE WHILE DO FOR CONTINUE BREAK RETURN COLON QUESTION

%union  {
	char   *string_val;
	int nargs;
	int my_nlabel;
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

	int yylex();
	int yyerror(const char * s);

	extern int line_number;
	const char * input_file;
	char * asm_file;
	FILE * fasm;
	FILE *temp_file[5];
	int nfiles = 0;

#define MAX_ARGS 5
	int nargs;
	char * args_table[MAX_ARGS];

#define MAX_GLOBALS 100
	int nglobals = 0;
	char * global_vars_table[MAX_GLOBALS];
	int global_vars_type[MAX_GLOBALS];


#define MAX_LOCALS 100
	int nlocals;
	char * local_vars_table[MAX_LOCALS];
	int local_vars_type[MAX_LOCALS];

#define MAX_STRINGS 100
	int nstrings = 0;
	char * string_table[MAX_STRINGS];

	char *regStk[]={ "rbx", "r10", "r13", "r14", "r15"};
	char nregStk = sizeof(regStk)/sizeof(char*);

	char *regArgs[]={ "rdi", "rsi", "rdx", "rcx", "r8", "r9"};
	char nregArgs = sizeof(regArgs)/sizeof(char*);


	int top = 0;

	int nargs =0;

	int nlabel_if = 0;

	int nlabel_loop = 0;

	int nlabel_and = 0;

	int nlabel_or = 0;

	int nlabel_tern = 0;

	int isCharStar;

	%}

	%%

	goal:   program
	;

program :
function_or_var_list;

function_or_var_list:
function_or_var_list function
| function_or_var_list global_var
| /*empty */
;

// Function definition: emits the label and prologue here, then the
// parameter list, body, and epilogue in the second half of the rule below.
function:
var_type WORD
{

	fprintf(fasm, "\t.text\n");
	fprintf(fasm, ".globl %s\n", $2);
	fprintf(fasm, "%s:\n", $2);

	//Save frame pointer and push stack pointer onto rbp
	fprintf(fasm, "\t# Save Frame pointer\n");
	fprintf(fasm, "\tpushq %%rbp\n");
	fprintf(fasm, "\tmovq %%rsp,%%rbp\n");

	//Subtract 800 bytes from stack pointer to allocate 100 local variables worth of space (set num to 0)
	fprintf(fasm, "\tsubq $800, %%rsp\n"); 
	nlocals = 0;

	//Push all the callee-saved registers onto the stack
	fprintf(fasm, "# Save registers. \n");
	fprintf(fasm, "# Push one extra to align stack to 16bytes\n");
	fprintf(fasm, "\tpushq %%rbx\n");
	fprintf(fasm, "\tpushq %%rbx\n");
	fprintf(fasm, "\tpushq %%r10\n");
	fprintf(fasm, "\tpushq %%r13\n");
	fprintf(fasm, "\tpushq %%r14\n");
	fprintf(fasm, "\tpushq %%r15\n");

}

// Parameter list and body; emits the epilogue once the body is done.
LPARENT arguments RPARENT compound_statement
{
	//Pop callee-saved registers before leaving
	fprintf(fasm, "# Restore registers\n");
	fprintf(fasm, "\tpopq %%r15\n");
	fprintf(fasm, "\tpopq %%r14\n");
	fprintf(fasm, "\tpopq %%r13\n");
	fprintf(fasm, "\tpopq %%r10\n");
	fprintf(fasm, "\tpopq %%rbx\n");
	fprintf(fasm, "\tpopq %%rbx\n");

	//Will restore the stack pointer
	fprintf(fasm, "\tleave\n");
	fprintf(fasm, "\tret\n");
}
;

arg_list:
arg
| arg_list COMA arg
;

arguments:
arg_list {
	nargs = 0;
}
| /*empty*/
;

arg: var_type WORD {
			 local_vars_table[nlocals] = $2;
			 local_vars_type[nlocals] = isCharStar;
			 fprintf(fasm, "\tmovq %%%s, -%d(%%rbp)\n", regArgs[nargs], (nlocals+1)*8);
			 nlocals++;
			 nargs++;
		 };

global_var: 
var_type global_var_list SEMICOLON;
global_var_list: WORD {
									 char * id = $1;
									 global_vars_table[nglobals] = id;
									 global_vars_type[nglobals] = isCharStar;
									 nglobals++;
									 fprintf(fasm, "\t.data\n");
									 fprintf(fasm, "\t.comm %s,8\n", id);
								 }
| global_var_list COMA WORD {
	char * id = $3;
	global_vars_table[nglobals] = id;
	global_vars_type[nglobals] = isCharStar;
	nglobals++;
	fprintf(fasm, "\t.data\n");
	fprintf(fasm, "\t.comm %s,8\n", id);

}
;

var_type: CHARSTAR {
						isCharStar = 1;
					}

| CHARSTARSTAR {
	isCharStar = 0;
} 
| LONG {
	isCharStar = 0;
}
| LONGSTAR {
	isCharStar = 0;
}
| VOID {
	isCharStar = 0;
};

assignment:
WORD EQUAL expression {
	// By now `expression` has already emitted code leaving its value in %rbx
	char *id = $<string_val>1;
	int isGlobal = 1;
	for (int i = 0; i < nlocals; i++) {
		if (strcmp(local_vars_table[i], id) == 0) {
			isGlobal = 0;
			fprintf(fasm, "\tmovq %%rbx, -%d(%%rbp)\n", (i+1)*8);
			top = 0;
			break;
		}
	}
	if (isGlobal) {
		fprintf(fasm, "\tmovq %%rbx, %s\n", id);
		top = 0;
	}
}
| WORD LBRACE expression RBRACE EQUAL expression{
	char *id = $<string_val>1;
	int isGlobal = 1;
	for (int i = 0; i < nlocals; i++) {
		if (strcmp(local_vars_table[i], id) == 0) {
			isGlobal = 0;
			fprintf(fasm, "\tmovq -%d(%%rbp), %%rax\n", (i+1)*8);
			if (local_vars_type[i]) {
				fprintf(fasm, "\tmovb %%r10b, (%%rax, %%rbx, 1)\n");
			}
			else {
				fprintf(fasm, "\tmovq %%r10, (%%rax, %%rbx, 8)\n");
			}
			top = 0;
			break;
		}
	}
	if (isGlobal) {

		for (int i = 0; i < nglobals;i++){
			if (strcmp(global_vars_table[i], id) == 0) {
				fprintf(fasm, "\tmovq %s, %%rax\n", id);
				if (global_vars_type[i]) {
					fprintf(fasm, "\tmovb %%r10b, (%%rax, %%rbx, 1)\n");
				}
				else {
					fprintf(fasm, "\tmovq %%r10, (%%rax, %%rbx, 8)\n");
				}
				top = 0;
			}
		}
	}
}

call:
WORD LPARENT  call_arguments RPARENT {
	char * funcName = $<string_val>1;
	int nargs = $<nargs>3;
	int i;
	fprintf(fasm,"     # func=%s nargs=%d\n", funcName, nargs);
	fprintf(fasm,"     # Move values from reg stack to reg args\n");
	for (i=nargs-1; i>=0; i--) {
		top--;
		fprintf(fasm, "\tmovq %%%s, %%%s\n",
				regStk[top], regArgs[i]);
	}
	if (!strcmp(funcName, "printf")) {
		// printf has a variable number of arguments
		// and it need the following
		fprintf(fasm, "\tmovl    $0, %%eax\n");
	}
	fprintf(fasm, "\tcall %s\n", funcName);
	fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top]);
	top++;
}
;

call_arg_list:
expression {
	$<nargs>$=1;
}
| call_arg_list COMA expression {
	$<nargs>$++;
}

;

call_arguments:
call_arg_list { $<nargs>$=$<nargs>1; }
| /*empty*/ { $<nargs>$=0;}
;

expression :
logical_or_expr
| ternary
;

/* Alternate ternary implementation: branchless, using cmove instead of the
   jump-based approach used below.
ternary:
expression QUESTION expression COLON expression {
	fprintf(fasm, "\ttestq %%%s, %%%s\n", regStk[top - 3], regStk[top - 3]);
	fprintf(fasm, "\tmovq %%%s, %%%s\n", regStk[top - 2], regStk[top - 3]);
	fprintf(fasm, "\tcmove %%%s, %%%s\n", regStk[top - 1], regStk[top - 3]);
	top -=2;
}
*/

ternary:
	expression QUESTION {
		$<my_nlabel>1 = nlabel_tern++;
		fprintf(fasm, "\ttestq %%%s, %%%s\n", regStk[top - 1], regStk[top-1]);
		fprintf(fasm, "\tje tern_false_%d\n", $<my_nlabel>1);
		top--;
	}
	expression COLON{
	top--;
	fprintf(fasm, "\tjmp tern_real_end_%d\n", $<my_nlabel>1);
		fprintf(fasm, "\ttern_false_%d:\n", $<my_nlabel>1);
	}
	expression{
	fprintf(fasm, "\ttern_real_end_%d:\n", $<my_nlabel>1);
	};

logical_or_expr:
logical_and_expr
| logical_or_expr {
	// Short-circuit ||: if the left side is already true, skip the right side
	fprintf(fasm, "\n\t#OROR\n");
	if (top < nregStk) {
		$<my_nlabel>1 = nlabel_or;
		fprintf(fasm, "\ttestq %%%s, %%%s\n", regStk[top - 1], regStk[top - 1]);
		fprintf(fasm, "\tjne after_or_%d\n", $<my_nlabel>1); 
		top--;
	}
} OROR logical_and_expr {
	if (top < nregStk) {
		fprintf(fasm, "after_or_%d:\n", $<my_nlabel>1);
		nlabel_or++;
	}
}
;

logical_and_expr:
equality_expr
| logical_and_expr {
	// Short-circuit &&: if the left side is already false, skip the right side
	fprintf(fasm, "\n\t#ANDAND\n");
	if (top < nregStk) {
		$<my_nlabel>1 = nlabel_and;
		fprintf(fasm, "\ttestq %%%s, %%%s\n", regStk[top - 1], regStk[top - 1]);
		fprintf(fasm, "\tje after_and_%d\n", $<my_nlabel>1); 
		top--;
	}
} ANDAND equality_expr {
	if (top < nregStk) {
		fprintf(fasm, "after_and_%d:\n", $<my_nlabel>1);
		nlabel_and++;
	}
}
;

equality_expr:
relational_expr
| equality_expr EQUALEQUAL relational_expr {
	// Compare the top two register-stack slots, push 1 if equal else 0
	fprintf(fasm, "\n\t#EQUALEQUAL\n");
	if (top < nregStk) {
		fprintf(fasm, "\txorq %%rax, %%rax\n");
		fprintf(fasm, "\tcmpq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		fprintf(fasm, "\tsete %%al\n");
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
| equality_expr NOTEQUAL relational_expr {
	// Compare the top two register-stack slots, push 1 if unequal else 0
	fprintf(fasm, "\n\t#NOTEQUAL\n");
	if (top < nregStk) {
		fprintf(fasm, "\txorq %%rax, %%rax\n");
		fprintf(fasm, "\tcmpq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		fprintf(fasm, "\tsetne %%al\n");
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
;

relational_expr:
additive_expr
| relational_expr LESS additive_expr {
	// Compare the top two register-stack slots, push 1 if left < right else 0
	fprintf(fasm, "\n\t#LESS\n");
	if (top < nregStk) {
		fprintf(fasm, "\txorq %%rax, %%rax\n");
		fprintf(fasm, "\tcmpq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		fprintf(fasm, "\tsetl %%al\n");
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
| relational_expr GREAT additive_expr {
	// Compare the top two register-stack slots, push 1 if left > right else 0
	fprintf(fasm, "\n\t#GREAT\n");
	if (top < nregStk) {
		fprintf(fasm, "\txorq %%rax, %%rax\n");
		fprintf(fasm, "\tcmpq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		fprintf(fasm, "\tsetg %%al\n");
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
| relational_expr LESSEQUAL additive_expr {
	// Compare the top two register-stack slots, push 1 if left <= right else 0
	fprintf(fasm, "\n\t#LESSEQUAL\n");
	if (top < nregStk) {
		fprintf(fasm, "\txorq %%rax, %%rax\n");
		fprintf(fasm, "\tcmpq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		fprintf(fasm, "\tsetle %%al\n");
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
| relational_expr GREATEQUAL additive_expr {
	// Compare the top two register-stack slots, push 1 if left >= right else 0
	fprintf(fasm, "\n\t#GREATEQUAL\n");
	if (top < nregStk) {
		fprintf(fasm, "\txorq %%rax, %%rax\n");
		fprintf(fasm, "\tcmpq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		fprintf(fasm, "\tsetge %%al\n");
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
;

additive_expr:
multiplicative_expr
| additive_expr PLUS multiplicative_expr {
	// Add the top two register-stack slots, leaving the sum in the lower one
	fprintf(fasm,"\n\t# +\n");
	if (top<nregStk) {
		fprintf(fasm, "\taddq %%%s,%%%s\n", 
				regStk[top-1], regStk[top-2]);
		top--;
	}
}
| additive_expr MINUS multiplicative_expr {
	// Subtract the top register-stack slot from the one below it
	fprintf(fasm,"\n\t# -\n");
	if (top < nregStk) {
		fprintf(fasm, "\tsubq %%%s,%%%s\n", regStk[top-1], regStk[top-2]);
		top--;
	}
}
;

multiplicative_expr:
primary_expr
| multiplicative_expr TIMES primary_expr {
	// Multiply the top two register-stack slots, leaving the product in the lower one
	fprintf(fasm,"\n\t# *\n");
	if (top<nregStk) {
		fprintf(fasm, "\timulq %%%s,%%%s\n", 
				regStk[top-1], regStk[top-2]);
		top--;
	}
}
| multiplicative_expr DIVIDE primary_expr {
	// Divide the two slots via idivq; quotient ends up in %rax
	fprintf(fasm,"\n\t# /\n");
	if (top < nregStk) {
		fprintf(fasm, "\tmovq %%%s, %%rax\n", regStk[top-2]);
		fprintf(fasm, "\tcqto\n");
		fprintf(fasm, "\tidivq %%%s\n", regStk[top-1]);
		fprintf(fasm, "\tmovq %%rax, %%%s\n", regStk[top-2]);
		top--;
	}
}
| multiplicative_expr PERCENT primary_expr {
	// Same as division above, but keep the remainder from %rdx instead
	fprintf(fasm,"\n\t# %%\n");
	if (top < nregStk) {
		fprintf(fasm, "\tmovq %%%s, %%rax\n", regStk[top-2]);
		fprintf(fasm, "\tcqto\n");
		fprintf(fasm, "\tidivq %%%s\n", regStk[top-1]);
		fprintf(fasm, "\tmovq %%rdx, %%%s\n", regStk[top-2]);
		top--;
	}
}
;



primary_expr:
STRING_CONST {
	// Add string to string table.
	// String table will be produced later
	string_table[nstrings]=$<string_val>1;
	fprintf(fasm, "\t#top=%d\n", top);
	fprintf(fasm, "\n\t# push string %s top=%d\n",
			$<string_val>1, top);
	if (top<nregStk) {
		fprintf(fasm, "\tmovq $string%d, %%%s\n", 
				nstrings, regStk[top]);
		top++;
	}
	nstrings++;
}
| call
| WORD {
	char *id = $1;
	int isGlobal = 1;
	for (int i = 0; i < nlocals; i++) {
		if (strcmp(local_vars_table[i], id) == 0) {
			isGlobal = 0;
			fprintf(fasm, "\tmovq -%d(%%rbp), %%%s\n", (i+1)*8, regStk[top]);
			top++;
			break;
		}
	}
	if (isGlobal) {
		fprintf(fasm, "\tmovq %s, %%%s\n", id, regStk[top]);
		top++;
	}
}
// Array/pointer indexing: id[expr]
| WORD LBRACE expression RBRACE {
	char *id = $1;
	int isGlobal = 1;
	for (int i = 0; i < nlocals; i++) {
		if (strcmp(local_vars_table[i], id) == 0) {
			isGlobal = 0;
			if (local_vars_type[i]) {
				fprintf(fasm, "\tmovq -%d(%%rbp), %%rax\n", (i+1)*8);
				fprintf(fasm, "\tmovzbq (%%rax, %%%s, 1), %%%s\n", regStk[top-1], regStk[top-1]);
			}
			else{
				fprintf(fasm, "\tmovq -%d(%%rbp), %%rax\n", (i+1)*8);
				fprintf(fasm, "\tmovq (%%rax, %%%s, 8), %%%s\n", regStk[top-1], regStk[top-1]);
			}
			break;
		}
	}
	if (isGlobal) {
		for (int i = 0; i < nglobals; i++) {
			if (strcmp(global_vars_table[i], id) == 0) {
				if (global_vars_type[i]) {
					fprintf(fasm, "\tmovq %s, %%rax\n", id);
					fprintf(fasm, "\tmovzbq (%%rax, %%%s, 1), %%%s\n", regStk[top-1], regStk[top-1]);
				}
				else {
					fprintf(fasm, "\tmovq %s, %%rax\n", id);
					fprintf(fasm, "\tmovq (%%rax, %%%s, 8), %%%s\n", regStk[top-1], regStk[top-1]);
				}
				break;
			}
		}
	}
}
| AMPERSAND WORD {
	char *id = $2;
	int isGlobal = 1;
	for (int i = 0; i < nlocals; i++) {
		if (strcmp(local_vars_table[i], id) == 0) {
			isGlobal = 0;
			fprintf(fasm, "\tleaq -%d(%%rbp), %%%s\n", (i+1)*8, regStk[top]);
			top++;
			break;
		}
	}
	if (isGlobal) {
		fprintf(fasm, "\tleaq %s, %%%s\n", id, regStk[top]);
		top++;
	}
}
| INTEGER_CONST {
	fprintf(fasm, "\n\t# push %s\n", $<string_val>1);
	if (top<nregStk) {
		fprintf(fasm, "\tmovq $%s,%%%s\n", 
				$<string_val>1, regStk[top]);
		top++;
	}
}
| LPARENT expression RPARENT
;

compound_statement:
LCURLY statement_list RCURLY
;

statement_list:
statement_list statement
| /*empty*/
;

local_var:
var_type local_var_list SEMICOLON;

local_var_list: WORD {
									char *id = $1;
									local_vars_table[nlocals] = id;
									local_vars_type[nlocals] = isCharStar;
									nlocals++;

								}
| local_var_list COMA WORD {
	char *id = $3;
	local_vars_table[nlocals] = id;
	local_vars_type[nlocals] = isCharStar;
	nlocals++;
};




statement:
assignment SEMICOLON
| call SEMICOLON { top= 0; /* Reset register stack */ }
| local_var
| compound_statement
| IF LPARENT {
	$<my_nlabel>1 = nlabel_if;
	fprintf(fasm, "if_%d:\n", $<my_nlabel>1);
	nlabel_if++;
}
expression RPARENT {
	fprintf(fasm, "\ttestq %%rbx, %%rbx\n");
	fprintf(fasm, "\tje else_%d\n", $<my_nlabel>1);
	top--;
}
statement {
	fprintf(fasm, "\tjmp else_end_%d\n", $<my_nlabel>1);
	fprintf(fasm, "else_%d:\n", $<my_nlabel>1);
} else_optional {
	fprintf(fasm, "else_end_%d:\n", $<my_nlabel>1);
}
| WHILE LPARENT {
	$<my_nlabel>1=nlabel_loop;
	fprintf(fasm, "start_%d:\n", $<my_nlabel>1);
	nlabel_loop++;
}
expression RPARENT {
	fprintf(fasm, "\ttestq %%rbx, %%rbx\n");
	fprintf(fasm, "\tje end_%d\n", $<my_nlabel>1);
	top--;
}
statement {
	fprintf(fasm, "\tjmp start_%d\n", $<my_nlabel>1);
	fprintf(fasm, "end_%d:\n", $<my_nlabel>1);
}
| DO {
	$<my_nlabel>1=nlabel_loop;
	fprintf(fasm, "start_%d:\n", $<my_nlabel>1);
	nlabel_loop++;
}
statement WHILE LPARENT expression RPARENT SEMICOLON {
	fprintf(fasm, "\ttestq %%rbx, %%rbx\n");
	fprintf(fasm, "\tjne start_%d\n", $<my_nlabel>1);
	fprintf(fasm, "end_%d:\n", $<my_nlabel>1);
	top--;
}
// FOR LOOP
| FOR LPARENT assignment SEMICOLON {
                $<my_nlabel>1=nlabel_loop;
                fprintf(fasm, "for_start_%d:\n", $<my_nlabel>1);
                nlabel_loop++;
         }
    expression SEMICOLON {
								fprintf(fasm, "\ttestq %%rbx, %%rbx\n");
                fprintf(fasm, "\tje end_%d\n", $<my_nlabel>1);
                top--;
                temp_file[nfiles] = fasm;
                fasm = tmpfile();
         }
         assignment RPARENT {

                FILE *t = fasm;
                fasm = temp_file[nfiles];
                temp_file[nfiles] = t;
								nfiles++;
         }
         statement {
                fprintf(fasm, "start_%d:\n", $<my_nlabel>1);
                char buffer[100]; 
								fseek(temp_file[nfiles - 1], 0, SEEK_SET);
                while (fgets(buffer, sizeof(buffer), temp_file[nfiles - 1])) {
                        fputs(buffer, fasm);
                }
								nfiles--;
                fprintf(fasm, "\tjmp for_start_%d\n", $<my_nlabel>1);
                fprintf(fasm, "end_%d:\n", $<my_nlabel>1);
         }
         | jump_statement
         ;

/* Alternate FOR implementation: jumps directly into the loop body instead
   of buffering the increment statement into a temp file and replaying it
   at the end of each iteration (the approach used above).
| FOR LPARENT assignment SEMICOLON {
	$<my_nlabel>1=nlabel_loop;
	fprintf(fasm, "for_start_%d:\n", $<my_nlabel>1);
	nlabel_loop++;
}
expression SEMICOLON {
	fprintf(fasm, "\ttestq %%rbx, %%rbx\n");
	fprintf(fasm, "\tje end_%d\n", $<my_nlabel>1);
	top--;
	fprintf(fasm, "\tjmp body_%d\n", $<my_nlabel>1);
	fprintf(fasm, "\tstart_%d:\n", $<my_nlabel>1);
}
assignment RPARENT {
	fprintf(fasm, "jmp for_start_%d\n", $<my_nlabel>1);
	fprintf(fasm, "\tbody_%d:\n", $<my_nlabel>1);
}
statement {
	fprintf(fasm, "\tjmp start_%d\n", $<my_nlabel>1);
	fprintf(fasm, "end_%d:\n", $<my_nlabel>1);
}
| jump_statement
;
*/


else_optional:
ELSE  statement
| /* empty */
;

jump_statement:
CONTINUE SEMICOLON {
	fprintf(fasm, "jmp start_%d\n", nlabel_loop - 1);
}
| BREAK SEMICOLON {
	fprintf(fasm, "jmp end_%d\n", nlabel_loop - 1);
}
| RETURN expression SEMICOLON {
	fprintf(fasm, "\tmovq %%rbx, %%rax\n");
	top = 0;
	fprintf(fasm, "# Restore registers\n");
	fprintf(fasm, "\tpopq %%r15\n");
	fprintf(fasm, "\tpopq %%r14\n");
	fprintf(fasm, "\tpopq %%r13\n");
	fprintf(fasm, "\tpopq %%r10\n");
	fprintf(fasm, "\tpopq %%rbx\n");
	fprintf(fasm, "\tpopq %%rbx\n");

	//Will restore the stack pointer
	fprintf(fasm, "\tleave\n");
	fprintf(fasm, "\tret\n");
}
;

%%

void yyset_in (FILE *  in_str );

	int
yyerror(const char * s)
{
	fprintf(stderr,"%s:%d: %s\n", input_file, line_number, s);
	return 0;
}



























	int
main(int argc, char **argv)
{
	if (argc <2) {
		fprintf(stderr, "Usage: simple file\n");
		exit(1);
	}

	input_file = strdup(argv[1]);

	int len = strlen(input_file);
	if (len < 2 || input_file[len-2]!='.' || input_file[len-1]!='c') {
		fprintf(stderr, "Error: file extension is not .c\n");
		exit(1);
	}

	// Derive the output assembly filename by swapping the .c extension for .s
	asm_file = strdup(input_file);
	asm_file[len-1]='s';

	FILE * f = fopen(input_file, "r");
	if (f==NULL) {
		fprintf(stderr, "Cannot open file %s\n", input_file);
		perror("fopen");
		exit(1);
	}

	// fasm is the global output stream every grammar action writes assembly to
	fasm = fopen(asm_file, "w");
	if (fasm==NULL) {
		fprintf(stderr, "Cannot open file %s\n", asm_file);
		perror("fopen");
		exit(1);
	}

	// Uncomment for debugging: dump generated assembly to stderr instead of a file
	//fasm = stderr;

	yyset_in(f);
	yyparse();

	// Generate string table
	int i;
	for (i = 0; i<nstrings; i++) {
		fprintf(fasm, "string%d:\n", i);
		fprintf(fasm, "\t.string %s\n\n", string_table[i]);
	}

	fclose(f);
	fclose(fasm);

	return 0;
}
