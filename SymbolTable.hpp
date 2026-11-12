#ifndef EXPRINTER_SYMBOLTABLE_HPP
#define EXPRINTER_SYMBOLTABLE_HPP

#include <map>
#include <string>

// Phase 3 - values are now descriptors
#include "ValueDescriptor.hpp"

class SymbolTable {
public:
    // Phase 3 - takes a ValueDescriptor* (the table now owns it) instead of an int
    void setValueFor(const std::string &variableName, ValueDescriptor *value);
    [[nodiscard]] bool isDefined(const std::string &variableName) const;

    // Phase 3 - read-only pointer, still owned by the table (caller must not delete)
    [[nodiscard]] const ValueDescriptor *getValueFor(const std::string &variableName) const;
    void print() const;

    // Phase 3 - needed because deleting the copy constructor removes the default one
    SymbolTable() = default;

    // Phase 3 - deletes every descriptor still stored
    ~SymbolTable();

    // Phase 3 - given by professor: no copying or moving (would cause double deletes)
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable &operator=(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;
    SymbolTable &operator=(SymbolTable &&) = delete;

private:
    // Phase 3 - stores descriptors instead of ints
    std::map<std::string, ValueDescriptor *> values;
};

#endif // EXPRINTER_SYMBOLTABLE_HPP