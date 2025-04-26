#ifndef REGEX_HPP
#define REGEX_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <unordered_set>
#include <sstream>

extern int listid;

enum state_type{
    NORMAL,
    SPLIT,
    MATCH
};

std::string regex_to_postfix(const std::string& regex);

struct State{
    //public:
        char transition_character;
        state_type state_name;
        State* out1;
        State* out2;
        int lastlist{0};

        State()                                                    : transition_character('\0'), state_name(NORMAL), 
                                                                     out1(nullptr), out2(nullptr){};
        State(state_type state_name, State* out1, State* out2)     : transition_character('\0'), state_name(state_name), 
                                                                     out1(out1), out2(out2){};
        State(char transition_character, State* out1, State* out2) : transition_character(transition_character), 
                                                                     state_name(NORMAL), out1(out1), out2(out2){};                                                                                         
};

struct Ptrlist{
    //public:
        State** outptr;
        Ptrlist* next;

    //public:
        Ptrlist() : outptr(nullptr), next(nullptr){};
};

Ptrlist* make_ptrlist(State** outptr);
Ptrlist* append(Ptrlist* l1, Ptrlist* l2);

struct Fragment{
    //public:
        State* start_state;
        Ptrlist* out_list;

        Fragment() : start_state(nullptr), out_list(nullptr){};
        Fragment(State* start_state, Ptrlist* out_list) : start_state(start_state), out_list(out_list){};

        /* ~Fragment(){
            while(out_list != nullptr){
                Ptrlist* to_delete = out_list;
                out_list = out_list -> next;
                delete to_delete;
            }
        } */
};

void patch(Ptrlist* list, State* s);

State* postfix_to_nfa(const std::string& postfix_expr);

bool match(State* start, const std::string& input, std::vector<State*>& clist, std::vector<State*>& nlist);

bool isMatch(std::vector<State*>& clist);

std::vector<State*> start_list(State* s, std::vector<State*>& list);

void add_state(std::vector<State*>& list, State* s);

void step(std::vector<State*>& clist, std::vector<State*>& nlist, char c);

void delete_ptrlist(Ptrlist* list);

void delete_state_objects(State* nfa_start, std::unordered_set<State*>& visited_states);

void print_nfa(State* s, std::unordered_set<State*>& visited);

#endif