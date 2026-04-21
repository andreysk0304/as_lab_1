#include "tree.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Копирует ключ в буфер
void copy_key(char destination[BTREE_KEY_SIZE], const char* source) {
    
    strncpy(destination, source, BTREE_KEY_SIZE - 1);
    destination[BTREE_KEY_SIZE - 1] = '\0';
}

// Копирует пару ключ и его значение между узлами
void copy_entry(BTreeNode* node, int destination, BTreeNode* source, int index) {
    copy_key(node->keys[destination], source->keys[index]);
    node->values[destination] = source->values[index];
}

// Сдвигает ключи и значения вправо
void shift_keys_right(BTreeNode* node, int from_index) {
    for (int i = node->key_count; i > from_index; i--) {
        copy_key(node->keys[i], node->keys[i - 1]);
        node->values[i] = node->values[i - 1];
    }
}

// Сдвигает потомков вправо
void shift_children_right(BTreeNode* node, int from_index) {
    for (int i = node->key_count + 1; i > from_index; i--) {
        node->children[i] = node->children[i - 1];
    }
}

// Ищет позицию ключа внутри узла
int find_key_index(BTreeNode* node, const char* key) {
    int index = 0;

    while (index < node->key_count && strcmp(node->keys[index], key) < 0) {
        index++;
    }

    return index;
}

// Делит переполненного потомка на два узла
int split_child(BTreeNode* parent, int child_index) {
    BTreeNode* full_child = parent->children[child_index];
    BTreeNode* new_child = create_node(full_child->leaf);

    if (!new_child) return 0;

    new_child->key_count = BTREE_T - 1;

    for (int i = 0; i < BTREE_T - 1; i++) {
        copy_entry(new_child, i, full_child, i + BTREE_T);
    }

    if (!full_child->leaf) {
        for (int i = 0; i < BTREE_T; i++) {
            new_child->children[i] = full_child->children[i + BTREE_T];
            full_child->children[i + BTREE_T] = NULL;
        }
    }

    full_child->key_count = BTREE_T - 1;

    shift_children_right(parent, child_index + 1);
    parent->children[child_index + 1] = new_child;

    shift_keys_right(parent, child_index);
    copy_entry(parent, child_index, full_child, BTREE_T - 1);
    parent->key_count++;

    return 1;
}

// Вставляет ключ в узел, который точно не переполнен
int insert_non_full(BTreeNode* node, const char* key, double value) {
    int index = node->key_count - 1;

    if (node->leaf) {
        while (index >= 0 && strcmp(key, node->keys[index]) < 0) {
            copy_key(node->keys[index + 1], node->keys[index]);
            node->values[index + 1] = node->values[index];
            index--;
        }

        copy_key(node->keys[index + 1], key);
        node->values[index + 1] = value;
        node->key_count++;
        return 1;
    }

    while (index >= 0 && strcmp(key, node->keys[index]) < 0) {
        index--;
    }

    index++;

    if (node->children[index]->key_count == BTREE_MAX_KEYS) {
        if (!split_child(node, index)) {
            return 0;
        }
        if (strcmp(key, node->keys[index]) > 0) {
            index++;
        }
    }

    return insert_non_full(node->children[index], key, value);
}

// Берёт максимальный ключ из левого поддерева
void get_predecessor(BTreeNode* node, int index, char key[BTREE_KEY_SIZE], double* value) {
    BTreeNode* current = node->children[index];

    while (!current->leaf) {
        current = current->children[current->key_count];
    }

    copy_key(key, current->keys[current->key_count - 1]);
    *value = current->values[current->key_count - 1];
}

// Берёт минимальный ключ из правого поддерева
void get_successor(BTreeNode* node, int index, char key[BTREE_KEY_SIZE], double* value) {
    BTreeNode* current = node->children[index + 1];

    while (!current->leaf) {
        current = current->children[0];
    }

    copy_key(key, current->keys[0]);
    *value = current->values[0];
}

