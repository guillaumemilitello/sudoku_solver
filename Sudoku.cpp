/*
 * Sudoku.cpp
 *
 *  Created on: 10 Oct 2015
 *      Author: guillaume
 */

#include <Sudoku.h>

Sudoku::Sudoku() : unsolved(0), p_search_l(1)
{
	initGrid();
}

Sudoku::Sudoku(int v[9][9]) : unsolved(0), p_search_l(1)
{
	setGrid(v);
}

Sudoku::Sudoku(Sudoku &other) : unsolved(other.unsolved), p_search_l(other.p_search_l)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
		    p_square[i][j] = other.p_square[i][j];
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
		    p_line[i][j] = other.p_line[i][j];
		}
	}
	for (int x = 0; x < 9; ++x)
	{
		for (int y = 0; y < 9; ++y)
		{
			setValue(x, y, other.grid[x][y].value);
			grid[x][y].p_values = other.grid[x][y].p_values;
		}
	}
}

const void Sudoku::initGrid()
{
	for (int i = 0; i < 9; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			setValue(i, j, 0);
		}
	}
}

const void Sudoku::setGrid(const int v[9][9])
{
	for (int x = 0; x < 9; ++x)
	{
		for (int y = 0; y < 9; ++y)
		{
			setValue(x, y, v[x][y]);
		}
	}

	// fill shared squares and lines probabilities
	fillSharedPossibilities();

	// fill possibilities for each grid position
	fillPossibilities();
}

const bool Sudoku::isSolved() const
{
	return unsolved == 0;
}

const bool Sudoku::isSolved(int x, int y) const
{
	return grid[x][y].value != 0;
}

const void Sudoku::itemSolved()
{
	unsolved--;
}

const int Sudoku::getValue(int x, int y) const
{
	return grid[x][y].value;
}

const void Sudoku::setValue(int x, int y, int v)
{
	grid[x][y].value = v;
}

const bool Sudoku::isPossibility(int x, int y, int v) const
{
	return p_square[x / 3][y / 3].find(v) == p_square[x / 3][y / 3].end() && p_line[0][x].find(v) == p_line[0][x].end()
			&& p_line[1][y].find(v) == p_line[1][y].end();
}

const int Sudoku::getPossibility(int x, int y, unsigned int it) const
{
	set<int>::iterator pit = grid[x][y].p_values.begin();
	for (unsigned int i = 0; i < it; ++i)
		++pit;
	return *pit;
}

const void Sudoku::insertPossibility(int x, int y, int v)
{
	grid[x][y].p_values.insert(v);
}

const bool Sudoku::removePossibility(int x, int y, int v)
{
	grid[x][y].p_values.erase(v);
	return grid[x][y].p_values.empty();
}

const bool Sudoku::numberPossibilities(int x, int y, unsigned int n) const
{
	return grid[x][y].p_values.size() == n;
}

const bool Sudoku::checkInSquare(int x, int y, int v)
{
	int _x = x / 3 * 3;
	int _y = y / 3 * 3;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			// don't check the value itself
			if (_x + i == x && _y + j == y)
			{
				continue;
			}
			// check if the value exist
			if (getValue(_x + i, _y + j) == v)
			{
				return false;
			}
			// check if there is a unique possibility problem
			else if (getValue(_x + i, _y + j) == 0 && numberPossibilities(_x + i, _y + j, 1)
					&& getPossibility(_x + i, _y + j, 0) == v)
			{
				return false;
			}
		}
	}
	return true;
}
const bool Sudoku::checkInLines(int x, int y, int v)
{
	for (int i = 0; i < 9; ++i)
	{
		// check if the value exist
		if (getValue(x, i) == v || getValue(i, y) == v)
		{
			return false;
		}
		// check if there is a unique possibility problem
		else if ((i != y && getValue(x, i) == 0 && numberPossibilities(x, i, 1) && getPossibility(x, i, 0) == v)
			  || (i != x && getValue(i, y) == 0 && numberPossibilities(i, y, 1) && getPossibility(i, y, 0) == v))
		{
			return false;
		}
	}
	return true;
}

const void Sudoku::removeInSquare(int x, int y, int v)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			grid[x / 3 * 3 + i][y / 3 * 3 + j].p_values.erase(v);
		}
	}
}

const void Sudoku::removeInLines(int x, int y, int v)
{
	for (int i = 0; i < 9; ++i)
	{
		grid[x][i].p_values.erase(v);
		grid[i][y].p_values.erase(v);
	}
}
const bool Sudoku::emptyPossibilities() const
{
	for (int j = 0; j < 9; ++j)
	{
		for (int i = 0; i < 9; ++i)
		{
			if (grid[i][j].value == 0 && grid[i][j].p_values.empty())
			{
				return true;
			}
		}
	}
	return false;
}

