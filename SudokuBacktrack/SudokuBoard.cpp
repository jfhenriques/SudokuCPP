

#include "SudokuBoard.h"

#include <string.h>
#include <vector>
#include <algorithm>

using namespace std;

namespace Sudoku
{


	/******************************************************************************
	*	Board:: Constructors
	******************************************************************************/

	Board::Board(void)
	{
		memset(this->_board, 0, SUDOKU_CELLS);
	}

	Board::Board(char &board)
	{
		memcpy(this->_board, &board, SUDOKU_CELLS);
	}

	Board::Board(const Board& board)
	{
		this->SetMemory(board._board);

	}




	/******************************************************************************
	*	Board:: Checks
	******************************************************************************/


	bool Board::CheckRow(int r, int v)
	{
		char row = SUDOKU_ROW_CONVERT_CELL(r - 1);

		for (int i = 0; i < SUDOKU_ROW; i++)
		{
			if (this->_board[row + i] == v)
				return false;
		}

		return true;
	}

	bool Board::CheckCol(int c, int v)
	{
		c -= 1;

		for (int i = 0; i < SUDOKU_ROW; i++)
		{
			if (this->_board[c] == v)
				return false;

			c += SUDOKU_ROW;
		}

		return true;
	}



	bool Board::CheckBox(int r, int c, int v)
	{
		int base, t1;

		base = SUDOKU_GET_BOX_START_OFFSET(r - 1, c - 1);

		for (int i = 0; i < SUDOKU_ROW; i++)
		{
			t1 = i % SUDOKU_BOX;

			if (t1 == 0 && i >= SUDOKU_BOX)
				base += SUDOKU_ROW;

			if (this->_board[base + t1] == v)
				return false;

		}

		return true;
	}

	bool Board::CheckRules(int r, int c, int v)
	{
		return CheckRow(r, v) && CheckCol(c, v) && CheckBox(r, c, v);
	}




	bool Board::CheckRowCell(int cell, int v)
	{
		char val = SUDOKU_CELL_GET_ROW_START(cell);

		for (int i = 0; i < SUDOKU_ROW; i++)
		{
			if (this->_board[val] == v)
				return false;

			val += 1;
		}

		return true;
	}
	bool Board::CheckColCell(int cell, int v)
	{
		char val = SUDOKU_CELL_GET_COL_START(cell);

		for (int i = 0; i < SUDOKU_ROW; i++)
		{
			if (this->_board[val] == v)
				return false;

			val += SUDOKU_ROW;
		}

		return true;
	}


	bool Board::CheckBoxCell(int cell, int v)
	{
		int base, t1;

		base = SUDOKU_GET_BOX_START_OFFSET(
					SUDOKU_CELL_CONVERT_ROW(cell),
					SUDOKU_CELL_CONVERT_COL(cell));

		for (int i = 0; i < SUDOKU_ROW; i++)
		{
			t1 = i % SUDOKU_BOX;

			if (t1 == 0 && i >= SUDOKU_BOX)
				base += SUDOKU_ROW;

			if (this->_board[base + t1] == v)
				return false;

		}

		return true;
	}

	bool Board::CheckRulesCell(int cell, int v)
	{
		return CheckRowCell(cell, v) && CheckColCell(cell, v) && CheckBoxCell(cell, v);
	}



	/******************************************************************************
	*	Board:: Others
	******************************************************************************/


	char* Board::GetMemory(void)
	{
		return this->_board;
	}

	void Board::SetMemory(const char *memory)
	{
		memcpy(this->_board, memory, SUDOKU_CELLS);
	}




	bool Board::Backtrack(const int c, int *solutions)
	{
		if (c >= SUDOKU_CELLS)
		{
			if (solutions == NULL)
				return true;

			(*solutions)++;

			return (*solutions > 1); // break at two;
		}

		if (_board[c] != 0)
			return Backtrack(c + 1, solutions);

		else
		{
			for (int i = 1; i <= SUDOKU_ROW; i++)
			{
				if (CheckRulesCell(c, i))
				{
					_board[c] = i;

					if (Backtrack(c + 1, solutions))
						return true;

					_board[c] = 0;
				}
			}
		}

		return false;
	}




	void Board::SetCell(int r, int c, int v)
	{
		this->_board[SUDOKU_ROW_CONVERT_CELL(r - 1) + (c - 1)] = (char)v;
	}
	void Board::SetCell(int cell, int v)
	{
		this->_board[cell] = (char)v;
	}



	char Board::GetCell(int cell)
	{
		return this->_board[cell];
	}

	char &Board::operator[](int cell)
	{
		return this->_board[cell];
	}


	void Board::Shuffle(void)
	{
		char tmp_board[SUDOKU_CELLS];
		int s_row[SUDOKU_ROW], n_s_row[SUDOKU_ROW],
			s_col[SUDOKU_ROW], n_s_col[SUDOKU_ROW],
			inc = 0, val_r, val_c, tmp;

		vector<int> row{ 0, 1, 2 },
					col{ 0, 1, 2 };

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

			n_s_row[i] = s_row[tmp + row[i % SUDOKU_BOX]];
			n_s_col[i] = s_col[tmp + col[i % SUDOKU_BOX]];
		}

		for (int i = 0; i < SUDOKU_CELLS; i++)
		{
			val_r = SUDOKU_CELL_CONVERT_ROW(i);
			val_c = SUDOKU_CELL_GET_COL_START(i);

			tmp = SUDOKU_ROW_CONVERT_CELL(n_s_row[val_r]) + n_s_col[val_c];

			tmp_board[i] = this->_board[tmp];
		}

		memcpy(this->_board, tmp_board, SUDOKU_CELLS);
	}

}