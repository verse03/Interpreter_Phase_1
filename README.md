# Interpreter_Phase_1

## Team Members
===============

- Bryan Dang
- Peter Rubin

## Meetings

-Thursday, September 17, 9:30pm - 10:00pm
-Monday, September 21, 2:30pm - 4:00pm

## Other Communication

-Sunday, September 20: shared progress updates over messages

## Contributions: briefly describe each partners contributions 

We discussed implementation for how we would approach the project and we tested for similar results with the edge cases. And we worked through any additions to the project when comparing our code functions. The code implementation was different in some ways but made sure that the parsing and the functionality worked good.

## AI use: If AI was used for permitted conceptual explanations, list the tool, the topics discussed, and the extent of its use

Used it for researching concepts on parsing and how symbol tables work as background research for understanding the topic better. The tool we used was ChatGPT for understanding better the topic and for research backgrounds. 

From my understanding, parsing is the step where the interpreter checks that a program follows the language's grammar and builds a tree showing how its pieces fit together, before anything is actually running. The tokenizer first breaks the text into tokens like a, +, b, *, 2, and the parser then arranges them into a tree that shows how they group, so a + b * 2 ---> a + (b * 2)

## Build and running

make 

## tests ran on both projects
./interpreter.x tests/arthmetic_unary_regression.txt 
./interpreter.x tests/blank_lines_and_whitespace.txt
./interpreter.x tests/pnested_loops.txt 

## all tests

./interpreter.x tests/arithRegress.txt
./interpreter.x tests/blanklineswhitespace.txt
./interpreter.x tests/equality_ordering.txt
./interpreter.x tests/loops.txt
./interpreter.x tests/malformed_input.txt
./interpreter.x tests/nested_loops.txt
./interpreter.x tests/printstatements.txt
./interpreter.x tests/relational_operators.txt