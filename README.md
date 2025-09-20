# Simple UNIX Shell

A basic implementation of a UNIX shell written in C that supports fundamental shell operations including command execution, I/O redirection, pipes, and background processes.

## Features

- **Command Execution**: Execute standard UNIX commands and programs
- **I/O Redirection**: 
  - Output redirection using `>` operator
  - Input redirection using `<` operator
- **Pipes**: Chain commands together using `|` operator
- **Background Processing**: Run commands in background using `&` operator
- **Interactive Prompt**: Clean command-line interface with `ssh>>` prompt
- **Exit Command**: Type `exit` to quit the shell

## Compilation

```bash
gcc -o simple-shell simple-shell.c
```

## Usage

Run the compiled executable:

```bash
./simple-shell
```

The shell will display a prompt `ssh>>` where you can enter commands.

### Command Examples

#### Basic Commands
```bash
ssh>> ls
ssh>> pwd
ssh>> echo "Hello World"
```

#### Background Processing
```bash
ssh>> sleep 10 &
ssh>> ps
```

#### Output Redirection
```bash
ssh>> ls > output.txt
ssh>> echo "Hello" > greeting.txt
```

#### Input Redirection
```bash
ssh>> sort < unsortedList
ssh>> wc < textfile.txt
```

#### Pipes
```bash
ssh>> ls | grep .txt
ssh>> cat file.txt | sort | uniq
```

#### Combined Operations
```bash
ssh>> cat < input.txt | sort > sorted_output.txt
```

#### Exit Shell
```bash
ssh>> exit
```

## Code Structure

### Main Components

- **`main()`**: Main program loop that handles user input and command processing
- **`prompt_cmd()`**: Displays the shell prompt and reads user input
- **`parse_cmd()`**: Parses the input command into arguments
- **`execute_cmd()`**: Handles command execution with I/O redirection
- **`execute_cmd_with_pipe()`**: Handles pipe operations between commands

### Key Constants

- `MAX_LENGTH`: Maximum command length (80 characters)
- `MAX_LENGTH_PARAMS`: Maximum number of parameters (41)
- `MAX_HISTORY`: Maximum history entries (10) - *Note: History feature not yet implemented*

### Global Variables

- `ampersand`: Flag to track background process requests
- `hasPipe`: Flag to indicate presence of pipe operations

## Technical Implementation

### Process Management
- Uses `fork()` to create child processes for command execution
- Parent process waits for child completion unless `&` is specified
- Proper error handling for fork failures

### I/O Redirection
- Uses `open()`, `dup2()`, and `close()` system calls
- Supports both input (`<`) and output (`>`) redirection
- File permissions: `S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP`

### Pipe Implementation
- Creates pipe using `pipe()` system call
- Separates commands at `|` symbol
- Uses `dup2()` to redirect stdin/stdout between processes

## Limitations

1. **History Feature**: History array is defined but not implemented
2. **Error Recovery**: Limited error recovery for malformed commands
3. **Advanced Features**: No support for:
   - Command aliases
   - Environment variable expansion
   - Job control beyond basic background processing
   - Multiple pipes in a single command
4. **Input Validation**: Minimal validation of command syntax

## Sample Files

The project includes `unsortedList` as a sample file for testing input redirection and pipe operations.

## Future Enhancements

- Implement command history functionality
- Add support for multiple pipes
- Enhance error handling and user feedback
- Add job control features
- Support for environment variables
- Command completion and history navigation

## Author

CS333 Project 2 - Simple Shell Implementation
