#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TASK_DESC 256
#define MAX_TASKS 100
#define DB_FILE "tasks.db"

typedef struct {
    int id;
    char description[MAX_TASK_DESC];
    int completed;
    time_t created_time;
    time_t modified_time;
} Task;

// Function to read binary database
int read_binary_db(Task tasks[], int *task_count) {
    FILE *file = fopen(DB_FILE, "rb");
    if (!file) {
        printf("Error: Could not open binary database file for reading.\n");
        return 0;
    }
    
    // Read task count from header
    if (fread(task_count, sizeof(int), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    
    // Check if task count seems valid
    if (*task_count < 0 || *task_count > MAX_TASKS) {
        printf("Error: Binary database file appears to be corrupted.\n");
        fclose(file);
        return 0;
    }
    
    printf("Found %d tasks in database.\n", *task_count);
    
    // Read each task
    for (int i = 0; i < *task_count; i++) {
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

// Function to display tasks
void display_tasks(Task tasks[], int task_count) {
    if (task_count == 0) {
        printf("No tasks available.\n");
        return;
    }
    
    printf("\n%-4s %-50s %-10s %-20s %-20s\n", "ID", "Description", "Status", "Created", "Modified");
    printf("---------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < task_count; i++) {
        char created_time_str[26];
        char modified_time_str[26];
        
        struct tm *tm_created = localtime(&tasks[i].created_time);
        struct tm *tm_modified = localtime(&tasks[i].modified_time);
        
        strftime(created_time_str, 26, "%Y-%m-%d %H:%M:%S", tm_created);
        strftime(modified_time_str, 26, "%Y-%m-%d %H:%M:%S", tm_modified);
        
        printf("%-4d %-50s %-10s %-20s %-20s\n", 
               tasks[i].id, 
               tasks[i].description, 
               tasks[i].completed ? "Completed" : "Pending",
               created_time_str,
               modified_time_str);
    }
    printf("\n");
}

int main() {
    Task tasks[MAX_TASKS];
    int task_count = 0;
    
    printf("Task Database Viewer\n");
    printf("-------------------\n");
    
    // Read the binary database
    if (!read_binary_db(tasks, &task_count)) {
        printf("Failed to read database.\n");
        return 1;
    }
    
    // Display the tasks
    display_tasks(tasks, task_count);
    
    printf("Press Enter to exit...");
    getchar();
    
    return 0;
} 