#pragma once
#include "Libraries.h"


class Car {
public:
	char name;//имя машины
	size_t length;//длина машины
	enum in_space orientation;//ориентация в пространстве
	size_t xpos;//столбец, в котором записан первый элемент машины
	size_t ypos;//строка, в которой записан первый элемент машины
};

typedef Car* t_cars;


class Game_board {
public:
	t_matr A;//матрица с игровым полем
	t_cars cars;//массив машин на поле
	size_t k;//количество машин на поле
	size_t m;//количество строк в матрице A
	size_t n;//количество столбцов в матрице A
	size_t exit;//номер строки с выходом

	Game_board() {
		A = nullptr;
		cars = nullptr;
		m = 0;
		n = 0;
		k = 0;
		exit = 0;
	}


	Game_board(const char* f_name) {
		m = find_M(f_name);
		n = find_N(f_name);
		k = ((m - 2) * (n - 2) / 2);
		create_game_board();
		copy_from_file(f_name);
		assignment_cars();
	}

	Game_board(const Game_board& a) {
		copy_board(a);
	}


	~Game_board() {
		delete_game_board();
	}


	Game_board& operator=(const Game_board& a) {
		if (this != &a) {
			delete_game_board();
			copy_board(a);
		}
		return *this;
	}




	size_t find_N(const char* f_name) {
		ifstream f(f_name);
		char c;
		size_t n = 0;
		do {
			f.get(c);
			++n;
		} while (c != '\n');
		f.close();
		return n - 1;
	}

	size_t find_M(const char* f_name) {
		ifstream f(f_name);
		char s[256];
		size_t m = 0;
		while (!f.eof()) {
			f.getline(s, 256);
			++m;
		}
		f.close();
		return m;
	}

	void create_game_board() {
		A = (char**)calloc(m, sizeof(char*));
		for (size_t i = 0; i < m; ++i)
			A[i] = (char*)calloc(n, sizeof(char));
		cars = (t_cars)calloc(k, sizeof(Car));
	}

	void delete_game_board() {
		if (A) {
			for (size_t i = 0; i < m; ++i)
				free(A[i]);
			free(A);
		}
		if(cars)
			free(cars);
	}

	void copy_from_file(const char* f_name) {
		ifstream f(f_name);
		char c;
		for (size_t i = 0; i < m; ++i) {
			for (size_t j = 0; j < n; ++j) {
				f >> c;
				A[i][j] = c;
			}
			if (c == '|' || c == 'X' || c == 'x')
				exit = i;
		}
		f.close();
	}

	int is_exist(const char x) {
		size_t i;
		for (i = 0; i < k && x != cars[i].name; ++i);
		int res = (i < k) ? i : -1;
		return res;
	}

	void assignment_cars() {
		k = 0;
		for (size_t i = 1; i < m - 1; ++i) {
			for (size_t j = 1; j < n - 1; ++j) {
				if (A[i][j] != '-' && A[i][j] != '*') {
					int cur = is_exist(A[i][j]);
					if (cur >= 0) {
						++cars[cur].length;
						if (i == cars[cur].ypos)
							cars[cur].orientation = HORIZONTAL;
						else
							cars[cur].orientation = VERTICAL;
					}
					else {
						cars[k].name = A[i][j];
						cars[k].length = 1;
						cars[k].xpos = j;
						cars[k].ypos = i;
						if ((cars[k].name == 'X' || cars[k].name == 'x') && (A[exit][n - 1] == 'X' || A[exit][n - 1] == 'x'))
							++cars[k].length;
						++k;
					}
				}
			}
		}
		cars = (Car*)realloc(cars, k * sizeof(Car));
	}

	void copy_matr(const t_matr& B) {
		for (size_t i = 0; i < m; ++i) {
			for (size_t j = 0; j < n; ++j)
				A[i][j] = B[i][j];
		}
	}

	void copy_cars(const t_cars& B) {
		for (size_t i = 0; i < k; ++i) {
			cars[i] = B[i];
		}
	}

	void copy_board(const Game_board& board) {
		m = board.m;
		n = board.n;
		k = board.k;
		exit = board.exit;
		create_game_board();
		copy_matr(board.A);
		copy_cars(board.cars);
	}

	int comp(const t_matr& B) {
		int f = 1;
		for (size_t i = 0; i < m && f; ++i)
			for (size_t j = 0; j < n && f; ++j)
				f = A[i][j] == B[i][j];
		return f;
	}

