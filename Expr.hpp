#ifndef EXPRINTER_EXPR_HPP
#define EXPRINTER_EXPR_HPP

#include "SymbolTable.hpp"
#include "Token.hpp"

// Phase 3 - values are now descriptors
#include "ValueDescriptor.hpp"

// Phase 3 - every evaluate() returns a NEW descriptor that the caller owns

class ExprNode {
public:
    explicit ExprNode(Token token);
    virtual ~ExprNode() = default;

    [[nodiscard]] const Token &token() const;
    virtual void print() const = 0;
    [[nodiscard]] virtual ValueDescriptor *evaluate(const SymbolTable &symbolTable) const = 0;

private:
    Token expressionToken;
};

class BinaryExprNode final : public ExprNode {
public:
    BinaryExprNode(Token token, ExprNode *left, ExprNode *right);
    ~BinaryExprNode() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *leftOperand;
    ExprNode *rightOperand;
};

class UnaryExprNode final : public ExprNode {
public:
    UnaryExprNode(Token token, ExprNode *operand);
    ~UnaryExprNode() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *expression;
};

class IntegerLiteral final : public ExprNode {
public:
    explicit IntegerLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

// Phase 3 - double literal, e.g. 3.14
class DoubleLiteral final : public ExprNode {
public:
    explicit DoubleLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

// Phase 3 - string literal, e.g. "hello"
class StringLiteral final : public ExprNode {
public:
    explicit StringLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

// Phase 3 - True or False
class BooleanLiteral final : public ExprNode {
public:
    explicit BooleanLiteral(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

class Variable final : public ExprNode {
public:
    explicit Variable(Token token);

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;
};

// Phase 3 - str(expression), owns its inner expression like UnaryExprNode
class StringConversionExpression final : public ExprNode {
public:
    StringConversionExpression(Token token, ExprNode *expression);
    ~StringConversionExpression() override;

    void print() const override;
    [[nodiscard]] ValueDescriptor *evaluate(const SymbolTable &symbolTable) const override;

private:
    ExprNode *_expression;
};

#endif // EXPRINTER_EXPR_HPP