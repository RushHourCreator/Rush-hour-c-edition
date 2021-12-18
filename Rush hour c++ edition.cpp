#include "Functions.h"
#include "Libraries.h"
#include "Classes.h"



int main() {
	setlocale(LC_ALL, "RUS");

	//printf("ֲגוהטעו טלÿ פאיכא: ");

	char f_name[256] = "C:\\Users\\sasha\\Desktop\\qwe2.txt";

	//char f_name[256];
	//scanf("%c", &f_name);

	Game_board board(f_name);
	Game_states solution(board);
	solution = get_solution(board);
	print_solution(solution);
}
