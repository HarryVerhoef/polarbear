#include <unordered_map>
#include <set>
#include <unordered_set>
#include "ast.h"
using namespace std;

class type {
    private:
        static unordered_map<string, shared_ptr<type>> typemap;
        static unordered_set<string> typenames;
        string ident = "";
        int length = 0;
        set<string> vset = {};
        vector<funcsig> funcsigs = {};
        vector<varsig> varsigs = {};
        vector<opsig> opsigs = {};

    public:
        type() {};
        type(string i) { this->setType(i); };
        void setType(string& i) {
            typemap[i] = make_shared<type>(*this);
            typenames.insert(i);
        };
        shared_ptr<type> getType(string s) { return typemap[s]; };
        set<string> getVset() { return vset; };
        string getIdent() { return ident; };
        void setIdent(string& i) { ident = i; };
        void setVset(set<string>& s) { vset = s; };
        void addFuncsig(funcsig& f) { funcsigs.push_back(f); };
        void addVarsig(varsig& v) { varsigs.push_back(v); };
        void addOpsig(opsig& o) { opsigs.push_back(o); };
        bool hasType(string i) { return typenames.find(i) != typenames.end(); };
};

class stype : public type {
    public:
        stype(string& i, unique_ptr<polarset> vs) {
            this->setIdent(i);
            this->setVset(vs);
            this->setType(i);
        }
};

class ctype : public type {
    private:
        type supertype;
        unique_ptr<vector<unique_ptr<complexblock>>> complexes;
    public:
        ctype(string& i, unique_ptr<vector<unique_ptr<complexblock>>>& cs) {
            this->setIdent(i);
            this->setType(i);
            /*
            ** iterate through complexes adding var and func sigs
            ** if come to a constructor then use to construct vset along with varsigs?
            */
        };
};

/*
** built-in types:
** container
** iterator
** map
** set
** tuple
** array
** ...
*/

class sig {
    private:
        shared_ptr<type> t;
        string ident = "";
        ACCESS access = ACCESS::PRIVATE;
    public:
        sig() {};
        shared_ptr<type> getType() { return t; };
        string getIdent() { return ident; };
        ACCESS getAccess() { return access; };
        void setType(shared_ptr<type> o) { t = o; };
        void setIdent(string& s) { ident = s; };
        void setAccess(ACCESS a) { access = a; };
};

class varsig : public sig {
    public:
        varsig(shared_ptr<type> vartype, string varident, ACCESS varaccess) {
            this->setType(vartype);
            this->setIdent(varident);
            this->setAccess(varaccess);
        };
        varsig(shared_ptr<type> vartype, string varident) {
            this->setType(vartype);
            this->setIdent(varident);
        };
};

class funcsig : public sig {
    private:
        vector<varsig> p = {};
    public:
        funcsig(vector<varsig>& funcparams, string funcident, ACCESS funcaccess, shared_ptr<type> functype) {
            this->setFuncParams(funcparams);
            this->setType(functype);
            this->setIdent(funcident);
            this->setAccess(funcaccess);
        };
        funcsig(string funcident, ACCESS funcaccess, shared_ptr<type> functype) {
            this->setType(functype);
            this->setIdent(funcident);
            this->setAccess(funcaccess);
        };
        void setFuncParams(vector<varsig>& funcparams) { p = funcparams; };
        vector<varsig> getFuncParams() { return p; };
};

class opsig : public sig {
    private:
        vector<varsig> p = {};
    public:
        opsig(vector<varsig>& funcparams, string funcident, ACCESS funcaccess, shared_ptr<type> functype) {
            this->setOpParams(funcparams);
            this->setType(functype);
            this->setIdent(funcident);
            this->setAccess(funcaccess);
        };
        opsig(string funcident, ACCESS funcaccess, shared_ptr<type> functype) {
            this->setType(functype);
            this->setIdent(funcident);
            this->setAccess(funcaccess);
        };
        void setOpParams(vector<varsig>& funcparams) { p = funcparams; };
        vector<varsig> getOpParams() { return p; };
};

/*
** What functions does a container have?
** - length()
** - add()
** - remove()
** -
*/
class pcontainer : public type {
    public:
        pcontainer() { /* Typed container constructor */
            funcsig length = funcsig("length", ACCESS::PUBLIC, this->getType("int"));
            this->addFuncsig(length);
        };
};

