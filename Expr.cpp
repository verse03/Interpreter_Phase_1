#include <cstdint>
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

// Phase 3 - returns a new descriptor; deletes both operand descriptors after reading them
ValueDescriptor *BinaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    const ValueDescriptor *leftValue = leftOperand->evaluate(symbolTable);
    const ValueDescriptor *rightValue = rightOperand->evaluate(symbolTable);

    // Phase 3 - type of each operand
    const bool leftInt = leftValue->type() == ValueDescriptor::Type::integer;
    const bool rightInt = rightValue->type() == ValueDescriptor::Type::integer;
    const bool leftDouble = leftValue->type() == ValueDescriptor::Type::doubleValue;
    const bool rightDouble = rightValue->type() == ValueDescriptor::Type::doubleValue;
    const bool leftString = leftValue->type() == ValueDescriptor::Type::string;
    const bool rightString = rightValue->type() == ValueDescriptor::Type::string;
    const bool leftBoolean = leftValue->type() == ValueDescriptor::Type::boolean;
    const bool rightBoolean = rightValue->type() == ValueDescriptor::Type::boolean;

    // Phase 3 - bothDoubles also covers int with double (the int gets promoted)
    const bool bothInts = leftInt && rightInt;
    const bool bothDoubles = (leftDouble || leftInt) && (rightDouble || rightInt) && !bothInts;
    const bool bothNumbers = bothInts || bothDoubles;
    const bool bothStrings = leftString && rightString;
    const bool bothBooleans = leftBoolean && rightBoolean;

    // Phase 3 - read the values out; integers are also copied as doubles for promotion
    std::int64_t leftNum = 0;
    std::int64_t rightNum = 0;
    double leftDoubleNum = 0;
    double rightDoubleNum = 0;
    std::string leftStringActual = "";
    std::string rightStringActual = "";
    bool leftBoolValue = false;
    bool rightBoolValue = false;

    if (leftInt) {
        leftNum = static_cast<const IntegerValueDescriptor *>(leftValue)->value();
        leftDoubleNum = static_cast<double>(leftNum);
    } else if (leftDouble) {
        leftDoubleNum = static_cast<const DoubleValueDescriptor *>(leftValue)->value();
    } else if (leftString) {
        leftStringActual = static_cast<const StringValueDescriptor *>(leftValue)->value();
    } else if (leftBoolean) {
        leftBoolValue = static_cast<const BooleanValueDescriptor *>(leftValue)->value();
    }

    if (rightInt) {
        rightNum = static_cast<const IntegerValueDescriptor *>(rightValue)->value();
        rightDoubleNum = static_cast<double>(rightNum);
    } else if (rightDouble) {
        rightDoubleNum = static_cast<const DoubleValueDescriptor *>(rightValue)->value();
    } else if (rightString) {
        rightStringActual = static_cast<const StringValueDescriptor *>(rightValue)->value();
    } else if (rightBoolean) {
        rightBoolValue = static_cast<const BooleanValueDescriptor *>(rightValue)->value();
    }

    // Phase 3 - error messages, built before the descriptors are deleted
    std::string operatorText = token().multiCharSymbol();
    if (operatorText.empty())
        operatorText = std::string(1, token().symbol());
    const std::string location = " at line " + std::to_string(token().lineNumber()) +
                                 ", column " + std::to_string(token().columnNumber());
    const std::string typeError = "Type error: " + leftValue->typeName() + " " + operatorText + " " +
                                  rightValue->typeName() + location;

    // Phase 3 - all values are extracted, so the operand descriptors are no longer needed
    delete leftValue;
    delete rightValue;

    // Phase 3 - == and != accept any two values; unrelated types are unequal
    if (token().isEqualityOperator() || token().isNotEqualOperator()) {
        bool equal = false;
        if (bothInts)
            equal = leftNum == rightNum;
        else if (bothDoubles)
            equal = leftDoubleNum == rightDoubleNum;
        else if (bothStrings)
            equal = leftStringActual == rightStringActual;
        else if (bothBooleans)
            equal = leftBoolValue == rightBoolValue;

        if (token().isEqualityOperator())
            return new BooleanValueDescriptor(equal);
        return new BooleanValueDescriptor(!equal);
    }

    // Phase 3 - < <= > >= accept two numbers or two strings
    if (token().isLessThanOperator() || token().isLessThanOrEqualOperator() ||
        token().isGreaterThanOperator() || token().isGreaterThanOrEqualOperator()) {
        bool less = false;
        bool greater = false;
        if (bothInts) {
            less = leftNum < rightNum;
            greater = leftNum > rightNum;
        } else if (bothDoubles) {
            less = leftDoubleNum < rightDoubleNum;
            greater = leftDoubleNum > rightDoubleNum;
        } else if (bothStrings) {
            less = leftStringActual < rightStringActual;
            greater = leftStringActual > rightStringActual;
        } else {
            throw std::runtime_error(typeError);
        }

        if (token().isLessThanOperator())
            return new BooleanValueDescriptor(less);
        if (token().isLessThanOrEqualOperator())
            return new BooleanValueDescriptor(!greater);
        if (token().isGreaterThanOperator())
            return new BooleanValueDescriptor(greater);
        return new BooleanValueDescriptor(!less);
    }

    // Phase 3 - + also joins two strings
    if (token().isAdditionOperator() && bothStrings)
        return new StringValueDescriptor(leftStringActual + rightStringActual);

    // Phase 3 - everything left (+ - * / // %) needs two numbers
    if (!bothNumbers)
        throw std::runtime_error(typeError);

    // Phase 3 - / always gives a double; == 0 also catches 0.0 and -0.0
    if (token().isDivisionOperator()) {
        if (rightDoubleNum == 0)
            throw std::runtime_error("Cannot divide by zero" + location);
        return new DoubleValueDescriptor(leftDoubleNum / rightDoubleNum);
    }

    // Phase 3 - // and % need two integers and follow Python's floor rules
    if (token().isFloorDivisionOperator() || token().isModuloOperator()) {
        if (!bothInts)
            throw std::runtime_error(typeError);
        if (rightNum == 0)
            throw std::runtime_error("Cannot use " + operatorText + " with zero" + location);

        std::int64_t quotient = leftNum / rightNum;
        std::int64_t remainder = leftNum % rightNum;
        if (remainder != 0 && ((leftNum < 0) != (rightNum < 0))) {
            --quotient;
            remainder += rightNum;
        }

        if (token().isFloorDivisionOperator())
            return new IntegerValueDescriptor(quotient);
        return new IntegerValueDescriptor(remainder);
    }

    // Phase 3 - only + - * are left: two ints stay int, otherwise double
    if (bothInts) {
        if (token().isAdditionOperator())
            return new IntegerValueDescriptor(leftNum + rightNum);
        if (token().isSubtractionOperator())
            return new IntegerValueDescriptor(leftNum - rightNum);
        return new IntegerValueDescriptor(leftNum * rightNum);
    }

    if (token().isAdditionOperator())
        return new DoubleValueDescriptor(leftDoubleNum + rightDoubleNum);
    if (token().isSubtractionOperator())
        return new DoubleValueDescriptor(leftDoubleNum - rightDoubleNum);
    return new DoubleValueDescriptor(leftDoubleNum * rightDoubleNum);
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

