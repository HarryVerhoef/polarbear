#include <iostream>

#include "parser.h"

using namespace std;

/*
** lexer: 
*/

template <class T>
bool isElem(T& e, const vector<T>& v) {
    for (const T& elem: v) {
        if (elem == e)
            return true;
    }
    return false;
};

struct TOKEN {
    string lexeme = "";
    TOKEN_TYPE type = TOKEN_TYPE::INVALID;
    string typeString = "INVALID";
    unsigned long intVal = 0;
    float floatVal = 0.0;
    int lineNo = 0;
    int columnNo = 0;
};

FILE* pFile;
TOKEN curTok;
deque<TOKEN> tok_buffer;
int lineNo = 0;
int columnNo = 0;
string identStr = "";
bool parsedMain = false;

class AsmParseException : public exception {
    private:
        string expected = "";
    public:
        AsmParseException(const char* e) : expected("'" + string(e) + "'") {};
        AsmParseException(string e) : expected("'" + e + "'") {};
        AsmParseException(const vector<TOKEN_TYPE>& e) {
            if (expected.size() != 0)
                expected = "'" + tokToLex(e[0]) + "'";
            for (int i=1; i < e.size(); i++) {
                expected += " or '" + tokToLex(e[i]) + "'";
            }
        }
        const char* what() const throw() {
            string eString = "AsmParseException: Got '" + curTok.lexeme + "', expected " + expected;
            eString +=  " at " + to_string(curTok.lineNo) + ":" + to_string(curTok.columnNo);
            return eString.c_str();
        };
};

TOKEN returnTok(string lexeme, TOKEN_TYPE type, unsigned long intVal=0, float floatVal=0.0) {
    TOKEN tok;
    tok.lexeme = lexeme;
    tok.type = type;
    tok.intVal = intVal;
    tok.floatVal = floatVal;
    tok.lineNo = lineNo;
    tok.columnNo = columnNo;
    return tok;
}

/*  Return the next token   */
static TOKEN getTok() {
    static int lastChar = ' ';
    static int nextChar = ' ';

    // Skip whitespace
    while (isspace(lastChar)) {
        if (lastChar == '\n' || lastChar == '\r') {
            lineNo++;
            columnNo = 1;
            lastChar = getc(pFile);
            return returnTok("EOL", TOKEN_TYPE::EOL);
        }
        lastChar = getc(pFile);
        columnNo++;
    }
        
    if (isalpha(lastChar) || lastChar == '_') { // Last character is alphabetic or an underscore
        identStr = lastChar;
        columnNo++;

        while (isalnum(lastChar = getc(pFile)) || lastChar == '_') { // While characters are alphanumeric
            identStr += lastChar;
            columnNo++;
        }

        /* Types */
        if (identStr == "int")
            return returnTok("int", TOKEN_TYPE::INT);
        if (identStr == "real")
            return returnTok("real", TOKEN_TYPE::REAL);
        if (identStr == "bool")
            return returnTok("bool", TOKEN_TYPE::BOOL);
        if (identStr == "char")
            return returnTok("char", TOKEN_TYPE::CHAR);
        if (identStr == "string")
            return returnTok("string", TOKEN_TYPE::STRING);
        if (identStr == "set")
            return returnTok("set", TOKEN_TYPE::SET);
        if (identStr == "array")
            return returnTok("array", TOKEN_TYPE::ARRAY);
        
        /* If type but none of these types then return ident and check while parsing whether or not ident has been typedeffed */
        
        /* Statements */
        if (identStr == "if")
            return returnTok("if", TOKEN_TYPE::IF);
        if (identStr == "else")
            return returnTok("else", TOKEN_TYPE::ELSE);
        if (identStr == "for")
            return returnTok("for", TOKEN_TYPE::FOR);
        if (identStr == "while")
            return returnTok("while", TOKEN_TYPE::WHILE)
        

        if (identStr == "in")
            return returnTok("in", TOKEN_TYPE::IN);
        

        /* Syntax */
        if (identStr == "return")
            return returnTok("return", TOKEN_TYPE::RETURN);

        if (identStr == "main")
            return returnTok("main", TOKEN_TYPE::MAIN);
        
        return returnTok("IDENT - " + identStr, TOKEN_TYPE::IDENT);

    }

    /* Operators */
    if (lastChar == '/') {
        nextChar = getc(pFile);
        if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("/=", TOKEN_TYPE::DEQ);
        } else if (nextChar == '\\') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("/\\", TOKEN_TYPE::INTERSECT);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("/", TOKEN_TYPE::DIV);
    }
    if (lastChar == '\\') {
        nextChar = getc(pFile);
        if (nextChar == '/') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("\\/", TOKEN_TYPE::UNION);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("\\", TOKEN_TYPE::BACKSLASH);
    }
    if (lastChar == '*') {
        nextChar = getc(pFile);
        if (nextChar == '*') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("**", TOKEN_TYPE::EXP);
        } else if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("*=", TOKEN_TYPE::TEQ);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("*", TOKEN_TYPE::TIMES);
    }
    if (lastChar == '+') {
        nextChar = getc(pFile);
        if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("+=", TOKEN_TYPE::PEQ);
        } else if (nextChar == '+') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("++", TOKEN_TYPE::INCREMENT);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("+", TOKEN_TYPE::ADD);
    }
    if (lastChar == '-') {
        nextChar = getc(pFile);
        if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("-=", TOKEN_TYPE::MEQ);
        } else if (nextChar == '-') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("--", TOKEN_TYPE::DECREMENT);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("-", TOKEN_TYPE::MINUS);
    }
    if (lastChar == '>') {
        nextChar = getc(pFile);
        if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok(">=", TOKEN_TYPE::GEQ);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok(">", TOKEN_TYPE::GT);
    }
    if (lastChar == '<') {
        nextChar = getc(pFile);
        if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("<=", TOKEN_TYPE::LEQ);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("<", TOKEN_TYPE::LT);
    }
    if (lastChar == '=') {
        nextChar = getc(pFile);
        if (nextChar == '=') {
            lastChar = getc(pFile);
            columnNo += 2;
            return returnTok("==", TOKEN_TYPE::EQUAL);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("=", TOKEN_TYPE::ASSIGN);
    }

    if (identStr == "/\\")
        return returnTok("/\\", TOKEN_TYPE::INTERSECT);
    if (identStr == "\\/")
        return returnTok("\\/", TOKEN_TYPE::UNION);
    if (identStr == ">=")
        return returnTok(">=", TOKEN_TYPE::GEQ);
    if (identStr == "<=")
        return returnTok("<=", TOKEN_TYPE::LEQ);
    if (identStr == "<==>")
        return returnTok("<==>", TOKEN_TYPE::EQUIV);
    

    /* Return ident if none of ops match, and during parse can check optable for ident lexvals when expecting ops */

    if (lastChar == '.') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok(".", TOKEN_TYPE::DOT);
    }
    if (lastChar == '$') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("$", TOKEN_TYPE::DOLLAR);
    }
    if (lastChar == '%') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("%", TOKEN_TYPE::PERCENT);
    }
    if (lastChar == ',') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok(",", TOKEN_TYPE::COMMA);
    }
    if (lastChar == ':') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok(":", TOKEN_TYPE::COLON);
    }
    if (lastChar == '(') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("(", TOKEN_TYPE::LPAR);
    }
    if (lastChar == ')') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok(")", TOKEN_TYPE::RPAR);
    }
    if (lastChar == '-') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("-", TOKEN_TYPE::MINUS);
    }
    
    if (isdigit(lastChar)) {
        string numStr = "";
        do {
            numStr += lastChar;
            lastChar = getc(pFile);
            columnNo++;
        } while (isdigit(lastChar));
        
        if (isalnum(lastChar)) {
            do {
                numStr += lastChar;
                lastChar = getc(pFile);
                columnNo++;
            } while (isalnum(lastChar) || lastChar == '_');

            if (numStr == "4byte_literals")
                return returnTok("4byte_literals", TOKEN_TYPE::B_L_4);
            if (numStr == "8byte_literals")
                return returnTok("8byte_literals", TOKEN_TYPE::B_L_8);
            if (numStr == "0x90")
                return returnTok("0x90", TOKEN_TYPE::NOP);

            cout << "INVALID: " << numStr;
            
        } else {
            return returnTok(numStr, TOKEN_TYPE::INT_LIT, stoul(numStr));
        }

        
    }

    if (lastChar == EOF) {
        columnNo++;
        return returnTok("EOF", TOKEN_TYPE::E_O_F);
    }

    // If token unidentifiable then return lastChar and ascii value
    
    return returnTok("INVALID", TOKEN_TYPE::INVALID);

}




