#include <iostream>
#include <vector>
#include "builtin.h"
#include "parser.h"
#include "ast.h"
#include "types.h"

using namespace std;

class op {
    private:
        int argc = 0;
        string lexeme;
        shared_ptr<type> returntype;
        vector<shared_ptr<type>> optypes = {};
    public:
        op(string l, shared_ptr<type> t, vector<shared_ptr<type>>& os) {
            lexeme = l;
            returntype = t;
            argc = optypes.size();
            optypes = os;
        };
        string getStringSig() {
            string s = returntype->getIdent() + "-" + lexeme;
            for (shared_ptr<type> t : optypes) {
                s += "-" + t->getIdent();
            };
            return s;
        };

};

class domain {
    /*
    ** for a in A for b in B
    **
    */
    private:
        unique_ptr<vector<pair<string,string>>> dom;
    public:
        domain(unique_ptr<vector<pair<string,string>>>& d) {
            dom = move(d);
        };
};

class polarset {
    /*
    ** polarset is a baseclass for the different types of set representations in polarbear.
    ** polarset is not affiliated with the type system.
    ** Syntactic set formats:
    ** (1) {a,b,c}
    ** (2) {<expr> : <domain> : <expr<bool>>}
    ** Type 1 cannot be infinite, but type 2 can be either infinite or finite depending entirely on the finiteness of the domain.
    **
    ** Semantically, there are 3 types of polar set
    ** (1) Nonabstract finite set:
    **       Iff the set is of syntactic form 1.
    ** (2) Abstract finite set:
    **       A finite set of syntactic form 2.
    ** (3) Abstract infinite set:
    **       An infinite set of syntactic form 2: A set whose domain must be infinite and whose constraints turn the domain finite.
    **
    ** Checking membership for type 1 semantic sets is O(1).
    ** Checking membershup for abstract sets is O(f(x)) where f(x) = time complexity of the constraints.
    ** iterating through any abstract set will always be an infinite process, since the next method on an abstract set will constantly pseudo-randomly select from domain and check if it satisfies constraint until it does, at which point it returns this element from next 
    */
    public:
        polarset() {};
};

class nonabstractpolarset : public polarset {
    /*
    ** Nonabstract set:
    ** {a,b,c}
    */
    private:
        unique_ptr<unordered_set<expr>> elems;
    public:
        nonabstractpolarset(unique_ptr<unordered_set<expr>>& s) {
            elems = move(s);
        };
};

class abstractpolarset : public polarset {
    /*
    ** Abstract set:
    ** {<expr> : <domain> : <expr<bool>>}
    */
    private:
        unique_ptr<expr> production;
        unique_ptr<domain> dom;
        unique_ptr<expr> condition;
    public:
        abstractpolarset(unique_ptr<expr>& p, unique_ptr<domain>& d, unique_ptr<expr>& c) {
            production = move(p);
            dom = move(d);
            condition = move(c);
        };
};

