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
    if (token.isForKeyword())
        die("Parser::statement", "for-statements are not implemented in the base interpreter", token);
    if (token.isPrintKeyword())
        die("Parser::statement", "print-statements are not implemented in the base interpreter", token);

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

ExprNode *Parser::relExpr() {
    // <rel-expr> -> <rel-term> [ <equality-op> <rel-term> ]
    // The optional equality operation is left for students to implement.
    return relTerm();
}

ExprNode *Parser::relTerm() {
    // <rel-term> -> <rel-primary> [ <ordering-op> <rel-primary> ]
    // The optional ordering operation is left for students to implement.
    return relPrimary();
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
