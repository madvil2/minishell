# Minishell

A minimal shell implementation written in C that mimics the behavior of bash. This project is part of the 42 School curriculum and demonstrates understanding of processes, file descriptors, and system calls.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Built-in Commands](#built-in-commands)
- [Grammar and Parsing](#grammar-and-parsing)
- [Project Structure](#project-structure)
- [Testing](#testing)
- [Authors](#authors)

## Overview

Minishell is a simplified shell that provides a command-line interface for interacting with the operating system. It supports command execution, piping, redirections, environment variable expansion, and several built-in commands.

## Features

### Core Functionality
- **Interactive command prompt** with readline support
- **Command execution** with PATH resolution
- **Environment variable expansion** (`$VAR` syntax)
- **Signal handling** (Ctrl+C, Ctrl+D, Ctrl+\)
- **Exit status management**

### Advanced Features
- **Pipes** (`|`) for chaining commands
- **Logical operators** (`&&`, `||`) for conditional execution
- **Parentheses** for command grouping
- **Input/Output redirections**:
  - `<` (input redirection)
  - `>` (output redirection)
  - `>>` (append redirection)
  - `<<` (heredoc)
- **Globbing** (wildcard expansion)
- **Quote handling** (single and double quotes)

## Installation

### Prerequisites
- GCC compiler
- GNU Readline library
- Make

### Build Instructions

1. Clone the repository:
```bash
git clone <repository-url>
cd minishell
```

2. Build the project:
```bash
make
```

The build process will automatically clone and compile the required `libft` library.

3. Run the shell:
```bash
./minishell
```

### Clean Up
```bash
make clean    # Remove object files
make fclean   # Remove executable and object files
make re       # Rebuild everything
```

## Usage

Once started, minishell provides an interactive prompt where you can enter commands:

```bash
type shit: ls -la
type shit: echo "Hello, World!"
type shit: cat file.txt | grep "pattern" | wc -l
type shit: export MY_VAR="value" && echo $MY_VAR
```

## Built-in Commands

Minishell implements the following built-in commands:

| Command | Description | Usage |
|---------|-------------|-------|
| `echo` | Display text | `echo [-n] [text...]` |
| `cd` | Change directory | `cd [directory]` |
| `pwd` | Print working directory | `pwd` |
| `export` | Set environment variables | `export [VAR=value]` |
| `unset` | Unset environment variables | `unset [VAR]` |
| `env` | Display environment variables | `env` |
| `exit` | Exit the shell | `exit [status]` |

## Grammar and Parsing

Minishell uses a context-free grammar with 23 production rules to parse command lines. The parser implements a pushdown automaton (PDA) approach for syntax analysis.

### Grammar Rules

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
11. <simple_command> ::= ( <and_or_sequence> ) <compound_command_tail>
12. <simple_command> ::= <io_redirect> TOK_WORD <simple_command_tail>
13. <simple_command_tail> ::= TOK_WORD <simple_command_tail>
14. <simple_command> ::= TOK_WORD <simple_command_tail>
15. <simple_command_tail> ::= <io_redirect> TOK_WORD <simple_command_tail>
16. <simple_command_tail> ::= ''
17. <compound_command_tail> ::= <io_redirect> TOK_WORD <compound_command_tail>
18. <compound_command_tail> ::= ''
19. <io_redirect> ::= <
20. <io_redirect> ::= >
21. <io_redirect> ::= <<
22. <io_redirect> ::= >>
```

### Parsing Table Analysis

The parser builds First and Follow sets for each nonterminal:

| Nonterminal            | Nullable? | First set                              | Follow set                         |
|------------------------|-----------|----------------------------------------|------------------------------------|
| S                      | no        | "(", TOK_WORD, "<", ">", "<<", ">>", $ |                                    |
| complete_command       | yes       | "(", TOK_WORD, "<", ">", "<<", ">>"    | $                                  |
| and_or_sequence        | no        | "(", TOK_WORD, "<", ">", "<<", ">>"    | ")", $                             |
| and_or_sequence_tail   | yes       | "||", "&&"                             | ")", $                             |
| and_or                 | no        | "||", "&&"                             | "(", TOK_WORD, "<", ">", "<<", ">>" |
| pipe_sequence          | no        | "(", TOK_WORD, "<", ">", "<<", ">>"    | "||", "&&", ")", $                 |
| pipe_sequence_tail     | yes       | "|"                                    | "||", "&&", ")", $                 |
| simple_command         | no        | "(", TOK_WORD, "<", ">", "<<", ">>"    | "|", "||", "&&", ")", $            |
| simple_command_tail    | yes       | TOK_WORD, "<", ">", "<<", ">>"         | "|", "||", "&&", ")", $            |
| compound_command_tail  | yes       | "<", ">", "<<", ">>"                   | "|", "||", "&&", ")", $            |
| io_redirect            | no        | "<", ">", "<<", ">>"                   | TOK_WORD                           |

## Project Structure

```
minishell/
├── includes/
│   └── minishell.h          # Main header file with grammar definitions
├── srcs/
│   ├── minishell.c          # Main entry point and shell loop
│   ├── envp.c               # Environment variable management
│   ├── builtins/            # Built-in command implementations
│   │   ├── cd.c             # Change directory command
│   │   ├── cd_utils.c       # CD utility functions
│   │   ├── echo.c           # Echo command
│   │   ├── env.c            # Environment display command
│   │   ├── exit.c           # Exit command
│   │   ├── export.c         # Export command
│   │   ├── pwd.c            # Print working directory
│   │   └── unset.c          # Unset environment variables
│   ├── parsing/             # Tokenization and parsing modules
│   │   ├── tokenize.c       # Main tokenizer
│   │   ├── tokenize2.c      # Additional tokenization functions
│   │   ├── pda_parse.c      # Pushdown automaton parser
│   │   ├── pda_parse2.c     # Parser utilities
│   │   ├── pda_parse3.c     # Additional parser functions
│   │   ├── expand_env_vars.c # Environment variable expansion
│   │   ├── globbing.c       # Wildcard expansion
│   │   ├── globbing2.c      # Additional globbing functions
│   │   ├── form_words.c     # Word formation
│   │   ├── flattening.c     # Parse tree flattening
│   │   └── utils.c          # Parsing utilities
│   └── executor/            # Command execution modules
│       ├── execute_ptree.c  # Parse tree execution
│       ├── execute_pipe_sequence.c # Pipe handling
│       ├── execute_single_command.c # Single command execution
│       ├── execute_simple_command.c # Simple command execution
│       ├── execute_simple_command_wrapper.c # Command wrapper
│       ├── exit_status.c    # Exit status management
│       ├── heredoc.c        # Heredoc implementation
│       ├── redirections.c   # I/O redirection handling
│       ├── redirections2.c  # Additional redirection functions
│       ├── redirections3.c  # More redirection utilities
│       ├── signals.c        # Signal handling
│       └── signals2.c       # Additional signal functions
├── libft/                   # Custom C library (submodule)
├── testers/                 # Test suites
│   ├── minishell_tester/    # Comprehensive test suite
│   ├── tester/              # Basic functionality tests
│   └── tester2/             # Advanced feature tests
├── Makefile                 # Build configuration
└── README.md               # This file
```

## Testing

The project includes several testing frameworks in the `testers/` directory:

- **minishell_tester/**: Comprehensive test suite
- **tester/**: Basic functionality tests  
- **tester2/**: Advanced feature tests with Python test runner

To run tests:
```bash
cd testers/tester2
python3 minitester.py
```

## Technical Details

### Memory Management
- Custom garbage collector for automatic memory cleanup
- Separate allocators for permanent and temporary memory
- Memory leak prevention and cleanup on exit

### Signal Handling
- Proper handling of SIGINT (Ctrl+C)
- SIGQUIT (Ctrl+\) handling
- EOF (Ctrl+D) detection
- Interactive vs non-interactive signal behavior

### Process Management
- Fork/exec for external command execution
- Proper process synchronization
- Exit status propagation
- Pipe management with file descriptor handling

### Parsing Architecture
- Lexical analysis with tokenization
- Syntax analysis using pushdown automaton
- Parse tree construction and flattening
- Environment variable expansion
- Globbing and quote removal

## Authors

- **kokaimov** - Main developer (kokaimov@student.42berlin.de)

## License

This project is part of the 42 School curriculum and follows the school's academic guidelines.