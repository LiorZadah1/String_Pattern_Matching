#include "slist.h"
#include "pattern_matching.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

int pm_init(pm_t * pmt){
    /*
     * the pmt is already allocate. here we malloc the zreo state(ROOT)
     * and init all he's parameters.
     * we made a list for the transitions to know what are the next state of the machine
     */
    pmt -> zerostate = (pm_state_t*)malloc(sizeof(pm_state_t));
    pmt -> zerostate -> _transitions = (dbllist_t*) malloc(sizeof(dbllist_t));
    pmt -> zerostate -> output = (dbllist_t*) malloc(sizeof(dbllist_t));//??? is it belong here?
    if (pmt -> zerostate == NULL || pmt -> zerostate -> output == NULL ||pmt -> zerostate -> _transitions == NULL )
        return -1;
    pmt -> zerostate -> id = 0;
    pmt -> zerostate -> depth = 0;
    pmt -> newstate = 1;
    dbllist_init(pmt -> zerostate -> _transitions);//init the new list of the states.
    dbllist_init(pmt -> zerostate -> output);//init the new list of the states.???
    pmt -> zerostate -> fail = NULL;
    return 0;
}

/*
 * AddString is very important func because it connects between the set&get func to build the black arrows.
 * we declare on two states that points to the ROOT, then we will seek state with the compatible letter,
 * we will continue with this loop until we will not find the right state, every time we progress we take the
 * second state with us.
 * then, if GET == NULL we create new state, update his data(id,depth...) and proceed to the next.
 */
int pm_addstring(pm_t * pmt, unsigned char *string, _size_t n){
    /*
     * pmt is the pointer to the machine with the root.
     * we need to add the machine the char at size n
     */
    pm_state_t * stateOne = pmt ->zerostate;
    pm_state_t * stateTwo = pmt ->zerostate;
    int i;//index to move on the letters on the string
    for (i = 0; i < n; i++) {
        stateOne = pm_goto_get(stateOne, string[i]);
        if(stateOne == NULL){
            break;
        }
        stateTwo = stateOne;//(from state == to state)
    }///need to check if we are missing a letter because of the string [i+1]
    ///what is happening when i don't need to get to the set?

    //here we check if we break from the loop at 33
    //if we did we want to creat new black arrows from the state we are to the next one
    //One is after two because we didn't get to line 37 which we promote stateTwo
        while(i < n){
            stateOne = (pm_state_t *) malloc(sizeof (pm_state_t));
            if(stateOne == NULL){
                perror("malloc FAIL");
                return -1;
            }
            stateOne -> depth = stateTwo -> depth + 1;
            stateOne -> id = pmt -> newstate;
            pmt -> newstate++; //new state is added
            stateOne -> _transitions = (dbllist_t*) malloc(sizeof(dbllist_t));//in set func
            stateOne -> output = (dbllist_t*) malloc(sizeof(dbllist_t));//in set func
            if(stateOne->output == NULL || stateOne->_transitions == NULL){
                perror("malloc FAIL");
                return -1;
            }
            dbllist_init(stateOne->_transitions);
            dbllist_init(stateOne->output);
            stateOne -> fail = NULL;
            pm_goto_set(stateTwo, string[i], stateOne);
            stateTwo = stateOne;
            stateOne = NULL;
            i++;
        }
    dbllist_append(stateTwo -> output, string);
    return 0;
}

/*
 * This Func is very important for our program, and the most complicated. i will do my best
 * start with declare on a new list and init it. then with a node were going to move on the states
 * with depth 1. add them all to our queue & set their fail = 0;
 *
 */
