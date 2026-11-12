#include <iostream>
#include <stdexcept>

#include "SymbolTable.hpp"

// Phase 3 - table takes ownership; deletes the old descriptor if the variable already exists
void SymbolTable::setValueFor(const std::string &variableName, ValueDescriptor *value) {
    auto found = values.find(variableName);

    if (found != values.end()) {
        delete found->second;
    }
    values[variableName] = value;
}

bool SymbolTable::isDefined(const std::string &variableName) const {
    return values.contains(variableName);
}

// Phase 3 - returns a read-only pointer (caller must not delete it)
const ValueDescriptor *SymbolTable::getValueFor(const std::string &variableName) const {
    const auto entry = values.find(variableName);
    if (entry == values.end())
        throw std::runtime_error("use of undefined variable '" + variableName + "'");
    return entry->second;
}

// Phase 3 - prints each value with toString()
void SymbolTable::print() const {
    for (const auto &[variable, value] : values)
        std::cout << variable << " = " << value->toString() << '\n';
}

// Phase 3 - deletes every descriptor the table still owns
SymbolTable::~SymbolTable() {
    for (const auto &[variable, value] : values) {
        delete value;
    }
}