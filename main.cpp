#include "regex.hpp"

int main(int argc, char** argv){
    
    while(true){

        std::string input{""};
        std::cout << "regex > ";
        std::getline(std::cin, input);
        std::istringstream iss(input);
        std::vector<std::string> words;
        std::string word;

        while(iss >> word){
            words.push_back(word);
        }

        if (words[0] == ".exit") return 0;

        if (words.size() < 2 || words.size() > 2){
            std::cout << "<regexpression> <input_string>\n";
            return 1;
        }

        std::string regex = words[0];
        std::string input_string = words[1];

        std::string postfix_expr = regex_to_postfix(regex);

        // std::cout << postfix_expr << "\n";

        State* nfa_start = postfix_to_nfa(postfix_expr);

        std::vector<State*> l1;
        std::vector<State*> l2;

        //std::unordered_set<State*> visited;
        //print_nfa(nfa_start, visited);

        bool matched = match(nfa_start, input_string, l1, l2);

        if(matched){
            std::cout << "Input String Matched\n";
        }
        else{
            std::cout << "Input String not matched\n";
        }

        std::unordered_set<State*> visited_states;
        delete_state_objects(nfa_start, visited_states);
    }

    return 0;
}