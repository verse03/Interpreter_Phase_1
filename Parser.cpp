#include <cstdlib>
#include <iostream>
#include <string>

#include "Parser.hpp"

void Parser::die(const std::string &where,
                 const std::string &message,
                 const Token &token) const {
    std::cerr << where << ": " << message << " at line " << token.lineNumber()
              << ", column " << token.columnNumber() << ". Got: ";
    token.print(std::cerr);
    std::cerr << "\n\nTokens identified up to this point:\n";
    tokenizer.printProcessedTokens(std::cerr);
    std::exit(EXIT_FAILURE);
}

Statements *Parser::program() {
    // <program> -> <statements> EOF
    Statements *parsedStatements = statements();
    Token eof = tokenizer.getToken();
    if (!eof.isEof()) {
        delete parsedStatements;
        die("Parser::program", "expected EOF", eof);
    }
    return parsedStatements;
}

Statements *Parser::statements() {
    // <statements> -> <statement> NEWLINE { <statement> NEWLINE }
    auto *parsedStatements = new Statements();
    parsedStatements->addStatement(statement());

    Token newline = tokenizer.getToken();
    if (!newline.isNewline()) {
        delete parsedStatements;
        die("Parser::statements", "expected NEWLINE after statement", newline);
    }

    Token next = tokenizer.getToken();
    while (next.isIdentifier() || next.isKeyword()) {
        tokenizer.ungetToken();
        parsedStatements->addStatement(statement());

        newline = tokenizer.getToken();
        if (!newline.isNewline()) {
            delete parsedStatements;
            die("Parser::statements", "expected NEWLINE after statement", newline);
        }

        next = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return parsedStatements;
}

Statement *Parser::statement() {
    // <statement> -> <for-statement>
    //             | <assignment-statement>
    //             | <print-statement>
    Token token = tokenizer.getToken();

    if (token.isIdentifier()) {
        tokenizer.ungetToken();
        return assignmentStatement();
    }
    
    //Part 4:
    // Changed: this used to die, now it hands off to forStatement()
    // ungetToken() puts for back so forStatement() can read itself

    // if (token.isForKeyword())
    //    die("Parser::statement", "for-statements are not implemented in the base interpreter", token);

    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return forStatement();
    }

    // Part 3:
    // Changed: this used to die. Now it hands off to printStatement()
    // ungetToken () puts print back so printStatement() can read itself
    if (token.isPrintKeyword()) {
        tokenizer.ungetToken();
        return printStatement();
    }
        

    die("Parser::statement", "expected a statement", token);
}

AssignmentStatement *Parser::assignmentStatement() {
    // <assignment-statement> -> <id> = <rel-expr>
    // The caller consumes the context-dependent terminator: NEWLINE in a
    // statement list or ';' in a future for-loop header.
    Token variable = tokenizer.getToken();
    if (!variable.isIdentifier())
        die("Parser::assignmentStatement", "expected an identifier", variable);

    Token assignmentOperator = tokenizer.getToken();
    if (!assignmentOperator.isAssignmentOperator())
        die("Parser::assignmentStatement", "expected '='", assignmentOperator);

    return new AssignmentStatement(variable.identifier(), relExpr());
}

// Part 3
// Added: a print statement, built the same way as the assignment statement (after doing expr.cpp and statements.cpp)
PrintStatement *Parser::printStatement() {

    // <print-statement> -> print <rel-expr>
    Token printKeyword = tokenizer.getToken();
    if (!printKeyword.isPrintKeyword())
        die("Parser::printStatement", "expected 'print'", printKeyword);

    return new PrintStatement(relExpr());
}

    // ADDED (Part 4): reads each piece of the loop in order, left to right.
