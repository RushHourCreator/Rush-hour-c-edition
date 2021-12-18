#include "Classes.h"
#include <SFML/Graphics.hpp>

using namespace sf;

direction& operator++(direction& m) {
	if (m == BACK) 
		m = direction(int(m) + 2);
	else
		m = direction(int(m) - 2);
	return m;
}

Game_states restore_steps(const Game_states& queue) {
	int steps = 0;
	for (int i = queue.isol; i; i = queue.prev[i], ++steps);

	Game_states solution(steps + 1, queue);
	solution.isol = steps;
	int k = queue.isol;
	for (int i = steps; i >= 0; --i) {
		solution.states[i] = queue.states[k];
		solution.prev[i] = i - 1;
		++solution.elems;
		k = queue.prev[k];
	}
	return solution;
}

Game_states get_solution(const Game_board& start_board) {
	size_t start_size = 0;
	Game_states queue(start_size, start_board);
	queue.write_start_board(start_board);

	size_t cur = 0, wr = 1;
	for (cur; cur < queue.elems && !queue.states[cur].is_solution(); ++cur) {
		for (size_t i = 0; i < start_board.k; ++i) {
			if (i != queue.states[cur].num) {
				direction dir = BACK;
				do {
					size_t range = queue.states[cur].find_range(i, dir);
					for (size_t dis = 1; dis <= range; ++dis) {
						queue.write_step(queue.states[cur]);
						queue.states[wr].move_car(i, dir, dis);

						if (queue.not_visited(queue.states[wr])) {
							//printf("Состояние %lu -> состояние %lu\n", cur, wr);

							queue.prev[wr] = cur;
							++wr;
							++queue.elems;
						}
					}
					++dir;
				} while (dir == FORWARD);
			}
		}
	}
	Game_states solution(start_board);

	if (cur < queue.elems) {
		queue.isol = cur;
		solution = restore_steps(queue);
	}
	else {
		solution.write_start_board(start_board);
	}
	return solution;
}


