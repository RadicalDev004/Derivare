#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include "graphics.h"
#include "ArboreBinar.cpp"
#pragma comment(lib,"graphics.lib")
using namespace std;
ArbNod* ArbNod::Temp = nullptr;

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

                if (arborePostfixat.size() != 1) {
                    ArbNod *newNod = new ArbNod(
                        stiva.top(), 
                        new ArbNod(arborePostfixat.at(arborePostfixat.size() - 2)), 
                        new ArbNod(arborePostfixat.at(arborePostfixat.size() - 1))
                        );
                   
                    arborePostfixat.pop_back(); arborePostfixat.pop_back();
                    ArbNod::Temp = newNod;
                }
                else {
                    ArbNod *newNod = new ArbNod(
                        stiva.top(), 
                        new ArbNod(arborePostfixat.at(arborePostfixat.size() - 1)),
                        ArbNod::Temp
                    );

                    arborePostfixat.pop_back();
                    ArbNod::Temp = newNod;
                }
                

                //arborePostfixat += stiva.top();
                stiva.pop();
            }
            stiva.push(ch);
        }
    }
    while (!stiva.empty()) {

        if (arborePostfixat.size() < 2) {
            ArbNod* newNod = new ArbNod(
                stiva.top(),
                new ArbNod(arborePostfixat.at(arborePostfixat.size() - 1)),
                ArbNod::Temp
            );

            arborePostfixat.pop_back();
            ArbNod::Temp = newNod;
        }
        else {
            ArbNod* newNod = new ArbNod(
                stiva.top(),
                new ArbNod(arborePostfixat.at(arborePostfixat.size() - 2)),
                new ArbNod(arborePostfixat.at(arborePostfixat.size() - 1))
            );
            
            arborePostfixat.pop_back(); arborePostfixat.pop_back();

            if (ArbNod::Temp != nullptr) {
                stiva.pop();
                ArbNod* tp = new ArbNod(
                    stiva.top(),
                    newNod,
                    ArbNod::Temp
                );
                ArbNod::Temp = tp;
            }
            else
                ArbNod::Temp = newNod;
        }      

        //arborePostfixat += stiva.top();
        stiva.pop();
    }
    return arborePostfixat;
}

void Derivare(ArbNod* oper);
char DerivareIndiv(char info);
void SDR(ArbNod* rad);
void SRD(ArbNod* rad);

int main()
{
    std::string expresie;
    std::cout << "Introduceti o expresie matematica: ";
    std::cin >> expresie;

    if (verificare(expresie)) {
        std::string arborePostfixat = postfixare(expresie);

        SRD(ArbNod::Temp);
        
        SDR(ArbNod::Temp);
        cout << '\n';
        SRD(ArbNod::Temp);
    }
    else {
        std::cout << "Expresia matematica introdusa este invalida." << std::endl;
    }

    return 0;
}

char DerivareIndiv(char info)
{
    if (isalpha(info))
        return '1';
    if (isdigit(info))
        return '0';

}
void Derivare(ArbNod* oper)
{
    switch (oper->info)
    {
    case '+':
    {
        oper->St->info = DerivareIndiv(oper->St->info);
        oper->Dr->info = DerivareIndiv(oper->Dr->info);
        break;
    }
    case '^':
    {
        if (isalpha(oper->St->info) && isdigit(oper->Dr->info)) {
            oper->Dr->info = (oper->Dr->info - '0' - 1 + '0');
        }
        break;
    }
    }

}
void SRD(ArbNod* rad)
{
    if (rad->St != nullptr)
        SRD(rad->St);
    cout << rad->info;
    if (rad->Dr != nullptr)
        SRD(rad->Dr);
}
void SDR(ArbNod* rad)
{
    if (rad->St != nullptr)
        SDR(rad->St);
    if (rad->Dr != nullptr)
        SDR(rad->Dr);

    if (rad->HasChildren() && !rad->St->HasChildren() && !rad->Dr->HasChildren())
        Derivare(rad);
}