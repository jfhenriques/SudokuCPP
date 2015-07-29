

#include "SudokuBoard.h"
#include "SudokuGenerator.h"

#include <vector>
#include <algorithm>


using namespace std;

namespace Sudoku {

	/******************************************************************************
	*	sequences
	******************************************************************************/

	void gen_sequence(vector<int> *vect)
	{
		vect->clear();
		for (int i = 0; i < SUDOKU_CELLS; i++)
			vect->push_back(i);
	}

	void gen_random_sequence(vector<int> *vect)
	{
		gen_sequence(vect);
		random_shuffle(vect->begin(), vect->end());
	}

	void gen_s_sequence(vector<int> *vect)
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



	/******************************************************************************
	*	difficulty
	******************************************************************************/

	void config_difficulty(SudokuDifficulty difficulty, DifficultyConfig &config)
	{
		int givens,
			lower_bound;

		switch (difficulty)
		{

		// Extremely Easy
		default:
		case SudokuDifficulty::EXTREMELY_EASY:
			config.sort_func = gen_random_sequence;
			givens = 50;
			lower_bound = 5;
			
			break;

		// Easy
		case SudokuDifficulty::EASY:
			config.sort_func = gen_random_sequence;
			givens = 36;
			lower_bound = 4;

			break;

		// Medium
		case SudokuDifficulty::MEDIUM:
			config.sort_func = gen_random_sequence;
			givens = 32;
			lower_bound = 3;

			break;

		//Difficult
		case SudokuDifficulty::DIFFICULT:
			config.sort_func = gen_s_sequence;
			givens = 28;
			lower_bound = 2;

			break;

		//Evil
		case SudokuDifficulty::EVIL:
			config.sort_func = gen_sequence;
			givens = 22;
			lower_bound = 0;

			break;
		}

		config.max_empty_cells = SUDOKU_CELLS - givens;
		config.upper_bound = SUDOKU_ROW - lower_bound;
	}






	/******************************************************************************
	*	Generator:: Constructors
	******************************************************************************/

	Generator::Generator(int maxBacktracks)
		: MAX_BACKTRACKS(maxBacktracks)
	{
	}




	/******************************************************************************
	*	Generator:: Helpers
	******************************************************************************/

	void Generator::fill_possible_solutions(void)
	{
		int cell;
		char v;

		for (cell = 0; cell < SUDOKU_CELLS; cell++)
		{
			board_poss[cell].clear();

			if (board[cell] == 0)
			{
				for (v = 1; v <= SUDOKU_ROW; v++)
				{
					if (board.CheckRulesCell(cell, v))
						board_poss[cell].push_back(v);
				}

				if (board_poss[cell].size() > 1)
					random_shuffle(board_poss[cell].begin(), board_poss[cell].end());
			}
		}
	}

	bool Generator::backtrack_one_solution(int c, bool *solution)
	{
		if (c >= SUDOKU_CELLS)
		{
			(*solution) = true;
			return true;
		}

		if (board[c] != 0)
			return backtrack_one_solution(c + 1, solution);

		else
		{
			for (vector<char>::iterator it = board_poss[c].begin(); it != board_poss[c].end(); ++it)
			{
				if (board.CheckRulesCell(c, *it))
				{
					board[c] = *it;

					if (backtrack_one_solution(c + 1, solution))
						return true;

					if ((backtracks)++ >= this->MAX_BACKTRACKS)
						return true;

					board[c] = 0;
				}
			}
		}

		return false;
	}




	bool Generator::can_digg(int cell, int upper_bound)
	{
		int empty_cells_row = 0,
			empty_cells_col = 0,
			i,
			row = SUDOKU_CELL_GET_ROW_START(cell),
			col = SUDOKU_CELL_GET_COL_START(cell);

		for (i = 0; i < SUDOKU_ROW; i++)
		{
			if (board[row] == 0)
				empty_cells_row++;

			if (board[col] == 0)
				empty_cells_col++;

			row++;
			col += SUDOKU_ROW;
		}

		return (empty_cells_row < upper_bound && empty_cells_col < upper_bound);
	}





	/******************************************************************************
	*	Generator:: functions
	******************************************************************************/


	void Generator::CopyToBoard(Board &b)
	{
		b.SetMemory(this->board.GetMemory());
	}
	void Generator::CopyFromBoard(Board &b)
	{
		this->board.SetMemory(b.GetMemory());
	}



	void Generator::DigHoles(DifficultyConfig &difficulty)
	{
		vector<int> order;
		int empty_cells = 0,
			solutions = 0;
		Board tmp;

		difficulty.sort_func(&order);

		for (vector<int>::iterator it = order.begin(); it != order.end(); ++it)
		{
			if (!can_digg(*it, difficulty.upper_bound))
				continue;

			solutions = 0;

			tmp.SetMemory(board.GetMemory());

			tmp[*it] = 0;
			tmp.Backtrack(0, &solutions);

			if (solutions == 1)
			{
				board[*it] = 0;
				empty_cells++;
			}

			if (empty_cells >= difficulty.max_empty_cells)
				break;
		}

		difficulty.givens = SUDOKU_CELLS - empty_cells;
	}


	bool Generator::LasVegas(const int n)
	{
		int r, cell;
		bool solution = false;

		this->backtracks = 0;

		memset(board.GetMemory(), 0, SUDOKU_CELLS);

		for (int i = 0; i < n; i++)
		{

			do {
				cell = rand() % SUDOKU_CELLS;
			} while (board[cell] != 0);

			do
			{
				r = 1 + (rand() % SUDOKU_ROW);

				if (board.CheckRulesCell(cell, r))
				{
					board[cell] = r;
					break;
				}

			} while (true);
		}

		this->fill_possible_solutions();
		return this->backtrack_one_solution(0, &solution) && solution;
	}




}