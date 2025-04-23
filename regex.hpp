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

        State(char transition_character) : transition_character(transition_character), out1(nullptr), out2(nullptr){};
};

struct Ptrlist{
    //public:
        State** outptr;
        Ptrlist* next;

    //public:
        Ptrlist() : outptr(nullptr), next(nullptr){};
        Ptrlist* make_ptrlist(State** outptr);
        Ptrlist* append(Ptrlist* l1, Ptrlist* l2);
};

struct Fragment{
    //public:
        State* start_state;
        Ptrlist* out_list;

        Fragment() : start_state(nullptr), out_list(nullptr){};
};

void patch(Ptrlist* list, State* s);

State* postfix_to_nfa(std::string& postfix_expr);

#endif