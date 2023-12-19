#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <cmath>
#include "graphics.h"
#include "ArboreBinar.cpp"
#pragma comment(lib,"graphics.lib")

using namespace std;

ifstream fin("fisier.in");
ofstream fout("fisier.out");

ArbNod* ArbNod::Temp = nullptr;
//variabile globale
string variabila;
string OperatiiTrigonometrice[] = { "sin", "cos", "tan",  "sqrt", "log" };

//operatii pe arbori/expresii
bool verificaParanteze(string expresie);
bool verificare(string expresie);
bool verificaSemne(string expresie);
bool verificaSemneInvalide(string expresie);
int prioritate(char operatorr);
string simplificare(string &rezultat);
string simplificare0(string &rezultat);
string simplificare1(string &rezultat);
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
    fout << "Expresia matematica este: ";
    fin >> expresie;
    fin >> variabila;
    fout << expresie << endl;
    fout << "Variabila pentru care se va deriva este: " << variabila << endl;
    string mortii = "x-0-2+3*1";
    simplificare(mortii);
    fout << mortii;
    if (verificare(expresie)) {
        string arborePostfixat = postfixare(expresie);
        SRD(ArbNod::Temp);
        
        SDR(ArbNod::Temp);
        fout << '\n';
        SRD(ArbNod::Temp);
    }
    else {
        fout << "Expresia matematica introdusa este invalida." << endl;
    }

    return 0;
}

string DerivareIndiv(string info)
{
    if (info == "+" || info == "^" || info == "*") return info;

    if (info[0]==variabila[0])
        return "1";
    if (isdigit(info[0]) or (isalpha(info[0]) && info != variabila))
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
string postfixare(string expresie)
{
    stack<char> stiva;
    stack<string> stivaPostfixata;
    string temp;

    string arborePostfixat;
    unsigned int i = 0;
    unsigned int k = 0;
    unsigned int count = 0;

    for (unsigned int j = 0; j < expresie.size(); j++) {
        char ch = expresie.at(j);
        if (isalpha(ch)) 
        {
            unsigned int j2 = j;
            bool IsCorrectFunction = false;

            temp = "";
            while (j2 < expresie.size() && isalpha(expresie.at(j2))) { //cat timp gaseste litere le pune in temp
                temp += expresie.at(j2++);
            }
            
            for (int exprInd = 0; exprInd <= 4; exprInd++) { //verifica daca temp este functie
                if (OperatiiTrigonometrice[exprInd] == temp) 
                {
                    j = j2 - 1; //actualizam noua pozitie daca am gasit functie corecta
                    //facem ceva cu functia
                    IsCorrectFunction = true;
                    stivaPostfixata.push(temp);

                    arborePostfixat += temp;
                    break;
                }
            }
            
            if (!IsCorrectFunction) {
                stivaPostfixata.push(string(1, ch));
                arborePostfixat += ch;
            }
        }
        else if (isdigit(ch)) //cat timp gaseste cifre una dupa alta le pune in temp apoi in stiva postfixata
        {
            unsigned int j2 = j;
            
            temp = "";

            while (j2 < expresie.size() && isdigit(expresie.at(j2))) { //cat timp gaseste cifre consecutive le pune in temp
                temp += expresie.at(j2);
                j2++;
            }
            j = j2 - 1;
            
           
            arborePostfixat += temp;
            stivaPostfixata.push(temp); //plasam numarul creat in stiva postfixata
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
                if (stivaPostfixata.size() > 1) {
                    string dr = stivaPostfixata.top(); stivaPostfixata.pop(); 
                    string st = stivaPostfixata.top(); stivaPostfixata.pop();
                    cout << "new node " << stiva.top() << " " << st << " " << dr << '\n';

                    ArbNod* newNod = new ArbNod(
                        string(1, stiva.top()),
                        new ArbNod(st),
                        new ArbNod(dr)
                    );

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
                        new ArbNod(stivaPostfixata.top()),
                        ArbNod::Temp
                    );

                    stivaPostfixata.pop();
                    ArbNod::Temp = newNod;
                }

                arborePostfixat += stiva.top();
                stiva.pop();
            }
            stiva.push(ch);
        }
    }
    // se aplica regulile de construire de la while anterior doar ca aparent am scris invers cele 2 conditii
    while (!stiva.empty()) {

        if (stivaPostfixata.size() < 2) {
            ArbNod* newNod = new ArbNod(
                string(1, stiva.top()),
                new ArbNod(stivaPostfixata.top()),
                ArbNod::Temp
            );

            stivaPostfixata.pop();
            ArbNod::Temp = newNod;
        }
        else {
            string dr = stivaPostfixata.top(); stivaPostfixata.pop();
            string st = stivaPostfixata.top(); stivaPostfixata.pop();
            cout << "new node " << stiva.top() << " " << st << " " << dr << '\n';

            ArbNod* newNod = new ArbNod(
                string(1, stiva.top()),
                new ArbNod(st),
                new ArbNod(dr)
            );


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

        arborePostfixat += stiva.top();
        stiva.pop();
    }
    //fout << arborePostfixat;
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
bool verificaParanteze(string expresie) {
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
bool verificaSemne(string expresie)
{
    char semne[] = "+-*^/!";
    for (int i=0; i<size(expresie)-1; i++)
    {
        if (strchr(semne, expresie[i]) and strchr(semne, expresie[i + 1]))
        {
            return 0;
        }
    }
    if (strchr(semne, expresie[size(expresie) - 1]))
        return 0;
    return 1;
}
bool verificaSemneInvalide(string expresie)
{
    char semne[] = "+-*^/!.";
    for (int i = 0; i < size(expresie); i++)
    {
        if (!strchr(semne, expresie[i]) or !isalpha(expresie[i]) or !isdigit(expresie[i]))
            return 0;
    }
    return 1;
}
bool verificare(string expresie)
{
    bool gyatt=1;
    if (!verificaParanteze(expresie))
    {
        fout << "Parantezele nu sunt corect plasate." << endl;
        gyatt = 0;
    }
    if (!verificaSemne(expresie))
    {
        fout << "Semnele matematice nu sunt corect plasate." << endl;
        gyatt = 0;
    }
    if (verificaSemneInvalide(expresie))
    {
        fout << "Exista semne invalide in expresie." << endl;
        gyatt = 0;
    }
    return gyatt;
}
string simplificare0(string &rezultat)
{
    string rezultatFinal;
    for (unsigned int i = 0; i < rezultat.size(); i++)
    {
        if (!((rezultat[i] == '+' or rezultat[i]=='-') and rezultat[i + 1] == '0'))
        {
            rezultatFinal += rezultat[i];
        }
        else
            i = i + 1;
    }
    rezultat = rezultatFinal;
    return rezultat;
}
string simplificare1(string &rezultat)
{
    string rezultatFinal;
    for (unsigned int j = 0; j < rezultat.size() - 2; j++)
    {
        if (!((rezultat[j] == '*' or rezultat[j]=='/') and rezultat[j + 1] == '1' and !(isdigit(rezultat[j + 2]))))
            rezultatFinal += rezultat;
        else
            j = j + 1;
    }
    rezultat = rezultatFinal;
    return rezultatFinal;
}
string simplificare(string &rezultat)
{
    string rezultatFinal;
    simplificare0(rezultat);
    simplificare1(rezultat);

    return rezultatFinal;
}
