#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

#include "Statements.hpp"

Statements::~Statements() {
    for (auto *statement : statements)
        delete statement;
}

void Statements::addStatement(Statement *statement) {
    statements.push_back(statement);
}

void Statements::print() const {
    for (const auto *statement : statements)
        statement->print();
}

void Statements::evaluate(SymbolTable &symbolTable) const {
    for (const auto *statement : statements)
        statement->evaluate(symbolTable);
}

AssignmentStatement::AssignmentStatement(
    std::string variableName,
    ExprNode *expression)
    : variableName{std::move(variableName)}, expression{expression} {}

AssignmentStatement::~AssignmentStatement() {
    delete expression;
}

void AssignmentStatement::evaluate(SymbolTable &symbolTable) const {
    symbolTable.setValueFor(variableName, expression->evaluate(symbolTable));
}

void AssignmentStatement::print() const {
    std::cout << variableName << " = ";
    expression->print();
    std::cout << '\n';
}

//Part 3:
// Added: the three Print statement functions

// stores the expression that was parsed after the word "print"
PrintStatement::PrintStatement(ExprNode* expression) : expression{expression} {}

// Doesn't transfer ownership, so it deletes it
PrintStatement::~PrintStatement() {
    delete expression;
}

// running a print statement, work out the number then show it
// followed by a newline
void PrintStatement::evaluate(SymbolTable &symbolTable) const {
    std::cout << expression->evaluate(symbolTable) << std::endl;
}

// This displays the parsed program, not the programs output
void PrintStatement::print() const {
    std::cout << "print ";
    expression->print();
    std::cout << std::endl;
}

// Phase 2:
// Changed: the For statement functions for the Python-style loop

// store the variable name, the range, and the indented lines
ForStatement::ForStatement(std::string variable, RangeExpression *range, Statements *suite) :
    variable{std::move(variable)}, range{range}, suite{suite} {}

ForStatement::~ForStatement() {
    delete range;
    delete suite;
}

void ForStatement::evaluate(SymbolTable &symbolTable) const {

    EvaluatedRange evalRange = range->evaluate(symbolTable);

    int nextValue = evalRange.start();

    if (evalRange.hasIteration()) {
        while (evalRange.shouldContinue(nextValue)) {
            symbolTable.setValueFor(variable, nextValue);
            suite->evaluate(symbolTable);
            nextValue += evalRange.step();
        }
    }
}

// shows the parsed loop -> for i in range(3):
void ForStatement::print() const {
    std::cout << "for " << variable << " in ";
    range->print(std::cout);
    std::cout << ":\n";
    suite->print();
}

// Phase 2:
// Added: the EvaluatedRange functions

// save the three numbers, but a step of 0 would loop forever, so stop here
EvaluatedRange::EvaluatedRange(int start, int stop, int step)
    : start_{start}, stop_{stop}, step_{step} {
    if (step_ == 0) {
        std::cerr << "EvaluatedRange: step must be non-zero.\n";
        std::exit(EXIT_FAILURE);
    }
}

int EvaluatedRange::start() const { return start_; }
int EvaluatedRange::stop() const { return stop_; }
int EvaluatedRange::step() const { return step_; }

// does the range have at least one value? just test the start value
bool EvaluatedRange::hasIteration() const {
    return shouldContinue(start_);
}

// counting up (positive step): value must stay below stop
// counting down (negative step): value must stay above stop
bool EvaluatedRange::shouldContinue(int nextValue) const {
    return (step_ > 0) ? (nextValue < stop_) : (nextValue > stop_);
}

// Phase 2:
// Added: the RangeExpression functions
// anything not written in range( ... ) is stored as nullptr

// range(stop)
RangeExpression::RangeExpression(ExprNode *stop)
    : RangeExpression(nullptr, stop, nullptr) {}

// range(start, stop)
RangeExpression::RangeExpression(ExprNode *start, ExprNode *stop)
    : RangeExpression(start, stop, nullptr) {}

// range(start, stop, step)
RangeExpression::RangeExpression(ExprNode *start, ExprNode *stop, ExprNode *step)
    : startExpression{start}, stopExpression{stop}, stepExpression{step} {}

// owns all three expressions, so delete them (deleting nullptr does nothing)
RangeExpression::~RangeExpression() {
    delete startExpression;
    delete stopExpression;
    delete stepExpression;
}

// turn the expressions into numbers, once, in the order written
// missing start -> 0, missing step -> 1
EvaluatedRange RangeExpression::evaluate(const SymbolTable &symbolTable) const {
    const int start = startExpression ? startExpression->evaluate(symbolTable) : 0;
    const int stop = stopExpression->evaluate(symbolTable);
    const int step = stepExpression ? stepExpression->evaluate(symbolTable) : 1;
    return {start, stop, step};
}

// show it the way it was written -> range(2, 10, 2)
void RangeExpression::print(std::ostream &output) const {
    output << "range(";
    if (startExpression) {
        startExpression->print();
        output << ", ";
    }
    stopExpression->print();
    if (stepExpression) {
        output << ", ";
        stepExpression->print();
    }
    output << ')';
}