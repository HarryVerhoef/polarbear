#include <iostream>
#include <deque>
#include <vector>
#include "parser.h"
#include "ast.cpp"

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
    TOKEN_TYPE type = TOKEN_TYPE::UNKNOWN;
    string typeString = "INVALID";
    string left = "";
    string right = "";
    bool boolVal = false;
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

class PolarParseException : public exception {
    private:
        string expected = "";
        string exceptionname = "";
    public:
        PolarParseException() {};
        PolarParseException(const char* e) { this->setExpected(e); };
        PolarParseException(string e) { this->setExpected(e); };
        const char* what() throw() {
            string eString = this->exceptionName() + ": Got '" + curTok.lexeme + "', expected " + this->getExpected();
            eString +=  " at " + to_string(curTok.lineNo) + ":" + to_string(curTok.columnNo);
            return eString.c_str();
        };
        void setExpected(const char* e) { expected = "'" + string(e) + "'"; };
        void setExpected(string e) { expected = "'" + e + "'"; };
        string getExpected() { return expected; };
        string exceptionName() { return exceptionname; };
};

class PolarUndefinedTypeException : public PolarParseException {
    private:
        string typeident = "";
        string exceptionname = "PolarUndefinedTypeException";
    public:
        PolarUndefinedTypeException(const char* e) { this->setTypeIdent(e); };
        PolarUndefinedTypeException(string e) { this->setTypeIdent(e); };
        void setTypeIdent(string& i) { typeident = i; };
        void setTypeIdent(const char* e) { typeident = string(e); };
        string getTypeIdent() { return typeident; };
        const char* what() throw() {
            string eString = this->exceptionName() + ": Type '" + this->getTypeIdent() + "' is undefined.";
            return eString.c_str();
        };
};

TOKEN returnTok(string lexeme, TOKEN_TYPE type, bool boolVal=false) {
    TOKEN tok;
    tok.lexeme = lexeme;
    tok.type = type;
    tok.boolVal = boolVal;
    tok.lineNo = lineNo;
    tok.columnNo = columnNo;
    return tok;
}
TOKEN returnTok(string lexeme, TOKEN_TYPE type, string left, string right) {
    TOKEN tok;
    tok.lexeme = lexeme;
    tok.type = type;
    tok.left = left;
    tok.right = right;
    tok.lineNo = lineNo;
    tok.columnNo = columnNo;
    return tok;
}

