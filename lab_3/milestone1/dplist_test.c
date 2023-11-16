#define _GNU_SOURCE



#include "dplist.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include<string.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name); //asprintf requires _GNU_SOURCE
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}

void yourtest1()
    {
        // Test free NULL, don't use callback
        dplist_t *list = NULL;
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free NULL, use callback
        list = NULL;
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, don't use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // vanaf hier mijn test code.
        /*content to be added*/
        my_element_t *content = (my_element_t *)malloc(sizeof(my_element_t));
        content->id = 7;
        content->name = "teststring";
        my_element_t *content1 = (my_element_t *)malloc(sizeof(my_element_t));
        content1->id = 10;
        content1->name = "eststring";
        my_element_t *content2 = (my_element_t *)malloc(sizeof(my_element_t));
        content2->id = 1;
        content2->name = "testing";

    /* creation of the list*/
    dplist_t *test_list = NULL;
    test_list = dpl_create(element_copy, element_free,element_compare);
    dplist_t *null_list = NULL;
    dplist_t *empty_list;
    empty_list = dpl_create(element_copy, element_free, element_compare);

    //testing whether it is created

    // testing the size + insertion operations

    /* freeing of a dynamic malloc for name*/
    dpl_insert_at_index(test_list, content1, 10, true);
    dpl_free(&test_list, true);

    /*freeing of a static malloc for name
     * TWEE OPMERKINGEN voor mezelf:
     *  1. free_element = false bij dpl_free omdat de values niet dynamisch worden gedefinieerd!
     *  2. insert_copy moet ook op false staan, namelijk omdat ze niet dynamisch worden gedefinieerd, en er dus anders
     *      een memory leak zal ontstaan!(hierdoor moet free_element oko op false staan. */
    test_list = dpl_create(element_copy, element_free,element_compare);

    ck_assert_msg(dpl_size(test_list) == 0, "test_list may not contain elements.");

    dpl_insert_at_index(test_list, content, 0, false);
    ck_assert_msg(dpl_size(test_list) == 1, "test_list must contain 1 element.");

    dpl_insert_at_index(test_list, content1, -1, false);
    ck_assert_msg(dpl_size(test_list) == 2, "test_list must contain 2 elements.");
        /* copy is inserted!*/
    dpl_insert_at_index(test_list, content2, 100, false);
    ck_assert_msg(dpl_size(test_list) == 3, "test_list must contain 3 elements.");


        // testing the get_reference_at_index operations + use of get_element_at_reference
        dplist_node_t *test_node;

        /*first el*/
        test_node = (dplist_node_t*)dpl_get_reference_at_index(test_list, -2);
            /* NOTE THIS IS NOT POSSIBLE AS IT IS BLACK BOX TESTING,
            * SO INSTEAD YOU USE THE CREATED FUNCTIONS TO ACCESS IT!!
            ck_assert_msg(test_node->prev == NULL, "This is not the first node!");*/
        ck_assert_msg(dpl_get_element_at_reference(test_list, test_node) ==
        dpl_get_element_at_index(test_list, -2), "This is not the first node!");

        /*last el*/
        test_node = (dplist_node_t*)dpl_get_reference_at_index(test_list, 20);
        ck_assert_msg(dpl_get_element_at_reference(test_list, test_node) ==
            dpl_get_element_at_index(test_list, 20), "Last node has note has not been returned!");

        /*second el*/
        test_node = (dplist_node_t*)dpl_get_reference_at_index(test_list, 1);
        ck_assert_msg(dpl_get_element_at_reference(test_list, test_node) ==
            dpl_get_element_at_index(test_list, 1), "This is the intermediary note.");

        /* empty list given*/
        test_node = (dplist_node_t*)dpl_get_reference_at_index(empty_list, 2);
        ck_assert_msg(test_node == NULL, "For an empty list, NULL should be returned.");

    /* list is NULL given*/
        test_node = NULL;
        test_node = (dplist_node_t*)dpl_get_reference_at_index(NULL, 2);
        ck_assert_msg(test_node == NULL, "For an empty node, NULL should be returned.");


        // testing the dpl_get_element_at_index operations
        my_element_t *test_element = (my_element_t*)dpl_get_element_at_index(test_list, -1);
        ck_assert_msg(test_element->id  == 10,"This does not return the first element in LL.");

        test_element = (my_element_t*)dpl_get_element_at_index(test_list, 0);
        ck_assert_msg(test_element->id == 10,"This does not return the first element in LL.");

        test_element = (my_element_t*)dpl_get_element_at_index(test_list, 10);
        ck_assert_msg(test_element->id == 1,"This does not return the last element in LL.");

        test_element = (my_element_t*)dpl_get_element_at_index(test_list, 1);
        ck_assert_msg(test_element->id == 7,"This does not return the second element in LL.");

        test_element = (my_element_t*)dpl_get_element_at_index(empty_list, 100);
        ck_assert_msg(test_element == NULL, "The function did not return NULL.");

        test_element = (my_element_t*)dpl_get_element_at_index(null_list, 3);
        ck_assert_msg(test_element == NULL, "The function did not return NULL.");

        // testing dpl_get_index_of_element operations
        int index;

        index = dpl_get_index_of_element(test_list, "testing");
        ck_assert_msg(index == -1, "The element should not have existed in the LL.");

        index = dpl_get_index_of_element(test_list, content);
        ck_assert_msg(index == 1, "The element does not exist in the LL.");

        index = dpl_get_index_of_element(NULL, content1);
        ck_assert_msg(index == -1, "The list should have returned -1 because the list is empty!");

        /* added case myself where when list empty list, return -1 as well.*/
        index = dpl_get_index_of_element(empty_list, content2);
        ck_assert_msg(index == -1, "The value should have been -1.");

        /* testing two more cases where index should be 0 and last element.*/
        index = dpl_get_index_of_element(test_list, content1);
        ck_assert_msg(index == 0, "The index should have been 0.");
        index = dpl_get_index_of_element(test_list, content2);
        ck_assert_msg(index == 2, "The index should have been 2.");

//     testing the removing operations
    test_list = dpl_remove_at_index(test_list, -1, false);
    ck_assert_msg(dpl_size(test_list) == 2, "test_list must contain 2 elements.");

    test_list = dpl_remove_at_index(test_list, 0, false);
    ck_assert_msg(dpl_size(test_list) == 1, "test_list must contain 1 elements.");

    test_list = dpl_remove_at_index(test_list, 3, false);
    ck_assert_msg(dpl_size(test_list) == 0, "The last element should has been removed.");

    test_list = dpl_remove_at_index(test_list, -1, true);
//    ck_assert_msg(dpl_size(test_list) != 0, "The correct action has been taken, i.e. no action at all!");
    ck_assert_msg(dpl_size(test_list) == 0, "The list is empty and thus should return an empty list to test_list.");

    /*testing a null_list as well*/
    null_list =  dpl_remove_at_index(null_list, 2, true);
    ck_assert_msg(null_list == NULL, "The null_list is not equal to null, inspect this issue.");



// I just realized the importance of testing all of your code! it really makes the code bulletproof


    // testing the freeing operations
    dpl_free(&test_list, false); // note true, can only be used if name is dynamically allocated!!
    free(content2);
    free(content1);
    free(content);
    dpl_free(&empty_list, true);

}






int main(void) {

    yourtest1();
    return 0;
}
