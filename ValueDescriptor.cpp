#include "ValueDescriptor.hpp"

#include <sstream>
#include <utility>

//IntegerValueDescriptor 

// Phase 3 - store the integer value
IntegerValueDescriptor::IntegerValueDescriptor(std::int64_t value)
    : integerValue{value} {}

ValueDescriptor::Type IntegerValueDescriptor::type() const {
    return Type::integer;
}

std::string IntegerValueDescriptor::typeName() const {
    return "integer";
}

// Phase 3 - decimal notation
std::string IntegerValueDescriptor::toString() const {
    return std::to_string(integerValue);
}

// Phase 3 - caller owns the copy
ValueDescriptor *IntegerValueDescriptor::clone() const {
    return new IntegerValueDescriptor(integerValue);
}

std::int64_t IntegerValueDescriptor::value() const {
    return integerValue;
}

// DoubleValueDescriptor 

// Phase 3 - store the double value
DoubleValueDescriptor::DoubleValueDescriptor(double value)
    : doubleValue{value} {}

ValueDescriptor::Type DoubleValueDescriptor::type() const {
    return Type::doubleValue;
}

std::string DoubleValueDescriptor::typeName() const {
    return "double";
}

// Phase 3 - default C++ stream formatting (no fixed, no precision)
std::string DoubleValueDescriptor::toString() const {
    std::ostringstream out;
    out << doubleValue;
    return out.str();
}

// Phase 3 - caller owns the copy
ValueDescriptor *DoubleValueDescriptor::clone() const {
    return new DoubleValueDescriptor(doubleValue);
}

double DoubleValueDescriptor::value() const {
    return doubleValue;
}

// BooleanValueDescriptor

// Phase 3 - store the boolean value
BooleanValueDescriptor::BooleanValueDescriptor(bool value)
    : booleanValue{value} {}

ValueDescriptor::Type BooleanValueDescriptor::type() const {
    return Type::boolean;
}

std::string BooleanValueDescriptor::typeName() const {
    return "boolean";
}

// Phase 3 - print as True or False
std::string BooleanValueDescriptor::toString() const {
    if (booleanValue) {
        return "True";
    } else {
        return "False";
    }
}

// Phase 3 - caller owns the copy
ValueDescriptor *BooleanValueDescriptor::clone() const {
    return new BooleanValueDescriptor(booleanValue);
}

bool BooleanValueDescriptor::value() const {
    return booleanValue;
}

// StringValueDescriptor 

// Phase 3 - store the string value
StringValueDescriptor::StringValueDescriptor(std::string value)
    : stringValue{std::move(value)} {}

ValueDescriptor::Type StringValueDescriptor::type() const {
    return Type::string;
}

std::string StringValueDescriptor::typeName() const {
    return "string";
}

// Phase 3 - no surrounding quotes
std::string StringValueDescriptor::toString() const {
    return stringValue;
}

// Phase 3 - caller owns the copy
ValueDescriptor *StringValueDescriptor::clone() const {
    return new StringValueDescriptor(stringValue);
}

const std::string &StringValueDescriptor::value() const {
    return stringValue;
}