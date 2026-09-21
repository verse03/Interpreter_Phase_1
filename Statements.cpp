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

// Part 4:
// Added: the four For statement functions

//store the four parts of the loop
ForStatement::ForStatement(AssignmentStatement *initalization, ExprNode *condition, AssignmentStatement *update, Statements *loopBody) : 
initialization{initalization}, condition{condition}, update{update}, loopBody{loopBody} {}

// the loop owns all four pieces, so it deletes all four
ForStatement::~ForStatement() {
    delete initialization;
    delete condition;
    delete update;
    delete loopBody;
}

void ForStatement::evaluate(SymbolTable &symbolTable) const {
    // step 1: run the start once
    initialization->evaluate(symbolTable);

    // step 2 and 3: check the condition before every lap,
    // and keep going while it is not 0
    while (condition->evaluate(symbolTable) != 0) {
        //step 4: run every line inside { }
        loopBody->evaluate(symbolTable);

        //step 5: run the step after each lap
        update->evaluate(symbolTable);
    }
}

// This displays the parsed loop, not the programs output
void ForStatement::print() const {
    std::cout << "for (" << std::endl;
    initialization->print();
    condition->print();
    std::cout << std::endl;
    update->print();
    std::cout << ") {" << std::endl;
    loopBody->print();
    std::cout << "}" << std::endl;
}
