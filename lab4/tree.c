#include "tree.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int insert_tree_raw(BTree* tree, const char* key, double value);
void delete_tree_raw(BTree* tree, const char* key);

// Копирует ключ в буфер
void copy_key(char destination[BTREE_KEY_SIZE], const char* source) {
    
    strncpy(destination, source, BTREE_KEY_SIZE - 1);
    destination[BTREE_KEY_SIZE - 1] = '\0';
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
    int pos = 0;

    while (pos < node->key_count && strcmp(node->keys[pos], key) < 0) {
        pos++;
    }

    return pos;
}

// Делит переполненного потомка на два узла
int split_child(BTreeNode* parent, int child_index) {
    BTreeNode* left_node = parent->children[child_index];
    BTreeNode* right_node = create_node(left_node->leaf);

    if (!right_node) return 0;

    right_node->key_count = BTREE_T - 1;

    for (int i = 0; i < BTREE_T - 1; i++) {
        copy_key(right_node->keys[i], left_node->keys[i + BTREE_T]);
        right_node->values[i] = left_node->values[i + BTREE_T];
    }

    if (!left_node->leaf) {
        for (int i = 0; i < BTREE_T; i++) {
            right_node->children[i] = left_node->children[i + BTREE_T];
            left_node->children[i + BTREE_T] = NULL;
        }
    }

    left_node->key_count = BTREE_T - 1;

    shift_children_right(parent, child_index + 1);
    parent->children[child_index + 1] = right_node;

    shift_keys_right(parent, child_index);
    copy_key(parent->keys[child_index], left_node->keys[BTREE_T - 1]);
    parent->values[child_index] = left_node->values[BTREE_T - 1];
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
    BTreeNode* cur = node->children[index];

    while (!cur->leaf) {
        cur = cur->children[cur->key_count];
    }

    copy_key(key, cur->keys[cur->key_count - 1]);
    *value = cur->values[cur->key_count - 1];
}

// Берёт минимальный ключ из правого поддерева
void get_successor(BTreeNode* node, int index, char key[BTREE_KEY_SIZE], double* value) {
    BTreeNode* cur = node->children[index + 1];

    while (!cur->leaf) {
        cur = cur->children[0];
    }

    copy_key(key, cur->keys[0]);
    *value = cur->values[0];
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
    BTreeNode* a = node->children[index];
    BTreeNode* b = node->children[index - 1];
    int i = 0;

    for (i = a->key_count; i > 0; i--) {
        copy_key(a->keys[i], a->keys[i - 1]);
        a->values[i] = a->values[i - 1];
    }

    if (!a->leaf) {
        for (i = a->key_count + 1; i > 0; i--) {
            a->children[i] = a->children[i - 1];
        }
    }

    copy_key(a->keys[0], node->keys[index - 1]);
    a->values[0] = node->values[index - 1];

    if (!a->leaf) {
        a->children[0] = b->children[b->key_count];
        b->children[b->key_count] = NULL;
    }

    copy_key(node->keys[index - 1], b->keys[b->key_count - 1]);
    node->values[index - 1] = b->values[b->key_count - 1];

    a->key_count = a->key_count + 1;
    b->key_count = b->key_count - 1;
}

// Заимствует ключ у правого брата
void borrow_from_next(BTreeNode* node, int index) {
    BTreeNode* a = node->children[index];
    BTreeNode* b = node->children[index + 1];
    int i = 0;

    copy_key(a->keys[a->key_count], node->keys[index]);
    a->values[a->key_count] = node->values[index];

    if (!a->leaf) {
        a->children[a->key_count + 1] = b->children[0];
    }

    copy_key(node->keys[index], b->keys[0]);
    node->values[index] = b->values[0];

    for (i = 0; i < b->key_count - 1; i++) {
        copy_key(b->keys[i], b->keys[i + 1]);
        b->values[i] = b->values[i + 1];
    }

    if (!b->leaf) {
        for (i = 0; i < b->key_count; i++) {
            b->children[i] = b->children[i + 1];
        }
        b->children[b->key_count] = NULL;
    }

    a->key_count = a->key_count + 1;
    b->key_count = b->key_count - 1;
}

// Склеивает двух соседних потомков в один
void merge_children(BTreeNode* node, int index) {
    BTreeNode* a = node->children[index];
    BTreeNode* b = node->children[index + 1];
    int i = 0;

    copy_key(a->keys[BTREE_T - 1], node->keys[index]);
    a->values[BTREE_T - 1] = node->values[index];

    for (i = 0; i < b->key_count; i++) {
        copy_key(a->keys[i + BTREE_T], b->keys[i]);
        a->values[i + BTREE_T] = b->values[i];
    }

    if (!a->leaf) {
        for (i = 0; i <= b->key_count; i++) {
            a->children[i + BTREE_T] = b->children[i];
        }
    }

    for (i = index; i < node->key_count - 1; i++) {
        copy_key(node->keys[i], node->keys[i + 1]);
        node->values[i] = node->values[i + 1];
    }

    for (i = index + 1; i < node->key_count; i++) {
        node->children[i] = node->children[i + 1];
    }

    a->key_count = a->key_count + b->key_count + 1;
    node->key_count = node->key_count - 1;
    free(b);
}

