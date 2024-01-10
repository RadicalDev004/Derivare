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
stack<ArbNod> ArbNod::TempStack;
//variabile globale
string expresie;
string variabila;
string OperatiiTrigonometrice[] = { "sin", "cos", "tan",  "sqrt", "ln" };
int parantezeCnt = 0;
bool ConstHelper = false;
string DerivataNesimplificata;

//operatii pe arbori/expresii
bool verificaSemneInvalide(string expresie);
string postfixare(string expresie);

//Derivare propriu-zisa
void Derivare(ArbNod* oper);
void Derivare2(ArbNod* oper);
string DerivareIndiv(string info);

//Parcurgere si procesare
void SDR(ArbNod* rad);
void SRD(ArbNod* rad);

//functii ajutatoare
int prioritate(char operatorr);
bool verificaParanteze(string expresie);
bool verificare(string expresie);
bool verificaSemne(string expresie);
string simplificare(string rezultat);
void simplificare0(string& rezultat);
void simplificare1(string& rezultat);
void simplificareparanteze(string& rezultat);
bool IsNumber(string s);
bool IsVariable(string s);
bool IsConstant(ArbNod* oper, int c = 0);
bool IsExpresie(string str);
bool debug(string expr);


int main()
{
    fout << "Expresia matematica este: ";
    fin >> expresie;
    fin >> variabila;
    fout << expresie << endl;
    fout << "Variabila pentru care se va deriva este: " << variabila << endl;
    if (debug(expresie))
        return 0;

    if (verificare(expresie)) {
        string arborePostfixat = postfixare(expresie);
        //fout << arborePostfixat << '\n';
        SRD(ArbNod::Temp);
        fout << DerivataNesimplificata;
        fout << '\n';
        Derivare2(ArbNod::Temp);
        fout << '\n';
        SRD(ArbNod::Temp);
    }
    else {
        fout << "Expresia matematica introdusa este invalida." << endl;
    }
   
  fout<<"aa" << ' ' << simplificare(DerivataNesimplificata);
    return 0;
}