// varsig addelem = varsig(make_shared<type>(t), "e");
//             vector<varsig> addparams = {addelem};
//             funcsig add = funcsig(addparams, "add", ACCESS::PUBLIC, make_shared<type>(*this)); /* Returns a shared pointer to the type of that invoked */
//             this->addFuncsig(add);
            
//             varsig removeelem = varsig(make_shared<type>(t), "e");
//             vector<varsig> removeparams = {removeelem};
//             funcsig remove = funcsig(removeparams, "add", ACCESS::PUBLIC, make_shared<type>(*this)); /* Returns a shared pointer to the type of that invoked */
//             this->addFuncsig(remove);

/*
** iterator methods:
** next
** get(int index)
** []
*/
class piterator : public pcontainer {
    public:
        piterator(type& t) { /* Typed iterator constructor (array) */
            funcsig next = funcsig("next", ACCESS::PUBLIC, make_shared<type>(t));
            this->addFuncsig(next);
            funcsig get = funcsig("get", ACCESS::PUBLIC, make_shared<type>(t));
            this->addFuncsig(get);

            varsig index = varsig(this->getType("int"), "i");
            vector<varsig> operands = {index};
            opsig sqrbraget = opsig(operands, "[]", ACCESS::PUBLIC, make_shared<type>(t));
            this->addOpsig(sqrbraget);
        };
        piterator() { /* untyped iterator constructor (set) */
            funcsig next = funcsig("next", ACCESS::PUBLIC, this->getType("type"));
            this->addFuncsig(next);
            funcsig get = funcsig("get", ACCESS::PUBLIC, this->getType("type"));
            this->addFuncsig(get);

            varsig index = varsig(this->getType("int"), "i");
            vector<varsig> operands = {index};
            opsig sqrbraget = opsig(operands, "[]", ACCESS::PUBLIC, this->getType("type"));
            this->addOpsig(sqrbraget);
        };
};

class op {
    private:
        int argc = 0;
        string lexeme;
        vector<type> optypes = {};
        TOKEN_TYPE default_op = TOKEN_TYPE::UNKNOWN;
    public:
        op(string l, vector<type>& os) {
            lexeme = l;
            argc = optypes.size();
            optypes = os;
        };
        op(TOKEN_TYPE t) {
            default_op = t;
        };
};

