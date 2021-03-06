%{
#include <cstdio>
void yyerror(const char *msg) {
	fprintf(stderr, "YYERROR: %s\n", msg);
}

int yylex();
extern int yylineno;
extern char *yytext;

%}

%union {
	long intconst;
	double realconst;
	char *strconst;
	char charconst;
	char *text;
}

%token <text> IDENT TYPE
%token <intconst> INTCONST
%token <strconst> STRCONST
%token <charconst> CHARCONST
%token <realconst> REALCONST
%token CONST STRUCT FOR WHILE DO IF ELSE BREAK CONTINUE RETURN LPAR RPAR LBRACKET RBRACKET LBRACE RBRACE DOT COMMA SEMI QUEST COLON PLUS MINUS STAR SLASH MOD TILDE PIPE AMP BANG DPIPE DAMP ASSIGN PLUSASSIGN MINUSASSIGN STARASSIGN SLASHASSIGN INCR DECR EQUALS NEQUAL GT LT GE LE
%token ERROR END

%left COMMA
%left ASSIGN PLUSASSIGN MINUSASSIGN STARASSIGN SLASHASSIGN
%left COLON QUEST
%left DPIPE
%left DAMP
%left PIPE
%left AMP
%left EQUALS NEQUAL
%left GT GE LT LE
%left PLUS MINUS
%left STAR SLASH MOD
%left BANG TILDE UMINUS INCR DECR TYPE
%left DOT LPAR RPAR LBRACKET RBRACKET

%%

program:
	| vardec program
	| funcprototype program
	| funcdef program
	;

funcprototype: funcdec SEMI ;

funcdef: funcdec LBRACE vardeclist statementlist RBRACE ;

vardec: typename vardecitem SEMI ;

vardecitem: vardecitem COMMA vardecitem
	| IDENT
	| IDENT LBRACKET INTCONST RBRACKET
	;

vardeclist:
	| vardeclist vardec
	;

typename: TYPE ;

funcdec: typename IDENT LPAR optionalparamlist RPAR ;

formalparam: typename IDENT
	| typename IDENT LBRACKET RBRACKET
	;

optionalparamlist:
	| paramlist
	;

paramlist: formalparam
	| paramlist COMMA formalparam
	;

statementblock: LBRACE statementlist RBRACE ;

statement: SEMI
	| expression SEMI
	| BREAK SEMI
	| CONTINUE SEMI
	| RETURN optionalexpression SEMI
	| IF ifheader ifstatement
	| WHILE ifheader ifstatementpart
	| DO ifstatementpart WHILE ifheader
	| FOR forheader ifstatementpart
	;

statementlist:
	| statementlist statement
	;

ifheader: LPAR expression RPAR ;

forheader: LPAR optionalexpression SEMI optionalexpression SEMI optionalexpression RPAR

ifstatement: ifstatementpart
	| ifstatementpart ELSE ifstatementpart
	;

ifstatementpart: statement
	| statementblock
	;

optionalexpression:
	| expression
	;

expressionlist: expression
	| expressionlist COMMA expression
	;

optionalexpressionlist:
	| expressionlist
	;

expression: constant
	| IDENT LPAR optionalexpressionlist RPAR
	| lvalue
	| lvalue assignop expression
	| crementer
	| unaryop expression
	| expression binop expression
	| expression QUEST expression COLON expression
	| LPAR typename RPAR expression
	| LPAR expression RPAR
	;

crementer: INCR lvalue
	| DECR lvalue
	| lvalue INCR
	| lvalue DECR

constant: INTCONST
	| STRCONST
	| CHARCONST
	| REALCONST
	;

lvalue: IDENT
	| IDENT LBRACKET expression RBRACKET
	;

unaryop: BANG
	| MINUS
	| TILDE
	;

binop: EQUALS
	| NEQUAL
	| LE
	| LT
	| GE
	| GT
	| PLUS
	| MINUS
	| STAR
	| SLASH
	| MOD
	| PIPE
	| DPIPE
	| AMP
	| DAMP
	;

assignop: ASSIGN
	| PLUSASSIGN
	| MINUSASSIGN
	| STARASSIGN
	| SLASHASSIGN
	;

%%
