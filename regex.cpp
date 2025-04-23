#include "regex.hpp"

std::string regex_to_postfix(const std::string& regex){
    std::unordered_map<char, int> operator_precedence =   
    {
        {'*', 3},
        {'+', 3},
        {'?', 3},
        {'.', 2},
        {'|', 1}
    };

    std::stack<char> operator_stack;
    std::string postfix_notation;
    postfix_notation.reserve(regex.size());

    std::string regex_copy;
    for (size_t i = 0; i < regex.size(); ++i) {
        char c1 = regex[i];
        regex_copy += c1;

        if (i + 1 < regex.size()) {
            char c2 = regex[i + 1];
            if ((std::isalnum(c1) || c1 == ')' || c1 == '*' || c1 == '+' || c1 == '?') &&
                (std::isalnum(c2) || c2 == '(')) {
                regex_copy += '.';
            }
        }
    }

    for(const char& c : regex_copy){
        if(std::isalnum(c)){
            postfix_notation += c;
            continue;
        }
        else if(c == '('){
            operator_stack.push(c);
        }
        else if(c == ')'){
            while(!operator_stack.empty() && operator_stack.top() != '('){
                postfix_notation += operator_stack.top();
                operator_stack.pop();
            }
            operator_stack.pop();
        }
        else{
            while(!operator_stack.empty() && operator_precedence[c] <= operator_precedence[operator_stack.top()]){
                postfix_notation += operator_stack.top();
                operator_stack.pop();
            }
            operator_stack.push(c);
        }
    }
    while(!operator_stack.empty()){
        postfix_notation += operator_stack.top();
        operator_stack.pop();
    }
    return postfix_notation;
}

State* postfix_to_nfa(const std::string& postfix_expr){
    std::stack<Fragment*> fragment_stack;
    Fragment *frag, *frag1, *frag2;
    for(const char& c : postfix_expr){
        switch(c){

            case '.' :                                             // CONCATENATION
                frag1 = fragment_stack.top();
                fragment_stack.pop();
                frag2 = fragment_stack.top();
                fragment_stack.pop();
                patch(frag1 -> out_list, frag2 -> start_state);
                Fragment* new_fragment = new Fragment(frag1 -> start_state, frag2 -> out_list);
                fragment_stack.push(new_fragment);
                break;

            case '|':                                             // ALTERNATION
                frag1 = fragment_stack.top();
                fragment_stack.pop();
                frag2 = fragment_stack.top();
                fragment_stack.pop();
                State* new_state = new State(SPLIT, frag1 -> start_state, frag2 -> start_state);
                Ptrlist* new_list = append(frag1 -> out_list, frag2 -> out_list);
                Fragment* new_fragment = new Fragment(new_state, new_list);
                fragment_stack.push(new_fragment);
                break;

            case '?':                                             //EXISTENCE
                frag = fragment_stack.top();
                fragment_stack.pop();
                State* new_state = new State(SPLIT, frag -> start_state, nullptr);
                Ptrlist* new_list = append(frag -> out_list, make_ptrlist(&(new_state -> out1)));
                Fragment* new_fragment = new Fragment(new_state, new_list);
                fragment_stack.push(new_fragment);
                break;

            case '*':                                             // KLENEE STAR
                frag = fragment_stack.top();
                fragment_stack.pop();
                State* new_state = new State(SPLIT, frag -> start_state, nullptr);
                patch(frag -> out_list, new_state);
                Fragment* new_fragment = new Fragment(new_state, make_ptrlist(&(new_state -> out1)));
                fragment_stack.push(new_fragment);
                break;

            case '+':                                             //ONE OR MORE EXISTENCE
                frag = fragment_stack.top();
                fragment_stack.pop();
                State* new_state = new State(SPLIT, frag -> start_state, nullptr);
                patch(frag -> out_list, new_state);
                Fragment* new_fragment = new Fragment(frag -> start_state, make_ptrlist(&(new_state -> out1)));
                fragment_stack.push(new_fragment);
                break;

            default:
                State* new_state = new State(c, nullptr, nullptr);
                Fragment* new_fragment = new Fragment(new_state, make_ptrlist(&(new_state -> out1)));
                fragment_stack.push(new_fragment);
                break;
        }
    }

    frag = fragment_stack.top();
    fragment_stack.pop();
    State* matchstate = new State(MATCH, nullptr, nullptr);
    patch(frag -> out_list, matchstate);
    State* start = frag -> start_state;
    return start;  
}

Ptrlist* make_ptrlist(State** outptr){
    Ptrlist* new_ptr_list = new Ptrlist;
    new_ptr_list -> outptr = outptr;
    return new_ptr_list;
}

Ptrlist* append(Ptrlist* l1, Ptrlist* l2){
    if(!l1) return l2;
    Ptrlist* head = l1;
    while(l1 -> next != nullptr){
        l1 = l1 -> next;
    }
    l1 -> next = l2;
    return head;
}

void patch(Ptrlist* list, State* s){
    while(list -> next != nullptr){
        *(list -> outptr) = s;
        list = list -> next;
    }
}