ForStatement *Parser::forStatement() {
    // for ( int a = 0; i < 10 ; i++ ) { statements }

    // the word "for"
    Token forKeyword = tokenizer.getToken();
    if (!forKeyword.isForKeyword())
        die("Parser::forStatement", "expected 'for'", forKeyword);

    // (
    Token openParen = tokenizer.getToken();
    if (!openParen.isOpenParen())
        die("Parser::forStatement", "expected '('", openParen);

    // the start, e.g. i = 0
    AssignmentStatement *initialization = assignmentStatement();

    // first ;
    Token firstSemicolon = tokenizer.getToken();
    if (!firstSemicolon.isSemicolon())
        die("Parser::forStatement", "expected ';'", firstSemicolon);

    // the check, e.g. i < 4
    ExprNode *condition = relExpr();

    // second ;
    Token secondSemicolon = tokenizer.getToken();
    if (!secondSemicolon.isSemicolon())
        die("Parser::forStatement", "expected ';'", secondSemicolon);

    // the step, e.g. i = i + 1
    AssignmentStatement *update = assignmentStatement();

    // )
    Token closeParen = tokenizer.getToken();
    if (!closeParen.isCloseParen())
        die("Parser::forStatement", "expected ')'", closeParen);

    // {
    Token openBrace = tokenizer.getToken();
    if (!openBrace.isOpenBracket())
        die("Parser::forStatement", "expected '{'", openBrace);

    // the { must be followed by a NEWLINE
    Token newline = tokenizer.getToken();
    if (!newline.isNewline())
        die("Parser::forStatement", "expected NEWLINE after '{'", newline);

    // the lines inside { }. statements() needs at least one,
    // so an empty loop body is rejected automatically.
    Statements *loopBody = statements();

    // }
    Token closeBrace = tokenizer.getToken();
    if (!closeBrace.isClosedBracket())
        die("Parser::forStatement", "expected '}'", closeBrace);

    return new ForStatement(initialization, condition, update, loopBody);
}


ExprNode *Parser::relExpr() {
    // <rel-expr> -> <rel-term> [ <equality-op> <rel-term> ]
    // The optional equality operation is left for students to implement.

    // Part 2:
    // Added: the optional equality operation
    ExprNode *left = relTerm();
    Token token = tokenizer.getToken();

    // Using "if" instead of "while" because the grammar allows AT MOST ONE
    // equality opernator here. using while would wrongly accept "a == b == c" as valid, which is not allowed by the grammar.
    if (token.isEqualityOperator() || token.isNotEqualOperator()) {
        ExprNode *right = relTerm();
        left = new BinaryExprNode(token, left, right);
        return left;
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::relTerm() {
    // <rel-term> -> <rel-primary> [ <ordering-op> <rel-primary> ]
    // The optional ordering operation is left for students to implement.

    // Added: the optional ordering operation
    ExprNode *left = relPrimary();
    Token token = tokenizer.getToken();

    // Using "if" instead of "while" because the grammar allows AT MOST ONE
    if (token.isLessThanOperator() || token.isLessThanOrEqualOperator() ||
        token.isGreaterThanOperator() || token.isGreaterThanOrEqualOperator()) {
        ExprNode *right = relPrimary();
        left = new BinaryExprNode(token, left, right);
        return left;
    }
    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::relPrimary() {
    // <rel-primary> -> <arith-expr>
    return arithExpr();
}

ExprNode *Parser::arithExpr() {
    // <arith-expr> -> <arith-term> { <add-op> <arith-term> }
    ExprNode *left = arithTerm();
    Token token = tokenizer.getToken();

    while (token.isAdditionOperator() || token.isSubtractionOperator()) {
        ExprNode *right = arithTerm();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithTerm() {
    // <arith-term> -> <arith-primary> { <mult-op> <arith-primary> }
    ExprNode *left = arithPrimary();
    Token token = tokenizer.getToken();

    while (token.isMultiplicationOperator() ||
           token.isDivisionOperator() ||
           token.isModuloOperator()) {
        ExprNode *right = arithPrimary();
        left = new BinaryExprNode(token, left, right);
        token = tokenizer.getToken();
    }

    tokenizer.ungetToken();
    return left;
}

ExprNode *Parser::arithPrimary() {
    // <arith-primary> -> [ <sign> ] <arith-atom>
    Token token = tokenizer.getToken();
    if (token.isAdditionOperator() || token.isSubtractionOperator())
        return new UnaryExprNode(token, arithAtom());

    tokenizer.ungetToken();
    return arithAtom();
}

ExprNode *Parser::arithAtom() {
    // <arith-atom> -> <id> | <integer> | '(' <rel-expr> ')'
    Token token = tokenizer.getToken();

    if (token.isInteger())
        return new IntegerLiteral(token);
    if (token.isIdentifier())
        return new Variable(token);
    if (token.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closeParen = tokenizer.getToken();
        if (!closeParen.isCloseParen())
            die("Parser::arithAtom", "expected ')'", closeParen);
        return expression;
    }

    die("Parser::arithAtom", "expected an identifier, integer, or '('", token);
}
