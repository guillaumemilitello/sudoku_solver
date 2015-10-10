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

const unsigned int p_search_l_max = 4;

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
	unsigned int p_search_l;

public:
	Sudoku();
	Sudoku(int v[9][9]);
	Sudoku(Sudoku &other);

	virtual ~Sudoku(){};

	const void initGrid();
	const void setGrid(const int v[9][9]);

	const bool isSolved() const;
	const bool isSolved(int x, int y) const;
	const void itemSolved();

	const int  getValue(int x, int y) const;
	const void setValue(int x, int y, int v);

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

	const bool solve();
	const bool recursiveSolve();
	const bool solveValue(unsigned int p_nb, unsigned int p_it);

	const bool checkGrid() const;

	friend ostream& operator<<(ostream& os, const Sudoku& su);

};

#endif /* SUDOKU_H_ */
