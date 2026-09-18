#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "Expr.hpp"

ExprNode::ExprNode(Token token) : expressionToken{std::move(token)} {}

const Token &ExprNode::token() const {
    return expressionToken;
}

BinaryExprNode::BinaryExprNode(Token token, ExprNode *left, ExprNode *right)
    : ExprNode{std::move(token)},
      leftOperand{left},
      rightOperand{right} {}

BinaryExprNode::~BinaryExprNode() {
    delete leftOperand;
    delete rightOperand;
}

int BinaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    const int leftValue = leftOperand->evaluate(symbolTable);
    const int rightValue = rightOperand->evaluate(symbolTable);

    if (token().isAdditionOperator())
        return leftValue + rightValue;
    if (token().isSubtractionOperator())
        return leftValue - rightValue;
    if (token().isMultiplicationOperator())
        return leftValue * rightValue;
    if (token().isDivisionOperator()) {
        if (rightValue == 0)
            throw std::runtime_error(
                "division by zero at line " + std::to_string(token().lineNumber()) +
                ", column " + std::to_string(token().columnNumber()));
        return leftValue / rightValue;
    }
    if (token().isModuloOperator()) {
        if (rightValue == 0)
            throw std::runtime_error(
                "modulo by zero at line " + std::to_string(token().lineNumber()) +
                ", column " + std::to_string(token().columnNumber()));
        return leftValue % rightValue;
    }

    throw std::logic_error("unsupported infix operator");
}

void BinaryExprNode::print() const {
    std::cout << '(';
    leftOperand->print();
    token().print(std::cout);
    rightOperand->print();
    std::cout << ')';
}

UnaryExprNode::UnaryExprNode(Token token, ExprNode *operand)
    : ExprNode{std::move(token)}, expression{operand} {}

UnaryExprNode::~UnaryExprNode() {
    delete expression;
}

int UnaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    const int value = expression->evaluate(symbolTable);
    if (token().isAdditionOperator())
        return value;
    if (token().isSubtractionOperator())
        return -value;
    throw std::logic_error("unsupported prefix operator");
}

void UnaryExprNode::print() const {
    std::cout << token().symbol();
    expression->print();
}

IntegerLiteral::IntegerLiteral(Token token) : ExprNode{std::move(token)} {}

void IntegerLiteral::print() const {
    token().print(std::cout);
}

int IntegerLiteral::evaluate(const SymbolTable &) const {
    return token().integerValue();
}

Variable::Variable(Token token) : ExprNode{std::move(token)} {}

void Variable::print() const {
    token().print(std::cout);
}

int Variable::evaluate(const SymbolTable &symbolTable) const {
    return symbolTable.getValueFor(token().identifier());
}
