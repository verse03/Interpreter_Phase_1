#ifndef EXPRINTER_SYMBOLTABLE_HPP
#define EXPRINTER_SYMBOLTABLE_HPP

#include <map>
#include <string>

class SymbolTable {
public:
    void setValueFor(const std::string &variableName, int value);
    [[nodiscard]] bool isDefined(const std::string &variableName) const;
    [[nodiscard]] int getValueFor(const std::string &variableName) const;
    void print() const;

private:
    std::map<std::string, int> values;
};

#endif // EXPRINTER_SYMBOLTABLE_HPP
