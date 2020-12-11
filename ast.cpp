#include <unordered_map>
#include <set>
#include <unordered_set>
#include "ast.h"
#include "builtin.h"
#include "types.h"
#include "types.cpp"

using namespace std;

class astnode {
    private:
        AST_TYPE astType = AST_TYPE::ASTNODE;
    public:
        astnode() {};
        void setType(AST_TYPE t) {
            astType = t;
        };
        AST_TYPE getType() {
            return astType;
        };
};

class program : public astnode {
    private:
        unordered_map<string, shared_ptr<op>> opmap = {};
        unique_ptr<vector<unique_ptr<def>>> defs = {};
    public:
        program() {
            this->setType(AST_TYPE::PROGRAM);
        }
        program(unique_ptr<vector<unique_ptr<def>>>& d) {
            this->setType(AST_TYPE::PROGRAM);
            defs = move(d);
        };
        void addOp(op& o) {
            opmap[o.getStringSig()] = make_shared<op>(o);
        };
        shared_ptr<op> getOp(string s) { return opmap[s]; };
};

class def : public program {
    public:
        def() {
            this->setType(AST_TYPE::DEF);
        };
};

class functionast : public def {
    private:
        shared_ptr<type> functype;
        string ident = "";
        unique_ptr<params> fparams;
        unique_ptr<block> fblock;
    public:
        functionast(shared_ptr<type> t, string i, unique_ptr<params>& p, unique_ptr<block>& b) {
            this->setType(AST_TYPE::FUNCTION);
            functype = t;
            ident = move(i);
            fparams = move(p);
            fblock = move(b);
        };
};

class params : public program {
    private:
        unique_ptr<vector<variable>> ps = {};
    public:
        params(unique_ptr<vector<variable>>& p) {
            this->setType(AST_TYPE::PARAMS);
            ps = move(p);
        };
};

class variable : public def {
    private:
        shared_ptr<type> vtype;
        string ident = "";
    public:
        variable(shared_ptr<type>& t, string& i) {
            this->setType(AST_TYPE::VARIABLE);
            vtype = move(t);
            ident = i;
        };
};

class complexblock : public def {
    private:
        ACCESS access = ACCESS::PRIVATE;
        unique_ptr<vector<unique_ptr<tmember>>> members = {};
        unique_ptr<vector<unique_ptr<tconstructor>>> constructors = {};
    public:
        complexblock(ACCESS a, unique_ptr<vector<unique_ptr<tmember>>>& d) {
            access = a;
            defs = move(d);
        };
};

class tdef : public def {
    private:
        string ident = "";
        shared_ptr<type> newtype;
    public:
        tdef() {};
        void setIdent(string i) { ident = i; };
        void setNewType(shared_ptr<type> t) { newtype = t; };
        string getIdent() { return ident; };
        shared_ptr<type> getNewType() { return newtype; };
};

class simpletdef : public tdef {
    public:
        simpletdef(string i, shared_ptr<polarset>& s) {
            type simple = simpletype(i, s);
            this->setIdent(i);
            this->setNewType(simple.getType(i));
        };
};

class complextdef : public tdef {
    private:
        shared_ptr<type> supertype;
    public:
        complextdef(string i, unique_ptr<vector<unique_ptr<complexblock>>>& cs) {
            type complex = complextype(i, cs);
            this->setIdent(i);
            this->setNewType(complex.getType(i));
        };
        complextdef(string i, shared_ptr<type> s, unique_ptr<vector<unique_ptr<complexblock>>>& cs) {
            type complex = complextype(i, s, cs);
            supertype = s;
            this->setIdent(i);
            this->setNewType(complex.getType(i));
        };
};

class tconstructor : def {
    private:
        unique_ptr<params> constructorparams;
        unique_ptr<block> constructorblock;
    public:
        tconstructor(unique_ptr<params>& p, unique_ptr<block>& b) {
            constructorparams = move(p);
            constructorblock = move(b);
        };
};

class tmember : def {
    private:
        string memberident = "";
        shared_ptr<type> membertype;
    public:
        tmember() {};
        string getIdent() { return memberident; };
        shared_ptr<type> getType() { return membertype; };
        void setIdent(string& i) { memberident = i; };
        void setType(shared_ptr<type> t) { membertype = t; };
};

class tmembervar : tmember {
    private:
        unique_ptr<expr> memberexpr;
    public:
        tmembervar() {};
        tmembervar(string i, shared_ptr<type> t, unique_ptr<expr>& e) {
            this->setIdent(i);
            this->setType(t);
            memberexpr = move(e);
        };
};

class tmemberfunc : tmember {
    private:
        unique_ptr<params> memberparams;
        unique_ptr<block> memberblock;
    public:
        tmemberfunc() {};
        tmemberfunc(string i, shared_ptr<type> t, unique_ptr<params>& p, unique_ptr<block>& b) {
            this->setIdent(i);
            this->setType(t);
            memberparams = move(p);
            memberblock = move(b);
        };
};


class block : public program {
    private:
        vector<unique_ptr<stmt>> stmts = {};
    public:
        block(vector<unique_ptr<stmt>>& ss) {
            this->setType(AST_TYPE::BLOCK);
            stmts = move(ss);
        };
};