// Удаляет ключ из листа
void remove_from_leaf(BTreeNode* node, int index) {
    for (int i = index + 1; i < node->key_count; i++) {
        copy_key(node->keys[i - 1], node->keys[i]);
        node->values[i - 1] = node->values[i];
    }

    node->key_count--;
}

// Заимствует ключ у левого брата
void borrow_from_prev(BTreeNode* node, int index) {
    BTreeNode* child = node->children[index];
    BTreeNode* sibling = node->children[index - 1];

    for (int i = child->key_count - 1; i >= 0; i--) {
        copy_key(child->keys[i + 1], child->keys[i]);
        child->values[i + 1] = child->values[i];
    }

    if (!child->leaf) {
        for (int i = child->key_count; i >= 0; i--) {
            child->children[i + 1] = child->children[i];
        }
    }

    copy_key(child->keys[0], node->keys[index - 1]);
    child->values[0] = node->values[index - 1];

    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->key_count];
        sibling->children[sibling->key_count] = NULL;
    }

    copy_key(node->keys[index - 1], sibling->keys[sibling->key_count - 1]);
    node->values[index - 1] = sibling->values[sibling->key_count - 1];

    child->key_count++;
    sibling->key_count--;
}

// Заимствует ключ у правого брата
void borrow_from_next(BTreeNode* node, int index) {
    BTreeNode* child = node->children[index];
    BTreeNode* sibling = node->children[index + 1];

    copy_key(child->keys[child->key_count], node->keys[index]);
    child->values[child->key_count] = node->values[index];

    if (!child->leaf) {
        child->children[child->key_count + 1] = sibling->children[0];
    }

    copy_key(node->keys[index], sibling->keys[0]);
    node->values[index] = sibling->values[0];

    for (int i = 1; i < sibling->key_count; i++) {
        copy_key(sibling->keys[i - 1], sibling->keys[i]);
        sibling->values[i - 1] = sibling->values[i];
    }

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->key_count; i++) {
            sibling->children[i - 1] = sibling->children[i];
        }
        sibling->children[sibling->key_count] = NULL;
    }

    child->key_count++;
    sibling->key_count--;
}

// Склеивает двух соседних потомков в один
void merge_children(BTreeNode* node, int index) {
    BTreeNode* child = node->children[index];
    BTreeNode* sibling = node->children[index + 1];

    copy_key(child->keys[BTREE_T - 1], node->keys[index]);
    child->values[BTREE_T - 1] = node->values[index];

    for (int i = 0; i < sibling->key_count; i++) {
        copy_key(child->keys[i + BTREE_T], sibling->keys[i]);
        child->values[i + BTREE_T] = sibling->values[i];
    }

    if (!child->leaf) {
        for (int i = 0; i <= sibling->key_count; i++) {
            child->children[i + BTREE_T] = sibling->children[i];
        }
    }

    for (int i = index + 1; i < node->key_count; i++) {
        copy_key(node->keys[i - 1], node->keys[i]);
        node->values[i - 1] = node->values[i];
    }

    for (int i = index + 2; i <= node->key_count; i++) {
        node->children[i - 1] = node->children[i];
    }

    child->key_count += sibling->key_count + 1;
    node->key_count--;
    free(sibling);
}

// Готовит потомка к безопасному удалению
void fill_child(BTreeNode* node, int index) {
    if (index > 0 && node->children[index - 1]->key_count >= BTREE_T) {
        borrow_from_prev(node, index);
    } else if (index < node->key_count && node->children[index + 1]->key_count >= BTREE_T) {
        borrow_from_next(node, index);
    } else {
        if (index < node->key_count) {
            merge_children(node, index);
        } else {
            merge_children(node, index - 1);
        }
    }
}

