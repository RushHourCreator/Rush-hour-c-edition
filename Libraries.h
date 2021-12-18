#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <malloc.h>
#include <locale.h>

using namespace std;

enum in_space {
	HORIZONTAL = 0,
	VERTICAL = 1
};

enum direction {
	BACK = -1,
	FORWARD = 1
};

typedef char** t_matr;