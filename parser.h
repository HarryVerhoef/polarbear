#ifndef PARSER_H
#define PARSER_H

using namespace std;


// class ASTNode;

// class LabelPrefixAST;
// class BasicBlockAST;
// class GlobalVariableAST;

// class InstructionAST;
// class TertiaryOpAST;
// class BinaryOpAST;
// class UnaryOpAST;
// class NullaryOpAST;

// class ArgumentAST;
// class RegisterAST;
// class InRegisterAST;
// class LiteralAST;
// class LabelAST;
// class LiteralOffsetAST;
// class RelativeOffsetAST;

// class FunctionAST;

// class ProgramAST;

enum class TOKEN_TYPE {
    DOT,
    DOLLAR,
    PERCENT,
    COMMA,
    COLON,
    UNDERSCORE,
    INT_LIT,      // [0-9]+
    LPAR,
    RPAR,
    MINUS,

    // Registers
    AL,            // 'al'
    CL,            // 'cl'
    DL,            // 'dl'
    DIL,           // 'dil'
    RAX,           // 'rax'
    RCX,           // 'rcx'
    RBP,           // 'rbp'
    RSP,           // 'rsp'
    RIP,           // 'rip'
    EAX,           // 'eax'
    ECX,           // 'ecx'
    EDX,           // 'edx'
    EDI,           // 'edi'
    ESI,           // 'esi'
    R8B,           // 'r8b' (Basically the same as AL but for some reason not quite)
    R8W,
    R8D,           // 'r8d' 
    SIL,           // 'sil'
    XMM0,          // 'xmm0'
    XMM1,
    XMM2,
    XMM3,
    XMM4,
    XMM5,
    XMM6,
    XMM7,
    XMM8,
    XMM9,
    XMM10,



    // Data movement instructions
    MOVB,          // Move byte
    MOVL,          // Move long
    MOVD,          // Move double
    MOVQ,          // Move quad 
    MOVSS,         // Move scalar single precision floating-point value
    MOVSD,         // Move scalar double precision floating-point value
    MOVZBL,        // Move with zero extend (long)
    MOVAPS,        // Move Aligned Packed Single-Precision Floating-Point Values
    MOVABSQ,       // Move absolute quad
    PUSH,          // 'pushq'
    POP,           // 'popq'
    CVTSS2SD,      // 'cvtss2sd' Convert single-precision float to double-precision
    CVTSD2SS,      // 'cvtsd2ss' Opposite
    CLTD,          // 'cltd' Convert signed long to signed double-long


    // Arithmetic and logic instructions
    ADD,
    ADDL,          // Add long
    ADDSS,         // Add single-precision float
    ADDSD,         // Add double-precision float
    SUB,
    SUBL,          // Sub long
    SUBSS,         // Subtract single-precision float
    SUBSD,         // Subtract double-precision float

    IDIVL,         // Integer (long) division
    DIVSS,         // Divide scalar single precision floating-point values
    DIVSD,
    IMULL,         // Integer (long) multiplication
    MULSS,         // Multiply scalar single precision floating-point values
    MULSD,

    SETE,          // Set byte if equal
    SETNE,         // Set byte if not equal


    AND,
    ANDB,          // 'andb'  byte-wise logical AND 
    ANDL,          // 'andl'  long-wise logical AND
    OR,
    TEST,          // 'testb' (weird bitwise and)

    XOR,
    XORB,          // Bytewise XOR
    XORL,          // Longwise XOR

    SHLL,          // Shift Left (Long)

    // Control flow instructions

    JUMP,
    JE,            // Jump when equal
    JNE,           // Jump when not equal
    JZ,            // Jump when last result was 0
    JG,            // Jump when greater than
    JGE,           // Jump when greater than or equal to
    JL,            // Jump when less than
    JLE,           // Jump when less than or equal to

    CMP,
    CMPL,          // Compare unsigned

    CALL,          // 'callq'
    RET,           // 'retq'

    // Weird assembler metadata

    SECTION,        // 'section'
    REGULAR,        // 'regular'
    P_I,            // 'pure_instructions'
    B_V,            // 'build_version'
    MACOS,          // 'macos'
    SDK_V,          // 'sdk_version'
    B_L_4,          // '4byte_literals'
    B_L_8,          // '8byte_literals'
    GLOBAL,         // 'globl'
    ALIGN,          // 'p2align'
    ZEROFILL,       // 'zerofill'
    DATA,           // 'DATA'
    S_V_S,          // 'subsections_via_symbols'
    QUAD,           // 'quad'
    LONG,           // 'long'