const void Sudoku::printPossibilities() const
{
	for (int j = 0; j < 9; ++j)
	{
		for (int i = 0; i < 9; ++i)
		{
			if (getValue(i, j) != 0)
			{
				cout << getValue(i, j) << "--\t";
			}
			else
			{
				for (auto g : grid[i][j].p_values)
				{
					cout << g;
				}
				cout << "\t";
			}
		}
		cout << endl;
	}
	cout << endl;
}

const void Sudoku::fillSharedPossibilities()
{
	for(int y=0; y<9; ++y)
	{
		for(int x=0; x<9; ++x)
		{
			int value = getValue(x, y);

			if(value != 0)
			{
				p_square[x/3][y/3].insert(value);
				p_line[0][x].insert(value);
				p_line[1][y].insert(value);
			}
			else
			{
				unsolved++;
			}
		}
	}
}

const void Sudoku::fillPossibilities()
{
	for(int y=0; y<9; ++y)
	{
		for(int x=0; x<9; ++x)
		{
			if(!isSolved(x, y))
			{
				for(int v = 1; v <= 9; ++v)
				{
					if(isPossibility(x, y, v))
					{
						insertPossibility(x, y, v);
					}
				}
			}
		}
	}
}

const bool Sudoku::solve()
{
	// solve the gird by increasing the possibilities search limit
	while(++p_search_l < p_search_l_max)
	{
		// use a recursive solving solution
		if(recursiveSolve())
		{
			// final state error check, just in case
			if(checkGrid())
			{
				return true;
			}
			else
			{
				cout << "errors in the solution" << endl;
				return false;
			}
		}
	}

	// sudoku unsolved : error while finding solutions
	if(!isSolved())
	{
		cout << "sudoku unsolvable" << endl;
		return false;
	}
	else
	{
		return true;
	}
}

const bool Sudoku::recursiveSolve()
{
	// sudoku solved
	if(isSolved())
	{
		return true;
	}
	else
	{
		// solving unique value possibilities
		// check is the whole gird still got possibilities
		// run again the process untill the sudoku is solved
		if(solveValue(1, 0) && !emptyPossibilities() && recursiveSolve())
		{
			return true;
		}
		else
		{
			// some position in the gird does not have a possibility
			if(emptyPossibilities())
			{
				// error detected : exit branch
				return false;
			}

			// save the context before taking different branches
			Sudoku su_t = *this;

			for(unsigned int l = 2; l <= p_search_l; ++l)
			{
				// iterator through the possibilities
				for(unsigned int it=0; it<l; ++it)
				{
					if(su_t.solveValue(l, it) && !su_t.emptyPossibilities() && su_t.recursiveSolve())
					{
						// branch solution solved, returning the result
						*this = su_t;
						return true;
					}
					else
					{
						// branch unsolvable, getting back to the previous context
						su_t = *this;
					}
				}
			}
			return false;
		}
	}
}

const bool Sudoku::solveValue(unsigned int p_nb, unsigned int p_it)
{
	int solved = 0;

	for(int y=0; y<9; ++y)
	{
		for(int x=0; x<9; ++x)
		{
			// try to solve the value depending on the number of possibilities
			if(!isSolved(x, y) && numberPossibilities(x, y, p_nb))
			{
				int v = getPossibility(x, y, p_it);

				// check the viability of the solution
				if(checkInSquare(x, y, v) && checkInLines(x, y, v))
				{
					setValue(x, y, v);
					removeInSquare(x, y, v);
					removeInLines(x, y, v);
					itemSolved();
					solved++;

					// if it is not a unique solution, exit the loop and
					// solved solution with unique possibility
					if(p_nb > 1)
					{
						return true;
					}
				}
				else
				{
					// possibility unsolvable
					removePossibility(x, y, v);
				}
			}
		}
	}
	return solved > 0;
}

const bool Sudoku::checkGrid() const
{
	set<int> squares[3][3];
	set<int> lines[2][9];

	for(int x=0; x<9; ++x)
	{
		for(int y=0; y<9; ++y)
		{
			int value = getValue(y, x);

			if(value != 0)
			{
				if(squares[x/3][y/3].find(value) == squares[x/3][y/3].end())
				{
					squares[x/3][y/3].insert(value);
				}
				else
				{
					return false;
				}

				if(lines[0][y].find(value) == lines[0][y].end())
				{
					lines[0][y].insert(value);
				}
				else
				{
					return false;
				}

				if(lines[1][x].find(value) == lines[1][x].end())
				{
					lines[1][x].insert(value);
				}
				else
				{
					return false;
				}
			}
		}
	}
	return true;
}

ostream& operator<<(ostream& os, const Sudoku& su)
{
	for(int y=0; y<9; ++y)
	{
		for(int x=0; x<9; ++x)
		{
			cout << su.grid[x][y].value << " ";
		}
		cout << endl;
	}
	return os;
}

