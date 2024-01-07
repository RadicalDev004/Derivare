#include <iostream>
#include <stack>
#include <queue>
using namespace std;

class ArbNod {

public:
	ArbNod* St, * Dr;
	string info;

	ArbNod(string inf)
	{
		St = Dr = nullptr;
		info = inf;
	}
	ArbNod(string inf, ArbNod* n, bool nod = 0)
	{
		if (nod == 0)
			St = n, Dr = nullptr;
		else
			St = nullptr, Dr = n;

		info = inf;
	}
	ArbNod(string inf, ArbNod* st, ArbNod* dr, bool order = true)
	{
		if (order) {
			St = st;
			Dr = dr;
		}
		else {
			St = dr;
			Dr = st;
		}

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
		if (this == nullptr)
			return false;
		return St != nullptr || Dr != nullptr;
	}

	static void Copy(ArbNod* Original, ArbNod* toCopy)
	{
		Original->info = toCopy->info;

		if (!toCopy->HasChildren())
			return;


		if (toCopy->St != nullptr)
		{
			Original->St = new ArbNod("");
			ArbNod::Copy(Original->St, toCopy->St);
		}
		if (toCopy->Dr != nullptr)
		{
			Original->Dr = new ArbNod("");
			ArbNod::Copy(Original->Dr, toCopy->Dr);
		}
	}

	void Print(ArbNod* Original)
	{

		if (Original->St != nullptr)
			Print(Original->St);
		if (Original->Dr != nullptr)
			Print(Original->Dr);
		cout << Original->info << " ";
	}

	static ArbNod* Temp;
	static stack<ArbNod> TempStack;

	bool operator == (ArbNod* other)
	{
		return other->St == this->St && other->Dr == this->Dr && other->info == this->info;
	}
};