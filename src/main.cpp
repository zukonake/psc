#include <cmath>
#include <cstdio>
#include <string>
#include <stack>
#include <iostream>
#include <functional>

typedef double Val;

std::stack<Val> stack;
bool running;

void error(std::string const &msg) {
    fprintf(stderr, "psc: %s\n", msg.c_str());
}

int top(Val *val) {
    if(stack.size() == 0) {
        error("stack empty");
        return -1;
    } else {
        *val = stack.top();
        return 0;
    }
}

int pop() {
    if(stack.size() == 0) {
        error("stack empty");
        return -1;
    } else {
        stack.pop();
        return 0;
    }
}

void push(Val val) {
    stack.push(val);
}

void apply_unop(std::function<Val(Val const &)> f) {
    Val a;
    if(top(&a)) return;
    pop();
    push(f(a));
}

void apply_binop(std::function<Val(Val const &, Val const &)> f) {
    Val a;
    if(top(&a)) return;
    pop();
    Val b;
    if(top(&b)) return;
    pop();
    push(f(a, b));
}

void print_top() {
    Val a;
    if(top(&a)) return;
    printf("%f\n", a);
}

void dump_stack() {
    std::stack copy = stack;
    while(!copy.empty()) {
        printf("%f\n", copy.top());
        copy.pop();
    }
}

void duplicate() {
    Val a;
    if(top(&a)) return;
    push(a);
}

void clear_stack() {
    while(!stack.empty()) {
        stack.pop();
    }
}

void quit() {
    running = false;
}

Val w_sqrt(Val const &a) {
    return std::sqrt(a);
}

Val w_round(Val const &a) {
    return std::round(a);
}

Val w_floor(Val const &a) {
    return std::floor(a);
}

Val w_pow(Val const &a, Val const &b) {
    return std::pow(a, b);
}

Val fract(Val const &a) {
    return a - std::floor(a);
}

Val reciprocal(Val const &a) {
    return 1.0 / a;
}

int parse_digit(char ch, Val *val, unsigned base) {
    Val digit;
         if(ch == '0') digit = 0;
    else if(ch == '1') digit = 1;
    else if(ch == '2') digit = 2;
    else if(ch == '3') digit = 3;
    else if(ch == '4') digit = 4;
    else if(ch == '5') digit = 5;
    else if(ch == '6') digit = 6;
    else if(ch == '7') digit = 7;
    else if(ch == '8') digit = 8;
    else if(ch == '9') digit = 9;
    else if(ch == 'A' || ch == 'a') digit = 10;
    else if(ch == 'B' || ch == 'b') digit = 11;
    else if(ch == 'C' || ch == 'c') digit = 12;
    else if(ch == 'D' || ch == 'd') digit = 13;
    else if(ch == 'E' || ch == 'e') digit = 14;
    else if(ch == 'F' || ch == 'f') digit = 15;
    else {
        error(std::string("invalid digit ") + ch);
        return -1;
    }

    if(digit < base) {
        *val = digit;
        return 0;
    } else {
        error("invalid base " + std::to_string(base) + " for digit " + ch);
        return -1;
    }
}

int parse_fraction(std::string const &fraction, Val *val, unsigned base) {
    Val temp = 0.0;
    Val mult = 1.0 / (Val)base;
    for(auto it = fraction.cbegin(); it != fraction.cend(); ++it) {
        Val digit;
        if(parse_digit(*it, &digit, base)) return -1;
        temp += digit * mult;
        mult /= (Val)base;
    }
    *val = temp;
    return 0;
}

int parse_number(std::string const &number) {
    if(number.size() == 0) {
        error("empty word given");
        return -1;
    }
    unsigned base;
         if(number[0] == 'b') base = 2;
    else if(number[0] == 'x') base = 16;
    else if(number[0] == 'o') base = 8;
    else base = 10;
    unsigned start = base == 10 ? 0 : 1;
    Val val = 0.0;
    for(auto it = number.cbegin() + start; it != number.cend(); ++it) {
        Val digit;
        if(*it == '.') {
            Val fraction;
            if(parse_fraction(std::string(it + 1, number.cend()),
                               &fraction, base)) return -1;
            val += fraction * (Val)base;
            break;
        }
        if(parse_digit(*it, &digit, base)) return -1;
        val += digit;
        val *= (Val)base;
    }
    val /= (Val)base;
    push(val);
    return 0;
}

int parse_word(std::string const &word) {
         if(word == "+" || word == "add")   apply_binop(std::plus<Val>());
    else if(word == "-" || word == "sub")   apply_binop(std::minus<Val>());
    else if(word == "*" || word == "mul")   apply_binop(std::multiplies<Val>());
    else if(word == "/" || word == "div")   apply_binop(std::divides<Val>());
    else if(word == "v" || word == "sqrt")  apply_unop(w_sqrt);
    else if(word == "^" || word == "pow")   apply_binop(w_pow);
    else if(word == "~" || word == "round") apply_unop(w_round);
    else if(word == "fl")                   apply_unop(w_floor);
    else if(word == "fr")                   apply_unop(fract);
    else if(word == "rcp")                  apply_unop(reciprocal);
    else if(word == "_" || word == "neg")   apply_unop(std::negate<Val>());
    else if(word == "=")    print_top();
    else if(word == "dump") dump_stack();
    else if(word == "d")    duplicate();
    else if(word == "c")    clear_stack();
    else if(word == "q")    quit();
    else return parse_number(word);
    return 0;
}

void parse_line(std::string const &line) {
    std::string word;
    for(auto it = line.cbegin(); it != line.cend(); ++it) {
        if(*it == ' ') {
            if(parse_word(word)) error("couldn't parse " + word);
            word = "";
        } else if(it == line.cend() - 1) {
            if(parse_word(word + *it)) error("couldn't parse " + word + *it);
            word = "";
        } else word += *it;
    }
}

int main() {
    std::string input;
    running = true;
    while(running)
    {
        std::getline(std::cin, input);
        parse_line(input);
        input = "";
    }
}
