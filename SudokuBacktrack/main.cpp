
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>


#define SUDOKU_CELLS		81
#define SUDOKU_ROW			9
#define SUDOKU_BOX			3
#define SUDOKU_BOX_BLOCK	27 // 9*3

#define MAX_BACKTRACKS		1000


#define GET_SUDOKU_ROW_START(c) (c - ( c % SUDOKU_ROW))
#define GET_SUDOKU_COL_START(c) (c % SUDOKU_ROW)

using namespace std;



/******************************************************************************
 *	Checks
 ******************************************************************************/

static bool check_row(char *cells, char c, char v)
{
	static char val;
	static int i;

	val = GET_SUDOKU_ROW_START(c);

	for (i = 0; i < SUDOKU_ROW; i++)
	{
		if (cells[val] == v)
			return false;

		val += 1;
	}

	return true;
}

static bool check_coll(char *cells, char c, char v)
{
	static char val;
	static int i;

	val = GET_SUDOKU_COL_START(c);

	for (i = 0; i < SUDOKU_ROW; i++)
	{
		if (cells[val] == v)
			return false;

		val += SUDOKU_ROW;
	}

	return true;
}

static bool check_box(char *cells, char c, char v)
{
	static char val;
	static int i, t1,tr, base;

	base  = SUDOKU_BOX_BLOCK * (int)(c / SUDOKU_BOX_BLOCK);     // base row
	base += SUDOKU_BOX * (int)((c % SUDOKU_ROW) / SUDOKU_BOX); // base col

	for (i = 0; i < SUDOKU_ROW; i++)
	{
		tr = SUDOKU_ROW * (int)(i / SUDOKU_BOX);
		t1 = i % SUDOKU_BOX;

		val = base + tr + t1;

		if (cells[val] == v)
			return false;
	}

	return true;
}



static bool check_3_rules(char *cells, char cell, char v)
{
	return check_row(cells, cell, v) && check_coll(cells, cell, v) && check_box(cells, cell, v);
}




/******************************************************************************
*	Helpers
******************************************************************************/


static void set_cell(int row, int col, int val, char *cells)
{
	row--;
	col--;

	cells[(row * SUDOKU_ROW) + col] = val;

}


static void print_sol(const char *original, const char *cells)
{

	cout << "-------------------------------" << endl;

	for (int i = 0; i < SUDOKU_CELLS; i++)
	{

		if (i % SUDOKU_ROW == 0 && i >= 9)
			cout << "|" << endl;


		if (i % SUDOKU_BOX_BLOCK == 0 && i >= SUDOKU_BOX_BLOCK)
			cout << "-------------------------------" << endl;


		if (i % SUDOKU_BOX == 0)
			cout << "|";

		if (original != NULL && original[i] != 0)
			cout << "*" << (int)original[i] << "*";
		else if ( cells != NULL)
			cout << " " << (int)cells[i] << " ";
		else cout << "   ";
	}

	cout << "|" << endl << "-------------------------------" << endl;
}



/******************************************************************************
*	Backtracks
******************************************************************************/


static bool backtrack_solve(char *cells, const int c = 0, int *solutions = NULL)
{
	if (c >= SUDOKU_CELLS)
	{
		if (solutions == NULL)
			return true;

		(*solutions)++;

		return (*solutions > 1); // break at two;
	}

	if (cells[c] != 0)
		return backtrack_solve(cells, c + 1, solutions);

	else
	{
		for (int i = 1; i <= SUDOKU_ROW; i++)
		{
			if (check_3_rules(cells, c, i))
			{
				cells[c] = i;

				if (backtrack_solve(cells, c + 1, solutions))
					return true;
				
				cells[c] = 0;
			}
		}
	}

	return false;
}

static bool backtrack_find_one_solution(char *in, vector<char> *poss, const int c, int* backtracks, bool* solution)
{
	if (c >= SUDOKU_CELLS)
	{
		*solution = true;
		return true;
	}

	if (in[c] != 0)
		return backtrack_find_one_solution(in, poss, c + 1, backtracks, solution);

	else
	{
		for (vector<char>::iterator it = poss[c].begin(); it != poss[c].end(); ++it)
		{
			if (check_3_rules(in, c, *it))
			{
				in[c] = *it;

				if (backtrack_find_one_solution(in, poss, c + 1, backtracks, solution))
					return true;

				if ((*backtracks)++ >= MAX_BACKTRACKS)
					return true;

				in[c] = 0;
			}
		}
	}

	return false;
}






