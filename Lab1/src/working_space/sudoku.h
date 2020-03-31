#ifndef SUDOKU_H
#define SUDOKU_H

const bool DEBUG_MODE = false;
enum { ROW=9, COL=9, N = 81, NEIGHBOR = 20 };
const int NUM = 9;

extern int neighbors[N][NEIGHBOR];
extern int board[N];
extern int spaces[N];
extern int nspaces;
extern int (*chess)[COL];
struct job_t{
	int puzzleNo;	//题目在输入文件中的编号  在第n行就是n
	int board[81]; //题目
};

void init_neighbors();
void input(const char in[N]);
void init_cache();

bool available(int guess, int cell);

bool solve_sudoku_basic(int which_space);
bool solve_sudoku_min_arity(int which_space);
bool solve_sudoku_min_arity_cache(int which_space);
bool solve_sudoku_dancing_links(int unused);
//bool solve_sudoku_dancing_links_2(int unused, job_t job);
bool solved();
#endif
