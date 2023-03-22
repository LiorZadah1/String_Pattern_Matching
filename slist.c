#include "slist.h"
#include "pattern_matching.h"
#include <stdio.h>
#include <stdlib.h>
//
// Created by student on 11/16/22.
//
void dbllist_init(dbllist_t * list){
    //init the list parameters
    dbllist_head(list) = NULL;
    dbllist_size(list) = 0;
    dbllist_tail(list) = NULL;
}//V

void dbllist_destroy(dbllist_t * list, dbllist_destroy_t dealloc){
    /*
     * destroy the list; move in while loop and check if we need to
     * free data and free the new pointer
     * free the list
     */
    if(list == NULL)
        return;
    dbllist_node_t *ptr = dbllist_head(list);
    while (ptr != NULL){
        dbllist_node_t *next = dbllist_next(ptr);
        if(dealloc == DBLLIST_FREE_DATA) {
            free(dbllist_data(ptr));
            dbllist_data(ptr) = NULL;
        }
        free(ptr);
        ptr = next;
    }
    //free(list);
}

int dbllist_append(dbllist_t * list ,void * data){
    /*
     * add new node at the end of the list.
     * check if the list is empty
     */
    if(list == NULL || data == NULL)
        return -1;
    dbllist_node_t* newNode = (dbllist_node_t*)malloc(sizeof (dbllist_node_t));
    if(newNode == NULL){
        return -1;
    }
    dbllist_data(newNode) = data;
    dbllist_next(newNode) = NULL;
    dbllist_prev(newNode) = NULL;
    /*now I'll check if the list is empty.
     if it's empty I will define the head&tail of the list to be the new node
     else, I will add it to the end of the list
    */
    if(dbllist_head(list) == NULL){// maybe i will do this in private func
        dbllist_head(list) = newNode;
        dbllist_tail(list) = newNode;
    }
    else{
        dbllist_prev(newNode) = dbllist_tail(list);
        dbllist_next(dbllist_tail(list)) = newNode;
        dbllist_tail(list) = newNode;
    }
    dbllist_size(list)++;
//    free(newNode);
    return 0;
}//V

int dbllist_prepend(dbllist_t *list, void * data){
    /*
     *
     */
    if(list == NULL || data == NULL)
        return -1;
    dbllist_node_t* newNode = (dbllist_node_t*)malloc(sizeof (dbllist_node_t));
    if(newNode < 0){
        return -1;
    }
    dbllist_data(newNode) = data;
    dbllist_next(newNode) = NULL, dbllist_prev(newNode) = NULL;
    /*now I'll check if the list is empty.
     if it's empty I will define the head&tail of the list to be the new node
     else, I will add it to the start of the list, and we are saying that he is the
     first, pointing to the start of the list and his prev == 0
    */
    if(dbllist_head(list) == NULL){// maybe i will do this in private func
        dbllist_head(list) = newNode;
        dbllist_tail(list) = newNode;
    }
    else{
        dbllist_next(newNode) = dbllist_head(list);
        dbllist_prev(dbllist_head(list)) = newNode;
        dbllist_head(list) = newNode;
    }
    dbllist_size(list)++;
    return 0;
}//V

int dbllist_remove(dbllist_t *list, dbllist_node_t* node, dbllist_destroy_t dealloc){
    if(list == NULL || node == NULL) // if the list is empty
        return -1;
    dbllist_node_t *ptr = dbllist_head(list);//????
    if(ptr == node && dbllist_size(list) == 1){//if the head is the node we want to delete & the only one
       if(dealloc == DBLLIST_FREE_DATA)
           free(dbllist_data(node));
        dbllist_head(list) = NULL;//need to free data?
        dbllist_size(list)--;
        free(ptr);
        return 0;
    }
    if(ptr == node){ // if the node is the first
        dbllist_head(list) = dbllist_next(ptr);// the new head
        dbllist_prev(dbllist_next(ptr)) = NULL;
        dbllist_next(ptr) = NULL;
        if(dealloc == DBLLIST_FREE_DATA)
            free(dbllist_data(node));
        dbllist_size(list)--;
        free(ptr);
        return 0;
    }

    ptr = dbllist_next(ptr);//to countinue to the next one
    while(ptr != NULL){
        if(ptr == node){//if we found the node we want to delete
            if(ptr == dbllist_tail(list)){// if the node is the last
                dbllist_tail(list) = dbllist_prev(ptr);
                dbllist_next(dbllist_prev(ptr)) = NULL;
                dbllist_prev(ptr) = NULL;
            }
            else { // if we are in the middle
                printf("here");
                dbllist_next(dbllist_prev(ptr)) = dbllist_next(ptr);
                dbllist_prev(dbllist_next(ptr)) = dbllist_prev(ptr);
                dbllist_next(ptr) = NULL;
                dbllist_prev(ptr) = NULL;
            }
            if(dealloc == DBLLIST_FREE_DATA) {
                free(dbllist_data(node));
            }
            free(ptr);
            ptr = NULL;
            dbllist_size(list)--;
            return 0;
        }
        ptr = dbllist_next(ptr);//to continue to the next one
    }
    return -1;
}
