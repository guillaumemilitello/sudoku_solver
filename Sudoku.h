/*
 * Sudoku.h
 *
 *  Created on: 10 Oct 2015
 *      Author: guillaume
 */

#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <iostream>
#include <string>
#include <cstdlib>
#include <set>

using namespace std;

const unsigned int p_l_max = 4;

struct sudoku_grid
{
	int value;
	set<int> p_values;
};

class Sudoku
{

private:
	unsigned int unsolved;
	sudoku_grid grid[9][9];
	set<int> p_square[3][3];
	set<int> p_line[2][9];

public:
	Sudoku();
	Sudoku(int v[9][9]);
	Sudoku(Sudoku &other);

	virtual ~Sudoku(){};

	const void setGrid(const int v[9][9]);
	const void getGrid(int v[9][9]) const;
	const bool checkGrid() const;

	const bool isSolved() const;
	const bool isSolved(int x, int y) const;

	const int  getValue(int x, int y) const;

private:
	const void initGrid();
	const void setValue(int x, int y, int v);

	const void itemSolved();

	const bool isPossibility(int x, int y, int v) const;
	const int  getPossibility(int x, int y, unsigned int it) const;
	const void insertPossibility(int x, int y, int v);
	const bool removePossibility(int x, int y, int v);
	const bool numberPossibilities(int x, int y, unsigned int n) const;

	const bool checkInSquare(int x, int y, int v);
	const bool checkInLines(int x, int y, int v);
	const void removeInSquare(int x, int y, int v);
	const void removeInLines(int x, int y, int v);

	const bool emptyPossibilities() const;
	const void printPossibilities() const;

	const void fillPossibilities();
	const void fillSharedPossibilities();

	const bool solveValue(unsigned int p_nb, unsigned int p_it);

	const bool recursiveSolve(unsigned int p_l);

	friend ostream& operator<<(ostream& os, const Sudoku& su);

	friend const bool sudokuSolve(Sudoku& su);
};

#endif /* SUDOKU_H_ */
