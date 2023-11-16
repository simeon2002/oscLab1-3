/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"



///* NEEDS TO BE REMOVED!
// * The real definition of struct list / struct node
// */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

//DONE
dplist_t *dpl_create() {
//    dplist_t *list;
    dplist_t* list = (dplist_t*)malloc(sizeof(dplist_t));
    list->head = NULL;
  return list;
}

//DONE
void dpl_free(dplist_t **list) {
    /* opmerking --> never forget to also free the
    malloc for the pointers that point to the nodes inside of the struct! */
    /*1. access the head el.
     * 2. reference to the first element
     * 3. loop through all of the nodes and free them from memory every single time.
     * 4. free the dplist_t datatype itself also from memory.
     */
    dplist_node_t *current_node = (*list)->head;
    dplist_node_t *next_node;
    // clearing all the nodes.
    while(current_node!=NULL) {
        next_node = current_node->next;
        free(current_node);
        current_node = next_node;
    }
    free(*list);
    *list = NULL;
    //Do extensive testing with valgrind.
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


//DONE
dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index); // gives me the node at this index.
        assert(ref_at_index != NULL); // checking condition, if not true --> error displayed.
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3 --> last element in the list.
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

//DONE
dplist_t *dpl_remove_at_index(dplist_t *list, int index) {
    dplist_node_t *prev_node, *next_node, *node_at_index;
//    int i = 0;
    if (list == NULL) return NULL;
    else if(list->head == NULL) return list;
    else{
        node_at_index = dpl_get_reference_at_index(list, index);
        // assigning index_value of the node into index otherwise segfault occurs:
        index = dpl_get_index_of_element(list, node_at_index->element);
        // QUESTION --> ZOU IK HIER DAN EEN ASSSERT MOETEN GEBRUIKEN OF NIET voor node_at_index om NULL te checken?
        // volgens mij is dit niet echt nodig omdat we deze voorwaarde toch al hebben gecheckt, toch werd dit in de vb func gebruikt!
        //ik check hier zelf voor index==0 en doe een andere actie op basis daarvan. dus ik denk dat ik die assert dan niet nodig heb??
        assert(node_at_index != NULL);
        //CORNER CASE --> INSERTING(it should be removing) ELEMENT AT INDEX 0!
        next_node = node_at_index->next;
        prev_node = node_at_index->prev;
        if (index == 0) { // if first element to be removed.
            if (next_node!= NULL) {
                next_node->prev = NULL;
                list->head = next_node;
            } else {
                list->head = NULL;
            }
        } else if(index == dpl_size(list) - 1) { // if last element to be removed
            prev_node->next = NULL;
        } else { // the generic case
            next_node->prev = prev_node;
            prev_node->next = next_node;
        }
        // 3 cases for index, 1 default, 1 <0, 1 > dpl_size - 1
//        if(index <= 0) index = 0;
//        else if (index > dpl_size(list) - 1) index = dpl_size(list) - 1;
//        while (i < index) {
//            current_node = current_node->next;
//            i++;
//        }
        /*INSTEAD OF THIS, USE THE DPL_GET_REF_AT_INDEX FUNCTION!*/
        free(node_at_index);
    }
    return list;
}

//DONE
int dpl_size(dplist_t *list) {
    int counter = 0;
    if (list == NULL) return -1;
    else if(list->head == NULL) return 0;
    else {
        dplist_node_t *temp = list->head;
        while(temp!= NULL) {
            temp = temp->next;
            counter++;
        }
    }
    return counter;
}


//DONE
dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
//    int count = 0 ;
    dplist_node_t *dummy = NULL;

    if (list == NULL) return NULL; //case 1: list empty or list doesn't exist.
    if (list->head == NULL) return NULL;
    else {
        assert(dpl_size(list) >= 0); // DONE JUST TO MAKE SURE. ask whether it's necessary + use cases.
        // checking index errors:
        if (index > dpl_size(list) - 1) {
            index = dpl_size(list) - 1;
        } else if (index < 0) {
            index = 0;
        }
        // looping through DLL until index reached.
        dplist_node_t *temp = list->head;
        int i = 0;
        while( temp != NULL) { //OPMERKING FOR LOOP TOT EN MET DE INDEX ZELF KON HIER OOK WORDEN GEBRUIKT!
            if (i == index) {
                dummy = temp;
                break;
            }
            temp = temp->next;
            i++;
        }
    }
    return dummy;
}

//DONE
element_t dpl_get_element_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL) return 0;
    dplist_node_t *temp = dpl_get_reference_at_index(list, index);
    assert(temp!=NULL);
    return temp->element;
}
//DONE
int dpl_get_index_of_element(dplist_t *list, element_t element) {
    // case 1;
    if (list == NULL) return -1;
    // case 2;
    if (list->head == NULL) return -1;

    // case 3 and 4;
    int index = 0;
    dplist_node_t *temp = list->head;
    while(temp!= NULL) {
        if (temp->element == element) {
            break;
        }
        temp = temp->next;
        index++;
    }

    // case 3;
    if (index == dpl_size(list)) return -1;

    // case 4;
    return index;
}



