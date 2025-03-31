# Task Management CLI Tool

A command-line task management tool written in C that allows users to add, edit, complete, and delete tasks. The tool includes version control using Git to track task changes and provide task history.

## Features

- Add, edit, complete, and delete tasks
- List all tasks with their status
- Automatic Git integration for version control
- View task history (all changes made to a specific task)
- Efficient database-style storage in .db file

## Requirements

- C compiler (GCC recommended)
- Git
- Make (optional, for using the Makefile)

## Building

To build the project:

```
make
```

This will create an executable called `taskman`.

## Usage

Run the program:

```
./taskman
```

### Available Commands

- `add "task description"` - Add a new task
- `edit ID "new description"` - Edit a task's description
- `complete ID` - Mark a task as completed
- `delete ID` - Delete a task
- `list` - Show all tasks
- `history ID` - Show version history of a task
- `help` - Show help message
- `exit` - Exit the program

### Examples

```
> add "Complete the project proposal"
Task added with ID: 1

> add "Research new technologies"
Task added with ID: 2

> list
ID   Description                                        Status     Last Modified        
------------------------------------------------------------------------------------
1    Complete the project proposal                       Pending    2023-08-10 15:30:45
2    Research new technologies                           Pending    2023-08-10 15:31:02

> complete 1
Task 1 marked as completed.

> edit 2 "Research new technologies for upcoming project"
Task 2 updated.

> list
ID   Description                                        Status     Last Modified        
------------------------------------------------------------------------------------
1    Complete the project proposal                       Completed  2023-08-10 15:32:10
2    Research new technologies for upcoming project      Pending    2023-08-10 15:32:45

> history 2
History for Task 2:
a1b2c3d 2023-08-10 Edit task 2
e4f5g6h 2023-08-10 Add task: Research new technologies

> delete 1
Task 1 deleted.
```

## Data Storage

The tasks are stored in a binary database file named `tasks.db` in the same directory as the executable. Each modification to the tasks is committed to Git with an appropriate message.

### Database Format

The database file uses a simple binary format:
- Header: 4-byte integer indicating the number of tasks
- For each task:
  - 4 bytes: task ID
  - 256 bytes: description (fixed-length, null-padded)
  - 4 bytes: completed flag (0 or 1)
  - 8 bytes: creation timestamp
  - 8 bytes: last modified timestamp

## License

This project is open source and available under the MIT License. 