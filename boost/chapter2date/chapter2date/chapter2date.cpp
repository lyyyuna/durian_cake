// chapter2date.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost\timer.hpp>

using namespace boost;

int _tmain(int argc, _TCHAR* argv[])
{
	timer t;
	cout << "min elapsed " << t.elapsed_min() << "s" << endl;
	cout << "max elapsed " << t.elapsed_max() / 3600 <<  "h" << endl;

	cout << "time elapsed " << t.elapsed() << endl;
	return 0;
}