void Derivare2(ArbNod* oper)
{
    if (!(oper->HasChildren()))
    {
         oper->info = DerivareIndiv(oper->info);
         return;
    }
    
    if (oper->info == "+" || oper->info == "-")
    {
        Derivare2(oper->St);
        Derivare2(oper->Dr);
    }
    else if (oper->info == "^")
    {
        //cout << oper->info << " " << IsConstant(oper->St) << " " << IsConstant(oper->Dr) << '\n';
        if (!IsConstant(oper->St) && IsConstant(oper->Dr))
        {
            int a = stoi(oper->Dr->info);
            a--;
            oper->Dr->info = to_string(stoi((oper->Dr->info)) - 1);

            ArbNod* copy = new ArbNod(oper);
            oper->Dr = copy;
            oper->St = new ArbNod(copy->Dr);
            oper->St->info = to_string(stoi((copy->Dr->info)) + 1);
            oper->info = '*';
        }
        else if (IsConstant(oper->St) && !IsConstant(oper->Dr))
        {
            int a = stoi(oper->St->info);

            ArbNod* copy = new ArbNod(oper);
            oper->St = copy;
            oper->Dr = new ArbNod("ln", new ArbNod(to_string(a)), true);
            oper->info = '*';
        }

       
    }
    else if (oper->info == "*")
    {
        /*
        *  fg => f`g + fg`
        */
        
        //verific daca am operatii doar cu constante ca sa nu le mai derivez
        if (IsConstant(oper->St) && IsConstant(oper->Dr))
        {
            oper->info = "0";
            return;
        }
        else if (IsConstant(oper->St))
        {
            Derivare2(oper->Dr);
            return;
        }
        else if (IsConstant(oper->Dr))
        {
            Derivare2(oper->St);
            return;
        }
        

        ArbNod* newSt = new ArbNod("");
        ArbNod* newDr = new ArbNod("");

        ArbNod::Copy(newSt, oper);
        ArbNod::Copy(newDr, oper);

        oper->info = "+";
        oper->St = newSt;
        oper->Dr = newDr;

        Derivare2(newSt->St);
        Derivare2(newDr->Dr);
    }
    else if (oper->info == "/")
    {
        /*
        *  f/g => (f`g - fg`)/g^2
        */

        cout << "A " << oper->info << " " << oper->St->info << " " << oper->Dr->info << '\n';
        //daca avem f(x)/c refacem ca f(x)*(1/c) sau daca avem c/f(x) refacem ca c*(1/f(x))
        bool Bdr = IsConstant(oper->Dr);
        bool Bst = IsConstant(oper->St);

        if (Bdr && !Bst || !Bdr && Bst)
        {
            //cout << "Simple fr " << oper->St->info << " " << oper->Dr->info << '\n';
            if (oper->Dr->info == "1" || oper->St->info == "1")
            {

            }
            else {
                oper->info = "*";
                ArbNod* drp = new ArbNod("/", new ArbNod("1"), oper->Dr);
                oper->Dr = drp;
                Derivare2(oper);

                return;
            }
            
        }

        ArbNod* f = new ArbNod("");
        ArbNod* g = new ArbNod("");

        ArbNod* st = new ArbNod("-");
        ArbNod* dr = new ArbNod("^");

        ArbNod::Copy(f, oper->St);
        ArbNod::Copy(g, oper->Dr);

        oper->Dr = dr;
        dr->St = g;
        dr->Dr = new ArbNod("2");

        ArbNod* templ = new ArbNod("*", f, g);

        ArbNod* MinusSt = new ArbNod("");
        ArbNod::Copy(MinusSt, templ);

        ArbNod* MinusDr = new ArbNod("");
        ArbNod::Copy(MinusDr, templ);

        oper->St = st;
        st->St = MinusSt;
        st->Dr = MinusDr;

        Derivare2(MinusSt->St);
        Derivare2(MinusDr->Dr);
    }
    else if (oper->info == "sin")
    {
        ArbNod *cpy = new ArbNod("");
        ArbNod::Copy(cpy, oper);
        cpy->info = "cos";

        oper->info = "*";
        oper->St = cpy;

        Derivare2(oper->Dr);
    }
    else if (oper->info == "cos")
    {
        ArbNod* cpy = new ArbNod("");
        ArbNod::Copy(cpy, oper);
        cpy->info = "-sin";

        oper->info = "*";
        oper->St = cpy;

        Derivare2(oper->Dr);
    }
    else if (oper->info == "tan")
    {
        ArbNod* cpy = new ArbNod("");
        ArbNod::Copy(cpy, oper);

        ArbNod* cpyDr = new ArbNod("^");
        cpy->info = "cos";

        cpyDr->St = cpy;
        cpyDr->Dr = new ArbNod("2");

        oper->info = "/";
        oper->St = new ArbNod("1");
        oper->Dr = cpyDr;

        //Derivare2(oper->Dr);
    }
    else if (oper->info == "sqrt")
    {
        ArbNod* cpy = new ArbNod("");
        ArbNod* cpy2 = new ArbNod("");

        ArbNod::Copy(cpy, oper->Dr);
        ArbNod::Copy(cpy2, cpy);

        oper->info = "*";
        oper->St = new ArbNod("/",
            new ArbNod("1"),
            new ArbNod("*",
                new ArbNod("2"),
                new ArbNod("sqrt",
                    cpy2,
                    true
                )
            )
        );
        oper->Dr = cpy;


        Derivare2(cpy);
    }
    else if (oper->info == "ln")
    {
        ArbNod* cpy = new ArbNod("");
        ArbNod* cpy2 = new ArbNod("");

        ArbNod::Copy(cpy, oper->Dr);
        ArbNod::Copy(cpy2, cpy);

        oper->info = "*";
        oper->St = new ArbNod("/",
            new ArbNod("1"),
            cpy2
        );
        oper->Dr = cpy;


        Derivare2(cpy);
    }
}