    TEXT_CAPS,      // 'TEXT'
    TEXT_LOWER,     // 'text'
    COMMON,         // 'common'
    C,              // 'c'
    LITERAL_4,      // '__literal4'
    LITERAL_8,      // '__literal8'

    MAIN,           // '_main'


    NOP,            // '0x90' == no-operation

    EOL,
    E_O_F,

    IDENT,

    INVALID
};

string tokToLex(TOKEN_TYPE t) {
    switch (t) {
        case TOKEN_TYPE::DOT:
            return ".";
        case TOKEN_TYPE::DOLLAR:
            return "$";
        case TOKEN_TYPE::PERCENT:
            return "%";
        case TOKEN_TYPE::COMMA:
            return ",";
        case TOKEN_TYPE::COLON:
            return ":";
        case TOKEN_TYPE::INT_LIT:
            return "INT_LIT";
        case TOKEN_TYPE::LPAR:
            return "(";
        case TOKEN_TYPE::RPAR:
            return ")";
        case TOKEN_TYPE::MINUS:
            return "-";
        
        case TOKEN_TYPE::AL:
            return "al";
        case TOKEN_TYPE::CL:
            return "cl";
        case TOKEN_TYPE::DL:
            return "dl";
        case TOKEN_TYPE::DIL:
            return "dil";
        case TOKEN_TYPE::RAX:
            return "rax";
        case TOKEN_TYPE::RCX:
            return "rcx";
        case TOKEN_TYPE::RBP:
            return "rbp";
        case TOKEN_TYPE::RSP:
            return "rsp";
        case TOKEN_TYPE::RIP:
            return "rip";
        case TOKEN_TYPE::EAX:
            return "eax";
        case TOKEN_TYPE::ECX:
            return "ecx";
        case TOKEN_TYPE::EDX:
            return "edx";
        case TOKEN_TYPE::EDI:
            return "edi";
        case TOKEN_TYPE::ESI:
            return "esi";
        case TOKEN_TYPE::R8B:
            return "r8b";
        case TOKEN_TYPE::R8W:
            return "r8w";
        case TOKEN_TYPE::R8D:
            return "r8d";
        case TOKEN_TYPE::SIL:
            return "sil";
        case TOKEN_TYPE::XMM0:
            return "xmm0";
        case TOKEN_TYPE::XMM1:
            return "xmm1";
        case TOKEN_TYPE::XMM2:
            return "xmm2";
        case TOKEN_TYPE::XMM3:
            return "xmm3";
        case TOKEN_TYPE::XMM4:
            return "xmm4";
        case TOKEN_TYPE::XMM5:
            return "xmm5";
        case TOKEN_TYPE::XMM6:
            return "xmm6";
        case TOKEN_TYPE::XMM7:
            return "xmm7";
        case TOKEN_TYPE::XMM8:
            return "xmm8";
        case TOKEN_TYPE::XMM9:
            return "xmm9";
        case TOKEN_TYPE::XMM10:
            return "xmm10";


        case TOKEN_TYPE::MOVB:
            return "movb";
        case TOKEN_TYPE::MOVL:
            return "movl";
        case TOKEN_TYPE::MOVD:
            return "movd";
        case TOKEN_TYPE::MOVQ:
            return "movq";
        case TOKEN_TYPE::MOVSS:
            return "movss";
        case TOKEN_TYPE::MOVSD:
            return "movsd";
        case TOKEN_TYPE::MOVZBL:
            return "movzbl";
        case TOKEN_TYPE::MOVAPS:
            return "movaps";
        case TOKEN_TYPE::MOVABSQ:
            return "movabsq";
        case TOKEN_TYPE::PUSH:
            return "pushq";
        case TOKEN_TYPE::POP:
            return "popq";
        case TOKEN_TYPE::CVTSS2SD:
            return "cvtss2sd";
        case TOKEN_TYPE::CVTSD2SS:
            return "cvtsd2ss";
        case TOKEN_TYPE::CLTD:
            return "cltd";
        
        case TOKEN_TYPE::ADD:
            return "add";
        case TOKEN_TYPE::ADDL:
            return "addl";
        case TOKEN_TYPE::ADDSS:
            return "addss";
        case TOKEN_TYPE::ADDSD:
            return "addsd";
        case TOKEN_TYPE::SUB:
            return "sub";
        case TOKEN_TYPE::SUBL:
            return "subl";
        case TOKEN_TYPE::SUBSS:
            return "subss";
        case TOKEN_TYPE::SUBSD:
            return "subsd";
        case TOKEN_TYPE::IDIVL:
            return "idvil";
        case TOKEN_TYPE::DIVSS:
            return "divss";
        case TOKEN_TYPE::DIVSD:
            return "divsd";
        case TOKEN_TYPE::IMULL:
            return "imull";
        case TOKEN_TYPE::MULSS:
            return "mulss";
        case TOKEN_TYPE::MULSD:
            return "mulsd";

        case TOKEN_TYPE::SETE:
            return "sete";
        case TOKEN_TYPE::SETNE:
            return "setne";

        case TOKEN_TYPE::AND:
            return "and";
        case TOKEN_TYPE::ANDB:
            return "andb";
        case TOKEN_TYPE::ANDL:
            return "andl";
        case TOKEN_TYPE::OR:
            return "or";
        case TOKEN_TYPE::TEST:
            return "testb";
        
        case TOKEN_TYPE::XOR:
            return "xor";
        case TOKEN_TYPE::XORB:
            return "xorb";
        case TOKEN_TYPE::XORL:
            return "xorl";

        case TOKEN_TYPE::SHLL:
            return "shll";

        case TOKEN_TYPE::JUMP:
            return "jmp";
        case TOKEN_TYPE::JE:
            return "je";
        case TOKEN_TYPE::JNE:
            return "jne";
        case TOKEN_TYPE::JZ:
            return "jz";
        case TOKEN_TYPE::JG:
            return "jg";
        case TOKEN_TYPE::JGE:
            return "jge";
        case TOKEN_TYPE::JL:
            return "jl";
        case TOKEN_TYPE::JLE:
            return "jle";

        case TOKEN_TYPE::CMP:
            return "cmp";
        case TOKEN_TYPE::CMPL:
            return "cmpl";

        case TOKEN_TYPE::CALL:
            return "callq";
        case TOKEN_TYPE::RET:
            return "retq";


        case TOKEN_TYPE::SECTION:
            return "section";
        case TOKEN_TYPE::REGULAR:
            return "regular";
        case TOKEN_TYPE::P_I:
            return "pure_instructions";
        case TOKEN_TYPE::B_V:
            return "build_version";
        case TOKEN_TYPE::MACOS:
            return "macos";
        case TOKEN_TYPE::SDK_V:
            return "sdk_version";
        case TOKEN_TYPE::B_L_4:
            return "4byte_literals";
        case TOKEN_TYPE::B_L_8:
            return "8byte_literals";
        case TOKEN_TYPE::GLOBAL:
            return "globl";
        case TOKEN_TYPE::ALIGN:
            return "p2align";
        case TOKEN_TYPE::ZEROFILL:
            return "zerofill";
        case TOKEN_TYPE::DATA:
            return "__DATA";
        case TOKEN_TYPE::S_V_S:
            return "subsections_via_symbols";
        case TOKEN_TYPE::QUAD:
            return "quad";
        case TOKEN_TYPE::LONG:
            return "long";

        case TOKEN_TYPE::TEXT_CAPS:
            return "__TEXT";
        case TOKEN_TYPE::TEXT_LOWER:
            return "__text";
        case TOKEN_TYPE::COMMON:
            return "__common";
        case TOKEN_TYPE::LITERAL_4:
            return "__literal4";
        case TOKEN_TYPE::LITERAL_8:
            return "__literal8";

        case TOKEN_TYPE::MAIN:
            return "_main";
        
        case TOKEN_TYPE::NOP:
            return "0x90";

        case TOKEN_TYPE::EOL:
            return "EOL";
        case TOKEN_TYPE::E_O_F:
            return "EOF";
        
        case TOKEN_TYPE::IDENT:
            return "IDENT";

        default:
            return "INVALID";
    }
}

#endif