# Interpreter_Phase_1

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
## Other Communication
Phase 1:
-Sunday, September 20: shared progress updates over messages

Phase 2:
Sun 10/5: (Discord) Shared progress/updates/ comparing code with each other - Token.cpp/Token.hpp/Tokenizer.hpp/Tokenizer.cpp
Wed 10/7: (Discord) Updates/comparing code with each other - Tokenizer.cpp/Statements.hpp
Thurs: (Discord) Updated code - Statements.hpp/Statement.cpp
Fri 10/9: (Discord) Added code/update -Statement.cpp/Parser.cpp/Parser.hpp/test file

## Contributions: briefly describe each partners contributions
Both Partners attempted every change on their own and then compared code line by line, so every added or edited line was written and reviewed by both of us.

Peter: contributed ForStatement with print output. Test files for range class and additional dedents. Implemented to code functions for isIdentifier and marking Indents and dedents. 

Bryan: led the tokenizer changes new tokens, the indentation stacks, INDENT/DEDENT, blank lines, tab and mismatch errors, and EOF handling. Also helped running checks for any dedents. 


## AI use: If AI was used for permitted conceptual explanations, list the tool, the topics discussed, and the extent of its use

We used ChatGPT as a learning aid to better understand concepts such as INDENT and DEDENT, suites, and range expressions for the for statements. And we also reviewed the lecture slides and project instructions to understand the grammar requirements.

Tokenizer: INDENT and DEDENT
Our first attempt used integer counters and assumed every level was 4 spaces. It produced extra INDENTs, no DEDENTs, and could not handle other indent widths. After reviewing the lecture slides, we switched from integers to a stack of space counts starting at `[0]`. At the start of each line, the tokenizer counts the leading spaces and compares them to the top of the stack. More spaces pushes the count and returns INDENT. Fewer spaces pops levels until the top matches, counting one DEDENT per pop in `pendingDedents`, since `getToken()` returns only one token per call. A line that matches no earlier level is an error, as is a tab in the indentation. At the end of the file, it sends a final NEWLINE if needed, then one DEDENT per open block, then EOF.

Parser: suite()
Following the revised grammar, `suite()` checks for NEWLINE and INDENT, calls `statements()` to read the indented lines, then checks for DEDENT. The tricky part was how `statements()` knows to stop: when it reaches the DEDENT, it puts it back with `ungetToken()` so `suite()` can check it. This is also what makes nested loops work.

Parser: rangeExpression() and rangeArguments()
Range parsing is split into two functions, one per grammar rule. `rangeExpression()` checks for `range`, `(`, and `)`, and calls `rangeArguments()` for what is inside. `rangeArguments()` reads one expression, and each comma means another one follows, returning the 1, 2, or 3 argument form of `RangeExpression`. With one argument, that expression is the stop value, so start defaults to 0.