// Готовит потомка к безопасному удалению
void fill_child(BTreeNode* node, int index) {
    if (index < node->key_count && node->children[index + 1]->key_count >= BTREE_T) {
        borrow_from_next(node, index);
        return;
    }

    if (index > 0 && node->children[index - 1]->key_count >= BTREE_T) {
        borrow_from_prev(node, index);
        return;
    }

    if (index == node->key_count) {
        merge_children(node, index - 1);
    } else {
        merge_children(node, index);
    }
}

// Рекурсивно удаляет ключ из поддерева
void delete_from_node(BTreeNode* node, const char* key, int* status) {
    int pos = find_key_index(node, key);

    if (pos < node->key_count && strcmp(node->keys[pos], key) == 0) {
        if (node->leaf) {
            remove_from_leaf(node, pos);
            *status = 1;
            return;
        }

        if (node->children[pos]->key_count >= BTREE_T) {
            char tmp_key[BTREE_KEY_SIZE];
            double tmp_value = 0.0;

            get_predecessor(node, pos, tmp_key, &tmp_value);
            copy_key(node->keys[pos], tmp_key);
            node->values[pos] = tmp_value;
            delete_from_node(node->children[pos], tmp_key, status);
            return;
        }

        if (node->children[pos + 1]->key_count >= BTREE_T) {
            char tmp_key[BTREE_KEY_SIZE];
            double tmp_value = 0.0;

            get_successor(node, pos, tmp_key, &tmp_value);
            copy_key(node->keys[pos], tmp_key);
            node->values[pos] = tmp_value;
            delete_from_node(node->children[pos + 1], tmp_key, status);
            return;
        }

        merge_children(node, pos);
        delete_from_node(node->children[pos], key, status);
        return;
    }

    if (node->leaf) {
        *status = 0;
        return;
    }

    if (node->children[pos]->key_count < BTREE_T) {
        fill_child(node, pos);
    }

    if (pos > node->key_count) {
        delete_from_node(node->children[pos - 1], key, status);
    } else {
        delete_from_node(node->children[pos], key, status);
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

    if (!key) return 0;
    if (!*key) return 0;

    length = strlen(key);
    if (length == 0 || length > 6) return 0;

    for (size_t i = 0; i < length; i++) {
        if (!isalpha((unsigned char)key[i])) return 0;
    }

    return 1;
}

// Считает высоту дерева
int tree_height(BTreeNode* node) {
    int h = 0;

    if (!node) return 0;
    if (node->leaf) return 1;

    h = tree_height(node->children[0]);
    return h + 1;
}

// Считает общее число ключей
int count_keys(BTreeNode* node) {
    int count = 0;

    if (!node) return 0;

    count += node->key_count;
    if (!node->leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            int child_count = count_keys(node->children[i]);
            count += child_count;
        }
    }

    return count;
}

// Ищет значение по ключу
int search_tree(BTreeNode* node, const char* key, double* value_out) {
    int pos = 0;

    if (!node) {
        return 0;
    }

    if (!is_valid_key(key)) {
        return 0;
    }

    while (pos < node->key_count && strcmp(key, node->keys[pos]) > 0) {
        pos++;
    }

    if (pos < node->key_count && strcmp(key, node->keys[pos]) == 0) {
        if (value_out) {
            *value_out = node->values[pos];
        }
        return 1;
    }

    if (node->leaf) {
        return 0;
    }

    return search_tree(node->children[pos], key, value_out);
}

// Добавляет новый ключ без сообщений
int insert_tree_raw(BTree* tree, const char* key, double value) {
    if (!tree) return 0;

    if (!tree->root) {
        tree->root = create_node(1);
        if (!tree->root) return 0;

        copy_key(tree->root->keys[0], key);
        tree->root->values[0] = value;
        tree->root->key_count = 1;
        return 1;
    }

    if (tree->root->key_count == BTREE_MAX_KEYS) {
        BTreeNode* new_root = create_node(0);

        if (!new_root) return 0;

        new_root->children[0] = tree->root;
        tree->root = new_root;
        if (!split_child(new_root, 0)) {
            tree->root = new_root->children[0];
            free(new_root);
            return 0;
        }
    }

    if (!insert_non_full(tree->root, key, value)) {
        return 0;
    }

    return 1;
}

