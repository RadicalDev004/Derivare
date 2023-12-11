#include <iostream>

class ArbNod {
	
public:
	ArbNod *St, *Dr;
	char info;
	ArbNod(char inf)
	{
		St = Dr = nullptr;
		info = inf;
	}
	ArbNod(char inf, ArbNod *n, bool nod = 0)
	{
		if(nod == 0)
			St = n, Dr = nullptr;
		else
			St = nullptr, Dr = n;

		info = inf;
	}
	ArbNod(char inf, ArbNod* st, ArbNod *dr)
	{
		St = st;
		Dr = dr;
		info = inf;
	}
	bool HasChildren()
	{
		return St != nullptr && Dr != nullptr;
	}

	static ArbNod *Temp;
};