// chapter2date.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost\timer.hpp>
#include <boost\progress.hpp>

#include <boost\enable_shared_from_this.hpp>
#include <boost\make_shared.hpp>

using namespace boost;

class self_shared : public boost::enable_shared_from_this < self_shared >
{
public:
	self_shared(int n) : x(n) {}
	int x;
	void print()
	{
		cout << "self shared: " << x << endl;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	timer t;
	cout << "min elapsed " << t.elapsed_min() << "s" << endl;
	cout << "max elapsed " << t.elapsed_max() / 3600 <<  "h" << endl;

	auto sp = make_shared<self_shared>(314);
	sp->print();
	auto p = sp->shared_from_this();
	p->x = 1002;
	p->print();
	sp->print();


	cout << "time elapsed " << t.elapsed() << endl;


	cout << endl << endl;
	progress_timer t2;
	float j = 23;
	for (int i = 1; i < 9000; i++)
	{
		j = j * i * .9;
	}
	cout << j << endl;


	return 0;
}