int pm_makeFSM(pm_t * pmt){
    dbllist_t* queue = (dbllist_t*)malloc(sizeof (dbllist_t));//the list with the states.depth=1
    if(queue == NULL){
        perror("malloc FAIL");
        return -1;
    }
    dbllist_init(queue);
    dbllist_node_t* ptr1 = dbllist_head(pmt -> zerostate -> _transitions);
    while(ptr1 != NULL){
        pm_labeled_edge_t* ptrState = (pm_labeled_edge_t*)dbllist_data(ptr1);
        dbllist_append(queue, ptrState->state);// adding state with depth 1 to the queue!!!
        ptrState->state->fail = pmt->zerostate; //make the fail func of them to br the root
        ptr1 = dbllist_next(ptr1);
        printf("Setting f(%d) = %d\n", ptrState->state->id, ptrState->state->fail->id);//print the fail func

    }
    ptr1 = NULL;
    free(ptr1);
    //use the head of the queue, remove it and add all his neighbors
    while (dbllist_size(queue) > 0){
        dbllist_node_t *ptr = dbllist_head(queue);
        pm_state_t * trans = ((pm_state_t*)dbllist_data(ptr));//head of the edge list
        dbllist_node_t *temp = dbllist_head(trans->_transitions);//holds the head of the transitions
        dbllist_remove(queue, ptr, DBLLIST_LEAVE_DATA);
        while (temp != NULL){//moving on state childes
            pm_labeled_edge_t * edge = (pm_labeled_edge_t*)dbllist_data(temp);//the edge moving from the state
            dbllist_append(queue, edge->state); //the state we want to insert to the list
            pm_state_t* transFAIL = trans->fail;
            int flag = 0; //flag to let us know if we got a hit
            while (transFAIL != NULL){
                dbllist_node_t* fail = dbllist_head(transFAIL->_transitions);//node that points to the head of the trans->fail->transitions list
                while (fail != NULL){
                    if(((pm_labeled_edge_t*)dbllist_data(fail))->label == edge->label){
                        edge->state->fail = ((pm_labeled_edge_t*)dbllist_data(fail))->state; //compare the states, if so break;
                        flag = 1; // need to exit the big loop also
                        break;
                    }
                    fail = dbllist_next(fail);//progress the loop
                }
                if(flag == 1)
                    break;
                transFAIL = transFAIL->fail;//progress the loop
            }
            if(edge->state->fail == NULL)
                edge->state->fail = pmt->zerostate;//if we didn't found fail state: the root is the fail!
            printf("Setting f(%d) = %d\n", edge->state->id, edge->state->fail->id);//print the fail func

            //connect between two of the lists!!
            if(edge->state->fail->output != NULL){
                dbllist_node_t* outputNode = dbllist_head(edge->state->fail->output);
                while(outputNode != NULL){
                    dbllist_append(edge->state->output,dbllist_data(outputNode));
                    outputNode = dbllist_next(outputNode);
                }
            }
            temp = dbllist_next(temp);
        }
    }
    dbllist_destroy(queue, DBLLIST_LEAVE_DATA);
    return 0;
}

/*
 * in the set func i create new state whit the char on the edge,
 * main use in edge becuose we create a new one.
 */
int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){
    if (from_state == NULL || to_state == NULL)
        return -1;
    //create new labeled edge and insert the symble and the state
    pm_labeled_edge_t* edge = (pm_labeled_edge_t*) malloc(sizeof (pm_labeled_edge_t));
    if(edge == NULL){
        perror("Malloc FAIL");
        return -1;
    }
    edge -> label = symbol;
    edge -> state = to_state;
    if(dbllist_append(from_state->_transitions, edge) != -1){
        // if append worked, print the state id and from where to where & the symbol
        printf("Allocating state %d\n", to_state -> id);//belong here?
        printf("%d -> %c -> %d\n",from_state -> id, edge -> label, to_state -> id );
    }
    else{
        fprintf(stderr, "append FAIL");
        return -1;
    }
    return 0;
}

/*
 * At the get func we want to move to the next avilable state whit the same letter
 * so we didn't need to create new state, thats how we create the tree.
 */
pm_state_t* pm_goto_get(pm_state_t *state, unsigned char symbol){
    if (state == NULL)
        return NULL;
    //ptr point to the head of the neibors of the given state.
    dbllist_node_t* ptr = dbllist_head(state -> _transitions);//the head of the list
    if(ptr == NULL)
        return NULL;
    while (ptr != NULL){//while we still can move to the next edge
        pm_labeled_edge_t *edge =(pm_labeled_edge_t*)(dbllist_data(ptr));
        if(edge -> label == symbol){
            return edge -> state;
        }
        ptr = (dbllist_next(ptr));//the promotion to the loop
    }
    return NULL;
}

