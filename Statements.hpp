#ifndef EXPRINTER_STATEMENTS_HPP
#define EXPRINTER_STATEMENTS_HPP

#include <string>
#include <vector>

#include "Expr.hpp"
#include "SymbolTable.hpp"

class Statement {
public:
    virtual ~Statement() = default;

    virtual void print() const = 0;
    virtual void evaluate(SymbolTable &symbolTable) const = 0;
};

class Statements {
public:
    ~Statements();

    void addStatement(Statement *statement);
    void evaluate(SymbolTable &symbolTable) const;
    void print() const;

private:
    std::vector<Statement *> statements;
};

class AssignmentStatement final : public Statement {
public:
    AssignmentStatement(std::string variableName,
                        ExprNode *expression);
    ~AssignmentStatement() override;

    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;

private:
    std::string variableName;
    ExprNode *expression;
};

// Part 3 (double check)
// Added: a print statment, built the same way as the assignment statement
// above, except that it holds only an expression and no varaible name.
// it owns the expression, so the destructor deletes it.
class PrintStatement final : public Statement {
public:
    PrintStatement(ExprNode *expression);
    ~PrintStatement() override;
    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;

private:
    ExprNode *expression;
};

// Part 4:
// added: a for loop
// it holds the start, the check, the step, and the loop body. it owns all of these, so the destructor deletes them.
class ForStatement final : public Statement {
public:
    ForStatement(AssignmentStatement *initialization, ExprNode *condition, AssignmentStatement *update, Statements *loopBody);
    ~ForStatement() override;
    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;
    
private:
    AssignmentStatement *initialization; // runs once at the start of the loop -> i=0
    ExprNode *condition; // checked each lap
    AssignmentStatement *update; // runs after each lap -> i=i+1
    Statements *loopBody; // the lines inside { }
};

#endif // EXPRINTER_STATEMENTS_HPP
