#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "types.h"
#include "ast.h"
#include "parser.h"
#include "builtin.cpp"
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
            ** determine vset on vsigs
            ** {bear() : : bear.a in A /\ bear.b in B}
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