	void output_matr() {
		for (size_t i = 0; i < m; ++i) {
			for (size_t j = 0; j < n; ++j)
				cout << A[i][j];
			cout << "\n";
		}
	}
};



class Step : public Game_board {
public:
	Car moved_car;//машина, которой был совершен ход
	size_t num;//номер машины, которой был совершен ход
	direction dir;//направление, в котором двигалась машина
	size_t dis;//расстояние, на которое двигалась машина


	Step(const Game_board& board) : Game_board(board) {

	}

	Step(const Step& a) : Game_board(a) {
		moved_car = a.moved_car;
		num = a.num;
		dir = a.dir;
		dis = a.dis;
	}


	Step& operator=(const Step& a) {
		if (this != &a) {
			delete_game_board();
			copy_board(a);
			
			moved_car = a.moved_car;
			num = a.num;
			dir = a.dir;
			dis = a.dis;
		}
		return *this;
	}



	int is_solution() {
		size_t i = exit;
		size_t j = n - 1;
		return A[i][j] == 'x' || A[i][j] == 'X';
	}



	size_t find_range(const int num, const direction dir) {
		size_t i = cars[num].ypos;
		size_t j = cars[num].xpos;
		size_t res = 0;
		if (cars[num].orientation == HORIZONTAL) {
			if (dir == BACK)
				--j;
			else
				j += cars[num].length;
			for (res; A[i][j] == '-'; ++res, j += dir);
			if (A[i][j] == '|')
				++res;

		}
		else {
			if (dir == BACK)
				--i;
			else
				i += cars[num].length;
			for (res; A[i][j] == '-'; ++res, i += dir);
		}
		return res;
	}

	void move_car(const int num, const direction dir, const size_t dis) {
		size_t i = cars[num].ypos;
		size_t j = cars[num].xpos;
		if (cars[num].orientation == HORIZONTAL) {
			for (size_t k = 0; k < cars[num].length; ++k, ++j)
				A[i][j] = '-';
			cars[num].xpos += dir * dis;
			j = cars[num].xpos;
			for (size_t k = 0; k < cars[num].length; ++k, ++j)
				A[i][j] = cars[num].name;
		}
		else {
			for (size_t k = 0; k < cars[num].length; ++k, ++i)
				A[i][j] = '-';
			cars[num].ypos += dir * dis;
			i = cars[num].ypos;
			for (size_t k = 0; k < cars[num].length; ++k, ++i)
				A[i][j] = cars[num].name;
		}
		moved_car = cars[num];
		this->num = num;
		this->dir = dir;
		this->dis = dis;
	}
};


typedef Step* t_steps;


class Game_states : public Game_board {
public:
	t_steps states;//вектор ходов
	size_t* prev;//массив предыдущих ходов
	size_t size;//размер вектора states
	size_t elems;//количество элементов, записанных в векторе
	int isol;//индекс решения


	Game_states(const Game_board& board) : Game_board(board) {
		size = 1;
		create_states(size);
		prev = (size_t*)malloc(size * sizeof(size_t));
		prev[0] = -1;
		elems = 0;
		isol = -1;
	}

	Game_states(const size_t size, const Game_board& board) : Game_board(board) {
		create_states(size);
		prev = (size_t*)malloc(size * sizeof(size_t));
		prev[0] = -1;
		this->size = size;
		elems = 0;
		isol = -1;
	}

	Game_states(const Game_states& a) : Game_board(a) {
		size = a.size;
		elems = a.elems;
		isol = a.isol;
		create_states(size);
		prev = (size_t*)malloc(size * sizeof(size_t));
		for (int i = 0; i < elems; i++) {
			states[i] = a.states[i];
			prev[i] = a.prev[i];
		}
	}

	~Game_states() {
		if (size) {
			for (size_t i = 0; i < size; ++i)
				states[i].delete_game_board();
			free(prev);
			free(states);
		}
	}


	Game_states& operator=(const Game_states& a) {
		if (this != &a) {
			copy_board(a);

			size = a.size;
			elems = a.elems;
			isol = a.isol;
			
			create_states(size);
			prev = (size_t*)malloc(size * sizeof(size_t));
			for (int i = 0; i < elems; i++) {
				states[i] = a.states[i];
				prev[i] = a.prev[i];
			}
		}
		return *this;
	}


