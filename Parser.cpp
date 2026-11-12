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
    // Phase 2:
    // Changed: <statements> -> <statement> { <statement> }
    // NEWLINE is no longer checked here. statement() handles it, because
    // only simple statements end with NEWLINE (a for loop does not).
    auto *parsedStatements = new Statements();
    parsedStatements->addStatement(statement());

    // keep reading while the next token can start a statement.
    // stops at DEDENT (end of a suite) or EOF (end of the program).
    Token next = tokenizer.getToken();
    while (next.isIdentifier() || next.isPrintKeyword() || next.isForKeyword()) {
        tokenizer.ungetToken();
        parsedStatements->addStatement(statement());
        next = tokenizer.getToken();
    }

    // put back the DEDENT or EOF so the caller can check it
    tokenizer.ungetToken();
    return parsedStatements;
}

Statement *Parser::statement() {
    // Phase 2:
    // Changed: <statement> -> <simple-statement> NEWLINE
    Token token = tokenizer.getToken();

    // compound statement: a for loop ends with its suite, so no NEWLINE after it
    if (token.isForKeyword()) {
        tokenizer.ungetToken();
        return forStatement();
    }

    // simple statements: assignment or print
    Statement *simpleStatement = nullptr;
    if (token.isIdentifier()) {
        tokenizer.ungetToken();
        simpleStatement = assignmentStatement();
    } else if (token.isPrintKeyword()) {
        tokenizer.ungetToken();
        simpleStatement = printStatement();
    } else {
        die("Parser::statement", "expected a statement", token);
    }

    // every simple statement must end with NEWLINE
    Token newline = tokenizer.getToken();
    if (!newline.isNewline()) {
        delete simpleStatement;
        die("Parser::statement", "expected NEWLINE after statement", newline);
    }

    return simpleStatement;
}

// Phase 2:
// Added: <range> -> range ( <range-arguments> )
RangeExpression *Parser::rangeExpression() {
    // the word "range"
    Token keyword = tokenizer.getToken();
    if (!keyword.isRangeKeyword())
        die("Parser::rangeExpression", "expected 'range'", keyword);

    // (
    Token openParen = tokenizer.getToken();
    if (!openParen.isOpenParen())
        die("Parser::rangeExpression", "expected '('", openParen);

    // what's inside the ( )
    RangeExpression *range = rangeArguments();

    // )
    Token closeParen = tokenizer.getToken();
    if (!closeParen.isCloseParen()) {
        delete range;
        die("Parser::rangeExpression", "expected ')'", closeParen);
    }

    return range;
}

// Phase 2:
// Added: <range-arguments> -> 1, 2, or 3 <rel-expr> separated by commas
RangeExpression *Parser::rangeArguments() {
    ExprNode *first = relExpr();

    // no comma: range(stop)
    Token comma = tokenizer.getToken();
    if (!comma.isComma()) {
        tokenizer.ungetToken();
        return new RangeExpression(first);
    }

    ExprNode *second = relExpr();

    // no second comma: range(start, stop)
    Token secondComma = tokenizer.getToken();
    if (!secondComma.isComma()) {
        tokenizer.ungetToken();
        return new RangeExpression(first, second);
    }

    // range(start, stop, step)
    ExprNode *third = relExpr();
    return new RangeExpression(first, second, third);
}

// Phase 2:
// Added: the indented lines under a for loop
Statements *Parser::suite(){
    // <suite> -> NEWLINE INDENT <statements> DEDENT

    Token newLineToken = tokenizer.getToken();
    if (!newLineToken.isNewline()){
        die("Parser::suite", "expected a NEWLINE", newLineToken);
    }

    // the next line must be indented further
    Token indentToken = tokenizer.getToken();
    if (!indentToken.isIndent()){
        die("Parser::suite", "expected an INDENT", indentToken);
    }

    // the indented lines (at least one)
    Statements *parsedStatements = statements();

    // check for dedent, if no dedent die
    Token dedentToken = tokenizer.getToken();
    if (!dedentToken.isDedent()) {
        delete parsedStatements;
        die("Parser::suite", "expected a DEDENT", dedentToken);
    }

    return parsedStatements;
}



