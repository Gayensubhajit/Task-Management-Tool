#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <direct.h> 

#define MAX_TASK_DESC 256
#define MAX_TASKS 100
#define DB_FILE "tasks.db"
#define GIT_DIR ".git"

// Custom strdup implementation
char* my_strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* new_str = malloc(len);
    if (new_str) {
        memcpy(new_str, str, len);
    }
    return new_str;
}

typedef struct {
    int id;
    char description[MAX_TASK_DESC];
    int completed;
    time_t created_time;
    time_t modified_time;
} Task;

Task tasks[MAX_TASKS];
int task_count = 0;

// GitSupportEnabled flag to disable Git functionality if not available
int git_support_enabled = 0;

int check_git_available() {
    // Just try using the git command directly - it will work if git is in the PATH
    if (system("git --version >nul 2>&1") == 0) {
        return 1;
    }
    return 0;
}

void init_git_repo() {
    // Check if git is available
    git_support_enabled = check_git_available();
    
    if (!git_support_enabled) {
        printf("Git is not available. Version control features will be disabled.\n");
        return;
    }
    
    // Configure git to allow safe directory - do this first
    char cwd[512];
    _getcwd(cwd, sizeof(cwd));
    char config_cmd[512];
    sprintf(config_cmd, "git config --global --add safe.directory \"%s\"", cwd);
    system(config_cmd);
    
    if (access(GIT_DIR, F_OK) != 0) {
        printf("Initializing git repository...\n");
        
        // Initialize git repo
        system("git init");
        
        // Create .gitignore
        FILE *gitignore = fopen(".gitignore", "w");
        if (gitignore) {
            fprintf(gitignore, "*.o\n*.exe\n");
            fclose(gitignore);
        }
        
        // Initial commit
        system("git add .");
        system("git commit -m \"Initial commit\"");
    }
}

void commit_changes(const char *message) {
    if (!git_support_enabled) return;
    
    char cmd[512];
    sprintf(cmd, "git add \"%s\"", DB_FILE);
    system(cmd);
    
    sprintf(cmd, "git commit -m \"%s\"", message);
    system(cmd);
}

/*
 * DB FORMAT:
 * - First 4 bytes: Integer with number of tasks
 * - For each task:
 *   - 4 bytes: task ID
 *   - 256 bytes: description (fixed size, null-padded)
 *   - 4 bytes: completed flag (0 or 1)
 *   - 8 bytes: created_time (time_t)
 *   - 8 bytes: modified_time (time_t)
 */

int save_tasks() {
    FILE *file = fopen(DB_FILE, "wb");
    if (!file) {
        printf("Error: Could not open database file for writing.\n");
        return 0;
    }
    
    // Write header with task count
    fwrite(&task_count, sizeof(int), 1, file);
    
    // Write each task
    for (int i = 0; i < task_count; i++) {
        fwrite(&tasks[i].id, sizeof(int), 1, file);
        fwrite(tasks[i].description, sizeof(char), MAX_TASK_DESC, file);
        fwrite(&tasks[i].completed, sizeof(int), 1, file);
        fwrite(&tasks[i].created_time, sizeof(time_t), 1, file);
        fwrite(&tasks[i].modified_time, sizeof(time_t), 1, file);
    }
    
    fclose(file);
    return 1;
}

int load_tasks() {
    FILE *file = fopen(DB_FILE, "rb");
    if (!file) {
        // It's okay if the file doesn't exist yet
        return 0;
    }
    
    // Read task count from header
    if (fread(&task_count, sizeof(int), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    
    // Check if task count seems valid
    if (task_count < 0 || task_count > MAX_TASKS) {
        printf("Error: Database file appears to be corrupted.\n");
        fclose(file);
        return 0;
    }
    
    // Read each task
    for (int i = 0; i < task_count; i++) {
        if (fread(&tasks[i].id, sizeof(int), 1, file) != 1 ||
            fread(tasks[i].description, sizeof(char), MAX_TASK_DESC, file) != MAX_TASK_DESC ||
            fread(&tasks[i].completed, sizeof(int), 1, file) != 1 ||
            fread(&tasks[i].created_time, sizeof(time_t), 1, file) != 1 ||
            fread(&tasks[i].modified_time, sizeof(time_t), 1, file) != 1) {
            
            printf("Error: Failed to read task %d from database.\n", i+1);
            fclose(file);
            return 0;
        }
    }
    
    fclose(file);
    return 1;
}

void add_task(const char *description) {
    if (task_count >= MAX_TASKS) {
        printf("Error: Maximum number of tasks reached.\n");
        return;
    }
    
    Task new_task;
    new_task.id = task_count > 0 ? tasks[task_count - 1].id + 1 : 1;
    strncpy(new_task.description, description, MAX_TASK_DESC - 1);
    new_task.description[MAX_TASK_DESC - 1] = '\0';
    new_task.completed = 0;
    new_task.created_time = time(NULL);
    new_task.modified_time = new_task.created_time;
    
    tasks[task_count++] = new_task;
    
    if (save_tasks()) {
        char commit_msg[300];
        sprintf(commit_msg, "Add task: %s", description);
        commit_changes(commit_msg);
        printf("Task added with ID: %d\n", new_task.id);
    }
}

void edit_task(int id, const char *new_description) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            strncpy(tasks[i].description, new_description, MAX_TASK_DESC - 1);
            tasks[i].description[MAX_TASK_DESC - 1] = '\0';
            tasks[i].modified_time = time(NULL);
            
            if (save_tasks()) {
                char commit_msg[300];
                sprintf(commit_msg, "Edit task %d", id);
                commit_changes(commit_msg);
                printf("Task %d updated.\n", id);
            }
            return;
        }
    }
    printf("Error: Task with ID %d not found.\n", id);
}