/*
** set methods:
** union
** intersection
** difference
** cartesian product
** in
** within
*/
class pset : public piterator {
    public:
        pset() {
            /* union */
            varsig set2_union = varsig(make_shared<type>(*this), "s");
            vector<varsig> unionparams = {set2_union};
            funcsig union_func = funcsig(unionparams, "union", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addFuncsig(union_func);

            varsig set2_union_op = varsig(make_shared<type>(*this), "s");
            vector<varsig> unionoperands = {set2_union_op};
            opsig union_op = opsig(unionoperands, "\\/", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addOpsig(union_op);

            /* intersection */
            varsig set2_intersection = varsig(make_shared<type>(*this), "s");
            vector<varsig> intersectionparams = {set2_intersection};
            funcsig intersection_func = funcsig(intersectionparams, "intersection", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addFuncsig(intersection_func);

            varsig set2_intersection_op = varsig(make_shared<type>(*this), "s");
            vector<varsig> intersectionoperands = {set2_intersection_op};
            opsig intersection_op = opsig(intersectionoperands, "/\\", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addOpsig(intersection_op);

            /* difference */
            varsig set2_difference = varsig(make_shared<type>(*this), "s");
            vector<varsig> differenceparams = {set2_difference};
            funcsig difference_func = funcsig(differenceparams, "difference", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addFuncsig(difference_func);

            varsig set2_difference_op = varsig(make_shared<type>(*this), "s");
            vector<varsig> differenceoperands = {set2_difference_op};
            opsig difference_op = opsig(differenceoperands, "-", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addOpsig(difference_op);

            /* cartesian product */
            varsig set2_product = varsig(make_shared<type>(*this), "s");
            vector<varsig> productparams = {set2_product};
            funcsig product_func = funcsig(productparams, "product", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addFuncsig(product_func);

            varsig set2_product_op = varsig(make_shared<type>(*this), "s");
            vector<varsig> productoperands = {set2_product_op};
            opsig product_op = opsig(productoperands, "*", ACCESS::PUBLIC, make_shared<type>(*this));
            this->addOpsig(product_op);

            /* in */
            varsig elem = varsig(this->getType("type"), "e");
            vector<varsig> inparams = {elem};
            funcsig in_func = funcsig(inparams, "in", ACCESS::PUBLIC, this->getType("bool"));
            this->addFuncsig(in_func);

            varsig elem_op = varsig(this->getType("type"), "e");
            vector<varsig> inoperands = {elem_op};
            opsig in_op = opsig(inoperands, "in", ACCESS::PUBLIC, this->getType("bool"));
            this->addOpsig(in_op);

            /* within */
            varsig set2_within = varsig(make_shared<type>(*this), "s");
            vector<varsig> withinparams = {set2_within};
            funcsig within_func = funcsig(withinparams, "within", ACCESS::PUBLIC, this->getType("bool"));
            this->addFuncsig(within_func);

            varsig set2_within_op = varsig(make_shared<type>(*this), "s");
            vector<varsig> withinoperands = {set2_within_op};
            opsig within_op = opsig(withinoperands, "within", ACCESS::PUBLIC, this->getType("bool"));
            this->addOpsig(within_op);

        };
};


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
        unique_ptr<vector<unique_ptr<def>>> defs = {};
    public:
        program() {
            this->setType(AST_TYPE::PROGRAM);
        }
        program(unique_ptr<vector<unique_ptr<def>>>& d) {
            this->setType(AST_TYPE::PROGRAM);
            defs = move(d);
        };
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
        unique_ptr<vector<unique_ptr<variable>>> ps = {};
    public:
        params(unique_ptr<vector<unique_ptr<variable>>>& p) {
            this->setType(AST_TYPE::PARAMS);
            ps = move(p);
        };
};

class variable : public def {
    private:
        unique_ptr<type> vtype;
        string ident = "";
    public:
        variable(unique_ptr<type>& t, string& i) {
            this->setType(AST_TYPE::VARIABLE);
            vtype = move(t);
            ident = i;
        };
};

class complexblock : public def {
    private:
        ACCESS access = ACCESS::PRIVATE;
        unique_ptr<vector<unique_ptr<def>>> defs = {};
    public:
        complexblock(ACCESS a, unique_ptr<vector<unique_ptr<def>>>& d) {
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
        simpletdef(string i, unique_ptr<polarset>& s) {
            type simple = stype(i, move(p));
            this->setIdent(i);
            this->setNewType(simple.getType(i));
        };
};

class complextdef : public tdef {
    public:
        complextdef(string i, unique_ptr<vector<unique_ptr<complexblock>>>& cs) {
            type complex = ctype(i, move(cs));
            this->setIdent(i);
            this->setNewType(complex.getType(i));
        };
};

class block : public program {
    private:
        unique_ptr<vector<stmt>> stmts = {};
    public:
        block(unique_ptr<vector<stmt>>& ss) {
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
        unique_ptr<expr> cond;
        unique_ptr<block> elseblock;
        unique_ptr<elsestmt> estmt;
    public:
        elsestmt(unique_ptr<block>& b) {
            this->setType(AST_TYPE::ELSESTMT);
            elseblock = move(b);
        };
        elsestmt(unique_ptr<expr>& c, unique_ptr<block>& b) {
            this->setType(AST_TYPE::ELSESTMT);
            cond = move(c);
            elseblock = move(b);
        };
        elsestmt(unique_ptr<expr>& c, unique_ptr<block>& b, unique_ptr<elsestmt>& e) {
            this->setType(AST_TYPE::ELSESTMT);
            cond = move(c);
            elseblock = move(b);
            estmt = move(e);
        };
};

class forstmt : public stmt {
    public:
        forstmt() {
            this->setType(AST_TYPE::FORSTMT);
        }
};

class foreach : public forstmt {
    private:
        string ident = "";
        unique_ptr<expr> iterable;
        unique_ptr<block> forblock;
    public:
        foreach(string& i, unique_ptr<expr>& c, unique_ptr<block>& b) {
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
        unique_ptr<op> operation;
    public:
        binop(unique_ptr<expr>& a1, unique_ptr<expr>& a2, unique_ptr<op>& o) {
            this->setType(AST_TYPE::BINOP);
            operand1 = move(a1);
            operand2 = move(a2);
            operation = move(o);
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