/*  Return the next token   */
static TOKEN getTok() {
    static int lastChar = ' ';
    static int nextChar = ' ';
    static bool embed = false; /* Used to determine whether to return futureToks yet */
    static deque<TOKEN> futureToks = {}; /* Used for when expressions are embed in strings */

    if (!embed && futureToks.size()) {
        TOKEN t = futureToks[0];
        futureToks.pop_front();
        return t;
    }

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
            return returnTok("while", TOKEN_TYPE::WHILE);
        
        /* alphanumeric operators */
        if (identStr == "in")
            return returnTok("in", TOKEN_TYPE::IN);
        

        /* Syntax */
        if (identStr == "return")
            return returnTok("return", TOKEN_TYPE::RETURN);
        if (identStr == "true")
            return returnTok("true", TOKEN_TYPE::BOOL_LIT, true);
        if (identStr == "false")
            return returnTok("false", TOKEN_TYPE::BOOL_LIT);

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
        } else if (nextChar == '*') {
            while (lastChar != EOF) {
                lastChar = getc(pFile);
                if (lastChar == '*') {
                    nextChar = getc(pFile);
                    if (nextChar == '/') {
                        lastChar = getc(pFile);
                        columnNo++;
                        return getTok();
                    }
                    lastChar = nextChar;
                    columnNo++;
                }
                columnNo++;
            }
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
            nextChar = getc(pFile);
            if (nextChar == '=') {
                nextChar = getc(pFile);
                if (nextChar == '>') {
                    lastChar = getc(pFile);
                    columnNo += 4;
                    return returnTok("<==>", TOKEN_TYPE::EQUIV);
                }
                lastChar = nextChar;
                columnNo += 3;
                return returnTok("<==", TOKEN_TYPE::UNKNOWN);
            }
            lastChar = nextChar;
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
            nextChar = getc(pFile);
            if (nextChar == '>') {
                lastChar = getc(pFile);
                columnNo += 3;
                return returnTok("==>", TOKEN_TYPE::IMPLIES);
            }
            lastChar = nextChar;
            columnNo += 2;
            return returnTok("==", TOKEN_TYPE::EQUAL);
        }
        lastChar = nextChar;
        columnNo++;
        return returnTok("=", TOKEN_TYPE::ASSIGN);
    }

    /* Return ident if none of ops match, and during parse can check optable for ident lexvals when expecting ops */

    if (lastChar == '.') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok(".", TOKEN_TYPE::DOT);
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
    if (lastChar == ';') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok(";", TOKEN_TYPE::SEMICOLON);
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
    if (lastChar == '\'') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("'", TOKEN_TYPE::SQUOTE);
    }
    if (lastChar == '{') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("{", TOKEN_TYPE::LBRA);
    }
    if (lastChar == '}') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("}", TOKEN_TYPE::RBRA);
    }
    if (lastChar == '[') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("[", TOKEN_TYPE::LSBRA);
    }
    if (lastChar == ']') {
        lastChar = getc(pFile);
        columnNo++;
        return returnTok("]", TOKEN_TYPE::RSBRA);
    }

    if (lastChar == '"') {
        TOKEN q1 = returnTok("\"", TOKEN_TYPE::DQUOTE);
        futureToks.push_back(q1);
        lastChar = getc(pFile);
        while (lastChar != '"') {
            if (lastChar == '{') {
                embed = true;
                while (lastChar != '}') {
                    TOKEN tok = getTok();
                    futureToks.push_back(tok);
                }
                embed = false;
                TOKEN rbra = returnTok("}", TOKEN_TYPE::RBRA);
                futureToks.push_back(rbra);
                columnNo++;
            } else {
                TOKEN c = returnTok(string(1,lastChar), TOKEN_TYPE::STRING_LIT);
                futureToks.push_back(c);
                columnNo++;
            }
            lastChar = getc(pFile);
        }
        TOKEN q2 = returnTok("\"", TOKEN_TYPE::DQUOTE);
        futureToks.push_back(q2);
        lastChar = getc(pFile);
    }

    if (!embed && futureToks.size() > 0) {
        TOKEN t = futureToks[0];
        futureToks.pop_front();
        return t;
    }
    
    /* int or real literals */
    if (isdigit(lastChar)) {
        string l = "";
        string r = "";

        do {
            l += lastChar;
            lastChar = getc(pFile);
            columnNo++;
        } while (isdigit(lastChar));
        
        if (lastChar == '.') {
            while (isdigit(lastChar = getc(pFile))) {
                r += lastChar;
                columnNo++;
            }
            return returnTok(r, TOKEN_TYPE::REAL_LIT, l, r);
        }
        return returnTok(l, TOKEN_TYPE::INT_LIT, l, r);
    }

    if (lastChar == EOF) {
        columnNo++;
        return returnTok("EOF", TOKEN_TYPE::E_O_F);
    }

    // If token unidentifiable then return lastChar and ascii value

    lastChar = getc(pFile);
    
    return returnTok("UNKNOWN", TOKEN_TYPE::UNKNOWN);
}

static TOKEN getNextToken() {
    if (tok_buffer.size() == 0)
        tok_buffer.push_back(getTok());
    
    TOKEN temp = tok_buffer.front();
    tok_buffer.pop_front();

    return curTok = temp;
};


/* parser: adds context to the program */
type basetype = type("type");

 

unique_ptr<def> def_parse() {
    if (curTok.type != TOKEN_TYPE::IDENT)
        throw PolarParseException("ident");
    string typeident = curTok.lexeme;
    getNextToken();

    if (basetype.hasType(i)) {
        /* Non-constructor */
        shared_ptr<type> membertype = basetype.getType(typeident);

        if (curTok.type != TOKEN_TYPE::IDENT)
            throw PolarParseException("ident");
        string memberident = curTok.lexeme;
        getNextToken();

        switch (curTok.type) {
            case TOKEN_TYPE::ASSIGN: {
                getNextToken();
                
                unique_ptr<expr> memberexpr = expr_parse();

                if (curTok.type != TOKEN_TYPE::SEMICOLON)
                    throw PolarParseException(";");
                getNextToken();

                return make_unique<def>(new tmembervar(memberident, membertype, memberexpr));

            };
            case TOKEN_TYPE::LPAR: {
                getNextToken();

                unique_ptr<params> memberparams = params_parse();

                if (curTok.type != TOKEN_TYPE::RPAR)
                    throw PolarParseException(")");
                getNextToken();

                if (curTok.type != TOKEN_TYPE::LBRA)
                    throw PolarParseException("{");
                getNextToken();

                unique_ptr<block> memberblock = block_parse();

                if (curTok.type != TOKEN_TYPE::RBRA)
                    throw PolarParseException("}");
                getNextToken();

                if (curTok.type != TOKEN_TYPE::SEMICOLON)
                    throw PolarParseException(";");
                getNextToken();

                return make_unique<def>(new tmemberfunc(memberident, membertype, memberparams, memberblock));
            };
            default: {
                throw PolarParseException("=' or '(");
            };
        };
    } else {
        /* Constructor */
        if (curTok.type != TOKEN_TYPE::LPAR)
            throw PolarParseException("(");
        getNextToken();

        unique_ptr<params> constructorparams = params_parse();

        if (curTok.type != TOKEN_TYPE::RPAR)
            throw PolarParseException(")");
        getNextToken();

        if (curTok.type != TOKEN_TYPE::LBRA)
            throw PolarParseException("{");
        getNextToken();

        unique_ptr<block> constructorblock = block_parse();

        if (curTok.type != TOKEN_TYPE::RBRA)
            throw PolarParseException("}");
        getNextToken();

        if (curTok.type != TOKEN_TYPE::SEMICOLON)
            throw PolarParseException(";");
        getNextToken();

        return make_unique<def>(new tconstructor(constructorparams, constructorblock));
    };
};

