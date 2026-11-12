#ifndef EXPRINTER_PARSER_HPP
#define EXPRINTER_PARSER_HPP

#include <string>

#include "Expr.hpp"
#include "Statements.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"

class Parser {
public:
    explicit Parser(Tokenizer &tokenizer) : tokenizer{tokenizer} {}

    Statements *program();
    Statements *statements();
    Statement *statement();
    AssignmentStatement *assignmentStatement();

    // Part 3:
    // added: a print statement (after doing expr.cpp and statements.cpp)
    PrintStatement *printStatement(); 

    // Part 4:
    // Added
    ForStatement *forStatement();

    // Phase 2: Added the Suites --> reads indented lines under a loop
    Statements *suite();

    // Phase 2:
    // Added: range ( <range-arguments> )
    RangeExpression *rangeExpression();

    // Phase 2:
    // Added: 1, 2, or 3 expressions inside range( )
    RangeExpression *rangeArguments();
    
    ExprNode *relExpr();
    ExprNode *relTerm();
    ExprNode *relPrimary();
    ExprNode *arithExpr();
    ExprNode *arithTerm();
    ExprNode *arithPrimary();
    ExprNode *arithAtom();

    // Phase 3 - <literal> -> integer | double | boolean | string
    ExprNode *literal();

    // Phase 3 - <string-conversion> -> "str" ( <rel-expr> )
    ExprNode *stringConversion();

private:
    Tokenizer &tokenizer;

    [[noreturn]] void die(const std::string &where,
                          const std::string &message,
                          const Token &token) const;
};

#endif // EXPRINTER_PARSER_HPP