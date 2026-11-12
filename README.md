# Interpreter_Phase_3


## Team Members
===============

- Bryan Dang
- Peter Rubin

## Meetings
Phase 1:
-Thursday, September 17, 9:30pm - 10:00pm
-Monday, September 21, 2:30pm - 4:00pm

Phase 2:
-Saturday, October 3, 2:00pm - 3pm (Discord)
-Tuesday, October 6, (in Person) talked about tokenizer for indent and dedent, and range
-Friday, October 9, 3:30pm - 4:30 (discord)

Phase 3:
- Wednesday, October 21, 9pm-10pm 
- Sunday, October 25, 4pm-

## Other Communication
Phase 1:
-Sunday, September 20: shared progress updates over messages

Phase 2:
Sun 10/5: (Discord) Shared progress/updates/ comparing code with each other - Token.cpp/Token.hpp/Tokenizer.hpp/Tokenizer.cpp
Wed 10/7: (Discord) Updates/comparing code with each other - Tokenizer.cpp/Statements.hpp
Thurs: (Discord) Updated code - Statements.hpp/Statement.cpp
Fri 10/9: (Discord) Added code/update -Statement.cpp/Parser.cpp/Parser.hpp/test file

Phase 3:
- Friday 10/23: (discord) shared progress, updates, compared code
- Saturday 10/24: (discord) continued implementation and updating
- Sunday 10/25: (discord) test case 

## Contributions: briefly describe each partners contributions
Both Partners attempted every change on their own and then compared code line by line, so every added or edited line was written and reviewed by both of us.

Peter: extending valueDescriptor functionality and string conversion in tokenizer classes, having toString, Value, and clone based on their data type classes, extending symbol table to take in more value types. Making sure tests on double, defined variables, and comparisons work. Adding functions that recognise floor, double and string operations, and validating data types taking from functions on different types. 

Bryan: working on range class, and updating the SymbolTable to store and own value descriptors, deleting replaced values, adding the destructor, and blocking copies. Updating the token for the new literals, keywords, and the // operator, and changing the range to use int64_t with integer-only arguments. Working on Expr to handle type checking, integer-to-double promotion, python style // and % and the ownership of descriptors.


## AI use: If AI was used for permitted conceptual explanations, list the tool, the topics discussed, and the extent of its use

We used ChatGPT as a learning aid to better understand concepts such as value descriptors and literals. And we also reviewed the project instructions to understand the grammar requirements.

Value Descriptor:
We learned that once values become ValueDescriptor on the heap, each one needs exactly one owner who deletes it. Every evaluate returns a new descriptor that the caller owns. A variable returns a clone, because handing out the symbol table’s own pointer would let the caller delete it. Operators and print delete their values using them and assignment gives its value to the symbol table, which deletes old values on reassignment, and the rest in its destructor. 

Literals 
We learned that it can take string and boolean literals and that it takes in int64_t, and that the range is checked for each digit added. After reading the digits, a decimal point (period) means that the number is a double and must have a digit between the decimal point otherwise its rejected. For example 5. And .5 are both rejected, and so is 1.2e3 since exponent notation is not allowed. 