/*  start with check the pmt and open a list & init it
 * Move on string len and find patterns
 * 1st while is a try to progress to the next state, and if it cant, and it's not the root,
 * go to the state fail hom we created in make_FSM
 * Later, I ask if I can get to the next state, if so take pmt there.
 */
dbllist_t* pm_fsm_search(pm_state_t * pmt,unsigned char * string, _size_t n){
    if(pmt == NULL)
        return NULL;
//Create a list that we can return & init it
    dbllist_t * list = (dbllist_t*) malloc(sizeof (dbllist_t));
    if (list == NULL){
        perror("malloc FAIL");
        return NULL;
    }
    dbllist_init(list);

    for(int i = 0; i < n; i++){
        while(pm_goto_get(pmt, string[i]) == NULL && pmt->id != 0) {
            pmt = pmt->fail;
        }

        if(pm_goto_get(pmt,string[i]) != NULL){
            pmt = pm_goto_get(pmt, string[i]);
        }
/*
 * now, i ask if the outpot of the func is !NULL & it's not empty.
 * later, i create a node so i could move on the output list.
 * then, if the node & the list is not null we move on and create the match,
 * update start&end&pattern, add the match to the list and move to the next node.
 */
        if(pmt->output != NULL&&pmt->output->size > 0 ){
            //creat match & update data
            dbllist_node_t* headOfOutput = dbllist_head(pmt->output);
            int startPos =(i - (int)(strlen(dbllist_data(headOfOutput))) + 1) ;
            int cursor = i - startPos;
            while(headOfOutput != NULL && pmt->output != NULL){
                pm_match_t * match = (pm_match_t*) malloc(sizeof (pm_match_t));
                if (match == NULL){
                    perror("malloc FAIL");
                    return NULL;
                }

                match->start_pos = i - (int)(strlen(dbllist_data(headOfOutput))) + 1;
                match->end_pos = i;
                match->fstate = pmt;
//                strncpy(match->pattern, (const char *) string[startPos], cursor);
                match->pattern = (char *) dbllist_data(headOfOutput);/// NEED TO UPDATE PATTERN
                cursor++;
//                dbllist_append(list,dbllist_data(headOfOutput));
                dbllist_append(list,match);//??? what is good  ???
                headOfOutput = dbllist_next(headOfOutput);
            }
        }
    }
    return list;
}
/*
 * the Distroy func will deAlloc evrey memory we alloceted in the program
 * we will free the data on the edge - letter and states.
 * we will move on using the root. move from him, adding every one of his neigbors
 * and move
 */
void pm_destroy(pm_t * pmt){
    if(pmt == NULL)
        return;
    //creating a list and check it & init
    dbllist_t* list = (dbllist_t*) malloc(sizeof(dbllist_t));
    if(list == NULL){
        perror("malloc FAIL");
        exit(-1);
    }
    dbllist_init(list);
    //append the root into the list
    if(dbllist_append(list,pmt->zerostate) < 0){
        perror("append FAIL");
        exit(-1);
    }
//with double loop we will enter each state and distory - de alloc every peace of data it have
    while(dbllist_size(list) > 0) {
        dbllist_node_t *headPtr = dbllist_head(list);
        pm_state_t *headData = dbllist_data(headPtr);
        if (headData == NULL){
            break;
        }//here to dealloc every one of the neighbors the state have.
        dbllist_node_t *distroier = dbllist_head(headData->_transitions);
        while(distroier != NULL){
            int temp = dbllist_append(list, ((pm_labeled_edge_t*)dbllist_data(distroier))->state);
            if(temp == -1){
                exit(-1);
            }//condition of progress
            distroier = dbllist_next(distroier);
        }//destroy all lists and dealloc
        dbllist_destroy(headData->output, DBLLIST_LEAVE_DATA);
        dbllist_destroy(headData->_transitions, DBLLIST_FREE_DATA);
        dbllist_remove(list, headPtr, DBLLIST_FREE_DATA);
    }
    dbllist_destroy(list, DBLLIST_FREE_DATA);
    //free(pmt);
}