/*
** grammar:
**
** Program -> Global_list Main
**
** Global_list -> Global Global_list
**              | e
**
** Global -> Type ident ( Params ) { Block } ;
**         | Expr ;
**         | Typedef
**
** Type -> <types>
**
** Typedef -> type ident = Set ;
**          | type ident { Complextype } ;
**          | type ident : ident { Complextype } ;
**
** Params -> Param_list
**         | e
**
** Param_list -> Param , Param_list
**             | Param
**
** Param -> Type ident
**
** Block -> IfStmt Block
**        | ForStmt Block
**        | WhileStmt Block
**        | Expr Block
**        | e
**
** IfStmt -> if ( Expr ) { Block } ElseStmt
**
** ElseStmt -> else IfStmt
**           | else { Block } ;
**           | ;
**
** ForStmt -> for ( ident : Expr ) { Block } ;
**          | for ( Expr ; Expr ; Expr ) { Block } ;
**
** WhileStmt -> while ( Expr ) { Block } ;
**
** Expr -> Type ident = Expr ;
**       | Or 
**
** Or -> And OrP
** OrP -> \/ And OrP
**      | e
**
** And -> In AndP
** AndP -> /\ In AndP
**       | e
**
** In -> Eq InP
** InP -> in Eq InP
**      | e
**
** Eq -> Comp EqP
** EqP -> == Comp EqP
**      | != Comp EqP
**      | <==> Comp EqP
**      | e
**
** Comp -> Sum CompP
** CompP -> <= Sum CompP
**        | < Sum CompP
**        | >= Sum CompP
**        | > Sum CompP
**        | e
**
** Sum -> Prod SumP
** SumP -> + Prod SumP
**       | - Prod SumP
**       | e
**
** Prod -> Neg ProdP
** ProdP -> * Neg ProdP
**        | / Neg ProdP
**        | e
**
** Neg -> NegP Term
** NegP -> ! NegP
**       | - NegP
**       | e
**
** Term -> ( Expr )
**       | ident TermP
**       | int_lit
**       | real_lit
**       | bool_lit 
**       | array_list
**       | char_lit
**       | string_lit
**       | Set
**
** TermP -> ( Args )
**        | e
**
** Args -> Args_list
**       | e
**
** Args_list -> Expr Args_listP
** Args_listP -> , Expr Args_listP
**             | e
*/