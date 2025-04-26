#include "regex.hpp"

int listid{0};

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
    Fragment *new_frag, *frag, *frag1, *frag2;
    State* new_state;
    Ptrlist* new_list;
    for(const char& c : postfix_expr){
        switch(c){

            case '.' :                                             // CONCATENATION
                frag2 = fragment_stack.top();
                fragment_stack.pop();
                frag1 = fragment_stack.top();
                fragment_stack.pop();
                patch(frag1 -> out_list, frag2 -> start_state);
                delete_ptrlist(frag1 -> out_list);
                new_frag = new Fragment(frag1 -> start_state, frag2 -> out_list);
                fragment_stack.push(new_frag);
                delete frag1;
                delete frag2;
                break;

            case '|':                                             // ALTERNATION
                frag2 = fragment_stack.top();
                fragment_stack.pop();
                frag1 = fragment_stack.top();
                fragment_stack.pop();
                new_state = new State(SPLIT, frag1 -> start_state, frag2 -> start_state);
                new_list = append(frag1 -> out_list, frag2 -> out_list);
                new_frag = new Fragment(new_state, new_list);
                fragment_stack.push(new_frag);
                delete frag1;
                delete frag2;
                break;

            case '?':                                             //EXISTENCE
                frag = fragment_stack.top();
                fragment_stack.pop();
                new_state = new State(SPLIT, frag -> start_state, nullptr);               
                new_list = append(frag -> out_list, make_ptrlist(&(new_state -> out2)));
                new_frag = new Fragment(new_state, new_list);
                fragment_stack.push(new_frag);
                delete frag;
                break;

            case '*':                                             // KLENEE STAR
                frag = fragment_stack.top();
                fragment_stack.pop();
                new_state = new State(SPLIT, frag -> start_state, nullptr);
                patch(frag -> out_list, new_state);
                delete_ptrlist(frag -> out_list);
                new_frag = new Fragment(new_state, make_ptrlist(&(new_state -> out2)));
                fragment_stack.push(new_frag);
                delete frag;
                break;

            case '+':                                             //ONE OR MORE EXISTENCE
                frag = fragment_stack.top();
                fragment_stack.pop();
                new_state = new State(SPLIT, frag -> start_state, nullptr);
                patch(frag -> out_list, new_state);
                delete_ptrlist(frag -> out_list);
                new_frag = new Fragment(frag -> start_state, make_ptrlist(&(new_state -> out2)));
                fragment_stack.push(new_frag);
                delete frag;
                break;

            default:
                new_state = new State(c, nullptr, nullptr);
                new_frag = new Fragment(new_state, make_ptrlist(&(new_state -> out1)));
                fragment_stack.push(new_frag);
                break;
        }
    }

    frag = fragment_stack.top();
    fragment_stack.pop();
    State* matchstate = new State(MATCH, nullptr, nullptr);
    patch(frag -> out_list, matchstate);
    State* start = frag -> start_state;
    delete_ptrlist(frag -> out_list);
    delete frag;
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
    while(list != nullptr){
        *(list -> outptr) = s;
        list = list -> next;
    }
}

std::vector<State*> start_list(State* s, std::vector<State*>& list){
    listid++;
    list.clear();
    add_state(list, s);
    return list;
}

bool match(State* start, const std::string& input, std::vector<State*>& l1, std::vector<State*>& l2){
    std::vector<State*> clist;
    std::vector<State*> nlist;
    clist = start_list(start, l1);
    nlist = l2;
    for(const char& c: input){
        step(clist, nlist, c);
        std::vector<State*> temp = std::move(clist);
        clist = std::move(nlist);
        nlist = std::move(temp);
    }
    return isMatch(clist);
}

bool isMatch(std::vector<State*>& list){
    for(auto& state : list){
        if(state -> state_name == MATCH) return true;
    }
    return false;
}

void add_state(std::vector<State*>& list, State* s){
    if(!s || s -> lastlist == listid) return;
    s -> lastlist = listid;
    if(s -> state_name == SPLIT){
        add_state(list, s -> out1);
        add_state(list, s -> out2);
        return;
    } 
    list.push_back(s);
}

void step(std::vector<State*>& clist, std::vector<State*>& nlist, char c){
    State* s;
    listid++;
    nlist.clear();
    for(auto& state : clist){
        if(state -> transition_character == c){
            add_state(nlist, state -> out1);
        }
    }
}

void delete_ptrlist(Ptrlist* list){
    while (list != nullptr) {
        Ptrlist* next = list->next;
        delete list;
        list = next;
    }
}

void delete_state_objects(State* nfa_start){
    if(nfa_start == nullptr) return;

    delete_state_objects(nfa_start -> out1);
    delete_state_objects(nfa_start -> out2);

    delete nfa_start;
}

void print_nfa(State* s, std::unordered_set<State*>& visited){
    if (!s || visited.count(s)) return;
    visited.insert(s);
    
    std::cout << "State " << s << ": ";
    if (s->state_name == MATCH) {
        std::cout << "MATCH\n";
        return;
    }
    if (s->state_name == SPLIT) {
        std::cout << "SPLIT -> " << s->out1 << ", " << s->out2 << "\n";
        print_nfa(s->out1, visited);
        print_nfa(s->out2, visited);
    } else {
        std::cout << "'" << s->transition_character << "' -> " << s->out1 << "\n";
        print_nfa(s->out1, visited);
    }
}