string DerivareIndiv(string info)
{
    cout << info << '\n';
    if (info == "+" || info == "^" || info == "*") return info;

    if (info == variabila)
        return "1";
    if (IsNumber(info) or (!IsNumber(info) && info != variabila))
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
    if (rad->St != nullptr && rad->St->HasChildren() && (rad->info == "^" || rad->info == "/"))
    {
        fout << "(";
        DerivataNesimplificata += '(';
    }
    if (rad->St != nullptr && rad->St->HasChildren() && rad->info == "*")
    {
        fout << "(";
        DerivataNesimplificata += '(';
    }

    if (rad->St != nullptr)
        SRD(rad->St);

    if (rad->St != nullptr && rad->St->HasChildren() && (rad->info == "/" || rad->info == "^" || rad->info == "*"))
    {
        fout << ")";
        DerivataNesimplificata += ')';
    }

    ///RADACINA
    DerivataNesimplificata += rad->info;
    fout << rad->info;

    if (IsExpresie(rad->info))
    {
        fout << "(";
        DerivataNesimplificata += '(';
       
    }
    if (rad->Dr != nullptr && rad->Dr->HasChildren() && rad->info == "/")
    {
        fout << "(";
        DerivataNesimplificata += '(';
        
    }
    if (rad->Dr != nullptr && rad->Dr->HasChildren() && rad->info == "*")
    {
        fout << "(";
        DerivataNesimplificata += '(';
       
    }

    if (rad->Dr != nullptr)
        SRD(rad->Dr);

    if (rad->Dr != nullptr && rad->Dr->HasChildren() && (rad->info == "^" || rad->info == "/"))
    {
        fout << ")";
        DerivataNesimplificata += ')';
    }
    if (rad->Dr != nullptr && rad->Dr->HasChildren() && rad->info == "*")
    {
        fout << ")";
    DerivataNesimplificata += ')';
    }
    if (IsExpresie(rad->info))
    {
        fout << "(";
        DerivataNesimplificata += ')';
    }
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
            parantezeCnt++;
        }
        else if (ch == ')') {
            parantezeCnt--;
            if (!stiva.empty() && stiva.top() == '(')
            {
                string var = stivaPostfixata.top(); stivaPostfixata.pop();
                stiva.pop();
                if (parantezeCnt > 0) {
                    
                    ArbNod::TempStack.push(new ArbNod(stivaPostfixata.top(), new ArbNod(var), true));
                    cout << "DA " << stivaPostfixata.top() << " " << var << '\n';
                    stivaPostfixata.pop();
                    stivaPostfixata.push("temp");
                                    
                }
                else
                {
                    if (!ArbNod::TempStack.empty() && var == "temp")
                    {
                       
                        if (ArbNod::Temp != nullptr)
                        {
                            ArbNod* n = new ArbNod("");
                            ArbNod::Copy(n, &ArbNod::TempStack.top());
                            
                            ArbNod* bn = new ArbNod(stivaPostfixata.top(), ArbNod::Temp, n);
                            ArbNod::Temp = bn;

                            stivaPostfixata.pop();
                            ArbNod::TempStack.pop();
                        }
                        else
                        {
                            ArbNod* n = new ArbNod("");
                            ArbNod::Copy(n, &ArbNod::TempStack.top());

                            ArbNod::Temp = new ArbNod(stivaPostfixata.top(), n, true);

                            stivaPostfixata.pop();
                            ArbNod::TempStack.pop();
                        }
                    }
                    else if (ArbNod::Temp == nullptr)
                    {
                        cout << !ArbNod::TempStack.empty() << " " << (stivaPostfixata.top() == "temp");
                        ArbNod::Temp = new ArbNod(stivaPostfixata.top(), new ArbNod(var), true);
                        cout << "new P12 node " << " " << stivaPostfixata.top() << " " << var << '\n';
                        stivaPostfixata.pop();
                    }
                    else 
                    {
                        ArbNod* nd = new ArbNod(string(1, stiva.top()), ArbNod::Temp, new ArbNod(stivaPostfixata.top(), new ArbNod(var), true));
                        ArbNod::Temp = nd;
                        stiva.pop();
                        stivaPostfixata.pop();
                        
                    }
                }
                continue;
            }
            while (!stiva.empty() && stiva.top() != '(') {

                if (stivaPostfixata.size() > 1) {
                    
                    ArbNod *dr = new ArbNod("");
                    if (!stivaPostfixata.empty() && stivaPostfixata.top() == "temp") {
                        ArbNod::Copy(dr, &ArbNod::TempStack.top());
                        ArbNod::TempStack.pop();
                        stivaPostfixata.pop();
                    }
                    else if (!stivaPostfixata.empty() && IsExpresie(stivaPostfixata.top())) {                     
                        ArbNod::Copy(dr, &ArbNod::TempStack.top());
                        ArbNod* tmp = new ArbNod(stivaPostfixata.top(), dr, true);
                        dr = tmp;
                        ArbNod::TempStack.pop();
                        stivaPostfixata.pop();
                    }
                    else {
                        dr = new ArbNod(stivaPostfixata.top());
                        stivaPostfixata.pop();
                    }

                    ArbNod *st = new ArbNod("");
                    if (!stivaPostfixata.empty() && stivaPostfixata.top() == "temp") {
                        ArbNod::Copy(st, &ArbNod::TempStack.top());
                        ArbNod::TempStack.pop();
                        stivaPostfixata.pop();
                    }
                    else if (!stivaPostfixata.empty() && IsExpresie(stivaPostfixata.top())) {
                        st = nullptr;
                    }
                    else {
                        st = new ArbNod(stivaPostfixata.top());
                        stivaPostfixata.pop();
                    }
                    cout << "new P1 node " << " " << stiva.top() << " " << ((st != nullptr) ? st->info : "LOL") << " " << dr->info << " " << '\n';

                    ArbNod* newNod = new ArbNod(
                        st == nullptr && !stivaPostfixata.empty() && IsExpresie(stivaPostfixata.top()) ? stivaPostfixata.top() : string(1, stiva.top()),
                        st,
                        dr
                    );

                    if (st == nullptr && !stivaPostfixata.empty() && IsExpresie(stivaPostfixata.top())) stivaPostfixata.pop();

                    if (parantezeCnt > 0) {
                        ArbNod::TempStack.push(newNod);
                        stivaPostfixata.push("temp");
                    }
                    else {
                        if (!stivaPostfixata.empty() && IsExpresie(stivaPostfixata.top()))
                        {
                            stiva.pop();
                            if (stiva.top() == '(')
                                stiva.pop();

                            if (ArbNod::Temp != nullptr) {
                                ArbNod* nd = new ArbNod(string(1, stiva.top()), ArbNod::Temp, new ArbNod(stivaPostfixata.top(), newNod, true));
                                ArbNod::Temp = nd;

                            }
                            else {
                                ArbNod::Temp = new ArbNod(stivaPostfixata.top(), newNod, true);
                                cout << "new P13 node " << stivaPostfixata.top() << " " << stiva.top() << " " << ((st != nullptr) ? st->info : "LOL") << " " << dr->info << " " << '\n';
                                stiva.push('?');
                            }
                            stivaPostfixata.pop();
                        }
                        // Daca operatia anterioara nu e null inseamna ca trebuie sa unim 2 operatii, cea curenta si cea anterioara cu semnul de la stiva.top()
                        else if (ArbNod::Temp != nullptr) {

                            stiva.pop();
                            if (stiva.top() == '(')
                                stiva.pop();

                            ArbNod* tp = new ArbNod(
                                string(1, stiva.top()),
                                newNod,
                                ArbNod::Temp,
                                false
                            );
                            ArbNod::Temp = tp;
                        }
                        else
                            ArbNod::Temp = newNod;
                    }

                    
                }
                //altfel inseamna ca a ramas un singur element care trebuie pus in operatie cu operatia anterioara
                else {
                    if (stiva.empty())
                    {
                        cout << "emptyStiva\n" << arborePostfixat;
                        return "";
                    }
                    if (stivaPostfixata.empty())
                    {
                        cout << "emptyStivaP\n" << arborePostfixat;
                        return "";
                    }
                    ArbNod* newNod = new ArbNod(
                        string(1, stiva.top()),
                        new ArbNod(stivaPostfixata.top()),
                        ArbNod::Temp
                    );

                    if (!stivaPostfixata.empty())
                        stivaPostfixata.pop();
                    ArbNod::Temp = newNod;
                }
                if (!stiva.empty())
                arborePostfixat += stiva.top();

                if (!stiva.empty())
                    stiva.pop();
            }
            if (!stiva.empty())
                stiva.pop();
            // elimina '('              
        }
        else {  // daca este operator
            while (!stiva.empty() && prioritate(ch) <= prioritate(stiva.top())) {

                //daca in stringul arborelui sunt mai mult de 2 elemente inseamna ca trebuie un nou varf cu 2 fii (o operatie noua)
                if (stivaPostfixata.size() > 1) {
                    ArbNod* dr = new ArbNod("");
                    if (!stivaPostfixata.empty() && stivaPostfixata.top() == "temp") {
                        ArbNod::Copy(dr, &ArbNod::TempStack.top());
                        ArbNod::TempStack.pop();
                    }
                    else {
                        dr = new ArbNod(stivaPostfixata.top());
                        stivaPostfixata.pop();
                    }

                    ArbNod* st = new ArbNod("");
                    if (!stivaPostfixata.empty() && stivaPostfixata.top() == "temp") {
                        ArbNod::Copy(st, &ArbNod::TempStack.top());
                        ArbNod::TempStack.pop();
                    }
                    else {
                        st = new ArbNod(stivaPostfixata.top());
                        stivaPostfixata.pop();
                    }
                    cout << "new N node " << stiva.top() << " " << ((st != nullptr) ? st->info : "LOL") << " " << dr->info << '\n';

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
        
        if (stivaPostfixata.size() > 1) {
            ArbNod* dr = new ArbNod("");
            if (!stivaPostfixata.empty() && stivaPostfixata.top() == "temp") {
                ArbNod::Copy(dr, &ArbNod::TempStack.top());
                ArbNod::TempStack.pop();
            }
            else {
                dr = new ArbNod(stivaPostfixata.top());
                stivaPostfixata.pop();
            }

            ArbNod* st = new ArbNod("");
            if (!stivaPostfixata.empty() && stivaPostfixata.top() == "temp") {
                ArbNod::Copy(st, &ArbNod::TempStack.top());
                ArbNod::TempStack.pop();
            }
            else {
                st = new ArbNod(stivaPostfixata.top());
                stivaPostfixata.pop();
            }
            cout << "new L node " << stiva.top() << " " << ((st != nullptr) ? st->info : "LOL") << " " << dr->info << '\n';

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
                ArbNod::Temp,
                false
            );
            cout << "new L1 node " << stiva.top() << " " << stivaPostfixata.top() << " " << ArbNod::Temp->info << '\n';
            stivaPostfixata.pop();
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
    stack<char> stiva;
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
void simplificare0(string &rezultat)
{
    string rezultatFinal;
    for (unsigned int i = 0; i < rezultat.size(); i++)
    {
            if (rezultat[i] == '0' and rezultat[i + 1] == '+')
            {
                i = i + 1;
                continue;
            }
        if (!((rezultat[i] == '+' or rezultat[i]=='-') and rezultat[i + 1] == '0'))
        {
            rezultatFinal += rezultat[i];
        }
        else
            i = i + 1;
    }
    rezultat = rezultatFinal;
}
void simplificare1(string &rezultat)
{
    string RezultatFinal;
    for (int i = 0; i <= rezultat.size()-1; i++)
    {
        if (rezultat[i] == '1')
        {
            if (rezultat[i + 1] == '*')
            {
                i++;
                continue;
            }
        }
        else
            if ((rezultat[i] == '*' or rezultat[i] == '/') and rezultat[i + 1] == '1')
            {
                i++;
                continue;
            }
        RezultatFinal += rezultat[i];
    }
    rezultat = RezultatFinal;
    string Rez2;
    for (int j = 0; j <= rezultat.size() - 1; j++)
    {
        if (rezultat[j] == '^' and rezultat[j + 1] == '1' and !isdigit(rezultat[j + 2]) and !isalpha(rezultat[j + 2]))
            j = j + 1;
        else
            Rez2 += rezultat[j];

    }
    rezultat = Rez2;
}
string simplificare(string rezultat)
{
    string DerivataSimplificata;
  for (int i = expresie.size(); i < DerivataNesimplificata.size(); i++)
    {
        DerivataSimplificata += DerivataNesimplificata[i];
    }
   
    simplificare0(DerivataSimplificata);
    simplificare1(DerivataSimplificata);
    simplificareparanteze(DerivataSimplificata);
    
    return DerivataSimplificata;
}
void simplificareparanteze(string& rezultat)
{
    string rez;
    stack<char> stiva;
    int k=0;
        for (int i = 0; i < rezultat.size(); i++)
        {
            if (k == 1)
            {
                k = 0;
                i++;
            }

            if (rezultat[i] == '(' and rezultat[i + 1] == ')')
                i++;
            else
                if (rezultat[i] == '(' and rezultat[i + 2] == ')')
                {
                    i++;
                    rez += rezultat[i];
                    k = 1;
                }
                else           
                rez += rezultat[i];

        }
        rezultat = rez;
        rez.clear();

}

bool debug(string expr)
{
    if (expr.size() == 1 and expr == variabila)
    {
        fout << 1;
        return 1;
    }
    return 0;

}

bool IsNumber(string s)
{
    for (char c : s)
        if (!isdigit(c))
            return false;
    return true;
}
bool IsVariable(string s)
{
    for (char c : s)
        if (!isalpha(c))
            return false;
    return true;
}
bool IsConstant(ArbNod* oper, int c) 
{
    if (c == 0) {
        ConstHelper = true;
    }

    if (IsVariable(oper->info)) {
        ConstHelper = false;
    }
    if (oper->St != nullptr) IsConstant(oper->St, c + 1);
    if (oper->Dr != nullptr) IsConstant(oper->Dr, c + 1);
    if (c == 0) {
        return ConstHelper;
    }
}
bool IsExpresie(string str)
{
    for (string s : OperatiiTrigonometrice)
        if (s == str)
            return true;
    return false;
}

