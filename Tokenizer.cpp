#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

#include "Tokenizer.hpp"

bool Tokenizer::isDigit(char character) {
    return std::isdigit(static_cast<unsigned char>(character)) != 0;
}

bool Tokenizer::isIdentifierStart(char character) {
    return std::isalpha(static_cast<unsigned char>(character)) != 0 || character == '_';
}

bool Tokenizer::isIdentifierPart(char character) {
    return std::isalnum(static_cast<unsigned char>(character)) != 0 || character == '_';
}

bool Tokenizer::isDiscardedWhitespace(char character) {
    return character != '\n' && std::isspace(static_cast<unsigned char>(character)) != 0;
}

bool Tokenizer::getCharacter(char &character) {
    if (!inputStream.get(character))
        return false;

    if (character == '\n') {
        ++lineNumber;
        columnNumber = 1;
    } else {
        ++columnNumber;
    }
    return true;
}

std::string Tokenizer::readIdentifier(char firstCharacter) {
    std::string identifier{firstCharacter};
    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());
        if (!isIdentifierPart(character))
            break;
        getCharacter(character);
        identifier += character;
    }
    return identifier;
}

int Tokenizer::readInteger(char firstDigit) {
    int value = firstDigit - '0';
    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());
        if (!isDigit(character))
            break;

        const int digit = character - '0';
        if (value > (std::numeric_limits<int>::max() - digit) / 10) {
            std::cerr << "Integer literal is too large at line " << lineNumber
                      << ", column " << columnNumber << ".\n";
            std::exit(EXIT_FAILURE);
        }

        getCharacter(character);
        value = value * 10 + digit;
    }
    return value;
}

Tokenizer::Tokenizer(std::ifstream &stream) : inputStream{stream} {}