// Phase 3 - unary + and - work on integers and doubles only
ValueDescriptor *UnaryExprNode::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor *value = expression->evaluate(symbolTable);
    ValueDescriptor *result = nullptr;

    if (value->type() == ValueDescriptor::Type::integer) {
        std::int64_t num = static_cast<IntegerValueDescriptor *>(value)->value();
        if (token().isSubtractionOperator())
            num = -num;
        result = new IntegerValueDescriptor(num);
    } else if (value->type() == ValueDescriptor::Type::doubleValue) {
        double dblNum = static_cast<DoubleValueDescriptor *>(value)->value();
        if (token().isSubtractionOperator())
            dblNum = -dblNum;
        result = new DoubleValueDescriptor(dblNum);
    }

    // Phase 3 - value is read, so the operand descriptor is no longer needed
    const std::string valueTypeName = value->typeName();
    delete value;

    if (result == nullptr)
        throw std::runtime_error("Type error: " + std::string(1, token().symbol()) + valueTypeName +
                                 " at line " + std::to_string(token().lineNumber()) +
                                 ", column " + std::to_string(token().columnNumber()));
    return result;
}

void UnaryExprNode::print() const {
    std::cout << token().symbol();
    expression->print();
}

IntegerLiteral::IntegerLiteral(Token token) : ExprNode{std::move(token)} {}

void IntegerLiteral::print() const {
    token().print(std::cout);
}

// Phase 3 - each literal makes a new descriptor from its token
ValueDescriptor *IntegerLiteral::evaluate(const SymbolTable &) const {
    return new IntegerValueDescriptor(token().integerValue());
}

DoubleLiteral::DoubleLiteral(Token token) : ExprNode{std::move(token)} {}

void DoubleLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor *DoubleLiteral::evaluate(const SymbolTable &) const {
    return new DoubleValueDescriptor(token().doubleValue());
}

StringLiteral::StringLiteral(Token token) : ExprNode{std::move(token)} {}

void StringLiteral::print() const {
    token().print(std::cout);
}

ValueDescriptor *StringLiteral::evaluate(const SymbolTable &) const {
    return new StringValueDescriptor(token().stringValue());
}

BooleanLiteral::BooleanLiteral(Token token) : ExprNode{std::move(token)} {}

void BooleanLiteral::print() const {
    token().print(std::cout);
}

// Phase 3 - True is the trueKeyword token
ValueDescriptor *BooleanLiteral::evaluate(const SymbolTable &) const {
    return new BooleanValueDescriptor(token().isTrueKeyword());
}

Variable::Variable(Token token) : ExprNode{std::move(token)} {}

void Variable::print() const {
    token().print(std::cout);
}

// Phase 3 - the table keeps its descriptor; we return a clone the caller owns
ValueDescriptor *Variable::evaluate(const SymbolTable &symbolTable) const {
    return symbolTable.getValueFor(token().identifier())->clone();
}

// Phase 3 - str(expression)
StringConversionExpression::StringConversionExpression(Token token, ExprNode *expression)
    : ExprNode{std::move(token)}, _expression{expression} {}

StringConversionExpression::~StringConversionExpression() {
    delete _expression;
}

void StringConversionExpression::print() const {
    std::cout << "str(";
    _expression->print();
    std::cout << ")";
}

// Phase 3 - uses toString(), the same function print uses
ValueDescriptor *StringConversionExpression::evaluate(const SymbolTable &symbolTable) const {
    ValueDescriptor *value = _expression->evaluate(symbolTable);
    std::string result = value->toString();

    delete value;
    return new StringValueDescriptor(result);
}