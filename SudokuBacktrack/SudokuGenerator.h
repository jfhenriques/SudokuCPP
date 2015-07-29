
#ifndef SUDOKU_GENERATOR_H_
#define SUDOKU_GENERATOR_H_

#include <vector>



namespace Sudoku {


	typedef void(*sort_difficulty_func)(std::vector<int>*);

	typedef struct DifficultyConfig
	{
		sort_difficulty_func sort_func = NULL;
		int max_empty_cells = 0;
		int upper_bound = 0;
		int givens = 0;
	} DifficultyConfig;

	typedef enum {
		EXTREMELY_EASY = 0,
		EASY,
		MEDIUM,
		DIFFICULT,
		EVIL
	} SudokuDifficulty;



	void gen_sequence(std::vector<int> *vect);
	void gen_random_sequence(std::vector<int> *vect);
	void gen_s_sequence(std::vector<int> *vect);

	void config_difficulty(SudokuDifficulty difficulty, DifficultyConfig &config);



	class Generator {
	private:
		const int MAX_BACKTRACKS;
		int backtracks = 0;
		Board board;
		std::vector<char> board_poss[SUDOKU_CELLS];

		bool backtrack_one_solution(int position, bool *solution);
		void fill_possible_solutions(void);
		bool can_digg(int cell, int upper_bound);


	public:
		Generator(int maxBacktracks = SUDOKU_MAX_BACKTRACKS);
		~Generator(void) {};

		void CopyToBoard(Board &b);
		void CopyFromBoard(Board &b);

		bool LasVegas(const int n = 11);
		void DigHoles(DifficultyConfig &difficulty);
		
	};
}


#endif /* SUDOKU_GENERATOR_H_ */