Token Tokenizer::getToken() {
    if (ungottenToken) {
        ungottenToken = false;
        return lastToken;
    }

    // Phase 2 - Return a waiting DEDENT first
    if (pendingDedents > 0) {
        --pendingDedents;
        Token token;
        token.setLocation(lineNumber, columnNumber);
        token.markAsDedent();
        tokens.push_back(token);
        return lastToken = token;
    }

    while (inputStream.peek() != std::char_traits<char>::eof()) {
        char character = static_cast<char>(inputStream.peek());

        // Phase 2 - Start of a line: count spaces
        if (atLineStart) {
            int spaces = 0;
            while (inputStream.peek() == ' ') {
                getCharacter(character);
                ++spaces;
            }

            if (inputStream.peek() == '\t') {
                std::cerr << "Tab in indentation at line " << lineNumber << ".\n";
                std::exit(EXIT_FAILURE);
            }

            // Blank line or end of file: skip
            if (inputStream.peek() == '\n' || inputStream.peek() == std::char_traits<char>::eof()) {
                getCharacter(character);
                continue;
            }

            atLineStart = false;
            Token token;
            token.setLocation(lineNumber, columnNumber);

            // More spaces: INDENT
            if (spaces > indentStack.back()) {
                indentStack.push_back(spaces);
                token.markAsIndent();
                tokens.push_back(token);
                return lastToken = token;
            }

            // Fewer spaces: queue one DEDENT per level popped
            while (spaces < indentStack.back()) {
                indentStack.pop_back();
                ++pendingDedents;
            }

            // Dedent Mismatch
            if (indentStack.back() != spaces) {
                std::cerr << "Indentation does not match an earlier level at line "
                          << lineNumber << ", column " << columnNumber << ".\n";
                std::exit(EXIT_FAILURE);
            }

            // Send the first DEDENT
            if (pendingDedents > 0)
                return getToken();

            continue;
        }


        if (isDiscardedWhitespace(character)) {
            getCharacter(character);
            continue;
        }

        if (character == '\n') {
            const auto newlineLine = lineNumber;
            const auto newlineColumn = columnNumber;
            getCharacter(character);

            if (lineContainsToken) {
                Token token;
                token.setLocation(newlineLine, newlineColumn);
                token.markAsNewline();
                lineContainsToken = false;
                //Phase 2: For atLineStart to work
                atLineStart = true;
                tokens.push_back(token);
                return lastToken = token;
            }

            // Newlines on blank or whitespace-only lines are insignificant.
            continue;
        }

        break;
    }

    Token token;
    token.setLocation(lineNumber, columnNumber);

    if (inputStream.peek() == std::char_traits<char>::eof()) {
        if (inputStream.bad()) {
            std::cerr << "Error while reading the input stream in Tokenizer.\n";
            std::exit(EXIT_FAILURE);
        }

        // Phase 2
        if (lineContainsToken) {
            token.markAsNewline();
            lineContainsToken = false;
        } else if (indentStack.size() > 1) {
            indentStack.pop_back();
            token.markAsDedent();
        } else {
            token.markAsEof();
        }

    } else {
        char character;
        getCharacter(character);

        // Only used to hold the second character of a two-character
        // operator after we decide to take it out of the file.
        char secondCharacter;

        if (isDigit(character)) {
            token.setIntegerValue(readInteger(character));
        
        // Part 1:
        // ADDED: '=' was moved out of the plain symbol list below, because it might really be the start of "=="
        // peak() looks at the next character without removing it from the file

        } else if (character == '=') {
            if (inputStream.peek() == '=') {
                getCharacter(secondCharacter);
                token.setMultiCharSymbol("==");
            } else {
                token.setSymbol('=');
            }

        // Part 1:
        // Added: '!' is only allowed when an '=' follows it
        // A '!' by itself is not a valid token
        } else if (character == '!') {
            if (inputStream.peek() == '=') {
                getCharacter(secondCharacter);
                token.setMultiCharSymbol("!=");
            } else {
                std::cerr << "Unknown character in input at line " << token.lineNumber()
                          << ", column " << token.columnNumber() << ": '"
                          << character << "'.\n";
                std::exit(EXIT_FAILURE);
            }
        
        // Part 1:
        // Added: '>' by itself, or ">=" when an '=' follows it
        } else if (character == '>') {
            if (inputStream.peek() == '=') {
                getCharacter(secondCharacter);
                token.setMultiCharSymbol(">=");
            } else {
                token.setSymbol('>');
            }

        // Part 1:
        // Added: '<' by itself, or "<=" when an '=' follows it
        } else if (character == '<') {
            if (inputStream.peek() == '=') {
                getCharacter(secondCharacter);
                token.setMultiCharSymbol("<=");
            } else {
                token.setSymbol('<');
            }
        
        //Part 1:
        //Changed: '=' removed from this list, and '{' and '}' added.
        //Phase 2: - added ':' and ',' 
        } else if (character == '+' || character == '-' || character == '*' ||
                   character == '/' || character == '%' || character == ';' ||
                   character == '(' || character == ')' || character == '{' || 
                   character == '}' || character == ':' || character == ',') {
            token.setSymbol(character);
        } else if (isIdentifierStart(character)) {
            std::string identifier = readIdentifier(character);
            if (identifier == "for")
                token.setKeyword(Keyword::forKeyword);
            else if (identifier == "print")
                token.setKeyword(Keyword::printKeyword);

            // Phase 2: - added in and range
            else if (identifier == "in")
                token.setKeyword(Keyword::inKeyword);
            else if (identifier == "range")
                token.setKeyword(Keyword::rangeKeyword);
            
            else
                token.setIdentifier(std::move(identifier));
        } else {
            std::cerr << "Unknown character in input at line " << token.lineNumber()
                      << ", column " << token.columnNumber() << ": '"
                      << character << "'.\n";
            std::exit(EXIT_FAILURE);
        }

        lineContainsToken = true;
    }

    tokens.push_back(token);
    return lastToken = token;
}

void Tokenizer::ungetToken() {
    if (ungottenToken) {
        std::cerr << "Tokenizer supports only one ungotten token.\n";
        std::exit(EXIT_FAILURE);
    }
    ungottenToken = true;
}

void Tokenizer::printProcessedTokens(std::ostream &output) const {
    for (const auto &token : tokens) {
        token.print(output);
        output << '\n';
    }
}
