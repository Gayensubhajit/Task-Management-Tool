# Task Management CLI Tool

A command-line task management tool written in C that allows users to add, edit, complete, and delete tasks. The tool includes version control using Git to track task changes and provide task history.

## Creator
Created by: Subhajit Gayen
Student Developer
Contact: subhajitgayen43@gmail.com
GitHub: https://github.com/silentbyte1
Also contrubuted Chirantan Biswas , Gunjan Basak

## Features

- Add, edit, complete, and delete tasks
- List all tasks with their status
- Automatic Git integration for version control
- View task history (all changes made to a specific task)
- Custom binary file storage system (not a standard database)
- Standalone database viewer tool

## Included Files

- `task_manager.c` - Source code for the main task management application
- `init-db.c` - Source code for the database viewer utility
- `tasks.db` - Binary database file containing all tasks
- `Makefile` - Build script (optional)

## Requirements

- C compiler (GCC recommended)
- Git (optional, for version control features)

### Easiest Method (Windows)
1. Double-click "taskmanager.exe" file
2. This will automatically run the pre-compiled executable with all dependencies


## Building

To compile the applications:

```bash
# Compile the main task manager
gcc -o task_manager task_manager.c

# Compile the database viewer
gcc -o view_binary_db init-db.c
```

Or if you have Make installed:

```bash
make
```

## Using the Task Manager

Run the main task management program:

```bash
./task_manager
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
1    Complete the project proposal                       Pending    2025-02-10 15:30:45
2    Research new technologies                           Pending    2025-02-10 15:31:02

> complete 1
Task 1 marked as completed.

> edit 2 "Research new technologies for upcoming project"
Task 2 updated.

> list
ID   Description                                        Status     Last Modified        
------------------------------------------------------------------------------------
1    Complete the project proposal                       Completed  2025-02-10 15:32:10
2    Research new technologies for upcoming project      Pending    2025-02-10 15:32:45

> history 2
History for Task 2:
a1b2c3d 2023-08-10 Edit task 2
e4f5g6h 2023-08-10 Add task: Research new technologies

> delete 1
Task 1 deleted.
```

## Using the Database Viewer

The database viewer is a standalone utility to view the contents of the tasks.db file:

```bash
./view_binary_db
```

This will display all tasks in the database in a tabular format, showing:
- Task ID
- Description
- Status (Pending/Completed)
- Creation time
- Last modified time

## Data Storage

The tasks are stored in a custom binary file named `tasks.db` in the same directory as the executable. This is not a standard database system like SQLite or MySQL, but rather a custom binary format designed specifically for this application. Each modification to the tasks is committed to Git with an appropriate message for version control.

### Database Format

The `tasks.db` file uses a simple custom binary format:
- Header: 4-byte integer indicating the number of tasks
- For each task:
  - 4 bytes: task ID
  - 256 bytes: description (fixed-length, null-padded)
  - 4 bytes: completed flag (0 or 1)
  - 8 bytes: creation timestamp
  - 8 bytes: last modified timestamp

Note: The file is not compatible with standard database management systems and requires the included viewer tool (`view_binary_db`) to inspect its contents outside the main application.

## Sharing and Distribution

When sharing this project with others:

1. Include all source files (`task_manager.c` and `init-db.c`)
2. Include the database file (`tasks.db`) if you want to share your existing tasks
3. Recipients can compile the programs on their machine and run them

## License

This project is open source and available under the MIT License.