// Рекурсивно удаляет ключ из поддерева
void delete_from_node(BTreeNode* node, const char* key, BTreeStatus* status) {
    int index = find_key_index(node, key);

    if (index < node->key_count && strcmp(node->keys[index], key) == 0) {
        if (node->leaf) {
            remove_from_leaf(node, index);
            *status = BTREE_OK;
            return;
        }

        if (node->children[index]->key_count >= BTREE_T) {
            char predecessor_key[BTREE_KEY_SIZE];
            double predecessor_value = 0.0;

            get_predecessor(node, index, predecessor_key, &predecessor_value);
            copy_key(node->keys[index], predecessor_key);
            node->values[index] = predecessor_value;
            delete_from_node(node->children[index], predecessor_key, status);
            return;
        }

        if (node->children[index + 1]->key_count >= BTREE_T) {
            char successor_key[BTREE_KEY_SIZE];
            double successor_value = 0.0;

            get_successor(node, index, successor_key, &successor_value);
            copy_key(node->keys[index], successor_key);
            node->values[index] = successor_value;
            delete_from_node(node->children[index + 1], successor_key, status);
            return;
        }

        merge_children(node, index);
        delete_from_node(node->children[index], key, status);
        return;
    }

    if (node->leaf) {
        *status = BTREE_KEY_NOT_FOUND;
        return;
    }

    if (node->children[index]->key_count < BTREE_T) {
        fill_child(node, index);
    }

    if (index > node->key_count) {
        delete_from_node(node->children[index - 1], key, status);
    } else {
        delete_from_node(node->children[index], key, status);
    }
}

// Возвращает текст ошибки или статуса
const char* status_text(BTreeStatus status) {
    switch (status) {
        case BTREE_OK:
            return "OK";
        case BTREE_INVALID_KEY:
            return "Ошибка, некорректный ключ";
        case BTREE_DUPLICATE_KEY:
            return "Ошибка, такой ключ уже существует";
        case BTREE_KEY_NOT_FOUND:
            return "Ошибка, ключ не найден";
        case BTREE_MEMORY_ERROR:
            return "Ошибка, не удалось выделить память";
        case BTREE_FILE_ERROR:
            return "Ошибка, не удалось открыть файл";
        case BTREE_INVALID_COMMAND:
            return "Ошибка, неверная команда";
        default:
            return "Ошибка!";
    }
}

// Пишет результат команды в файл
void write_command_result(FILE* out, int operation, BTreeStatus status, const char* key, double value, BTree* tree) {
    if (!out) return;

    if (status != BTREE_OK) {
        fprintf(out, "%s\n", status_text(status));
        return;
    }

    switch (operation) {
        case 1:
            fprintf(out, "Ключ \"%s\" добавлен, значение = %.10g\n", key, value);
            break;
        case 2:
            fprintf(out, "Ключ \"%s\" удалён\n", key);
            break;
        case 3:
            fprintf(out, "Текущее дерево:\n");
            print_tree(tree->root, 0, out);
            break;
        case 4:
            fprintf(out, "Найдено: \"%s\" -> %.10g\n", key, value);
            break;
        default:
            fprintf(out, "%s\n", status_text(status));
            break;
    }
}

// Убирает перевод строки в конце
void trim_newline(char* line) {
    size_t length = strlen(line);

    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
        line[length - 1] = '\0';
        length--;
    }
}

// Инициализирует пустое дерево
void init_tree(BTree* tree) {
    tree->root = NULL;
}

// Создаёт новый узел дерева
BTreeNode* create_node(int leaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));

    if (!node) {
        return NULL;
    }

    node->key_count = 0;
    node->leaf = leaf;

    for (int i = 0; i < BTREE_MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }

    for (int i = 0; i < BTREE_MAX_KEYS; i++) {
        node->keys[i][0] = '\0';
        node->values[i] = 0.0;
    }

    return node;
}

// Освобождает всё поддерево
void free_tree(BTreeNode* node) {
    if (!node) return;

    if (!node->leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            free_tree(node->children[i]);
        }
    }

    free(node);
}

