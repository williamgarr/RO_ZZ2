#pragma once

#include<iostream>
#include <chrono>
#include<fstream>
#include<sstream>
#include "glpk.h"

using namespace std;

class Jobshop
{
public:
	static void solve(string filename, bool display = false);
};