// Добавляет новый ключ в дерево
int insert_tree(BTree* tree, const char* key, double value) {
    double old_value = 0.0;

    if (!tree) {
        printf("Дерево не создано\n");
        return 0;
    }

    if (!is_valid_key(key)) {
        printf("Некорректный ключ\n");
        return 0;
    }

    if (search_tree(tree->root, key, &old_value)) {
        printf("Такой ключ уже есть\n");
        return 0;
    }

    if (!insert_tree_raw(tree, key, value)) {
        printf("Не удалось добавить узел\n");
        return 0;
    }

    return 1;
}

// Удаляет ключ без сообщений
void delete_tree_raw(BTree* tree, const char* key) {
    int status = 0;

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
}

// Удаляет ключ из дерева
int delete_tree(BTree* tree, const char* key) {
    double old_value = 0.0;

    if (!tree) {
        printf("Дерево не создано\n");
        return 0;
    }

    if (!is_valid_key(key)) {
        printf("Некорректный ключ\n");
        return 0;
    }

    if (!tree->root || !search_tree(tree->root, key, &old_value)) {
        printf("Ключ не найден\n");
        return 0;
    }

    delete_tree_raw(tree, key);
    return 1;
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

    fprintf(out, "< ");
    for (int i = 0; i < node->key_count; i++) {
        fprintf(out, "%s -> %.10g", node->keys[i], node->values[i]);
        if (i + 1 < node->key_count) {
            fprintf(out, " , ");
        }
    }
    fprintf(out, " >\n");

    if (!node->leaf) {
        for (int i = 0; i <= node->key_count; i++) {
            print_tree(node->children[i], depth + 1, out);
        }
    }
}

// Разбирает и выполняет одну команду
int process_command_line(BTree* tree, const char* line, FILE* out) {
    int cmd = 0;
    char key[64];
    char extra[64];
    double value = 0.0;
    double ans = 0.0;
    const char* ptr = line;

    if (!tree || !line) return 0;

    key[0] = '\0';
    extra[0] = '\0';

    while (*ptr && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    if (*ptr == '\0') {
        return 0;
    }

    if (sscanf(ptr, "%d", &cmd) != 1) {
        fprintf(out, "Ошибка в команде\n");
        return 0;
    }

    switch (cmd) {
        case 1:
            if (sscanf(ptr, "%d %63s %lf %63s", &cmd, key, &value, extra) != 3) {
                fprintf(out, "Ошибка в команде\n");
                return 0;
            }

            if (!is_valid_key(key)) {
                fprintf(out, "Некорректный ключ\n");
                return 0;
            }

            if (search_tree(tree->root, key, NULL)) {
                fprintf(out, "Такой ключ уже есть\n");
                return 0;
            }

            if (!insert_tree_raw(tree, key, value)) {
                fprintf(out, "Не удалось добавить узел\n");
                return 0;
            }

            fprintf(out, "Ключ \"%s\" добавлен, значение = %.10g\n", key, value);
            return 1;
        case 2:
            if (sscanf(ptr, "%d %63s %63s", &cmd, key, extra) != 2) {
                fprintf(out, "Ошибка в команде\n");
                return 0;
            }

            if (!is_valid_key(key)) {
                fprintf(out, "Некорректный ключ\n");
                return 0;
            }

            if (!search_tree(tree->root, key, NULL)) {
                fprintf(out, "Ключ не найден\n");
                return 0;
            }

            delete_tree_raw(tree, key);
            fprintf(out, "Ключ \"%s\" удалён\n", key);
            return 1;
        case 3: {
            char tail[8];

            if (sscanf(ptr, "%d %7s", &cmd, tail) != 1) {
                fprintf(out, "Ошибка в команде\n");
                return 0;
            }

            fprintf(out, "Текущее дерево:\n");
            print_tree(tree->root, 0, out);
            return 1;
        }
        case 4:
            if (sscanf(ptr, "%d %63s %63s", &cmd, key, extra) != 2) {
                fprintf(out, "Ошибка в команде\n");
                return 0;
            }

            if (!is_valid_key(key)) {
                fprintf(out, "Некорректный ключ\n");
                return 0;
            }

            if (!search_tree(tree->root, key, &ans)) {
                fprintf(out, "Ключ не найден\n");
                return 0;
            }

            fprintf(out, "Найдено: \"%s\" -> %.10g\n", key, ans);
            return 1;
        default:
            fprintf(out, "Неизвестная команда\n");
            return 0;
    }
}

// Выполняет все команды из входного файла
int process_commands_from_file(BTree* tree, const char* input_filename, const char* output_filename) {
    FILE* input = NULL;
    FILE* output = NULL;
    char line[256];

    if (!tree) return 0;

    input = fopen(input_filename, "r");
    if (!input) {
        printf("Не удалось открыть входной файл\n");
        return 0;
    }

    output = fopen(output_filename, "w");
    if (!output) {
        fclose(input);
        printf("Не удалось открыть выходной файл\n");
        return 0;
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
    return 1;
}
