#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include "graphics.h"
#include "ArboreBinar.cpp"
#pragma comment(lib,"graphics.lib")

using namespace std;

ifstream fin("fisier.in");
ofstream fout("fisier.out");

ArbNod* ArbNod::Temp = nullptr;

//operatii pe arbori/expresii
bool verificaParanteze(string expresie);
bool verificare(string expresie);
int prioritate(char operatorr);
string postfixare(string expresie);

//Derivare propriu-zisa
void Derivare(ArbNod* oper);
string DerivareIndiv(string info);

//Parcurgere si procesare
void SDR(ArbNod* rad);
void SRD(ArbNod* rad);

int main()
{
    string expresie;
    fin >> expresie;

    if (verificare(expresie)) {
        string arborePostfixat = postfixare(expresie);

        SRD(ArbNod::Temp);
        
        SDR(ArbNod::Temp);
        fout << '\n';
        SRD(ArbNod::Temp);
    }
    else {
        fout << "Expresia matematica introdusa este invalida." << std::endl;
    }

    return 0;
}

string DerivareIndiv(string info)
{
    if (info == "+" || info == "^" || info == "*") return info;

    if (isalpha(info[0]))
        return "1";
    if (isdigit(info[0]))
        return "0";

    return "?";
}
void Derivare(ArbNod* oper)
{
    if (oper->info == "+")
    {
        oper->St->info = DerivareIndiv(oper->St->info);
        oper->Dr->info = DerivareIndiv(oper->Dr->info);
    }
    if (oper->info == "^")
    {
        //daca fiul stanga e variabila si fiul dreapta e nr avem x^c unde c e contanta (ex c = 5)
        if (isalpha(oper->St->info[0]) && isdigit(oper->Dr->info[0])) {
            int a = stoi((oper->Dr->info));
            a--;
            oper->Dr->info = to_string(stoi((oper->Dr->info)) - 1);

            // realizam un nou nod deoarece x^5 devine 5*x^4, din desen 1 trebuie sa devina desen 2
            /*      
            *      ^
            *     / \
            *    x   5
            */
            /*
            *       * (- oper)
            *      / \
            *     /   \
            *    5     ^ (- copy)
            *         / \
            *        x   4
            */

            ArbNod* copy = new ArbNod(oper);
            oper->Dr = copy;
            oper->St = new ArbNod(copy->Dr);
            oper->St->info = to_string(stoi((copy->Dr->info)) + 1);
            oper->info = '*';
        }              
        
    }

}
void SRD(ArbNod* rad)
{
    //parcurgere inordine + afisare
    if (rad->St != nullptr)
        SRD(rad->St);
    fout << rad->info;
    if (rad->Dr != nullptr)
        SRD(rad->Dr);
}
void SDR(ArbNod* rad)
{
    //parcurgere postordine + derivare
    if (rad->St != nullptr)
        SDR(rad->St);
    if (rad->Dr != nullptr)
        SDR(rad->Dr);

    if (rad->HasChildren())
        Derivare(rad);
}
string postfixare(string expresie) {
    stack<char> stiva;
    string arborePostfixat;
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

                //daca in stringul arborelui sunt mai mult de 2 elemente inseamna ca trebuie un nou varf cu 2 fii (o operatie noua)
                if (arborePostfixat.size() > 1) {
                    ArbNod* newNod = new ArbNod(
                        string(1, stiva.top()),
                        new ArbNod(string(1, arborePostfixat.at(arborePostfixat.size() - 2))),
                        new ArbNod(string(1, arborePostfixat.at(arborePostfixat.size() - 1)))
                    );

                    arborePostfixat.pop_back(); arborePostfixat.pop_back();

                    // Daca operatia anterioara nu e null inseamna ca trebuie sa unim 2 operatii, cea curenta si cea anterioara cu semnul de la stiva.top()
                    if (ArbNod::Temp != nullptr) {
                        stiva.pop();
                        ArbNod* tp = new ArbNod(
                            string(1, stiva.top()),
                            newNod,
                            ArbNod::Temp
                        );
                        ArbNod::Temp = tp;
                    }
                    else
                        ArbNod::Temp = newNod;
                }
                //altfel inseamna ca a ramas un singur element care trebuie pus in operatie cu operatia anterioara
                else { 
                    ArbNod* newNod = new ArbNod(
                        string(1, stiva.top()),
                        new ArbNod(string(1, arborePostfixat.at(arborePostfixat.size() - 1))),
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
    // se aplica regulile de construire de la while anterior doar ca aparent am scris invers cele 2 conditii
    while (!stiva.empty()) {

        if (arborePostfixat.size() < 2) {
            ArbNod* newNod = new ArbNod(
                string(1, stiva.top()),
                new ArbNod(string(1, arborePostfixat.at(arborePostfixat.size() - 1))),
                ArbNod::Temp
            );

            arborePostfixat.pop_back();
            ArbNod::Temp = newNod;
        }
        else {
            ArbNod* newNod = new ArbNod(
                string(1, stiva.top()),
                new ArbNod(string(1, arborePostfixat.at(arborePostfixat.size() - 2))),
                new ArbNod(string(1, arborePostfixat.at(arborePostfixat.size() - 1)))
            );

            arborePostfixat.pop_back(); arborePostfixat.pop_back();

            if (ArbNod::Temp != nullptr) {
                stiva.pop();
                ArbNod* tp = new ArbNod(
                    string(1, stiva.top()),
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
bool verificare(string expresie) {
    return verificaParanteze(expresie);
    // adauga alte verificari daca este necesar
}