#ifndef EXPRINTER_STATEMENTS_HPP
#define EXPRINTER_STATEMENTS_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include "Expr.hpp"
#include "SymbolTable.hpp"

// Phase 2:
// Added: the range classes given by the professor.
// EvaluatedRange holds the three real numbers (start, stop, step).
class EvaluatedRange {
public:
    EvaluatedRange(int start, int stop, int step);

    [[nodiscard]] int start() const;
    [[nodiscard]] int stop() const;
    [[nodiscard]] int step() const;

    [[nodiscard]] bool hasIteration() const;
    [[nodiscard]] bool shouldContinue(int nextValue) const;

private:
    int start_;
    int stop_;
    int step_;
};

// Phase 2:
// RangeExpression holds the expressions written inside range( ... ).
// It owns them, so the destructor deletes them.
class RangeExpression {
public:
    explicit RangeExpression(ExprNode *stop);
    RangeExpression(ExprNode *start, ExprNode *stop);
    RangeExpression(
        ExprNode *start,
        ExprNode *stop,
        ExprNode *step
    );
    ~RangeExpression();

    RangeExpression(const RangeExpression &) = delete;
    RangeExpression &operator=(const RangeExpression &) = delete;

    [[nodiscard]] EvaluatedRange evaluate(
        const SymbolTable &symbolTable
    ) const;

    void print(std::ostream &output) const;

private:
    ExprNode *startExpression;
    ExprNode *stopExpression;
    ExprNode *stepExpression;
};


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
// added: a for loop (Phase 1 C-style version, replaced in Phase 2 below)
class ForStatement final : public Statement {
public:
    //ForStatement(AssignmentStatement *initialization, ExprNode *condition, AssignmentStatement *update, Statements *loopBody); //Phase 1 (removed because old C-style syntax doesnt work)

    // Phase 2:
    // Changed: Python-style for loop -> for i in range(...):
    // it holds the loop variable name, the range, and the loop body.
    // it owns the range and the body, so the destructor deletes them.
    ForStatement(std::string variable, RangeExpression *range, Statements *suite);
    ~ForStatement() override;
    void evaluate(SymbolTable &symbolTable) const override;
    void print() const override;
    
private:
    // Removed old C-style for loop members
    //AssignmentStatement *initialization; // runs once at the start of the loop -> i=0
    //ExprNode *condition; // checked each lap
    //AssignmentStatement *update; // runs after each lap -> i=i+1
    //Statements *loopBody; // the lines inside { }

    //Phase 2: Python-style for loop members
    std::string variable; // the name after "for" -> i
    RangeExpression *range;   // the range(...) part
    Statements *suite;         // the indented lines
};

#endif // EXPRINTER_STATEMENTS_HPP
