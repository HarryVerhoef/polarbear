#ifndef AST_H
#define AST_H

class astnode;

class program;

class def;
class function;
class params;
class variable;

class block;
class stmt;
class ifstmt;
class elsestmt;
class forstmt;
class foreach;
class forindex;
class whilestmt;
class expr;

class op;
class binop;
class unop;

class call;
class literal;
class int_lit;
class real_lit;
class string_lit;
class char_lit;



enum class AST_TYPE {
    ASTNODE,
    PROGRAM,
    DEF,
    FUNCTION,
    PARAMS,
    VARIABLE,
    TDEF,
    STYPE,
    CTYPE,
    BLOCK,
    STMT,
    IFSTMT,
    ELSESTMT,
    FORSTMT,
    FOREACH,
    FORINDEX,
    WHILESTMT,
    EXPR,
    BINOP,
    UNOP,
    CALL,
    LITERAL,
    INT_LIT,
    REAL_LIT,
    STRING_LIT,
    CHAR_LIT
};


#endif