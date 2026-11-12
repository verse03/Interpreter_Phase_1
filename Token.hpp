#ifndef ALEXICALANALYZER_TOKEN_HPP
#define ALEXICALANALYZER_TOKEN_HPP

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <utility>

enum class Keyword {
    none,
    forKeyword,
    printKeyword,

    // Phase 2 - In and Range added
    inKeyword,
    rangeKeyword,

    // Phase 3 - str, True, and False added
    strKeyword,
    trueKeyword,
    falseKeyword,
};

class Token {
public:
    [[nodiscard]] bool isEof() const { return _eof; }
    [[nodiscard]] bool isNewline() const { return _newline; }
    void markAsEof() { _eof = true; }
    void markAsNewline() { _newline = true; }

    void setLocation(std::size_t line, std::size_t column) {
        _lineNumber = line;
        _columnNumber = column;
    }

    // Phase 2 - Indent and Dedent added
    [[nodiscard]] bool isIndent() const { return _indent; }
    [[nodiscard]] bool isDedent() const { return _dedent; }
    void markAsIndent() { _indent = true; }
    void markAsDedent() { _dedent = true; }

    [[nodiscard]] std::size_t lineNumber() const { return _lineNumber; }
    [[nodiscard]] std::size_t columnNumber() const { return _columnNumber; }

    void setSymbol(char symbol) { _symbol = symbol; }

    // Phase 1 -Part 1:
    // Changed: takes a copy instead of a refernece, so we can call
    // setMutliCharSymbol("==") with text written directly in the code
    void setMultiCharSymbol(std::string multiCharSymbol) { _multiCharSymbol = std::move(multiCharSymbol); }

    [[nodiscard]] char symbol() const { return _symbol; }

    // Phase 1 -Part 1:
    // Added: lets other code read the two-character symbol back out
    // Token::print() needs this to show "==" , "!=" , ">=", "<=" 
    [[nodiscard]] const std::string &multiCharSymbol() const { return _multiCharSymbol; }

    [[nodiscard]] bool isOpenParen() const { return _symbol == '('; }
    [[nodiscard]] bool isCloseParen() const { return _symbol == ')'; }
    [[nodiscard]] bool isSemicolon() const { return _symbol == ';'; }
    [[nodiscard]] bool isAssignmentOperator() const { return _symbol == '='; }
    [[nodiscard]] bool isMultiplicationOperator() const { return _symbol == '*'; }
    [[nodiscard]] bool isAdditionOperator() const { return _symbol == '+'; }
    [[nodiscard]] bool isSubtractionOperator() const { return _symbol == '-'; }
    [[nodiscard]] bool isModuloOperator() const { return _symbol == '%'; }
    [[nodiscard]] bool isDivisionOperator() const { return _symbol == '/'; }

    // Phase 3 - Floor division operator added
    [[nodiscard]] bool isFloorDivisionOperator() const { return _multiCharSymbol == "//"; }

    // Phase 1 -Part 1:
    // Added: the six relational operators
    [[nodiscard]] bool isEqualityOperator() const { return _multiCharSymbol == "=="; }
    [[nodiscard]] bool isNotEqualOperator() const { return _multiCharSymbol == "!="; }
    [[nodiscard]] bool isGreaterThanOperator() const { return _symbol == '>'; }
    [[nodiscard]] bool isGreaterThanOrEqualOperator() const { return _multiCharSymbol == ">="; }
    [[nodiscard]] bool isLessThanOperator() const { return _symbol == '<'; }
    [[nodiscard]] bool isLessThanOrEqualOperator() const { return _multiCharSymbol == "<="; }
    [[nodiscard]] bool isOpenBracket() const { return _symbol == '{'; }
    [[nodiscard]] bool isClosedBracket() const { return _symbol == '}'; }
 
    //Phase 2 - Colon and Comma added
    [[nodiscard]] bool isColon() const { return _symbol == ':'; }
    [[nodiscard]] bool isComma() const { return _symbol == ','; }

    void setIdentifier(std::string identifier) { _identifier = std::move(identifier); }
    [[nodiscard]] bool isIdentifier() const { return !_identifier.empty(); }
    [[nodiscard]] const std::string &identifier() const { return _identifier; }

    void setKeyword(Keyword keyword) { _keyword = keyword; }
    [[nodiscard]] bool isKeyword() const { return _keyword != Keyword::none; }
    [[nodiscard]] bool isForKeyword() const { return _keyword == Keyword::forKeyword; }
    [[nodiscard]] bool isPrintKeyword() const { return _keyword == Keyword::printKeyword; }

    // Phase 2 - In and Range added
    [[nodiscard]] bool isInKeyword() const { return _keyword == Keyword::inKeyword; }
    [[nodiscard]] bool isRangeKeyword() const { return _keyword == Keyword::rangeKeyword; }

    // Phase 3 - str, True, and False added
    [[nodiscard]] bool isStrKeyword() const { return _keyword == Keyword::strKeyword; }
    [[nodiscard]] bool isTrueKeyword() const { return _keyword == Keyword::trueKeyword; }
    [[nodiscard]] bool isFalseKeyword() const { return _keyword == Keyword::falseKeyword; }

    [[nodiscard]] Keyword keyword() const { return _keyword; }

    // Phase 3 - integers are now int64_t
    void setIntegerValue(std::int64_t value) {
        _integerValue = value;
        _isInteger = true;
    }

    // Phase 3 - Double and String literals added
    void setDoubleValue(double value) {
        _doubleValue = value;
        _isDouble = true;
    }

    void setStringValue(std::string value) {
        _stringValue = value;
        _isString = true;
    }

    [[nodiscard]] bool isInteger() const { return _isInteger; }

    // Phase 3 - checks for the new literal types
    [[nodiscard]] bool isString() const { return _isString; }
    [[nodiscard]] bool isDouble() const { return _isDouble; }

    [[nodiscard]] std::int64_t integerValue() const { return _integerValue; }

    // Phase 3 - getters for the new literal values
    [[nodiscard]] double doubleValue() const { return _doubleValue; }
    [[nodiscard]] std::string stringValue() const { return _stringValue; }

    void print(std::ostream &output) const;

private:
    std::string _identifier{};
    Keyword _keyword{Keyword::none};
    bool _eof{false};
    bool _newline{false};
    bool _isInteger{false};
    char _symbol{'\0'};

    // Phase 2 - Indent and Dedent added
    bool _indent{false};
    bool _dedent{false};

    // Phase 1 -Part 1:
    // Changed: starts as a truly empty string
    // the old {'\0'} made a string holding one invisible character, so it 
    // was not actually empty
    std::string _multiCharSymbol{};

    // Phase 3 - changed from int to int64_t
    std::int64_t _integerValue{0};

    // Phase 3 - flags and values for the new literal types
    bool _isString{false};
    bool _isDouble{false};
    std::string _stringValue{};
    double _doubleValue{0};

    std::size_t _lineNumber{0};
    std::size_t _columnNumber{0};
};

#endif // ALEXICALANALYZER_TOKEN_HPP