	void create_state(const size_t i) {
		states[i].m = m;
		states[i].n = n;
		states[i].k = k;
		states[i].exit = exit;
		states[i].create_game_board();

	}

	void create_states(const size_t size) {
		states = (t_steps)malloc(size * sizeof(Step));
		for (size_t i = 0; i < size; ++i) {
			create_state(i);
		}
	}

	void delete_states() {
		if (size) {
			for (size_t i = 0; i < size; ++i)
				states[i].delete_game_board();
			free(prev);
		}
	}


	void extension_states() {
		size_t new_size = size * 2;
		states = (t_steps)realloc(states, new_size * sizeof(Step));
		for (size_t i = size; i < new_size; ++i)
			create_state(i);
		prev = (size_t*)realloc(prev, new_size * sizeof(size_t));
		size = new_size;
	}

	void extension_states(const size_t new_size) {
		states = (t_steps)realloc(states, new_size * sizeof(Step));
		if (new_size > size)
			for (size_t i = size; i < new_size; ++i)
				create_state(i);
		prev = (size_t*)realloc(prev, new_size * sizeof(size_t));
		size = new_size;
	}

	int not_visited(const Step& cur) {
		size_t i;
		for (i = 0; i < elems && !states[i].comp(cur.A); ++i);
		return i == elems;
	}

	void write_start_board(const Game_board& board) {
		if (!size) {
			extension_states(1);
			size = 1;
		}
		if (!elems)
			elems = 1;
		prev[0] = -1;
		states[0].copy_matr(board.A);
		states[0].copy_cars(board.cars);
		states[0].num = -1;
	}


	void write_step(const Step st) {
		if (elems == size)
			extension_states();
		states[elems] = st;
	}
};
































