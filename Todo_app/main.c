#define _CRT_SECURE_NO_WARNING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_DESCRIPTION_LENGHT 256
#define INITIAL_CAPACITY 10

typedef struct{
    int id;
    char description[MAX_DESCRIPTION_LENGHT];
    bool completed;
}   TodoItem;

TodoItem* todo_list = NULL;
int todo_count = 0;
int todo_capacity = 0;
int next_id = 1;
//ensuring capacity for todo_capacity.
void ensure_capacity(){
    if (todo_count >= todo_capacity){
        todo_capacity *= 2;
        TodoItem *temp = (TodoItem*)realloc(todo_list, sizeof(TodoItem) * todo_capacity);
        if (temp == NULL){
            printf("Error: failed to reallocate memory");
            exit(EXIT_FAILURE);
        }

        todo_list = temp;
        printf("Info: resized todo list to capacity of %d \n", todo_capacity);
    }
}

void add_todo(const char* description){
    if (strlen(description) == 0){
        printf("Error: description cannot be empty. \n");
        return;
    }

    ensure_capacity();

    todo_list[todo_count].id = next_id++;
    strcpy(todo_list[todo_count].description, description);
    todo_list[todo_count].completed = false;
    todo_count++;
    printf("Success: TodoItem added: ID %d - \"%s\"\n", todo_list[todo_count - 1].id, todo_list[todo_count - 1].description);
}
//initializing the todo list
    void initialize_todo_list(){
        todo_capacity = INITIAL_CAPACITY;
        todo_list = (TodoItem*)malloc(sizeof(TodoItem) * todo_capacity);
        if (todo_list == NULL){
            printf("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        todo_count = 0;
        next_id = 1;
    }
    void free_todo_list() {
        if (todo_list != NULL){
            free(todo_list);
            todo_list = NULL;
        }
        
        todo_count = 0;
    }
//listing todos
void list_todos() { 
    if (todo_count == 0){
        printf("The todo list is empty. \n");
        return;
    }

    printf("\n--- Current Todo Items (%d items) ---\n", todo_count);
    for (int i = 0; i < todo_count; i++) {
        printf("ID: %-4d | Status: %-8s | Descriptions: %s\n",
            todo_list[i].id,
            todo_list[i].completed ? "COMPLETE" : "PENDING",
            todo_list[i].description);
    }
    printf("-------------------------\n");

}

void complete_todo(int id){
    for (int i = 0; i < todo_count; i++){
        if (todo_list[i].id == id){
            if (todo_list[i].completed){
                printf("Info: Todo ID %d is already complete.\n", id);
            }
            else {
                todo_list[i].completed = true;
                printf("Success: Todo ID %d marked as COMPLETE.\n", id);
            }
            return;
        }
    }
    printf("Error: Todo with ID %d not found.\n", id);
}

void delete_todo(int id){
    int found_index = -1;
    // Find the index of todo item with the given ID
    for (int i = 0; i < todo_count; i++) {
        if (todo_list[i].id == id) {
            found_index = i;
            break;
        }
    }
    if (found_index != -1) {
        //free(todo)
        for (int i = found_index; i < todo_count - 1; i++){
            todo_list[i] = todo_list[i + 1];
        }
        todo_count--;
        printf("Success: Todo with ID %d deleted.\n", id);
    }
    else {
        printf("Error: Todo with ID %d not found.\n", id);
    }
}

void save_todo_to_file(const char* filename){
    FILE* file = fopen(filename, "w"); // Opening in write mode, overwriting if exists
    if (file == NULL){
        perror("Error: Could not open file for writing");
        return;
    }
    for (int i = 0; i < todo_count; i++) {
        // Write each todo item's data to the file
        fprintf(file, "%d;%d;%s\n",
            todo_list[i].id,
            todo_list[i].completed ? 1 : 0, // 1 for true, 0 for false
            todo_list[i].description);
    }
    fclose(file); // Close the file
}

void load_todos_from_file(const char* filename) {
    FILE* file = fopen(filename, "r"); //Open file for reading
    if (file == NULL) {
        printf("Info: No existing '%s' file found. Starting with empty list.\n", filename);
        return;
    }

    free_todo_list();
    initialize_todo_list();

    char line[MAX_DESCRIPTION_LENGHT + 50]; //Reading buffer for file
    int loaded_id, loaded_completed;
    char loaded_description[MAX_DESCRIPTION_LENGHT];
    int max_id_found = 0;

    while (fgets(line, sizeof(line), file) != NULL){
        //Find the first semicolon to parse ID
        char* first_semi = strchr(line, ';');
        if (first_semi == NULL){
            fprintf(stderr, "Warning: Skipping malformed line in file (missing first ';'): %s");
            continue;
        }
        *first_semi = '\0'; // Null-terminate for the ID
        loaded_id = atoi(line); // Converting ID string to int "1" -> 1

        //Find the second semicolon to parse completion status
        char* second_semi = strchr(first_semi + 1, ';');
        if (second_semi == NULL) {
            fprintf(stderr, "Warning: Skipping malformed line in file (missing first ';'): %s");
            continue;
        }
        *second_semi = '\0'; // Null-terminate for the completed status part
        loaded_completed = atoi(first_semi + 1); // Convert status string to int

        // Get description part and remove trailing newline
        strncpy(loaded_description, second_semi + 1, sizeof(loaded_description) - 1);
        loaded_description[sizeof(loaded_description) - 1] = '\0'; // Ensuring null termination
        loaded_description[strcspn(loaded_description, "\n")] = 0; //Remove newline if present

        ensure_capacity();

        // Populate the TodoItem struct
        todo_list[todo_count].id = loaded_id;
        strcpy(todo_list[todo_count].description, loaded_description);
        todo_list[todo_count].completed = (loaded_completed == 1); // Converting 0/1 to bool
        todo_count++; //Increment count

        // Update the maximum ID found to ensure future ne IDs are unique
        if (loaded_id >= max_id_found) {
            max_id_found = loaded_id;
        }
    }
    next_id = max_id_found + 1; // Setting nextId to be one greater than the highest loaded ID

    fclose(file);
    printf("Info: Loaded %d todos from '%s'.\n", todo_count,filename);

}

int main(void){
    
    initialize_todo_list();

    load_todos_from_file("todos.txt");

    int choice;
    char temp_description[MAX_DESCRIPTION_LENGHT];
    int temp_id;

    do {
        //display the menu
        printf("\n--- Todo List Application --- \n");
        printf("1. Add Todo\n");
        printf("2. List Todos\n");
        printf("3. Mark Todo as Completed\n");
        printf("4. Delete Todo\n");
        printf("5. Save Todo\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1){
            printf("Invalid input. Please enter a number.\n");
            while(getchar() != '\n')
            continue; 
        }

        while (getchar() != '\n');

        switch (choice){
        case 1: //new todo
            printf("Enter the new todo description: ");
            fgets(temp_description, sizeof(temp_description), stdin);
            temp_description[strcspn(temp_description, "\n")] = 0;
            add_todo(temp_description);
            break;
        case 2: //list todo
            list_todos();
            break;
        case 3: //complete todo
            printf("enter the ID of todo to complete: ");
            if (scanf("%d", &temp_id) != 1){
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                continue;
            }

            while (getchar() != '\n');
            complete_todo(temp_id);
            break;
        case 4:
            printf("Enter ID of Todo to delete: ");
            if (scanf("%d", &temp_id) != 1){
                printf("Invalid input. Please enter a number.\n");
                while (getchar() != '\n');
                continue;
            }

            while (getchar() != '\n');
            delete_todo(temp_id);
            break;
        case 5:
            //save todos
            save_todo_to_file("todos.txt");
            printf("Todos save to todos.txt.\n");
            break;

        case 6: //exit
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again...\n");
        }

  } while(choice != 6);

  free_todo_list();

    return 0;
}