/******************************************************************************
*	Generators
******************************************************************************/

static void fill_possible_solutions(char *in, vector<char> *cells)
{
	
	int cell;
	char v;

	for (cell = 0; cell < SUDOKU_CELLS; cell++)
	{
		cells[cell].clear();

		if (in[cell] == 0)
		{
			for (v = 1; v <= SUDOKU_ROW; v++)
			{
				if (check_3_rules(in, cell, v))
					cells[cell].push_back(v);
			}

			if (cells[cell].size() > 1)
				random_shuffle(cells[cell].begin(), cells[cell].end());
		}
	}
}

static bool las_vegas(char *in, const int n = 11)
{
	int r,cell;
	int backtracks = 0;
	bool solution = false;

	vector<char> cells[SUDOKU_CELLS];

	memset(in, 0, SUDOKU_CELLS);

	for (int i = 0; i < n; i++)
	{

		do {
			cell = rand() % SUDOKU_CELLS;
		} while (in[cell] != 0);

		do
		{
			r = 1 + (rand() % SUDOKU_ROW);

			if (check_3_rules(in, cell, r))
			{
				in[cell] = r;
				break;
			}

		} while (true);
	}

	fill_possible_solutions(in, cells);
	return backtrack_find_one_solution(in, cells, 0, &backtracks, &solution) && solution;
}


static void gen_sequence(vector<int> *vect, bool rand = false)
{
	for (int i = 0; i < SUDOKU_CELLS; i++)
		vect->push_back(i);

	if ( rand )
		random_shuffle(vect->begin(), vect->end());
}

static void gen_s_sequence(vector<int> *vect)
{
	int i, count = 1;
	int limit = 0;
	bool norm = true;

	for (i = 0; i < SUDOKU_CELLS; i++, count++)
	{
		if (count > SUDOKU_ROW)
		{
			count = 1;
			norm = !norm;

			limit = SUDOKU_ROW * (1 + (int)(i / SUDOKU_ROW));
		}

		vect->push_back(norm ? i : (limit - count));
	}
}



static void set_difficulty(int difficulty, vector<int> *order, int *max_empty_cells, int *upper_bound)
{
	int givens,
		lower_bound;

	switch (difficulty)
	{
		//Evil
	case 5:
		gen_sequence(order, false);
		givens = 22;
		lower_bound = 0;

		break;

		//Difficult
	case 4:
		gen_s_sequence(order);
		givens = 28;
		lower_bound = 2;

		break;

		// Medium
	case 3:
		gen_sequence(order, true);
		givens = 32;
		lower_bound = 3;

		break;

		// Easy
	case 2:
		gen_sequence(order, true);
		givens = 36;
		lower_bound = 4;

		break;

		// Extremely Easy
	case 1:
	default:
		gen_sequence(order, true);
		givens = 50;
		lower_bound = 5;

	}

	*max_empty_cells = SUDOKU_CELLS - givens;
	*upper_bound = SUDOKU_ROW - lower_bound;
}

static bool can_digg(char *in, int cell, int upper_bound)
{
	int empty_cells_row = 0,
		empty_cells_col = 0, i,
		row = GET_SUDOKU_ROW_START(cell),
		col = GET_SUDOKU_COL_START(cell);

	for (i = 0; i < SUDOKU_ROW; i++)
	{
		if (in[row] == 0)
			empty_cells_row++;

		if (in[col] == 0)
			empty_cells_col++;

		row++;
		col += SUDOKU_ROW;
	}

	return (empty_cells_row < upper_bound && empty_cells_col < upper_bound);
}

static void digg_holes(char *in, int difficulty)
{
	vector<int> order;
	int empty_cells = 0,
		max_empty_cells = 0,
		upper_bound = 0,
		solutions = 0;
	char tmp[SUDOKU_CELLS];
	char prev_value;


	set_difficulty(difficulty, &order, &max_empty_cells, &upper_bound);

	for (vector<int>::iterator it = order.begin(); it != order.end(); ++it)
	{
		if (!can_digg(in, *it, upper_bound))
			continue;

		prev_value = in[*it];

		in[*it] = 0;

		solutions = 0;
		memcpy(tmp, in, SUDOKU_CELLS);

		backtrack_solve(tmp, 0, &solutions);

		if (solutions == 1)
			empty_cells++;
		else
			in[*it] = prev_value;

		if (empty_cells >= max_empty_cells)
			break;
	}

	cout << "Max: " << max_empty_cells << " Empty: " << empty_cells << " Givens: " << (SUDOKU_CELLS - empty_cells) << endl;

}


