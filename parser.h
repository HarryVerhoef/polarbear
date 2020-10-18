#ifndef PARSER_H
#define PARSER_H


enum class TOKEN_TYPE {
    EOL,
    INT,
    REAL,
    BOOL,
    CHAR,
    STRING,
    SET,
    ARRAY,
    IF,
    ELSE,
    FOR,
    WHILE,
    IN,
    RETURN,
    BOOL_LIT,
    MAIN,
    IDENT,
    DEQ,
    INTERSECT,
    DIV,
    UNION,
    BACKSLASH,
    EXP,
    TEQ,
    TIMES,
    PEQ,
    INCREMENT,
    ADD,
    MEQ,
    DECREMENT,
    MINUS,
    GEQ,
    GT,
    EQUIV,
    LEQ,
    LT,
    IMPLIES,
    EQUAL,
    ASSIGN,
    DOT,
    PERCENT,
    COMMA,
    COLON,
    SEMICOLON,
    LSBRA,
    RSBRA,
    LPAR,
    RPAR,
    REAL_LIT,
    INT_LIT,
    SQUOTE,
    DQUOTE,
    LBRA,
    RBRA,
    STRING_LIT,
    E_O_F,
    UNKNOWN
};

std::string tokToLex(TOKEN_TYPE t) {
    switch (t) {
        case TOKEN_TYPE::EOL:
            return "EOL";
        case TOKEN_TYPE::INT:
            return "int";
        case TOKEN_TYPE::REAL:
            return "real";
        case TOKEN_TYPE::BOOL:
            return "bool";
        case TOKEN_TYPE::CHAR:
            return "char";
        case TOKEN_TYPE::STRING:
            return "string";
        case TOKEN_TYPE::SET:
            return "set";
        case TOKEN_TYPE::ARRAY:
            return "array";
        case TOKEN_TYPE::IF:
            return "if";
        case TOKEN_TYPE::ELSE:
            return "else";
        case TOKEN_TYPE::FOR:
            return "for";
        case TOKEN_TYPE::WHILE:
            return "while";
        case TOKEN_TYPE::IN:
            return "in";
        case TOKEN_TYPE::RETURN:
            return "return";
        case TOKEN_TYPE::BOOL_LIT:
            return "bool";
        case TOKEN_TYPE::MAIN:
            return "main";
        case TOKEN_TYPE::IDENT:
            return "ident";
        case TOKEN_TYPE::DEQ:
            return "/=";
        case TOKEN_TYPE::INTERSECT:
            return "/\\";
        case TOKEN_TYPE::DIV:
            return "/";
        case TOKEN_TYPE::UNION:
            return "\\/";
        case TOKEN_TYPE::BACKSLASH:
            return "\\";
        case TOKEN_TYPE::EXP:
            return "**";
        case TOKEN_TYPE::TEQ:
            return "*=";
        case TOKEN_TYPE::TIMES:
            return "*";
        case TOKEN_TYPE::PEQ:
            return "+=";
        case TOKEN_TYPE::INCREMENT:
            return "++";
        case TOKEN_TYPE::ADD:
            return "+";
        case TOKEN_TYPE::MEQ:
            return "-=";
        case TOKEN_TYPE::DECREMENT:
            return "--";
        case TOKEN_TYPE::MINUS:
            return "-";
        case TOKEN_TYPE::GEQ:
            return ">=";
        case TOKEN_TYPE::GT:
            return ">";
        case TOKEN_TYPE::EQUIV:
            return "<==>";
        case TOKEN_TYPE::LEQ:
            return "<=";
        case TOKEN_TYPE::LT:
            return "<";
        case TOKEN_TYPE::IMPLIES:
            return "==>";
        case TOKEN_TYPE::EQUAL:
            return "==";
        case TOKEN_TYPE::ASSIGN:
            return "=";
        case TOKEN_TYPE::DOT:
            return ".";
        case TOKEN_TYPE::PERCENT:
            return "%";
        case TOKEN_TYPE::COMMA:
            return ",";
        case TOKEN_TYPE::COLON:
            return ":";
        case TOKEN_TYPE::SEMICOLON:
            return ";";
        case TOKEN_TYPE::LPAR:
            return "(";
        case TOKEN_TYPE::RPAR:
            return ")";
        case TOKEN_TYPE::REAL_LIT:
            return "real_lit";
        case TOKEN_TYPE::INT_LIT:
            return "int_lit";
        case TOKEN_TYPE::SQUOTE:
            return "'";
        case TOKEN_TYPE::DQUOTE:
            return "\"";
        case TOKEN_TYPE::LBRA:
            return "{";
        case TOKEN_TYPE::RBRA:
            return "}";
        case TOKEN_TYPE::LSBRA:
            return "[";
        case TOKEN_TYPE::RSBRA:
            return "]";
        case TOKEN_TYPE::STRING_LIT:
            return "string_lit";
        case TOKEN_TYPE::E_O_F:
            return "EOF";
        case TOKEN_TYPE::UNKNOWN:
            return "UNKNOWN";

        default:
            return "INVALID";
    }
}

#endif