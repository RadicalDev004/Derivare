#include <iostream>
#include <string>
#include <stack>
#include "graphics.h"
#pragma comment(lib,"graphics.lib")
using namespace std;


// Verifica parantezele expresiei matematice
bool verificaParanteze(std::string expresie) {
    std::stack<char> stiva;
    for (char ch : expresie) {
        if (ch == '(') {
            stiva.push(ch);
        }
        else if (ch == ')') {
            if (stiva.empty()) return false;
            stiva.pop();
        }
    }
    return stiva.empty();  // daca stiva este goala, parantezele sunt corecte
}

// Verifica validitatea expresiei matematice
bool verificare(std::string expresie) {
    return verificaParanteze(expresie);
    // adauga alte verificari daca este necesar
}

// Returneaza prioritatea operatorilor
int prioritate(char operatorr) {
    switch (operatorr) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    default:
        return 0;
    }
}

// Converteste expresia intr-un arbore postfixat
std::string postfixare(std::string expresie) {
    std::stack<char> stiva;
    std::string arborePostfixat;
    for (char ch : expresie) {
        if (isdigit(ch) || isalpha(ch)) {  // daca este numar sau litera
            arborePostfixat += ch;
        }
        else if (ch == '(') {
            stiva.push(ch);
        }
        else if (ch == ')') {
            while (!stiva.empty() && stiva.top() != '(') {
                arborePostfixat += stiva.top();
                stiva.pop();
            }
            stiva.pop();  // elimina '('
        }
        else {  // daca este operator
            while (!stiva.empty() && prioritate(ch) <= prioritate(stiva.top())) {
                arborePostfixat += stiva.top();
                stiva.pop();
            }
            stiva.push(ch);
        }
    }
    while (!stiva.empty()) {
        arborePostfixat += stiva.top();
        stiva.pop();
    }
    return arborePostfixat;
}

int main()
{
    std::string expresie;
    std::cout << "Introduceti o expresie matematica: ";
    std::cin >> expresie;

    if (verificare(expresie)) {
        std::string arborePostfixat = postfixare(expresie);
        std::cout << "Arborele postfixat al expresiei este: " << arborePostfixat << std::endl;
    }
    else {
        std::cout << "Expresia matematica introdusa este invalida." << std::endl;
    }

    return 0;
}

