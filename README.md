String Pattern Matching Algorithm
Lior Zadah 
The algorithm matches multiple patterns simultaneously, by first constructing a
Deterministic Finite Automaton (DFA) representing the patterns set, and then, with
this DFA on its disposal, processing the text in a single pas.

we got a list of func to implement, in dbllist.c & patternMatching.c 
i will right the expletion on each func.
---------------------------------------------------------------------------------

int dbllist_remove(dbllist_t *, dbllist_node_t* ,dbllist_destroy_t);
// getting a node to remove from the list and in the parameters know if to free or leave the data 

int dbllist_prepend(dbllist_t *,void *);
//adding this node to the start of the list

int dbllist_append(dbllist_t *,void *);
//insert to the end of the listt

void dbllist_destroy(dbllist_t *,dbllist_destroy_t);
//destroy the list and free the data :)
----patternMatching----
    /*
     * the pmt is already allocate. here we malloc the zreo state(ROOT)
     * and init all he's parameters.
     * we made a list for the transitions to know what are the next state of the machine
     */
int pm_init(pm_t * pmt){


/*
 * AddString is very important func because it connects between the set&get func to build the black arrows.
 * we declare on two states that points to the ROOT, then we will seek state with the compatible letter,
 * we will continue with this loop until we will not find the right state, every time we progress we take the
 * second state with us.
 * then, if GET == NULL we create new state, update his data(id,depth...) and proceed to the next.
 */
int pm_addstring(pm_t * pmt, unsigned char *string, size_t n)


/*
 * This Func is very important for our program, and the most complicated. i will do my best
 * start with declare on a new list and init it. then with a node were going to move on the states
 * with depth 1. add them all to our queue & set their fail = 0;
 *
 */
int pm_makeFSM(pm_t * pmt)


/*
 * in the set func i create new state whit the char on the edge,
 * main use in edge becuose we create a new one.
 */
int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state)


/*
 * At the get func we want to move to the next avilable state whit the same letter
 * so we didn't need to create new state, thats how we create the tree.
 */
pm_state_t* pm_goto_get(pm_state_t *state, unsigned char symbol)

/*  start with check the pmt and open a list & init it
 * Move on string len and find patterns
 * 1st while is a try to progress to the next state, and if it cant, and it's not the root,
 * go to the state fail hom we created in make_FSM
 * Later, I ask if I can get to the next state, if so take pmt there.
 */
dbllist_t* pm_fsm_search(pm_state_t * pmt,unsigned char * string, size_t n)

/*
 * the Distroy func will deAlloc evrey memory we alloceted in the program
 * we will free the data on the edge - letter and states.
 * we will move on using the root. move from him, adding every one of his neigbors
 * and move
 */
void pm_destroy(pm_t * pmt)

----How To Compile-----
run - gcc dbllist.c pattern_matching.c main.c -o main
	./main

---input--- 
list of strings and a text, after we will search this patterns in the text 

---output---
telling us the list of edges from were to were. the fail func -> how gose were







