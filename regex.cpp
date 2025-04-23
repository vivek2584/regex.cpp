#include "regex.hpp"

std::string toPostfix(const std::string& regex){
    std::unordered_map<char, int> op_precedence =   
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
            while(operator_stack.top() != '('){
                postfix_notation += operator_stack.top();
                operator_stack.pop();
            }
            operator_stack.pop();
        }
        else{
            while(!operator_stack.empty() && op_precedence[c] <= op_precedence[operator_stack.top()]){
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