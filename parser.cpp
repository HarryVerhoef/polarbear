#include <iostream>;

using namespace std;

/*
** lexer: 
*/



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