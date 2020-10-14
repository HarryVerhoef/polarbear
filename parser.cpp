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

        if (identStr == "al")
            return returnTok("al", TOKEN_TYPE::AL);
        if (identStr == "cl")
            return returnTok("cl", TOKEN_TYPE::CL);
        if (identStr == "dl")
            return returnTok("dl", TOKEN_TYPE::DL);
        if (identStr == "dil")
            return returnTok("dil", TOKEN_TYPE::DIL);
        if (identStr == "rax")
            return returnTok("rax", TOKEN_TYPE::RAX);
        if (identStr == "rcx")
            return returnTok("rcx", TOKEN_TYPE::RCX);
        if (identStr == "rbp")
            return returnTok("rbp", TOKEN_TYPE::RBP);
        if (identStr == "rsp")
            return returnTok("rsp", TOKEN_TYPE::RSP);
        if (identStr == "rip")
            return returnTok("rip", TOKEN_TYPE::RIP);
        if (identStr == "eax")
            return returnTok("eax", TOKEN_TYPE::EAX);
        if (identStr == "ecx")
            return returnTok("ecx", TOKEN_TYPE::ECX);
        if (identStr == "edx")
            return returnTok("edx", TOKEN_TYPE::EDX);
        if (identStr == "edi")
            return returnTok("edi", TOKEN_TYPE::EDI);
        if (identStr == "esi")
            return returnTok("esi", TOKEN_TYPE::ESI);
        if (identStr == "r8b")
            return returnTok("r8b", TOKEN_TYPE::R8B);
        if (identStr == "r8w")
            return returnTok("r8w", TOKEN_TYPE::R8W);
        if (identStr == "r8d")
            return returnTok("r8d", TOKEN_TYPE::R8D);
        if (identStr == "sil")
            return returnTok("sil", TOKEN_TYPE::SIL);
        if (identStr == "xmm0")
            return returnTok("xmm0", TOKEN_TYPE::XMM0);
        if (identStr == "xmm1")
            return returnTok("xmm1", TOKEN_TYPE::XMM1);
        if (identStr == "xmm2")
            return returnTok("xmm2", TOKEN_TYPE::XMM2);
        if (identStr == "xmm3")
            return returnTok("xmm3", TOKEN_TYPE::XMM3);
        if (identStr == "xmm4")
            return returnTok("xmm4", TOKEN_TYPE::XMM4);
        if (identStr == "xmm5")
            return returnTok("xmm5", TOKEN_TYPE::XMM5);
        if (identStr == "xmm6")
            return returnTok("xmm6", TOKEN_TYPE::XMM6);
        if (identStr == "xmm7")
            return returnTok("xmm7", TOKEN_TYPE::XMM7);
        if (identStr == "xmm8")
            return returnTok("xmm8", TOKEN_TYPE::XMM8);
        if (identStr == "xmm9")
            return returnTok("xmm9", TOKEN_TYPE::XMM9);
        if (identStr == "xmm10")
            return returnTok("xmm10", TOKEN_TYPE::XMM10);




        if (identStr == "movb")
            return returnTok("movb", TOKEN_TYPE::MOVB);
        if (identStr == "movl")
            return returnTok("movl", TOKEN_TYPE::MOVL);
        if (identStr == "movd")
            return returnTok("movd", TOKEN_TYPE::MOVD);
        if (identStr == "movq")
            return returnTok("movq", TOKEN_TYPE::MOVQ);
        if (identStr == "movss")
            return returnTok("movss", TOKEN_TYPE::MOVSS);
        if (identStr == "movsd")
            return returnTok("movsd", TOKEN_TYPE::MOVSD);
        if (identStr == "movzbl")
            return returnTok("movzbl", TOKEN_TYPE::MOVZBL);
        if (identStr == "movaps")
            return returnTok("movaps", TOKEN_TYPE::MOVAPS);
        if (identStr == "movabsq")
            return returnTok("movabsq", TOKEN_TYPE::MOVABSQ);
        if (identStr == "pushq")
            return returnTok("pushq", TOKEN_TYPE::PUSH);
        if (identStr == "popq")
            return returnTok("popq", TOKEN_TYPE::POP);
        if (identStr == "addq")
            return returnTok("addq", TOKEN_TYPE::ADD);
        if (identStr == "addl")
            return returnTok("addl", TOKEN_TYPE::ADDL);
        if (identStr == "addss")
            return returnTok("addss", TOKEN_TYPE::ADDSS);
        if (identStr == "addsd")
            return returnTok("addsd", TOKEN_TYPE::ADDSD);
        if (identStr == "subq")
            return returnTok("subq", TOKEN_TYPE::SUB);
        if (identStr == "subl")
            return returnTok("subl", TOKEN_TYPE::SUBL);
        if (identStr == "subss")
            return returnTok("subss", TOKEN_TYPE::SUBSS);
        if (identStr == "subsd")
            return returnTok("subsd", TOKEN_TYPE::SUBSD);
        if (identStr == "idivl")
            return returnTok("idivl", TOKEN_TYPE::IDIVL);
        if (identStr == "divss")
            return returnTok("divss", TOKEN_TYPE::DIVSS);
        if (identStr == "divsd")
            return returnTok("divsd", TOKEN_TYPE::DIVSD);
        if (identStr == "imull")
            return returnTok("imull", TOKEN_TYPE::IMULL);
        if (identStr == "mulss")
            return returnTok("mulss", TOKEN_TYPE::MULSS);
        if (identStr == "mulsd")
            return returnTok("mulsd", TOKEN_TYPE::MULSD);
        if (identStr == "sete")
            return returnTok("sete", TOKEN_TYPE::SETE);
        if (identStr == "setne")
            return returnTok("setne", TOKEN_TYPE::SETNE);
        if (identStr == "and")
            return returnTok("and", TOKEN_TYPE::AND);
        if (identStr == "andb")
            return returnTok("andb", TOKEN_TYPE::ANDB);
        if (identStr == "andl")
            return returnTok("andl", TOKEN_TYPE::ANDL);
        if (identStr == "or")
            return returnTok("or", TOKEN_TYPE::OR);
        if (identStr == "testb")
            return returnTok("testb", TOKEN_TYPE::TEST);
        if (identStr == "xor")
            return returnTok("xor", TOKEN_TYPE::XOR); // Does this exist?
        if (identStr == "xorb")
            return returnTok("xorb", TOKEN_TYPE::XORB);
        if (identStr == "xorl")
            return returnTok("xorl", TOKEN_TYPE::XORL); // Does there exist an XORD, XORQ?
        if (identStr == "shll")
            return returnTok("shll", TOKEN_TYPE::SHLL);

        if (identStr == "jmp")
            return returnTok("jmp", TOKEN_TYPE::JUMP);
        if (identStr == "je")
            return returnTok("je", TOKEN_TYPE::JE);
        if (identStr == "jne")
            return returnTok("jne", TOKEN_TYPE::JNE);
        if (identStr == "jz")
            return returnTok("jz", TOKEN_TYPE::JZ);
        if (identStr == "jg")
            return returnTok("jg", TOKEN_TYPE::JG);
        if (identStr == "jge")
            return returnTok("jge", TOKEN_TYPE::JGE);
        if (identStr == "jl")
            return returnTok("jl", TOKEN_TYPE::JL);
        if (identStr == "jle")
            return returnTok("jle", TOKEN_TYPE::JLE);
        if (identStr == "cmp")
            return returnTok("cmp", TOKEN_TYPE::CMP);
        if (identStr == "cmpl")
            return returnTok("cmpl", TOKEN_TYPE::CMPL);
        if (identStr == "callq")
            return returnTok("callq", TOKEN_TYPE::CALL);
        if (identStr == "retq")
            return returnTok("retq", TOKEN_TYPE::RET);
        if (identStr == "cvtss2sd")
            return returnTok("cvtss2sd", TOKEN_TYPE::CVTSS2SD);
        if (identStr == "cvtsd2ss")
            return returnTok("cvtsd2ss", TOKEN_TYPE::CVTSD2SS);
        if (identStr == "cltd")
            return returnTok("cltd", TOKEN_TYPE::CLTD);
        

        if (identStr == "section")
            return returnTok("section", TOKEN_TYPE::SECTION);
        if (identStr == "regular")
            return returnTok("regular", TOKEN_TYPE::REGULAR);
        if (identStr == "pure_instructions")
            return returnTok("pure_instructions", TOKEN_TYPE::P_I);
        if (identStr == "build_version")
            return returnTok("build_version", TOKEN_TYPE::B_V);
        if (identStr == "macos")
            return returnTok("macos", TOKEN_TYPE::MACOS);
        if (identStr == "sdk_version")
            return returnTok("sdk_version", TOKEN_TYPE::SDK_V);
        if (identStr == "globl")
            return returnTok("globl", TOKEN_TYPE::GLOBAL);
        if (identStr == "retq")
            return returnTok("retq", TOKEN_TYPE::RET);
        if (identStr == "quad")
            return returnTok("quad", TOKEN_TYPE::QUAD);
        if (identStr == "long")
            return returnTok("long", TOKEN_TYPE::LONG);
        
        
        if (identStr == "p2align")
            return returnTok("p2align", TOKEN_TYPE::ALIGN);
        if (identStr == "zerofill")
            return returnTok("zerofill", TOKEN_TYPE::ZEROFILL);
        if (identStr == "__DATA")
            return returnTok("__DATA", TOKEN_TYPE::DATA);
        if (identStr == "subsections_via_symbols")
            return returnTok("subsections_via_symbols", TOKEN_TYPE::S_V_S);

        if (identStr == "__TEXT")
            return returnTok("__TEXT", TOKEN_TYPE::TEXT_CAPS);
        if (identStr == "__text")
            return returnTok("__text", TOKEN_TYPE::TEXT_LOWER);
        if (identStr == "__common")
            return returnTok("common", TOKEN_TYPE::COMMON);
        if (identStr == "c")
            return returnTok("c", TOKEN_TYPE::C);
        if (identStr == "__literal4")
            return returnTok("__literal4", TOKEN_TYPE::LITERAL_4);
        if (identStr == "__literal8")
            return returnTok("__literal8", TOKEN_TYPE::LITERAL_8);
        
        if (identStr == "_main")
            return returnTok("_main", TOKEN_TYPE::MAIN);
        
        return returnTok("IDENT - " + identStr, TOKEN_TYPE::IDENT);

    }

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