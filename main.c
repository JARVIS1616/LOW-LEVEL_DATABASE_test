#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 1024
#define ANSI_COLOR_GREEN   "\033[0;32m"
#define ANSI_COLOR_RESET   "\033[0m"
#define DB_FILE "database.tlv"

typedef struct Node {
    char *key;
    char *value;
    struct Node *next;
} Node;

Node* hash_table[TABLE_SIZE];

unsigned long hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

void clean_buffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF){
    }
}

void imput_string(char *array, int size, char *TEXT){
    printf("%s", TEXT);

    if (fgets(array, size, stdin) == NULL){
        return;
    }
    int position = strcspn(array, "\n");

    if (array[position] == '\n'){
        array[position] = '\0';
    }
    else{
        clean_buffer();
    }
}

bool comand_compare(char *array, char *defined_comand) {
    int comparison = strcmp(array, defined_comand);
    if (comparison == 0) {
        return true;
    }
    return false;
}

void print_help(void) {
    printf("QUIT - for exiting the program\nHELP - for listing comands\nSET - for creatng/seting keys\nGET - for geting a value of keys\nDEL - for deleting a key\nLIST - for listing all keys\n");
    printf("type [GET][SET][DEL] comands like this:\n>SET speed 25\n>GET speed\n>DEL speed\nfor other comands write this:\n>HELP\n>QUIT\n>LIST\n");
}

void hash_table_insert(char *key, char *value) {
    unsigned long index = hash((unsigned char*)key);

    Node *curr = hash_table[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            free(curr->value);
            curr->value = strdup(value);
            return;
        }
        curr = curr->next;
    }

    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Critical error: Memory allocation failed!\n");
        return;
    }
    new_node->key = strdup(key);
    new_node->value = strdup(value);

    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

Node* hash_table_lookup(char *key) {
    unsigned long index = hash((unsigned char*)key);

    Node *curr = hash_table[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

bool hash_table_delete(char *key) {
    unsigned long index = hash((unsigned char*)key);

    Node *curr = hash_table[index];
    Node *prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (prev == NULL) {
                hash_table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->key);
            free(curr->value);
            free(curr);
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

void save_database(void) {
    FILE *file = fopen(DB_FILE, "wb");
    if (file == NULL) {
        printf("ERROR: Could not open file for writing!\n");
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *curr = hash_table[i];
        while (curr != NULL) {
            unsigned char key_len = (unsigned char)strlen(curr->key);
            unsigned char val_len = (unsigned char)strlen(curr->value);
            unsigned char type = 0x00;

            fwrite(&key_len, sizeof(unsigned char), 1, file);
            fwrite(curr->key, sizeof(char), key_len, file);
            fwrite(&type, sizeof(unsigned char), 1, file);
            fwrite(&val_len, sizeof(unsigned char), 1, file);
            fwrite(curr->value, sizeof(char), val_len, file);

            curr = curr->next;
        }
    }

    fclose(file);
}

void load_database(void) {
    FILE *file = fopen(DB_FILE, "rb");
    if (file == NULL) {
        printf("No existing database file found. Starting with a fresh database.\n");
        return;
    }

    unsigned char key_len;
    while (fread(&key_len, sizeof(unsigned char), 1, file) == 1) {
        char *key = malloc(key_len + 1);
        fread(key, sizeof(char), key_len, file);
        key[key_len] = '\0';

        unsigned char type;
        fread(&type, sizeof(unsigned char), 1, file);

        unsigned char val_len;
        fread(&val_len, sizeof(unsigned char), 1, file);

        char *val = malloc(val_len + 1);
        fread(val, sizeof(char), val_len, file);
        val[val_len] = '\0';

        hash_table_insert(key, val);

        free(key);
        free(val);
    }

    fclose(file);
}

int main(void) {
    char input_line[256];

    load_database();

    printf("[%s OK %s] Database opened successfully.\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
    printf("comands [QUIT] [SET] [GET] [DEL] [LIST] [HELP]\n");

    while (true) {
        imput_string(input_line, sizeof(input_line), ">");

        if (strlen(input_line) == 0) {
            continue;
        }

        char *cmd = strtok(input_line, " \t\r\n");
        if (cmd == NULL) {
            continue;
        }

        if (comand_compare(cmd, "QUIT")) {
            save_database();
            printf("Database saved to %s. Goodbye, Jarvis.\n", DB_FILE);
            break;
        }

        if (comand_compare(cmd, "HELP")) {
            print_help();
            continue;
        }

        if (comand_compare(cmd, "SET")) {
            char *key = strtok(NULL, " \t\r\n");
            char *val = strtok(NULL, " \t\r\n");

            if (key == NULL || val == NULL) {
                printf("ERROR: SET command requires both key and value! (e.g., SET speed 25)\n");
                continue;
            }

            hash_table_insert(key, val);
            printf("OK: Saved %s -> %s\n", key, val);
            continue;
        }

        if (comand_compare(cmd, "GET")) {
            char *key = strtok(NULL, " \t\r\n");

            if (key == NULL) {
                printf("ERROR: GET command requires a key! (e.g., GET speed)\n");
                continue;
            }

            Node *uzel = hash_table_lookup(key);
            if (uzel != NULL) {
                printf("%s->%s %s\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET, uzel->value);
            } else {
                printf("ERR: Key '%s' does not exist.\n", key);
            }
            continue;
        }

        if (comand_compare(cmd, "DEL")) {
            char *key = strtok(NULL, " \t\r\n");

            if (key == NULL) {
                printf("ERROR: DEL command requires a key! (e.g., DEL speed)\n");
                continue;
            }

            if (hash_table_delete(key)) {
                printf("OK: Key '%s' deleted.\n", key);
            } else {
                printf("ERR: Key '%s' not found.\n", key);
            }
            continue;
        }

        if (comand_compare(cmd, "LIST")) {
            printf("--- LIST OF ALL KEYS ---\n");
            bool db_empty = true;

            for (int i = 0; i < TABLE_SIZE; i++) {
                Node *curr = hash_table[i];
                while (curr != NULL) {
                    printf("  %s -> %s\n", curr->key, curr->value);
                    db_empty = false;
                    curr = curr->next;
                }
            }

            if (db_empty) {
                printf("  Database is empty.\n");
            }
            continue;
        }

        printf("ERROR: Unknown command '%s'. Type HELP.\n", cmd);
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *curr = hash_table[i];
        while (curr != NULL) {
            Node *temp = curr;
            curr = curr->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }

    return 0;
}