void complete_task(int id) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            tasks[i].completed = 1;
            tasks[i].modified_time = time(NULL);
            
            if (save_tasks()) {
                char commit_msg[300];
                sprintf(commit_msg, "Complete task %d", id);
                commit_changes(commit_msg);
                printf("Task %d marked as completed.\n", id);
            }
            return;
        }
    }
    printf("Error: Task with ID %d not found.\n", id);
}

void delete_task(int id) {
    int found = 0;
    
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            found = 1;
            
            // Shift all tasks after this one up by one position
            for (int j = i; j < task_count - 1; j++) {
                tasks[j] = tasks[j + 1];
            }
            
            task_count--;
            
            if (save_tasks()) {
                char commit_msg[300];
                sprintf(commit_msg, "Delete task %d", id);
                commit_changes(commit_msg);
                printf("Task %d deleted.\n", id);
            }
            return;
        }
    }
    
    if (!found) {
        printf("Error: Task with ID %d not found.\n", id);
    }
}

void list_tasks() {
    if (task_count == 0) {
        printf("No tasks available.\n");
        return;
    }
    
    printf("\n%-4s %-50s %-10s %-20s\n", "ID", "Description", "Status", "Last Modified");
    printf("------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < task_count; i++) {
        char time_str[26];
        struct tm *tm_info = localtime(&tasks[i].modified_time);
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("%-4d %-50s %-10s %-20s\n", 
               tasks[i].id, 
               tasks[i].description, 
               tasks[i].completed ? "Completed" : "Pending",
               time_str);
    }
    printf("\n");
}

void show_task_history(int id) {
    if (!git_support_enabled) {
        printf("Git is not available. Cannot show task history.\n");
        return;
    }
    
    char cmd[512];
    // Use 'type' instead of 'cat' on Windows, and fix pipe output
    sprintf(cmd, "git log --pretty=format:\"%%h %%ad %%s\" --date=short --grep=\"task %d\"", id);
    printf("History for Task %d:\n", id);
    system(cmd);
    printf("\n");
}

void print_help() {
    printf("\nTask Manager - Command Options:\n");
    printf("  add \"task description\"       - Add a new task\n");
    printf("  edit ID \"new description\"    - Edit a task's description\n");
    printf("  complete ID                  - Mark a task as completed\n");
    printf("  delete ID                    - Delete a task\n");
    printf("  list                         - Show all tasks\n");
    printf("  history ID                   - Show version history of a task\n");
    printf("  help                         - Show this help message\n");
    printf("  exit                         - Exit the program\n\n");
}

int main() {
    char command[512];
    int id;
    
    printf("Task Manager v1.0 (DB Edition)\n");
    
    init_git_repo();
    load_tasks();
    
    print_help();
    
    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }
        
        // Remove trailing newline
        command[strcspn(command, "\n")] = '\0';
        
        if (strncmp(command, "add ", 4) == 0) {
            const char *desc_start = command + 4;
            // Skip leading whitespace
            while (*desc_start && (*desc_start == ' ' || *desc_start == '"')) {
                desc_start++;
            }
            
            // Remove trailing quote if present
            char *desc_copy = my_strdup(desc_start);
            if (desc_copy[strlen(desc_copy) - 1] == '"') {
                desc_copy[strlen(desc_copy) - 1] = '\0';
            }
            
            add_task(desc_copy);
            free(desc_copy);
        } 
        else if (strncmp(command, "edit ", 5) == 0) {
            char *id_str = strtok(command + 5, " ");
            char *new_desc = strtok(NULL, "");
            
            if (id_str && new_desc) {
                // Skip leading whitespace and quotes
                while (*new_desc && (*new_desc == ' ' || *new_desc == '"')) {
                    new_desc++;
                }
                
                // Remove trailing quote if present
                char *desc_copy = my_strdup(new_desc);
                if (desc_copy[strlen(desc_copy) - 1] == '"') {
                    desc_copy[strlen(desc_copy) - 1] = '\0';
                }
                
                edit_task(atoi(id_str), desc_copy);
                free(desc_copy);
            } else {
                printf("Error: Invalid edit command format.\n");
            }
        } 
        else if (strncmp(command, "complete ", 9) == 0) {
            id = atoi(command + 9);
            complete_task(id);
        } 
        else if (strncmp(command, "delete ", 7) == 0) {
            id = atoi(command + 7);
            delete_task(id);
        } 
        else if (strcmp(command, "list") == 0) {
            list_tasks();
        } 
        else if (strncmp(command, "history ", 8) == 0) {
            id = atoi(command + 8);
            show_task_history(id);
        } 
        else if (strcmp(command, "help") == 0) {
            print_help();
        } 
        else if (strcmp(command, "exit") == 0) {
            break;
        } 
        else {
            printf("Unknown command. Type 'help' for available commands.\n");
        }
    }
    
    return 0;
} 