unique_ptr<vector<unique_ptr<def>>> complex_def_list() {
    vector<unique_ptr<def>> defs = {};
    while (curTok.type != TOKEN_TYPE::RBRA) {
        unique_ptr<def> d = def_parse();
        defs.push_back(move(d));
    };
    return make_unique<vector<unique_ptr<def>>>(defs);
};

ACCESS access() {
    switch(curTok.type) {
        case TOKEN_TYPE::PUBLIC: {
            getNextToken();
            return ACCESS::PUBLIC;
        };
        case TOKEN_TYPE::PRIVATE: {
            getNextToken();
            return ACCESS::PRIVATE;
        };
        default: {
            throw PolarParseException("public' or 'private");
        };
    };
};

unique_ptr<vector<unique_ptr<complexblock>>> complex_type() {
    vector<unique_ptr<complexblock>> v = {};
    while (curTok.type != TOKEN_TYPE::RBRA) {
        ACCESS a = access();

        if (curTok.type != TOKEN_TYPE::LBRA)
            throw PolarParseException("{");
        getNextToken();

        unique_ptr<vector<unique_ptr<def>>> defs = complex_def_list();

        if (curTok.type != TOKEN_TYPE::RBRA)
            throw PolarParseException("}");
        getNextToken();

        if (curTok.type != TOKEN_TYPE::SEMICOLON)
            throw PolarParseException(";");
        getNextToken();
    };
    return make_unique<vector<unique_ptr<complexblock>>>(v);
};

unique_ptr<tdef> tdef_parse() {
    if (curTok.type != TOKEN_TYPE::TYPE)
        throw PolarParseException("type");
    getNextToken();

    if (curTok.type != TOKEN_TYPE::IDENT)
        throw PolarParseException("ident");
    string tdefident = curTok.lexeme;
    getNextToken();

    switch(curTok.type) {
        case TOKEN_TYPE::ASSIGN: {
            /* simple type */
            getNextToken();
            shared_ptr<polarset> vset = set_parse();

            if (curTok.type != TOKEN_TYPE::SEMICOLON)
                throw PolarParseException(";");
            return make_unique<tdef>(new simpletdef(tdefident, vset));
        };
        case TOKEN_TYPE::LBRA: {
            /* complex type without inheritance */
            getNextToken();

            unique_ptr<vector<unique_ptr<complexblock>>> cblocks = complex_type();

            if (curTok.type != TOKEN_TYPE::RBRA)
                throw PolarParseException("}");
            getNextToken();

            if (curTok.type != TOKEN_TYPE::SEMICOLON)
                throw PolarParseException(";");
            getNextToken();

            return make_unique<tdef>(new complextdef(tdefident, cblocks));
        };
        case TOKEN_TYPE::COLON: {
            /* complex type with inheritance */
            getNextToken();

            if (curTok.type != TOKEN_TYPE::IDENT)
                throw PolarParseException("ident");
            string supertypeident = curTok.lexeme;
            shared_ptr<type> supertype = basetype.getType(supertypeident);
            getNextToken();

            if (curTok.type != TOKEN_TYPE::LBRA)
                throw PolarParseException("{");
            getNextToken();

            unique_ptr<vector<unique_ptr<complexblock>>> cblocks = complex_type();

            if (curTok.type != TOKEN_TYPE::RBRA)
                throw PolarParseException("}");
            getNextToken();

            if (curTok.type != TOKEN_TYPE::SEMICOLON)
                throw PolarParseException(";");
            getNextToken();

            return make_unique<tdef>(new complextdef(tdefident, supertype, cblocks))

        }
    };
};