AssignmentStatement *Parser::assignmentStatement() {
    // <assignment-statement> -> <id> = <rel-expr>

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

// Phase 2:
// Changed: Python-style for loop, read left to right
ForStatement *Parser::forStatement() {
    // <for-statement> -> for <id> in <range> : <suite>

    // the word "for"
    Token forKeyword = tokenizer.getToken();
    if (!forKeyword.isForKeyword())
        die("Parser::forStatement", "expected 'for'", forKeyword);

    // the variable name, e.g. i
    Token variable = tokenizer.getToken();
    if (!variable.isIdentifier())
        die("Parser::forStatement", "expected a variable name", variable);

    // the word "in"
    Token inKeyword = tokenizer.getToken();
    if (!inKeyword.isInKeyword())
        die("Parser::forStatement", "expected 'in'", inKeyword);

    // range( ... )
    RangeExpression *range = rangeExpression();

    // :
    Token colon = tokenizer.getToken();
    if (!colon.isColon()) {
        delete range;
        die("Parser::forStatement", "expected ':'", colon);
    }

    // the indented lines -> NEWLINE INDENT statements DEDENT
    Statements *suiteStatements = suite();

    return new ForStatement(variable.identifier(), range, suiteStatements);
}


ExprNode *Parser::relExpr() {
    // <rel-expr> -> <rel-term> [ <equality-op> <rel-term> ]

    // Part 2:
    // Added: the optional equality operation
    ExprNode *left = relTerm();
    Token token = tokenizer.getToken();

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

    // Phase 3 - added // (floor division) to the mult-ops
    while (token.isMultiplicationOperator() ||
           token.isDivisionOperator() ||
           token.isModuloOperator() ||
           token.isFloorDivisionOperator()) {
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
    // Phase 3 - Changed: <arith-atom> -> <id> | <literal> | <string-conversion> | '(' <rel-expr> ')'
    Token token = tokenizer.getToken();

    if (token.isIdentifier())
        return new Variable(token);

    // Phase 3 - any literal: put the token back and let literal() read it
    if (token.isInteger() || token.isDouble() || token.isString() ||
        token.isTrueKeyword() || token.isFalseKeyword()) {
        tokenizer.ungetToken();
        return literal();
    }

    // Phase 3 - str( ... )
    if (token.isStrKeyword()) {
        tokenizer.ungetToken();
        return stringConversion();
    }

    if (token.isOpenParen()) {
        ExprNode *expression = relExpr();
        Token closeParen = tokenizer.getToken();
        if (!closeParen.isCloseParen())
            die("Parser::arithAtom", "expected ')'", closeParen);
        return expression;
    }

    die("Parser::arithAtom", "expected an identifier, literal, str conversion, or '('", token);
}

// Phase 3 - Added: <literal> -> <integer> | <double> | <boolean> | <string>
ExprNode *Parser::literal() {
    Token token = tokenizer.getToken();

    if (token.isInteger())
        return new IntegerLiteral(token);

    if (token.isDouble())
        return new DoubleLiteral(token);

    if (token.isTrueKeyword() || token.isFalseKeyword())
        return new BooleanLiteral(token);

    if (token.isString())
        return new StringLiteral(token);

    die("Parser::literal", "expected a literal", token);
}

// Phase 3 - Added: <string-conversion> -> "str" ( <rel-expr> )
ExprNode *Parser::stringConversion() {
    // the word "str"
    Token strToken = tokenizer.getToken();
    if (!strToken.isStrKeyword())
        die("Parser::stringConversion", "expected 'str'", strToken);

    // (
    Token openParen = tokenizer.getToken();
    if (!openParen.isOpenParen())
        die("Parser::stringConversion", "expected '('", openParen);

    // the expression inside
    ExprNode *expression = relExpr();

    // )
    Token closeParen = tokenizer.getToken();
    if (!closeParen.isCloseParen()) {
        delete expression;
        die("Parser::stringConversion", "expected ')'", closeParen);
    }

    return new StringConversionExpression(strToken, expression);
}