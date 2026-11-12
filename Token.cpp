#include <iostream>

#include "Token.hpp"

void Token::print(std::ostream &output) const {
    if (isNewline())
        output << "NEWLINE";
    else if (isEof())
        output << "EOF";

    // Phase 2 - Indent and Dedent added
    else if (isIndent())
        output << "INDENT";
    else if (isDedent())
        output << "DEDENT";

    else if (isForKeyword())
        output << "for";
    else if (isPrintKeyword())
        output << "print";

    // Phase 2 - In and Range added
    else if (isInKeyword())
        output << "in";
    else if (isRangeKeyword())
        output << "range";

    // Phase 3 - True, False, and STR
    else if (isTrueKeyword())
        output << "True";
    else if (isFalseKeyword())
        output << "False";
    else if (isStrKeyword())
        output << "STR";

    else if (isOpenParen())
        output << '(';
    else if (isCloseParen())
        output << ')';
    else if (isAssignmentOperator())
        output << " = ";
    else if (isSemicolon())
        output << ';';
    else if (isMultiplicationOperator())
        output << " * ";
    else if (isAdditionOperator())
        output << " + ";
    else if (isSubtractionOperator())
        output << " - ";

    // Phase 3 - Floor division operator added
    else if (isFloorDivisionOperator())
        output << " // ";

    else if (isModuloOperator())
        output << " % ";
    else if (isDivisionOperator())
        output << " / ";
    else if (isIdentifier())
        output << identifier();
    else if (isInteger())
        output << integerValue();

    // Phase 3 - Double and String literals added
    else if (isDouble())
        output << doubleValue();
    else if (isString())
        output << '"' << stringValue() << '"';

    // Phase 1- Part 1:
    // ADDED: the new relational operators from the grammar.
    // The two-character ones are listed before the one-character ones
    // so ">=" is never printed as just ">".
    else if (isEqualityOperator())
        output << " == ";
    else if (isNotEqualOperator())
        output << " != ";
    else if (isGreaterThanOrEqualOperator())
        output << " >= ";
    else if (isGreaterThanOperator())
        output << " > ";
    else if (isLessThanOrEqualOperator())
        output << " <= ";
    else if (isLessThanOperator())
        output << " < ";

    // Phase 1- Part 1:
    // ADDED: the braces that wrap a for loop body.
    // Printed without spaces, to match how '(' and ')' are printed above.
    else if (isOpenBracket())
        output << '{';
    else if (isClosedBracket())
        output << '}';

    // Phase 2 - Colon and Comma added
    else if (isColon())
        output << ':';
    else if (isComma())
        output << ',';

    else
        output << "uninitialized token";
}