// Полностью очищает дерево
void clear_tree(BTree* tree) {
    if (!tree) return;

    free_tree(tree->root);
    tree->root = NULL;
}

// Проверяет формат ключа
int is_valid_key(const char* key) {
    size_t length = 0;

    if (!key || !*key) return 0;

    length = strlen(key);
    if (length == 0 || length > 6) return 0;

    for (size_t i = 0; i < length; i++) {
        if (!isalpha((unsigned char)key[i])) return 0;
    }

    return 1;
}

// Считает высоту дерева
int tree_height(BTreeNode* node) {
    if (!node) return 0;
    if (node->leaf) return 1;
    return 1 + tree_height(node->children[0]);
}

// Считает общее число ключей
int count_keys(BTreeNode* node) {
    int count = 0;

    if (!node) return 0;

    count += node->key_count;
    if (!node->leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            count += count_keys(node->children[i]);
        }
    }

    return count;
}

// Ищет значение по ключу
BTreeStatus search_tree(BTreeNode* node, const char* key, double* value_out) {
    int index = 0;

    if (!is_valid_key(key)) {
        return BTREE_INVALID_KEY;
    }

    if (!node) {
        return BTREE_KEY_NOT_FOUND;
    }

    while (index < node->key_count && strcmp(key, node->keys[index]) > 0) {
        index++;
    }

    if (index < node->key_count && strcmp(key, node->keys[index]) == 0) {
        if (value_out) {
            *value_out = node->values[index];
        }
        return BTREE_OK;
    }

    if (node->leaf) {
        return BTREE_KEY_NOT_FOUND;
    }

    return search_tree(node->children[index], key, value_out);
}

// Добавляет новый ключ в дерево
BTreeStatus insert_tree(BTree* tree, const char* key, double value) {
    double existing_value = 0.0;

    if (!tree) return BTREE_MEMORY_ERROR;
    if (!is_valid_key(key)) return BTREE_INVALID_KEY;

    if (search_tree(tree->root, key, &existing_value) == BTREE_OK) {
        return BTREE_DUPLICATE_KEY;
    }

    if (!tree->root) {
        tree->root = create_node(1);
        if (!tree->root) return BTREE_MEMORY_ERROR;

        copy_key(tree->root->keys[0], key);
        tree->root->values[0] = value;
        tree->root->key_count = 1;
        return BTREE_OK;
    }

    if (tree->root->key_count == BTREE_MAX_KEYS) {
        BTreeNode* new_root = create_node(0);

        if (!new_root) return BTREE_MEMORY_ERROR;

        new_root->children[0] = tree->root;
        tree->root = new_root;
        if (!split_child(new_root, 0)) {
            tree->root = new_root->children[0];
            free(new_root);
            return BTREE_MEMORY_ERROR;
        }
    }

    if (!insert_non_full(tree->root, key, value)) {
        return BTREE_MEMORY_ERROR;
    }

    return BTREE_OK;
}

// Удаляет ключ из дерева
BTreeStatus delete_tree(BTree* tree, const char* key) {
    BTreeStatus status = BTREE_OK;
    double existing_value = 0.0;

    if (!tree) return BTREE_MEMORY_ERROR;
    if (!is_valid_key(key)) return BTREE_INVALID_KEY;
    if (!tree->root) return BTREE_KEY_NOT_FOUND;

    status = search_tree(tree->root, key, &existing_value);
    if (status != BTREE_OK) return status;

    delete_from_node(tree->root, key, &status);

    if (tree->root && tree->root->key_count == 0) {
        BTreeNode* old_root = tree->root;

        if (tree->root->leaf) {
            tree->root = NULL;
        } else {
            tree->root = tree->root->children[0];
        }

        free(old_root);
    }

    return status;
}