static void shuffle_grid(char *in, char *out)
{
	int s_row[SUDOKU_ROW], n_s_row[SUDOKU_ROW],
		s_col[SUDOKU_ROW], n_s_col[SUDOKU_ROW],
		inc = 0, val_r,val_c, tmp;

	vector<int> row, col;
	row.push_back(0);
	row.push_back(1);
	row.push_back(2);

	col.push_back(0);
	col.push_back(1);
	col.push_back(2);

	random_shuffle(row.begin(), row.end());
	random_shuffle(col.begin(), col.end());

	for (int i = 0; i < SUDOKU_ROW; i++)
	{
		if (i % SUDOKU_BOX == 0)
		{
			val_r = SUDOKU_BOX * row[inc];
			val_c = SUDOKU_BOX * col[inc];
			inc++;
		}

		s_row[i] = val_r++;
		s_col[i] = val_c++;
	}

	for (int i = 0; i < SUDOKU_ROW; i++)
	{
		if (i % SUDOKU_BOX == 0)
		{
			random_shuffle(row.begin(), row.end());
			random_shuffle(row.begin(), row.end());

			random_shuffle(col.begin(), col.end());
			random_shuffle(col.begin(), col.end());

			tmp = SUDOKU_BOX * (int)(i / SUDOKU_BOX);
		}

		n_s_row[i] = s_row[tmp + row[i%SUDOKU_BOX]];
		n_s_col[i] = s_col[tmp + col[i%SUDOKU_BOX]];
	}

	for (int i = 0; i < SUDOKU_CELLS; i++)
	{
		val_r = (int)(i / SUDOKU_ROW);
		val_c = GET_SUDOKU_COL_START(i);
		
		tmp = (n_s_row[val_r] * SUDOKU_ROW) + n_s_col[val_c];
		out[i] = in[tmp];
	}

}




/******************************************************************************
*	Main
******************************************************************************/


int main(void)
{
	srand(unsigned(time(NULL)));

	char in[SUDOKU_CELLS],
		 shuffle[SUDOKU_CELLS],
		 solve[SUDOKU_CELLS];

	int solutions = 0;

	memset(in, 0, SUDOKU_CELLS);

	//set_cell(1, 2, 5, in);
	//set_cell(1, 3, 2, in);
	//set_cell(1, 6, 6, in);

	//set_cell(2, 1, 1, in);
	//set_cell(2, 2, 6, in);
	//set_cell(2, 4, 9, in);
	//set_cell(2, 9, 4, in);

	//set_cell(3, 2, 4, in);
	//set_cell(3, 3, 9, in);
	//set_cell(3, 4, 8, in);
	//set_cell(3, 6, 3, in);
	//set_cell(3, 7, 6, in);
	//set_cell(3, 8, 2, in);

	//set_cell(4, 1, 4, in);
	//set_cell(4, 7, 8, in);

	//set_cell(5, 2, 8, in);
	//set_cell(5, 3, 3, in);
	//set_cell(5, 4, 2, in);
	//set_cell(5, 6, 1, in);
	//set_cell(5, 7, 5, in);
	//set_cell(5, 8, 9, in);

	//set_cell(6, 3, 1, in);
	//set_cell(6, 9, 2, in);

	//set_cell(7, 2, 9, in);
	//set_cell(7, 3, 7, in);
	//set_cell(7, 4, 3, in);
	//set_cell(7, 6, 5, in);
	//set_cell(7, 7, 2, in);
	//set_cell(7, 8, 4, in);

	//set_cell(8, 1, 2, in);
	//set_cell(8, 6, 9, in);
	//set_cell(8, 8, 5, in);
	//set_cell(8, 9, 6, in);

	//set_cell(9, 4, 1, in);
	//set_cell(9, 7, 9, in);
	//set_cell(9, 8, 7, in);

	vector<int> order;
	gen_s_sequence(&order);

	while (!las_vegas(in));

	digg_holes(in, 4);
	shuffle_grid(in, shuffle);
	print_sol(shuffle, NULL);

	memcpy(solve, shuffle, SUDOKU_CELLS);

	backtrack_solve(solve);
	print_sol(shuffle, solve);

	getchar();

}