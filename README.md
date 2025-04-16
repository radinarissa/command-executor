# Command Executor

A simple C program that executes two commands sequentially and logs the first command to a file if it's successful.

## Features

- Sequential command execution
- Conditional logging of successful commands
- Unicode/Cyrillic support
- Detailed console output with timestamps

## Requirements

- Windows 7/8/10/11
- GCC compiler (MinGW)

## Installation
gcc -Wall -std=c99 -o CommandExecutor.exe command_executor.c

## Usage
CommandExecutor.exe <command1> <command2> <filename>

### Example:
CommandExecutor.exe dir echo output.txt

## How It Works

The program executes both commands in order, logging the first command to the specified file only if it completes successfully. All operations are logged to the console with timestamps.