// Печатает дерево с отступами
void print_tree(BTreeNode* node, int depth, FILE* out) {
    if (!out) return;

    if (!node) {
        if (depth == 0) {
            fprintf(out, "Дерево пусто.\n");
        }
        return;
    }

    for (int i = 0; i < depth; i++) {
        fprintf(out, "  ");
    }

    fprintf(out, "[");
    for (int i = 0; i < node->key_count; i++) {
        fprintf(out, "%s=%.10g", node->keys[i], node->values[i]);
        if (i + 1 < node->key_count) {
            fprintf(out, " | ");
        }
    }
    fprintf(out, "]\n");

    if (!node->leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            print_tree(node->children[i], depth + 1, out);
        }
    }
}

// Разбирает и выполняет одну команду
BTreeStatus process_command_line(BTree* tree, const char* line, FILE* out) {
    int operation = 0;
    char key[64];
    char extra[64];
    double value = 0.0;
    BTreeStatus status = BTREE_INVALID_COMMAND;
    const char* cursor = line;

    if (!tree || !line) return BTREE_INVALID_COMMAND;

    key[0] = '\0';
    extra[0] = '\0';

    while (*cursor && isspace((unsigned char)*cursor)) {
        cursor++;
    }

    if (*cursor == '\0') {
        return BTREE_INVALID_COMMAND;
    }

    if (sscanf(cursor, "%d", &operation) != 1) {
        write_command_result(out, operation, BTREE_INVALID_COMMAND, NULL, 0.0, tree);
        return BTREE_INVALID_COMMAND;
    }

    switch (operation) {
        case 1:
            if (sscanf(cursor, "%d %63s %lf %63s", &operation, key, &value, extra) != 3) {
                status = BTREE_INVALID_COMMAND;
            } else {
                status = insert_tree(tree, key, value);
            }
            write_command_result(out, operation, status, key[0] ? key : NULL, value, tree);
            return status;
        case 2:
            if (sscanf(cursor, "%d %63s %63s", &operation, key, extra) != 2) {
                status = BTREE_INVALID_COMMAND;
            } else {
                status = delete_tree(tree, key);
            }
            write_command_result(out, operation, status, key[0] ? key : NULL, 0.0, tree);
            return status;
        case 3: {
            char tail[8];

            if (sscanf(cursor, "%d %7s", &operation, tail) != 1) {
                status = BTREE_INVALID_COMMAND;
            } else {
                status = BTREE_OK;
            }

            write_command_result(out, operation, status, NULL, 0.0, tree);
            return status;
        }
        case 4:
            if (sscanf(cursor, "%d %63s %63s", &operation, key, extra) != 2) {
                status = BTREE_INVALID_COMMAND;
                write_command_result(out, operation, status, key[0] ? key : NULL, 0.0, tree);
                return status;
            }

            status = search_tree(tree->root, key, &value);
            write_command_result(out, operation, status, key, value, tree);
            return status;
        default:
            write_command_result(out, operation, BTREE_INVALID_COMMAND, NULL, 0.0, tree);
            return BTREE_INVALID_COMMAND;
    }
}

// Выполняет все команды из входного файла
BTreeStatus process_commands_from_file(BTree* tree, const char* input_filename, const char* output_filename) {
    FILE* input = NULL;
    FILE* output = NULL;
    char line[256];

    if (!tree) return BTREE_MEMORY_ERROR;

    input = fopen(input_filename, "r");
    if (!input) {
        return BTREE_FILE_ERROR;
    }

    output = fopen(output_filename, "w");
    if (!output) {
        fclose(input);
        return BTREE_FILE_ERROR;
    }

    while (fgets(line, sizeof(line), input) != NULL) {
        trim_newline(line);

        if (line[0] == '\0') continue;

        fprintf(output, "%s\n", line);
        process_command_line(tree, line, output);
        fprintf(output, "\n");
    }

    fclose(input);
    fclose(output);
    return BTREE_OK;
}
