#pragma once
#include "Classes.h"

Game_states restore_steps(const Game_states& queue);

Game_states get_solution(const Game_board& start_board);

void print_solution(const Game_states& solution);