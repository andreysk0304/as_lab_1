#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

void test_insert_and_search(void) {
    BTree tree;
    double x = 0.0;

    printf("test 1, insert and search...\n");
    init_tree(&tree);

    assert(insert_tree(&tree, "delta", 4.4) == 1);
    assert(insert_tree(&tree, "alpha", 1.1) == 1);
    assert(insert_tree(&tree, "gamma", 7.7) == 1);
    assert(insert_tree(&tree, "beta", 2.2) == 1);

    assert(count_keys(tree.root) == 4);
    assert(tree_height(tree.root) >= 2);
    assert(search_tree(tree.root, "alpha", &x) == 1);
    assert(x == 1.1);
    assert(search_tree(tree.root, "beta", &x) == 1);
    assert(x == 2.2);
    assert(search_tree(tree.root, "delta", &x) == 1);
    assert(x == 4.4);
    assert(search_tree(tree.root, "omega", NULL) == 0);
    assert(insert_tree(&tree, "alpha", 9.9) == 0);

    clear_tree(&tree);
    printf("test 1 complete\n");
}

void test_delete_leaf_and_internal(void) {
    BTree tree;
    const char* keys[] = {"mango", "apple", "peach", "berry", "grape", "lemon", "orange", "banana"};
    const double values[] = {10.0, 1.0, 8.0, 2.0, 5.0, 6.0, 7.0, 3.0};
    double x = 0.0;

    printf("test 2, delete leaf and internal key...\n");
    init_tree(&tree);

    for (int i = 0; i < 8; i++) {
        assert(insert_tree(&tree, keys[i], values[i]) == 1);
    }

    assert(delete_tree(&tree, "banana") == 1);
    assert(search_tree(tree.root, "banana", NULL) == 0);

    assert(delete_tree(&tree, "mango") == 1);
    assert(search_tree(tree.root, "mango", NULL) == 0);
    assert(count_keys(tree.root) == 6);

    assert(search_tree(tree.root, "apple", &x) == 1);
    assert(x == 1.0);
    assert(search_tree(tree.root, "orange", &x) == 1);
    assert(x == 7.0);
    assert(delete_tree(&tree, "absent") == 0);
    assert(delete_tree(&tree, "melon") == 0);

    clear_tree(&tree);
    printf("test 2 complete\n");
}

void test_more_than_twenty_keys(void) {
    BTree tree;
    double x = 0.0;
    const char* keys[] = {
        "mango", "apple", "peach", "berry", "grape", "lemon", "orange",
        "banana", "melon", "papaya", "guava", "tomato", "kiwi", "plum",
        "pear", "fig", "lime", "olive", "dates", "raisin", "cherry", "cocoa"
    };

    printf("test 3, more than twenty keys...\n");
    init_tree(&tree);

    for (int i = 0; i < 22; i++) {
        assert(insert_tree(&tree, keys[i], (double)(i + 1)) == 1);
    }

    assert(count_keys(tree.root) == 22);
    assert(tree_height(tree.root) >= 3);
    assert(search_tree(tree.root, "apple", &x) == 1);
    assert(x == 2.0);
    assert(search_tree(tree.root, "raisin", &x) == 1);
    assert(x == 20.0);
    assert(search_tree(tree.root, "cocoa", &x) == 1);
    assert(x == 22.0);

    assert(delete_tree(&tree, "apple") == 1);
    assert(delete_tree(&tree, "orange") == 1);
    assert(delete_tree(&tree, "papaya") == 1);
    assert(delete_tree(&tree, "kiwi") == 1);

    assert(count_keys(tree.root) == 18);
    assert(search_tree(tree.root, "apple", NULL) == 0);
    assert(search_tree(tree.root, "melon", &x) == 1);
    assert(x == 9.0);
    assert(search_tree(tree.root, "tomato", &x) == 1);
    assert(x == 12.0);

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

    assert(process_commands_from_file(&tree, "test_input.txt", "test_output.txt") == 1);

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
    assert(strstr(buffer, "Ключ не найден") != NULL);

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
    assert(insert_tree(&tree, "ab12", 1.0) == 0);
    assert(insert_tree(&tree, "toolong", 1.0) == 0);

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
