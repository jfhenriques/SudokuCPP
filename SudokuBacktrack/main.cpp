

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <ctime>

#include "SudokuBoard.h"
#include "SudokuGenerator.h"


using namespace std;
using namespace Sudoku;








/******************************************************************************
*	Helpers
******************************************************************************/


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
*	Main
******************************************************************************/


int main(void)
{
	srand(unsigned(time(NULL)));

	int solutions = 0;
	Board puzzle, solve;
	Generator puzzleGenerator;
	DifficultyConfig difConfig;


	// Create difficulty configuration
	config_difficulty(SudokuDifficulty::EASY, difConfig);

	// Randomize a terminal board
	while (!puzzleGenerator.LasVegas());

	// Dig holes in the board, according with the chosen difficulty
	puzzleGenerator.DigHoles(difConfig);

	// Copy board from the generator
	puzzleGenerator.CopyToBoard(puzzle);

	// Shuffle board, keeping all sudoku rules
	puzzle.Shuffle();

	print_sol(puzzle.GetMemory(), NULL);
	cout << "Max givens: " << (SUDOKU_CELLS - difConfig.max_empty_cells) << " Givens: " << difConfig.givens << endl;

	// Copy board from memory
	solve.SetMemory(puzzle.GetMemory());

	// Backtrack possible solutions (stops if more than one is found)
	solve.Backtrack(0, &solutions);

	cout << "Solutions: " << solutions << endl;

	if (solutions == 1)
	{
		solve.SetMemory(puzzle.GetMemory());

		// Backtrack until first solution is found
		solve.Backtrack();

		print_sol(puzzle.GetMemory(), solve.GetMemory());
	}


	getchar();

}