class stmt : public program {
    public:
        stmt() {
            this->setType(AST_TYPE::STMT);
        };
};

class ifstmt : public stmt {
    private:
        unique_ptr<expr> cond;
        unique_ptr<block> ifblock;
        unique_ptr<elsestmt> estmt;
    public:
        ifstmt(unique_ptr<expr>& c, unique_ptr<block>& b) {
            this->setType(AST_TYPE::IFSTMT);
            cond = move(c);
            ifblock = move(b);
        };
        ifstmt(unique_ptr<expr>& c, unique_ptr<block>& b, unique_ptr<elsestmt>& e) {
            this->setType(AST_TYPE::IFSTMT);
            cond = move(c);
            ifblock = move(b);
            estmt = move(e);
        };
};

class elsestmt : public stmt {
    private:
        unique_ptr<block> elseblock;
        unique_ptr<ifstmt> istmt;
    public:
        elsestmt(unique_ptr<block>& b) {
            this->setType(AST_TYPE::ELSESTMT);
            elseblock = move(b);
        };
        elsestmt(unique_ptr<ifstmt>& i) {
            this->setType(AST_TYPE::ELSESTMT);
            istmt = move(i);
        };

};

class forstmt : public stmt {
    private:
        unique_ptr<expr>
    public:
        forstmt() {
            this->setType(AST_TYPE::FORSTMT);
        };
};

class foreach : public forstmt {
    private:
        string ident = "";
        unique_ptr<expr> iterable;
        unique_ptr<block> forblock;
    public:
        foreach(string i, unique_ptr<expr>& c, unique_ptr<block>& b) {
            this->setType(AST_TYPE::FOREACH);
            ident = i;
            iterable = move(c);
            forblock = move(b);
        };
};

class forindex : public forstmt {
    private:
        unique_ptr<expr> forexpr1;
        unique_ptr<expr> forcond;
        unique_ptr<expr> forexpr2;
        unique_ptr<block> forblock;
    public:
        forindex(unique_ptr<expr>& e1, unique_ptr<expr>& c, unique_ptr<expr>& e2, unique_ptr<block>& b) {
            this->setType(AST_TYPE::FORINDEX);
            forexpr1 = move(e1);
            forcond = move(c);
            forexpr2 = move(e2);
            forblock = move(b);
        };
};

class whilestmt : public stmt {
    private:
        unique_ptr<expr> whilecond;
        unique_ptr<block> whileblock;
    public:
        whilestmt(unique_ptr<expr>& c, unique_ptr<block>& b) {
            this->setType(AST_TYPE::WHILESTMT);
            whilecond = move(c);
            whileblock = move(b);
        };
};

class expr : public stmt {
    public:
        expr() {
            this->setType(AST_TYPE::EXPR);
        };
};

class binop : public expr {
    private:
        unique_ptr<expr> operand1;
        unique_ptr<expr> operand2;
        shared_ptr<op> operation;
    public:
        binop(unique_ptr<expr>& a1, unique_ptr<expr>& a2, shared_ptr<op> o) {
            this->setType(AST_TYPE::BINOP);
            operand1 = move(a1);
            operand2 = move(a2);
            operation = o;
        };
};

class unop : public expr {
    private:
        unique_ptr<expr> operand;
        unique_ptr<op> operation;
    public:
        unop(unique_ptr<expr>& a, unique_ptr<op>& o) {
            this->setType(AST_TYPE::UNOP);
            operand = move(a);
            operation = move(o);
        };
};

class call : public expr {
    private:
        string ident = "";
        unique_ptr<vector<expr>> args;
    public:
        call(string& i, unique_ptr<vector<expr>>& as) {
            this->setType(AST_TYPE::CALL);
            ident = i;
            args = move(as);
        };
};

class int_lit : public expr {
    private:
        string strval = "";
    public:
        int_lit(string v) {
            this->setType(AST_TYPE::INT_LIT);
            strval = v;
        };
};

class real_lit : public expr {
    private:
        string left = "";
        string right = "";
    public:
        real_lit(string& l, string& r) {
            this->setType(AST_TYPE::REAL_LIT);
            left = l;
            right = r;
        };
};

class string_lit : public expr {
    private:
        unique_ptr<vector<string>> stringparts;
        unique_ptr<vector<expr>> exprs;
        unique_ptr<vector<int>> exprlocations;
    public:
        string_lit(unique_ptr<vector<string>>& s, unique_ptr<vector<expr>>& e, unique_ptr<vector<int>>& l) {
            this->setType(AST_TYPE::STRING_LIT);
            stringparts = move(s);
            exprs = move(e);
            exprlocations = move(l);
        };
};

class char_lit : public expr {
    private:
        char val = 'c';
    public:
        char_lit(char v) {
            this->setType(AST_TYPE::CHAR_LIT);
            val = v;
        };
};




// An iterator is necessarily a container, but a container is not necessarily an iterator
// Hashmap is a container but not an iterator
// Hashmap.keys() returns a set which can be iterated through