shared_ptr<type> type_parse() {
    /*
    ** Token is definition type at this point
    ** Need to make sure token is not reserved syntax
    ** Need to search types to check if is a known type
    */
    if (curTok.type != TOKEN_TYPE::IDENT)
        throw PolarParseException("ident");

    if (!basetype.hasType(curTok.lexeme))
        throw PolarUndefinedTypeException(curTok.lexeme);

    shared_ptr<type> type = basetype.getType(curTok.lexeme);
    getNextToken();
    return type;
};

unique_ptr<def> global() {

    switch(curTok.type) {
        case TOKEN_TYPE::TYPE: {
            return tdef_parse();
        };
        default: {
            shared_ptr<type> globaltype = type_parse();

            if (curTok.type != TOKEN_TYPE::IDENT)
                throw PolarParseException("ident");
            string globalident = curTok.lexeme;
            getNextToken();

            if (curTok.type != TOKEN_TYPE::LPAR)
                throw PolarParseException("(");
            getNextToken();

            unique_ptr<params> globalparams = params_parse();

            if (curTok.type != TOKEN_TYPE::RPAR)
                throw PolarParseException(")");
            getNextToken();

            if (curTok.type != TOKEN_TYPE::LBRA)
                throw PolarParseException("{");
            getNextToken();

            unique_ptr<block> globalblock = block_parse();

            if (curTok.type != TOKEN_TYPE::RBRA)
                throw PolarParseException("}");
            getNextToken();

            if (curTok.type != TOKEN_TYPE::SEMICOLON)
                throw PolarParseException(";");
            getNextToken();

            return make_unique<def>(new functionast(globaltype, globalident, globalparams, globalblock));
        };
    };
};

unique_ptr<vector<unique_ptr<def>>> global_list() {
    vector<unique_ptr<def>> globals = {};
    while (curTok.type != TOKEN_TYPE::E_O_F) {
        unique_ptr<def> g = global();
        globals.push_back(move(g));
    };
    return make_unique<vector<unique_ptr<def>>>(globals);
};

unique_ptr<program> parser() {
    unique_ptr<vector<unique_ptr<def>>> globals = global_list();
    unique_ptr<program> prog = make_unique<program>(new program(globals));
    return prog;
};


int main(int argc, char **argv) {
    
    int lineNo = 1;
    int columnNo = 1;
    string filename = string(argv[1]);
    string process = string(argv[2]);
    pFile = fopen(filename.c_str(), "r");
    if (pFile == NULL) {
        cout << "Null file";
        return 1;
    };

    if (process == "-lex") {
        cout << "Lexing " << filename << "\n";
        do {
            getNextToken();
            cout << "Token: " << curTok.lexeme << "\n";
        } while (curTok.type != TOKEN_TYPE::E_O_F);
        return 0;
    } else if (process == "-parse") {
        cout << "Parsing " << filename << "\n";
        // unique_ptr<program> prog = parser();
        return 0;
    } else {
        cout << "Unknown process mode...\n";
        return 1;
    };
};


/*
** grammar:
**
** Program -> Global_list Main
**
** Global_list -> Global Global_list
**              | e
**
** Global -> Type ident ( Params ) { Block } ;
**         | Typedef
**
** Type -> <types>
**
** Typedef -> type ident = Set ;
**          | type ident { Complextype } ;
**          | type ident : ident { Complextype } ;
**
** Complextype -> AccessMembers Complextype
**              | e
**
** AccessMembers -> Access { Def_list } ;
**
** Access -> public
**         | private
** 
** Def_list -> Def Def_list
**           | e
** 
** Def -> Type ident = Expr ;
**      | Type ident ( Params ) { Block } ;
**      | ident ( Params ) { Block } ;
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
**       | Logic
**
** Logic -> Or LogicP
** LogicP -> <==> Or LogicP
**         | <= { Set } => Or LogicP
**         | ==> Or LogicP
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
**       | Array
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
**
** Set -> { }
**      | { Args_list }
**      | type
**      | ident
**      | { Expr : Iterators : Expr }
**
** Iterators -> for ident in Iterator Iterators
**            | e
**
** Iterator -> Set
**           | Array
**           | <other iterator subtypes>
**
** Array -> []
**        | [ Args_list ]
**        | ident
**        | [ Expr : Iterators : Expr ]
*/