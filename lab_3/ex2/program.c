#define _GNU_SOURCE
/**
 * \author Bert Lagaisse
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include "dplist.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}
int main(void)
{
    dplist_t *strings = NULL;
    strings = dpl_create();
    dplist_t *null_list = NULL;

    //testing whether it is created

    // testing the size + insertion operations
    ck_assert_msg(dpl_size(strings) == 0, "Numbers may not contain elements.");

    dpl_insert_at_index(strings, "test", 0);
    ck_assert_msg(dpl_size(strings) == 1, "Numbers must contain 1 element.");

    dpl_insert_at_index(strings, "test1", -1);
    ck_assert_msg(dpl_size(strings) == 2, "Numbers must contain 2 elements.");

    dpl_insert_at_index(strings, "test2", 100);
    ck_assert_msg(dpl_size(strings) == 3, "Numbers must contain 3 elements.");


    // testing the get_reference_at_index operations
    dplist_node_t *test_node;

    test_node = dpl_get_reference_at_index(strings, -2);
    ck_assert_msg(test_node->prev == NULL, "This is not the first node!");

    test_node = dpl_get_reference_at_index(strings, 20);
    ck_assert_msg(test_node->next == NULL, "Last node has note has not been returned!");

    test_node = dpl_get_reference_at_index(strings, 1);
    ck_assert_msg(test_node->prev != NULL && test_node->next != NULL, "This is an intermediary note.");

    test_node = dpl_get_reference_at_index(null_list, 2);
    ck_assert_msg(test_node == NULL, "For an empty list, NULL should be returned.");

    test_node = NULL;
    test_node = dpl_get_reference_at_index(null_list, 2);
    ck_assert_msg(test_node == NULL, "For an empty node, NULL should be returned.");


    // testing the dpl_get_element_at_index operations
    test_node = dpl_get_reference_at_index(strings, -1);
    ck_assert_msg(test_node->element == dpl_get_reference_at_index(strings, -1)->element,
                  "This does not return the first element in LL.");

    test_node = dpl_get_reference_at_index(strings, 0);
    ck_assert_msg(test_node->element == dpl_get_reference_at_index(strings, 0)->element,
                  "This does not return the first element in LL.");

    test_node = dpl_get_reference_at_index(strings, 10);
    ck_assert_msg(test_node->element == dpl_get_reference_at_index(strings, 10)->element,
                  "This does not return the last element in LL.");

    test_node = dpl_get_reference_at_index(strings, 1);
    ck_assert_msg(test_node->element == dpl_get_reference_at_index(strings, 1)->element,
                  "This does not return the second element in LL.");

    dplist_t *empty_list;
    empty_list = dpl_create();
    test_node = dpl_get_reference_at_index(empty_list, 100);
    ck_assert_msg(test_node == 0, "The function did not return 0.");

    test_node = dpl_get_reference_at_index(null_list, 3);
    ck_assert_msg(test_node == 0, "The function did not return 0.");

    // testing dpl_get_index_of_element operations
    int index;

    index = dpl_get_index_of_element(strings, "as");
    ck_assert_msg(index == -1, "The element should not have existed in the LL.");

    index = dpl_get_index_of_element(strings, "test1");
    ck_assert_msg(index == 0, "The index is not 0.");

    index = dpl_get_index_of_element(NULL, "test");
    ck_assert_msg(index == -1, "The list should have returned -1 because the list is empty!");

    /* added case myself where when list empty list, return -1 as well.*/
    index = dpl_get_index_of_element(empty_list, "test2");
    ck_assert_msg(index == -1, "The value should have been -1.");

    /* testing two more cases where index should be 0 and last element.*/
    index = dpl_get_index_of_element(strings, "test");
    ck_assert_msg(index == 1, "The index should have been 1.");

    index = dpl_get_index_of_element(strings, "test2");
    ck_assert_msg(index == 2, "The index should have been 2.");


//     testing the removing operations
    strings = dpl_remove_at_index(strings, -1);
    ck_assert_msg(dpl_size(strings) == 2, "Numbers must contain 2 elements.");

    strings = dpl_remove_at_index(strings, 0);
    ck_assert_msg(dpl_size(strings) == 1, "Numbers must contain 1 elements.");

    strings = dpl_remove_at_index(strings, 3);
    ck_assert_msg(dpl_size(strings) == 0, "The last element should has been removed.");

    strings = dpl_remove_at_index(strings, -1);
    ck_assert_msg(dpl_size(strings) != 0, "The correct action has been taken, i.e. no action at all!");
    ck_assert_msg(strings->head == NULL, "The list is empty and thus should return an empty list to strings.");

    /*testing a null_list as well*/
    null_list =  dpl_remove_at_index(null_list, 2);
    ck_assert_msg(null_list == NULL, "The null_list is not equal to null, inspect this issue.");



// I just realized the importance of testing all of your code! it really makes the code bulletproof


    // testing the freeing operations
    dpl_free(&strings);
    dpl_free(&empty_list);

    return 0;
}