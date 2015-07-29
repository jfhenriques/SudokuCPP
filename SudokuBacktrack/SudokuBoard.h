
#ifndef SUDOKU_BOARD_H_
#define SUDOKU_BOARD_H_

#include <vector>

#define SUDOKU_CELLS		81
#define SUDOKU_ROW			9
#define SUDOKU_BOX			3
#define SUDOKU_BOX_BLOCK	27 // 9*3

#define SUDOKU_MAX_BACKTRACKS		1000


#define SUDOKU_CELL_GET_ROW_START(c)			(c - ( c % SUDOKU_ROW))
#define SUDOKU_CELL_GET_COL_START(c)			(c % SUDOKU_ROW)

#define SUDOKU_GET_BOX_START_OFFSET(r,c)		( (SUDOKU_BOX_BLOCK * (int)((r) / SUDOKU_BOX)) + \
												  (SUDOKU_BOX * (int)((c) / SUDOKU_BOX)) )

#define SUDOKU_CELL_CONVERT_ROW(c)				((int)(c/SUDOKU_ROW))
#define SUDOKU_CELL_CONVERT_COL(c)				(c % SUDOKU_ROW)

#define SUDOKU_ROW_CONVERT_CELL(r)				((int)(r) * SUDOKU_ROW)


namespace Sudoku {


	class Board {
	private:
		char _board[SUDOKU_CELLS];

	public:

		Board(void);
		Board(const Board& board);
		Board(char &board);
		~Board(void) {};

		char* GetMemory(void);
		void SetMemory(const char *memory);

		bool CheckRow(int r, int v);
		bool CheckCol(int c, int v);
		bool CheckBox(int r, int c, int v);

		bool CheckRules(int r, int c, int v);


		bool CheckRowCell(int cell, int v);
		bool CheckColCell(int cell, int v);
		bool CheckBoxCell(int cell, int v);

		bool CheckRulesCell(int cell, int v);


		void SetCell(int r, int c, int v);
		void SetCell(int cell, int v);

		char GetCell(int cell);
		char &operator[](int cell);

		void Shuffle(void);
		
		bool Backtrack(const int c = 0, int *solutions = NULL);
	};

}



#endif /* SUDOKU_BOARD_H_ */