//#pragma once
//#include "Libraries.h"
//
//
//class Car {
//public:
//	char name;//имя машины
//	size_t length;//длина машины
//	enum in_space orientation;//ориентация в пространстве
//	size_t xpos;//столбец, в котором записан первый элемент машины
//	size_t ypos;//строка, в которой записан первый элемент машины
//
//	//копирует машину А в данный объект
//	void copy_car(Car& A) {
//		name = A.name;
//		length = A.length;
//		orientation = A.orientation;
//		xpos = A.xpos;
//		ypos = A.ypos;
//	}
//};
//
//typedef Car* t_cars;
//
//
//class Game_board {
//public:
//	t_matr A;//матрица с игровым полем
//	t_cars cars;//массив машин на поле
//	size_t k;//количество машин на поле
//	size_t m;//количество строк в матрице A
//	size_t n;//количество столбцов в матрице A
//	size_t exit;//номер строки с выходом
//
//	Game_board() {
//
//	}
//
//	Game_board(size_t m, size_t n, size_t k, size_t exit) {
//		this->m = m;
//		this->n = n;
//		this->k = k;
//		this->exit = exit;
//		create_game_board();
//		this->cars = new Car[k];
//	}
//
//	Game_board(char* f_name) {
//		m = find_M(f_name);
//		n = find_N(f_name);
//		create_game_board();
//		copy_from_file(f_name);
//		k = 0;
//		assignment_cars();
//	}
//
//
//	//~Game_board() {
//	//	delete_game_board();
//	//}
//
//	~Game_board() {
//
//	}
//
//	size_t find_N(char* f_name) {
//		ifstream f(f_name);
//		char c;
//		size_t n = 0;
//		do {
//			f.get(c);
//			++n;
//		} while (c != '\n');
//		f.close();
//		return n - 1;
//	}
//
//	size_t find_M(char* f_name) {
//		ifstream f(f_name);
//		char s[256];
//		size_t m = 0;
//		while (!f.eof()) {
//			f.getline(s, 256);
//			++m;
//		}
//		f.close();
//		return m;
//	}
//
//	void create_game_board() {
//		//A = (char**)calloc(m, sizeof(char*));
//		A = new char* [m];
//		for (size_t i = 0; i < m; ++i)
//			//A[i] = (char*)calloc(n, sizeof(char));
//			A[i] = new char[n];
//		cars = (Car*)calloc(((m - 2) * (n - 2) / 2), sizeof(Car));
//	}
//
//	void delete_game_board() {
//		//for (size_t i = 0; i < m; ++i)
//		//	free(A[i]);
//		//free(A);
//		//free(cars);
//		for (size_t i = 0; i < m; ++i)
//			delete[] A[i];
//		delete[] A;
//		delete[] cars;
//	}
//
//	void copy_from_file(char* f_name) {
//		ifstream f(f_name);
//		char c;
//		for (size_t i = 0; i < m; ++i) {
//			for (size_t j = 0; j < n; ++j) {
//				f >> c;
//				A[i][j] = c;
//			}
//			if (c == '|' || c == 'X' || c == 'x')
//				exit = i;
//		}
//		f.close();
//	}
//
//	int is_exist(char x) {
//		size_t i;
//		for (i = 0; i < k && x != cars[i].name; ++i);
//		int res = (i < k) ? i : -1;
//		return res;
//	}
//
//	void assignment_cars() {
//		for (size_t i = 1; i < m - 1; ++i) {
//			for (size_t j = 1; j < n - 1; ++j) {
//				if (A[i][j] != '-' && A[i][j] != '*') {
//					int cur = is_exist(A[i][j]);
//					if (cur >= 0) {
//						++cars[cur].length;
//						if (i == cars[cur].ypos)
//							cars[cur].orientation = HORIZONTAL;
//						else
//							cars[cur].orientation = VERTICAL;
//					}
//					else {
//						cars[k].name = A[i][j];
//						cars[k].length = 1;
//						cars[k].xpos = j;
//						cars[k].ypos = i;
//						if ((cars[k].name == 'X' || cars[k].name == 'x') && (A[exit][n - 1] == 'X' || A[exit][n - 1] == 'x'))
//							++cars[k].length;
//						++k;
//					}
//				}
//			}
//		}
//		cars = (Car*)realloc(cars, k * sizeof(Car));
//	}
//
//	void copy_matr(t_matr& B) {
//		for (size_t i = 0; i < m; ++i) {
//			for (size_t j = 0; j < n; ++j)
//				A[i][j] = B[i][j];
//		}
//	}
//
//	void copy_cars(t_cars& B) {
//		for (size_t i = 0; i < k; ++i) {
//			cars[i].name = B[i].name;
//			cars[i].length = B[i].length;
//			cars[i].orientation = B[i].orientation;
//			cars[i].xpos = B[i].xpos;
//			cars[i].ypos = B[i].ypos;
//		}
//	}
//
//	void copy_board(Game_board& board) {
//		copy_matr(board.A);
//		copy_cars(board.cars);
//	}
//
//	int comp(t_matr& B) {
//		int f = 1;
//		for (size_t i = 0; i < m && f; ++i)
//			for (size_t j = 0; j < n && f; ++j)
//				f = A[i][j] == B[i][j];
//		return f;
//	}
//
//	void output_matr() {
//		for (size_t i = 0; i < m; ++i) {
//			for (size_t j = 0; j < n; ++j)
//				cout << A[i][j];
//			cout << "\n";
//		}
//	}
//};
//
//
//
//class Step : public Game_board {
//public:
//	Car moved_car;//машина, которой был совершен ход
//	size_t num;//номер машины, которой был совершен ход
//	direction dir;//направление, в котором двигалась машина
//	size_t dis;//расстояние, на которое двигалась машина
//
//	Step() {
//
//	}
//
//	Step(Game_board& board) : Game_board(board.m, board.n, board.k, board.exit) {
//		//cars = (Car*)calloc(board.k, sizeof(Car));
//		//cars = new Car[k];
//
//		copy_board(board);
//	}
//
//	//~Step() {
//	//	//delete_game_board();
//	//}
//
//	int is_solution() {
//		size_t i = exit;
//		size_t j = n - 1;
//		return A[i][j] == 'x' || A[i][j] == 'X';
//	}
//
//	void copy_step(Step& S) {
//		copy_board(S);
//		/*	k = S.k;
//			exit = S.exit;*/
//		num = S.num;
//		moved_car.copy_car(S.moved_car);
//		dir = S.dir;
//		dis = S.dis;
//	}
//
//	size_t find_range(int num, direction dir) {
//		size_t i = cars[num].ypos;
//		size_t j = cars[num].xpos;
//		size_t res = 0;
//		if (cars[num].orientation == HORIZONTAL) {
//			if (dir == BACK)
//				--j;
//			else
//				j += cars[num].length;
//			for (res; A[i][j] == '-'; ++res, j += dir);
//			if (A[i][j] == '|')
//				++res;
//
//		}
//		else {
//			if (dir == BACK)
//				--i;
//			else
//				i += cars[num].length;
//			for (res; A[i][j] == '-'; ++res, i += dir);
//		}
//		return res;
//	}
//
//	void move_car(int num, direction dir, size_t dis) {
//		size_t i = cars[num].ypos;
//		size_t j = cars[num].xpos;
//		if (cars[num].orientation == HORIZONTAL) {
//			for (size_t k = 0; k < cars[num].length; ++k, ++j)
//				A[i][j] = '-';
//			cars[num].xpos += dir * dis;
//			j = cars[num].xpos;
//			for (size_t k = 0; k < cars[num].length; ++k, ++j)
//				A[i][j] = cars[num].name;
//		}
//		else {
//			for (size_t k = 0; k < cars[num].length; ++k, ++i)
//				A[i][j] = '-';
//			cars[num].ypos += dir * dis;
//			i = cars[num].ypos;
//			for (size_t k = 0; k < cars[num].length; ++k, ++i)
//				A[i][j] = cars[num].name;
//		}
//		moved_car.copy_car(cars[num]);
//		this->num = num;
//		this->dir = dir;
//		this->dis = dis;
//	}
//};
//
//
//class Game_states : public Game_board {
//public:
//	vector<Step> states;//вектор ходов
//	size_t* prev;//массив предыдущих ходов
//	size_t size;//размер вектора states
//	size_t elems;//количество элементов, записанных в векторе
//	int isol;//индекс решения
//
//
//	Game_states(Game_board& board) : Game_board(board.m, board.n, board.k, board.exit) {
//		size = 1;
//		states.resize(size);
//		create_states(size);
//		prev = (size_t*)malloc(size * sizeof(size_t));
//		prev[0] = -1;
//		elems = 0;
//		isol = -1;
//	}
//
//	Game_states(size_t size, Game_board& board) : Game_board(board.m, board.n, board.k, board.exit) {
//		states.resize(size);
//		create_states(size);
//		prev = (size_t*)malloc(size * sizeof(size_t));
//		prev[0] = -1;
//		this->size = size;
//		elems = 0;
//		isol = -1;
//	}
//
//	//~Game_states() {
//	//	if (size) {
//	//		//for (size_t i = 0; i < size; ++i)
//	//		//	states[i].delete_game_board();
//	//		free(prev);
//	//	}
//	//}
//
//
//	void create_state(size_t i) {
//		states[i].cars = new Car[k];
//		states[i].A = new char* [m];
//		for (size_t j = 0; j < m; ++j)
//			states[i].A[j] = new char[n];
//		states[i].m = m;
//		states[i].n = n;
//		states[i].k = k;
//		states[i].exit = exit;
//	}
//
//
//	void delete_states() {
//		if (size) {
//			for (size_t i = 0; i < size; ++i)
//				states[i].delete_game_board();
//		}
//		free(prev);
//	}
//
//	void create_states(size_t size) {
//		for (size_t i = 0; i < size; ++i) {
//			create_state(i);
//		}
//	}
//
//	void extension_states() {
//		size_t new_size = size * 2;
//		states.resize(new_size);
//		for (size_t i = size; i < new_size; ++i) {
//			create_state(i);
//		}
//		prev = (size_t*)realloc(prev, new_size * sizeof(size_t));
//		size = new_size;
//	}
//
//	void extension_states(size_t new_size) {
//		states.resize(new_size);
//		if (new_size > size)
//			for (size_t i = size; i < new_size; ++i) {
//				create_state(i);
//			}
//		prev = (size_t*)realloc(prev, new_size * sizeof(size_t));
//		size = new_size;
//	}
//
//	int not_visited(Step& cur) {
//		size_t i;
//		for (i = 0; i < elems && !states[i].comp(cur.A); ++i);
//		return i == elems;
//	}
//
//	void write_start_board(Game_board& board) {
//		if (!size) {
//			extension_states(1);
//			size = 1;
//		}
//		if (!elems)
//			elems = 1;
//		prev[0] = -1;
//		states[0].copy_board(board);
//		states[0].num = -1;
//	}
//
//
//	void write_step(Step st) {
//		if (elems == size)
//			extension_states();
//		states[elems].copy_step(st);
//
//	}
//
//
//};
//
//
//Game_states restore_steps(Game_states& queue);
//
//Game_states get_solution(Game_board& start_board);
//
//void print_solution_text(Game_states& solution);