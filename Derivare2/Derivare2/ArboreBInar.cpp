#include <iostream>
using namespace std;

class ArbNod {
	
public:
	ArbNod *St, *Dr;
	string info;
	ArbNod(string inf)
	{
		St = Dr = nullptr;
		info = inf;
	}
	ArbNod(string inf, ArbNod *n, bool nod = 0)
	{
		if(nod == 0)
			St = n, Dr = nullptr;
		else
			St = nullptr, Dr = n;

		info = inf;
	}
	ArbNod(string inf, ArbNod* st, ArbNod *dr)
	{
		St = st;
		Dr = dr;
		info = inf;
	}
	ArbNod(ArbNod* copy)
	{
		St = copy->St;
		Dr = copy->Dr;
		info = copy->info;
	}
	bool HasChildren()
	{
		return St != nullptr && Dr != nullptr;
	}

	static ArbNod *Temp;

	bool operator == (ArbNod* other)
	{
		return other->St == this->St && other->Dr == this->Dr && other->info == this->info;
	}
};