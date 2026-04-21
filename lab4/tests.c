#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

void assert_search_value(BTree* tree, const char* key, double expected_value) {
    double actual_value = 0.0;

    assert(search_tree(tree->root, key, &actual_value) == BTREE_OK);
    assert(actual_value == expected_value);
}

void test_insert_and_search(void) {
    BTree tree;

    printf("test 1, insert and search...\n");
    init_tree(&tree);

    assert(insert_tree(&tree, "delta", 4.4) == BTREE_OK);
    assert(insert_tree(&tree, "alpha", 1.1) == BTREE_OK);
    assert(insert_tree(&tree, "gamma", 7.7) == BTREE_OK);
    assert(insert_tree(&tree, "beta", 2.2) == BTREE_OK);

    assert(count_keys(tree.root) == 4);
    assert(tree_height(tree.root) >= 2);
    assert_search_value(&tree, "alpha", 1.1);
    assert_search_value(&tree, "beta", 2.2);
    assert_search_value(&tree, "delta", 4.4);
    assert(search_tree(tree.root, "omega", NULL) == BTREE_KEY_NOT_FOUND);
    assert(insert_tree(&tree, "alpha", 9.9) == BTREE_DUPLICATE_KEY);

    clear_tree(&tree);
    printf("test 1 complete\n");
}

void test_delete_leaf_and_internal(void) {
    BTree tree;
    const char* keys[] = {"mango", "apple", "peach", "berry", "grape", "lemon", "orange", "banana"};
    const double values[] = {10.0, 1.0, 8.0, 2.0, 5.0, 6.0, 7.0, 3.0};

    printf("test 2, delete leaf and internal key...\n");
    init_tree(&tree);

    for (int i = 0; i < 8; i++) {
        assert(insert_tree(&tree, keys[i], values[i]) == BTREE_OK);
    }

    assert(delete_tree(&tree, "banana") == BTREE_OK);
    assert(search_tree(tree.root, "banana", NULL) == BTREE_KEY_NOT_FOUND);

    assert(delete_tree(&tree, "mango") == BTREE_OK);
    assert(search_tree(tree.root, "mango", NULL) == BTREE_KEY_NOT_FOUND);
    assert(count_keys(tree.root) == 6);

    assert_search_value(&tree, "apple", 1.0);
    assert_search_value(&tree, "orange", 7.0);
    assert(delete_tree(&tree, "absent") == BTREE_KEY_NOT_FOUND);
    assert(delete_tree(&tree, "melon") == BTREE_KEY_NOT_FOUND);

    clear_tree(&tree);
    printf("test 2 complete\n");
}

void test_more_than_twenty_keys(void) {
    BTree tree;
    const char* keys[] = {
        "mango", "apple", "peach", "berry", "grape", "lemon", "orange",
        "banana", "melon", "papaya", "guava", "tomato", "kiwi", "plum",
        "pear", "fig", "lime", "olive", "dates", "raisin", "cherry", "cocoa"
    };

    printf("test 3, more than twenty keys...\n");
    init_tree(&tree);

    for (int i = 0; i < 22; i++) {
        assert(insert_tree(&tree, keys[i], (double)(i + 1)) == BTREE_OK);
    }

    assert(count_keys(tree.root) == 22);
    assert(tree_height(tree.root) >= 3);
    assert_search_value(&tree, "apple", 2.0);
    assert_search_value(&tree, "raisin", 20.0);
    assert_search_value(&tree, "cocoa", 22.0);

    assert(delete_tree(&tree, "apple") == BTREE_OK);
    assert(delete_tree(&tree, "orange") == BTREE_OK);
    assert(delete_tree(&tree, "papaya") == BTREE_OK);
    assert(delete_tree(&tree, "kiwi") == BTREE_OK);

    assert(count_keys(tree.root) == 18);
    assert(search_tree(tree.root, "apple", NULL) == BTREE_KEY_NOT_FOUND);
    assert_search_value(&tree, "melon", 9.0);
    assert_search_value(&tree, "tomato", 12.0);

    clear_tree(&tree);
    printf("test 3 complete\n");
}

void test_command_file_processing(void) {
    BTree tree;
    FILE* input = NULL;
    FILE* output = NULL;
    char buffer[2048];
    size_t length = 0;

    printf("test 4, command file processing...\n");
    init_tree(&tree);

    input = fopen("test_input.txt", "w");
    assert(input != NULL);
    fprintf(input, "1 alpha 1.5\n");
    fprintf(input, "1 beta 2.5\n");
    fprintf(input, "4 alpha\n");
    fprintf(input, "3\n");
    fprintf(input, "2 beta\n");
    fprintf(input, "4 beta\n");
    fclose(input);

    assert(process_commands_from_file(&tree, "test_input.txt", "test_output.txt") == BTREE_OK);

    output = fopen("test_output.txt", "r");
    assert(output != NULL);

    length = fread(buffer, 1, sizeof(buffer) - 1, output);
    buffer[length] = '\0';
    fclose(output);

    assert(strstr(buffer, "1 alpha 1.5") != NULL);
    assert(strstr(buffer, "Ключ \"alpha\" добавлен") != NULL);
    assert(strstr(buffer, "Найдено: \"alpha\" -> 1.5") != NULL);
    assert(strstr(buffer, "Текущее дерево:") != NULL);
    assert(strstr(buffer, "Ключ \"beta\" удалён") != NULL);
    assert(strstr(buffer, "Ошибка: ключ не найден.") != NULL);

    remove("test_input.txt");
    remove("test_output.txt");
    clear_tree(&tree);
    printf("test 4 complete\n");
}

void test_key_validation(void) {
    BTree tree;

    printf("test 5, key validation...\n");
    init_tree(&tree);

    assert(is_valid_key("abc") == 1);
    assert(is_valid_key("ABCdef") == 1);
    assert(is_valid_key("") == 0);
    assert(is_valid_key("toolong") == 0);
    assert(is_valid_key("ab12") == 0);
    assert(insert_tree(&tree, "ab12", 1.0) == BTREE_INVALID_KEY);
    assert(insert_tree(&tree, "toolong", 1.0) == BTREE_INVALID_KEY);

    clear_tree(&tree);
    printf("test 5 complete\n");
}

int main(void) {
    printf("STARTING LAB4 TESTS...\n\n");

    test_insert_and_search();
    test_delete_leaf_and_internal();
    test_more_than_twenty_keys();
    test_command_file_processing();
    test_key_validation();

    printf("\nALL TESTS PASSED\n");
    return 0;
}
