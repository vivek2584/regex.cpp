#ifndef REGEX_HPP
#define REGEX_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>

std::string regex_to_postfix(const std::string& regex);

struct State{
    //public:
        char transition_character;
        State* out1;
        State* out2;
};

struct Ptrlist{
    //public:
        State** outptr;
        Ptrlist* next;

    //public:
        Ptrlist* make_ptrlist(State** outptr);
        Ptrlist* append(Ptrlist* l1, Ptrlist* l2);
};

struct Fragment{
    //public:
        State* start_state;
        Ptrlist* out_list;
};

void patch(Ptrlist* list, State* s);

State* postfix_to_nfa(std::string& postfix_expr);

#endif