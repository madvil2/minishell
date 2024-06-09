Grammar rules:
```
0. <S> ::= <complete_command> $
1. <complete_command> ::= ''
2. <complete_command> ::= <and_or_sequence>
3. <and_or_sequence> ::= <pipe_sequence> <and_or_sequence_tail>
4. <and_or_sequence_tail> ::= ''
5. <and_or_sequence_tail> ::= <and_or> <pipe_sequence> <and_or_sequence_tail>
6. <and_or> ::= &&
7. <and_or> ::= ||
8. <pipe_sequence> ::= <simple_command> <pipe_sequence_tail>
9. <pipe_sequence_tail> ::= ''
10. <pipe_sequence_tail> ::= | <simple_command> <pipe_sequence_tail>
11. <simple_command> ::= ( <and_or_sequence> )
12. <simple_command> ::= <io_redirect> TOK_WORD <simple_command_tail>
13. <simple_command_tail> ::= <io_redirect> TOK_WORD <simple_command_tail>
14. <simple_command_tail> ::= ''
15. <io_redirect> ::= ''
16. <io_redirect> ::= <
17. <io_redirect> ::= >
18. <io_redirect> ::= <<
19. <io_redirect> ::= >>
```

Building parsing table:
```
| Nonterminal            | Nullable?     | First set            | Follow set  
|------------------------|---------------|----------------------|-------------
| 0.S                    | no            | Fi(1), $             |             
| 1.complete_command     | yes           | Fi(2)                | $           
| 2.and_or_sequence      | no            | Fi(5)                | Fo(1), ")"  
| 3.and_or_sequence_tail | yes           | Fi(4)                | Fo(2)       
| 4.and_or               | no            | "||", "&&"           | Fi(5)       
| 5.pipe_sequence        | no            | Fi(7)                | Fi(3), Fo(2)
| 6.pipe_sequence_tail   | yes           | "|"                  | Fo(5)       
| 7.simple_command       | no            | "(", TOK_WORD, Fi(9) | Fi(6), Fo(5)
| 8.simple_command_tail  | yes           | TOK_WORD, Fi(9)      | Fo(7)       
| 9.io_redirect          | yes           | "<", ">", "<<", ">>" | TOK_WORD    
```
After calculating sets:
```
| Nonterminal            | Nullable?     | First set                              | Follow set
|------------------------|---------------|----------------------------------------|------------------------------------
| 0.<S>                  | no            | "(", TOK_WORD, "<", ">", "<<", ">>", $ |                                    
| 1.complete_command     | yes           | "(", TOK_WORD, "<", ">", "<<", ">>"    | $                                  
| 2.and_or_sequence      | no            | "(", TOK_WORD, "<", ">", "<<", ">>"    | ")" ,$                             
| 3.and_or_sequence_tail | yes           | "||", "&&"                             | ")" ,$                             
| 4.and_or               | no            | "||", "&&"                             | "(", TOK_WORD, "<", ">", "<<", ">>"
| 5.pipe_sequence        | no            | "(", TOK_WORD, "<", ">", "<<", ">>"    |  "||", "&&", ")" ,$                
| 6.pipe_sequence_tail   | yes           | "|"                                    | "||", "&&", ")" ,$                 
| 7.simple_command       | no            | "(", TOK_WORD, "<", ">", "<<", ">>"    | "|" "||", "&&", ")" ,$             
| 8.simple_command_tail  | yes           | TOK_WORD, "<", ">", "<<", ">>"         | "|" "||", "&&", ")" ,$              
| 9.io_redirect          | yes           | "<", ">", "<<", ">>"                   | TOK_WORD                           
```
Transition table:
```
|                     | TOK_EOL | TOK_AND | TOK_OR | TOK_PIPE | TOK_L_PAREN | TOK_R_PAREN | TOK_WORD | TOK_OVERWRITE | TOK_APPEND | TOK_INPUT | TOK_HEREDOC 
|---------------------|---------|---------|--------|----------|-------------|-------------|----------|---------------|------------|-----------|-------------
|S                    |    0    |         |        |          |      0      |             |    0     |       0       |     0      |     0     |      0      
|COMPLETE_COMMAND     |    1    |         |        |          |      2      |             |    2     |       2       |     2      |     2     |      2      
|AND_OR_SEQUENCE      |         |         |        |          |      3      |             |    3     |       3       |     3      |     3     |      3      
|AND_OR_SEQUENCE_TAIL |    4    |    5    |    5   |          |             |      4      |          |               |            |           |             
|AND_OR               |         |    6    |    7   |          |             |             |          |               |            |           |             
|PIPE_SEQUENCE        |         |         |        |          |      8      |             |    8     |       8       |     8      |     8     |      8      
|PIPE_SEQUENCE_TAIL   |    9    |    9    |    9   |    10    |             |      9      |          |               |            |           |             
|SIMPLE_COMMAND       |         |         |        |          |     11      |             |    12    |      12       |    12      |    12     |     12      
|SIMPLE_COMMAND_TAIL  |    14   |   14    |   14   |    14    |             |     14      |    13    |      13       |    13      |    13     |     13      
|IO_REDIRECT          |         |         |        |          |             |             |    15    |      17       |    19      |    16     |     18      
```