void print_solution(const Game_states& solution) {
	const size_t size_cell = 80;//размеры клетки таблицы
	const size_t size_all_steps = 50;//Размер сообщения о количестве ходов
	const size_t size_cur_steps = 20;//размер сообщения о текущем ходе
	const size_t margin_between_steps = 30;//расстояние между сообщениями с ходами
	const size_t size_w_button = 80;//ширина кнопок 
	const size_t size_l_button = 240;//длина кнопок

	const size_t size_car = 52;//размер машины
	const size_t margin_between_car_n_line = 14;//расстояние между линией сетки и машиной
	const size_t size_main_line = 8;//размер толстых линий(например стены)
	const size_t size_line = 4;//размер тонких линий(например сетка)

	const size_t size_text_button = 30;//размер текста на кнопках
	const size_t size_text_step = 30;//размер текста в сообщении о всех ходах
	const size_t size_text_cur_step = 30;//размер текста в сообщении о текущем ходу
	const size_t size_text_car = 36;//размер текста в машинах 

	const size_t margin_up = 30;//отступ сверху окна
	const size_t margin_down = 40;//отступ снизу окна
	const size_t margin_between_buttons = 60;//расстояние между кнопками
	const size_t margin_between_but_n_tab = 60;//расстояние между полем и кнопками
	const size_t margin_left_for_step = 100;//отступ слева для сообщения о количестве ходов
	const size_t size_step = 150 /*size_all_steps + size_cur_steps + 2 * margin_between_steps + margin_up*/;

	size_t window_height = size_step + size_cell * (solution.m - 2) + margin_between_but_n_tab + size_w_button + margin_down;//длина окна
	size_t window_width;//ширина окна
	size_t margin_left;//отступ слева для поля
	size_t margin_right;//отступ справа для поля
	size_t margin_left_for_b;//отступ слева для кнопок
	size_t margin_right_for_b;//отступ справа для кнопок


	//вычисление размеров окна и отступов
	if (size_cell * (solution.n - 2) > size_l_button * 2 + margin_between_buttons) {
		margin_left = 100;
		margin_right = 100;
		window_width = margin_left + size_cell * (solution.n - 2) + margin_right;
		margin_left_for_b = window_width / 2 - margin_between_buttons / 2 - size_l_button;
		margin_right_for_b = margin_left_for_b;
	}
	else {
		margin_left_for_b = 100;
		margin_right_for_b = 100;
		window_width = margin_left_for_b + size_l_button * 2 + margin_between_buttons + margin_right_for_b;
		margin_left = (window_width - size_cell * (solution.n - 2)) / 2;
		margin_right = margin_left;
	}


	//Окно
	RenderWindow window(VideoMode(window_width, window_height), "Rush Hour Solver", Style::Close | Style::Titlebar);

	//Шрифт
	Font font_car;
	font_car.loadFromFile("C:\\Users\\sasha\\Desktop\\consolas.ttf");
	Font font_text;
	font_text.loadFromFile("C:\\Users\\sasha\\Desktop\\consolas.ttf");

	//Кнопка "назад"
	RectangleShape prev_button(Vector2f(size_l_button, size_w_button));
	prev_button.setFillColor(Color(201, 156, 156));
	prev_button.setOutlineColor(Color::Black);
	prev_button.setOutlineThickness(size_main_line);
	prev_button.setPosition(margin_left_for_b, window_height - margin_down - size_w_button);
	Text text_prev_button("Previous step", font_text, size_text_button);
	text_prev_button.setFillColor(Color::Black);
	text_prev_button.setPosition(margin_left_for_b + 10, window_height - margin_down - size_w_button + 20); //магическая константа

	//Кнопка "вперед"
	RectangleShape next_button(Vector2f(size_l_button, size_w_button));
	next_button.setFillColor(Color(153, 255, 204));
	next_button.setOutlineColor(Color::Black);
	next_button.setOutlineThickness(size_main_line);
	next_button.setPosition(window_width / 2 + margin_between_buttons / 2, window_height - margin_down - size_w_button);
	Text text_next_button("Next step", font_text, size_text_button);
	text_next_button.setFillColor(Color::Black);
	text_next_button.setPosition(window_width / 2 + margin_between_buttons / 2 + 40, window_height - margin_down - size_w_button + 20); //магическая константа

	//Стены
	vector<RectangleShape> walls;
	for (int i = 1; i < solution.m - 1; i++)
		for (int j = 1; j < solution.n - 1; j++) {
			if (solution.states[0].A[i][j] == '*') {
				RectangleShape wall(Vector2f(size_cell, size_cell));
				wall.setFillColor(Color::Black);
				wall.setPosition(margin_left + (j - 1) * size_cell, size_step + (i - 1) * size_cell);
				walls.push_back(wall);
			}
		}

	//Машины
	RectangleShape* cars = new RectangleShape[solution.k];
	Text* names = new Text[solution.k];
	for (size_t i = 0; i < solution.k; i++) {
		int len, wdt;
		if (solution.states[0].cars[i].orientation == HORIZONTAL) {
			len = (size_cell - margin_between_car_n_line) * 2 + size_cell * (solution.states[0].cars[i].length - 2);
			wdt = size_car;
		}
		else {
			len = size_car;
			wdt = (size_cell - margin_between_car_n_line) * 2 + size_cell * (solution.states[0].cars[i].length - 2);
		}
		RectangleShape car(Vector2f(len, wdt));
		if (solution.states[0].cars[i].name == 'X' || solution.states[0].cars[i].name == 'x')
			car.setFillColor(Color(255, 80, 80));
		else
			car.setFillColor(Color(153, 204, 255));

		Text car_name(solution.states[0].cars[i].name, font_car, size_text_car);
		car_name.setFillColor(Color::Black);
		//car_name.setStyle(Text::Bold);

		int x = (solution.states[0].cars[i].xpos - 1) * (size_cell)+margin_left + margin_between_car_n_line + 2;
		int y = (solution.states[0].cars[i].ypos - 1) * size_cell + size_step + margin_between_car_n_line + 2;
		car.setPosition(x, y);
		car_name.setPosition(x + len / 2 - 9, y + wdt / 2 - 22); //магическая константа
		car.setOutlineColor(Color::Black);
		car.setOutlineThickness(size_line);
		cars[i] = car;
		names[i] = car_name;
	}

	//Линии разделения строк сетки поля
	RectangleShape* str_lines = new RectangleShape[solution.m - 1];
	RectangleShape str_fline(Vector2f((solution.n - 2) * size_cell + size_main_line, size_main_line));
	str_fline.setFillColor(Color::Black);
	str_fline.setPosition(margin_left, size_step);
	str_lines[0] = str_fline;
	RectangleShape str_lline(Vector2f((solution.n - 2) * size_cell + size_main_line, size_main_line));
	str_lline.setFillColor(Color::Black);
	str_lline.setPosition(margin_left, size_step + (solution.m - 2) * size_cell);
	str_lines[solution.m - 2] = str_lline;
	for (size_t i = 1; i < solution.m - 2; i++) {
		RectangleShape str_line(Vector2f((solution.n - 2) * size_cell + size_main_line, size_line));
		str_line.setFillColor(Color::Black);
		str_line.setPosition(margin_left, size_step + i * size_cell);
		str_lines[i] = str_line;
	}

	//Линии разделения столбцов сетки поля
	RectangleShape* col_lines = new RectangleShape[solution.n];
	RectangleShape col_fline(Vector2f(size_main_line, (solution.m - 2) * size_cell));
	col_fline.setFillColor(Color::Black);
	col_fline.setPosition(margin_left, size_step);
	col_lines[0] = col_fline;
	for (size_t i = 1; i < solution.n - 2; i++) {
		RectangleShape col_line(Vector2f(size_line, (solution.m - 2) * size_cell + size_main_line));
		col_line.setFillColor(Color::Black);
		col_line.setPosition(margin_left + i * size_cell, size_step);
		col_lines[i] = col_line;
	}
	RectangleShape col_ex1(Vector2f(size_main_line, (solution.exit - 1) * size_cell));
	col_ex1.setFillColor(Color::Black);
	col_ex1.setPosition(margin_left + size_cell * (solution.n - 2), size_step);
	col_lines[solution.n - 2] = col_ex1;

	RectangleShape col_ex2(Vector2f(size_main_line, (solution.m - solution.exit - 2) * size_cell));
	col_ex2.setFillColor(Color::Black);
	col_ex2.setPosition(margin_left + size_cell * (solution.n - 2), size_step + solution.exit * size_cell);
	col_lines[solution.n - 1] = col_ex2;

	//Количесвто ходов
	Text all_steps("", font_text, size_text_step);
	if (solution.isol > -1) {
		ostringstream out_all_steps;
		out_all_steps << solution.elems - 1;
		all_steps.setString("Number of steps: " + out_all_steps.str());
		all_steps.setPosition(window_width / 2 - 154, margin_up); // магическая константа 
	}
	else {
		all_steps.setString("NO SOLUTION");
		all_steps.setPosition(window_width / 2 - 90, margin_up); // магическая константа 
	}
	all_steps.setFillColor(Color::Black);


	ostringstream cur_car_name;
	ostringstream cur_dis;
	ostringstream cur_num_state;
	Text cur_step("", font_text, size_text_cur_step);
	cur_step.setPosition(margin_left_for_step, margin_up + size_text_step + margin_between_steps);
	cur_step.setFillColor(Color::Black);

	//Стрелка текущего хода
	RectangleShape arrow_body(Vector2f(0, 0));
	arrow_body.setFillColor(Color(159, 128, 255));
	CircleShape arrow_head(0, 3);
	arrow_head.setFillColor(Color(159, 128, 255));



	size_t cur_state = 0;
	//Основной цикл отрисовки графики
	while (window.isOpen()) {

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();

			//если одна из кнопок нажата
			if (Mouse::isButtonPressed(Mouse::Left)) {
				Vector2i position = Mouse::getPosition(window);
				if (position.x >= margin_left_for_b - size_main_line && position.x <= margin_left_for_b + size_l_button + size_main_line) {
					if (position.y >= window_height - margin_down - size_w_button - size_main_line && position.y <= window_height - margin_down + size_main_line) {
						RectangleShape pushed_prev = prev_button;
						pushed_prev.setFillColor(Color(164, 91, 91));
						window.draw(pushed_prev);

						if (cur_state > 0) {
							int prev_state = cur_state - 1;
							if (prev_state) {
								cur_car_name << solution.states[prev_state].moved_car.name;
								cur_dis << solution.states[prev_state].dis;
								cur_num_state << prev_state;
								if (solution.states[prev_state].moved_car.orientation == HORIZONTAL)
									if (solution.states[prev_state].dir == BACK)
										cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " left " + cur_dis.str());
									else
										cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " right " + cur_dis.str());
								else
									if (solution.states[prev_state].dir == BACK)
										cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " up " + cur_dis.str());
									else
										cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " down " + cur_dis.str());

								cur_car_name.str("");
								cur_dis.str("");
								cur_num_state.str("");
							}
							else
								cur_step.setString("");

							int dx, dy;
							if (solution.states[cur_state].moved_car.orientation == HORIZONTAL) {
								dx = -solution.states[cur_state].dir;
								dy = 0;
							}
							else {
								dx = 0;
								dy = -solution.states[cur_state].dir;
							}

							int ax, ay;
							if (solution.states[prev_state].moved_car.orientation == HORIZONTAL) {
								ay = ((solution.states[prev_state].moved_car.ypos) - 1) * size_cell + size_step + size_cell / 2;
								if (prev_state) {
									arrow_body.setSize(Vector2f((solution.states[prev_state].dis - 1) * size_cell + size_cell / 2 + margin_between_car_n_line / 2, size_main_line));
									arrow_head.setRadius(size_main_line * 1.4);
								}
								else {
									arrow_body.setSize(Vector2f(0, 0));
									arrow_head.setRadius(0);
								}
								if (solution.states[prev_state].dir == FORWARD) {
									ax = (solution.states[prev_state].moved_car.xpos - 1 - solution.states[prev_state].dis) * (size_cell)+margin_left + size_cell / 2 - 2;
									arrow_head.rotate(-arrow_head.getRotation() + 330.f);
									arrow_head.setPosition(ax + (solution.states[prev_state].dis - 1) * size_cell + size_cell / 2 - margin_between_car_n_line / 2 - 7.5, ay);
								}
								else {
									ax = (solution.states[prev_state].moved_car.xpos - 1 + solution.states[prev_state].moved_car.length) * (size_cell)+margin_left - margin_between_car_n_line / 2 + 6;
									arrow_head.rotate(-arrow_head.getRotation() + 30.f);
									arrow_head.setPosition(ax + 3, ay - size_main_line * 1.4);
								}
								arrow_body.setPosition(ax, ay);
							}
							else {
								ax = (solution.states[prev_state].moved_car.xpos - 1) * size_cell + margin_left + size_cell / 2;
								if (prev_state) {
									arrow_body.setSize(Vector2f(size_main_line, (solution.states[prev_state].dis - 1) * size_cell + size_cell / 2 + margin_between_car_n_line / 2));
									arrow_head.setRadius(size_main_line * 1.4);
								}
								else {
									arrow_body.setSize(Vector2f(0, 0));
									arrow_head.setRadius(0);
								}
								if (solution.states[prev_state].dir == FORWARD) {
									ay = (solution.states[prev_state].moved_car.ypos - 1 - solution.states[prev_state].dis) * size_cell + size_step + size_cell / 2 - 4;
									arrow_head.rotate(-arrow_head.getRotation() + 60.f);
									arrow_head.setPosition(ax + size_main_line, ay + (solution.states[prev_state].dis - 1) * size_cell + size_cell / 2 - margin_between_car_n_line / 2 - 7);
								}
								else {
									arrow_head.rotate(-arrow_head.getRotation());
									ay = (solution.states[prev_state].moved_car.ypos - 1 + solution.states[prev_state].moved_car.length) * size_cell + size_step - margin_between_car_n_line / 2 + 18;
									arrow_head.setPosition(ax - size_main_line + 1.3, ay - size_main_line / 2 - 11);
								}
								arrow_body.setPosition(ax, ay);
							}


							float speed = 0.4;
							int j = 0;
							int num_car = solution.states[cur_state].num;
							for (float i = 0; i < solution.states[cur_state].dis * size_cell; i += speed, j++)
							{
								window.clear(Color::White);

								//отрисовка текста ходов
								window.draw(all_steps);
								window.draw(cur_step);

								if (j < 700)
									window.draw(pushed_prev);
								else
									window.draw(prev_button);
								window.draw(next_button);

								//отрисовка линий строк
								for (size_t i = 0; i < solution.m - 1; i++)
									window.draw(str_lines[i]);

								//отрисовка линий столбцов
								for (size_t i = 0; i < solution.n; i++)
									window.draw(col_lines[i]);

								//отрисовка стен
								for (size_t i = 0; i < walls.size(); i++)
									window.draw(walls[i]);

								//отрисовка машин
								for (size_t i = 0; i < solution.k; i++) {
									window.draw(cars[i]);
									window.draw(names[i]);
								}

								cars[num_car].move(speed * dx, speed * dy);
								names[num_car].move(speed * dx, speed * dy);
								window.draw(cars[num_car]);
								window.draw(names[num_car]);

								//отрисовка текста кнопок
								window.draw(text_prev_button);
								window.draw(text_next_button);

								window.display();
							}
							cur_state--;
						}
					}
				}
				else if (position.x >= window_width - margin_right_for_b - size_l_button - size_main_line && position.x <= window_width - margin_right_for_b + size_main_line) {
					if (position.y >= window_height - margin_down - size_w_button - size_main_line && position.y <= window_height - margin_down + size_main_line) {
						RectangleShape pushed_next = next_button;
						pushed_next.setFillColor(Color(0, 204, 102));
						window.draw(pushed_next);
						window.display();
						if (cur_state < solution.size - 1) {
							cur_state++;

							cur_car_name << solution.states[cur_state].moved_car.name;
							cur_dis << solution.states[cur_state].dis;
							cur_num_state << cur_state;
							if (solution.states[cur_state].moved_car.orientation == HORIZONTAL)
								if (solution.states[cur_state].dir == BACK)
									cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " left " + cur_dis.str());
								else
									cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " right " + cur_dis.str());
							else
								if (solution.states[cur_state].dir == BACK)
									cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " up " + cur_dis.str());
								else
									cur_step.setString("Step " + cur_num_state.str() + ": " + cur_car_name.str() + " down " + cur_dis.str());

							cur_car_name.str("");
							cur_dis.str("");
							cur_num_state.str("");

							int dx, dy, ax, ay;
							if (solution.states[cur_state].moved_car.orientation == HORIZONTAL) {
								dx = solution.states[cur_state].dir;
								dy = 0;

								ay = (solution.states[cur_state].moved_car.ypos - 1) * size_cell + size_step + size_cell / 2;
								arrow_body.setSize(Vector2f((solution.states[cur_state].dis - 1) * size_cell + size_cell / 2 + margin_between_car_n_line / 2, size_main_line));
								arrow_head.setRadius(size_main_line * 1.4);
								if (solution.states[cur_state].dir == FORWARD) {
									ax = (solution.states[cur_state].moved_car.xpos - 1 - solution.states[cur_state].dis) * (size_cell)+margin_left + size_cell / 2 - 2;
									arrow_head.rotate(-arrow_head.getRotation() + 330.f);
									arrow_head.setPosition(ax + (solution.states[cur_state].dis - 1) * size_cell + size_cell / 2 - margin_between_car_n_line / 2 - 7.5, ay);
								}
								else {
									ax = (solution.states[cur_state].moved_car.xpos - 1 + solution.states[cur_state].moved_car.length) * (size_cell)+margin_left - margin_between_car_n_line / 2 + 6;
									arrow_head.rotate(-arrow_head.getRotation() + 30.f);
									arrow_head.setPosition(ax + 3, ay - size_main_line * 1.4);
								}
								arrow_body.setPosition(ax, ay);
							}
							else {
								dx = 0;
								dy = solution.states[cur_state].dir;

								ax = (solution.states[cur_state].moved_car.xpos - 1) * size_cell + margin_left + size_cell / 2;

								arrow_body.setSize(Vector2f(size_main_line, (solution.states[cur_state].dis - 1) * size_cell + size_cell / 2 + margin_between_car_n_line / 2));
								arrow_head.setRadius(size_main_line * 1.4);
								if (solution.states[cur_state].dir == FORWARD) {
									ay = (solution.states[cur_state].moved_car.ypos - 1 - solution.states[cur_state].dis) * size_cell + size_step + size_cell / 2 - 4;
									arrow_head.rotate(-arrow_head.getRotation() + 60.f);
									arrow_head.setPosition(ax + size_main_line, ay + (solution.states[cur_state].dis - 1) * size_cell + size_cell / 2 - margin_between_car_n_line / 2 - 7);
								}
								else {
									arrow_head.rotate(-arrow_head.getRotation());
									ay = (solution.states[cur_state].moved_car.ypos - 1 + solution.states[cur_state].moved_car.length) * size_cell + size_step - margin_between_car_n_line / 2 + 18;
									arrow_head.setPosition(ax - size_main_line + 1.3, ay - size_main_line / 2 - 11);
								}
								arrow_body.setPosition(ax, ay);
							}

							float speed = 0.4;
							int j = 0;
							int num_car = solution.states[cur_state].num;
							for (float i = 0; i < solution.states[cur_state].dis * size_cell; i += speed, j++) {
								window.clear(Color::White);

								//отрисовка текста ходов
								window.draw(all_steps);
								window.draw(cur_step);

								if (j < 700)
									window.draw(pushed_next);
								else
									window.draw(next_button);
								window.draw(prev_button);

								//отрисовка линий строк
								for (size_t i = 0; i < solution.m - 1; i++)
									window.draw(str_lines[i]);

								//отрисовка линий столбцов
								for (size_t i = 0; i < solution.n; i++)
									window.draw(col_lines[i]);

								//отрисовка стен
								for (size_t i = 0; i < walls.size(); i++)
									window.draw(walls[i]);

								//отрисовка машин
								for (size_t i = 0; i < solution.k; i++) {
									window.draw(cars[i]);
									window.draw(names[i]);
								}

								cars[num_car].move(speed * dx, speed * dy);
								names[num_car].move(speed * dx, speed * dy);
								window.draw(cars[num_car]);
								window.draw(names[num_car]);

								//отрисовка текста кнопок
								window.draw(text_prev_button);
								window.draw(text_next_button);

								window.display();
							}
						}
					}
				}
			}
		}

		window.clear(Color::White);

		//отрисовка текста ходов
		window.draw(all_steps);
		window.draw(cur_step);

		//отрисовка кнопок
		window.draw(prev_button);
		window.draw(next_button);

		//отрисовка линий строк
		for (size_t i = 0; i < solution.m - 1; i++)
			window.draw(str_lines[i]);

		//отрисовка линий столбцов
		for (size_t i = 0; i < solution.n; i++)
			window.draw(col_lines[i]);

		//отрисовка стен
		for (size_t i = 0; i < walls.size(); i++)
			window.draw(walls[i]);

		//отрисовка машин
		for (size_t i = 0; i < solution.k; i++) {
			window.draw(cars[i]);
			window.draw(names[i]);
		}

		//отрисовка стрелки хода
		window.draw(arrow_body);
		window.draw(arrow_head);

		//отрисовка текста кнопок
		window.draw(text_prev_button);
		window.draw(text_next_button);


		window.display();
	}

}