// =======================================================================
// cells.h
// =======================================================================
// base class used for further cellular-atonoma related programs for
// use as a pseudorandom process that can be controlled in an embedded
// interactive music environment.
// Subsequent classes that inherit from cell (like conway's game of life)

// basic cell that stores a value and its respective row and column
// indexes.
template <typename T>
class cell {
public:
	cell() : x(0), y(0), val(nullptr);
	cell(int _x, int _y) : x(_x), y(_y);
	cell(int _x, int _y, int _v) : x(_x), y(_y), val(_v);

	int get_x() {return x;}
	int get_y() {return y;}
	T get_val() {return val;}
	void set_x(int x) {this->x = x;}
	void set_y(int y) {this->y = y;}
	void set_coords(int _x, int _y) {
		set_x(_x);
		set_y(_y);
	}
	void set_val(T val) {this->val = val;}
protected:
      	T val;
	int x;
	int y;
};

// base class, grid object container.
template <typename T>
class grid {
public:
	grid(int r, int c) : rows(r), cols(c) {
		grid.resize(rows, std::vector<T>(cols));
	}
	grid() : rows(10), cols(10) {
		grid.resize(rows, std::vector<T>(cols));
	}

	void initialize() {
		init_cells();
	}
public:
	void set_at(int _r, int _c, T v) {
		int r = _r;
		int c = _c;
		constrain_bounds(&r, &c);
		grid[r][c] = v;
	}
	T get_at(int _r, int _c) {
		int r = _r;
		int c = _c;
		constrain_bounds(&r, &c);
		return grid[r][c];
	}

	void set_north(int _r, int _c, T v) {set_at(_r - 1, _c, v);}
	void set_south(int _r, int _c, T v) {set_at(_r + 1, _c, v);}
	void set_east(int _r, int _c, T v) {set_at(_r, _c + 1, v);}
	void set_west(int _r, int _c, T v) {set_at(_r, _c - 1, v);}

	void set_northwest(int _r, int _c, T v){set_at(_r - 1, c - 1, v);}
	void set_northeast(int _r, int _c, T v){set_at(_r - 1, c + 1, v);}
	void set_southwest(int _r, int _c, T v){set_at(_r + 1, _c - 1, v);}
	void set_southeast(int _r, int _c, T v){set_at(_r + 1, _c + 1, v);}

	T get_north(int _r, int _c) {return get_at(_r - 1, _c);}
	T get_south(int _r, int _c) {return get_at(_r + 1, _c);}
	T get_east(int _r, int _c) {return get_at(_r, _c + 1);}
	T get_west(int _r, int _c) {return get_at(_r, _c - 1);}

	T get_northwest(int _r, int _c){return get_at(_r - 1, c - 1);}
	T get_northeast(int _r, int _c){return get_at(_r - 1, c + 1);}
	T get_southwest(int _r, int _c){return get_at(_r + 1, _c - 1);}
	T get_southeast(int _r, int _c){return get_at(_r + 1, _c + 1);}

	void constrain_bounds(int* r, int* c) {
		if(*r >= rows) *r = rows;
		if(*r < 0) *r = 0;
		if(*c >= cols) *c = cols;
		if(*c < 0) *c = 0;
	}

protected:
	int rows;
	int cols;
	std::vector<std::vector<T>> grid;
	virtual void init_cells() = 0;
}

// specialized cell for Conway's Game of Life implementation that
// makes things a bit more straightforward.
template <typename T>
class ca_cell : public cell<T>
{
public:
	ca_cell(int x, int y, int v) : cell(x, y, v), alive(false);
	ca_cell(int x, int y) : cell(x, y), alive(false);
	ca_cell() : cell(), alive(false);

	bool is_alive(int threshold) {
		return val > threshold;
	}
};


// data simulation based on Conway's Game of Life
// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life#Algorithms
// no visual aspect, just a data-based one simulated on a 2D array.
template <typename T>
class ca_grid : public grid<ca_cell<T>>
{
public:
	ca_grid(int r, int c, int u, int o, T t) : 
		grid(r, c), u_thresh(u), o_thresh(o), health(t) {
	}

	ca_grid(int u, int o, T t) : grid(), u_thresh(u),
		o_thresh(o), health(t);

	// we initialize this grid to parameters for Conway's Game
	// of Life by default.
	ca_grid(int r, int c) : grid(r, c), u_thresh(2),
		o_thresh(3), health(nullptr);
	ca_grid() : grid(), u_thresh(2), o_thresh(3),
		health(nullptr);
	// reminder that you still need to implement the init_cells()
	// routine specified by parent class grid<T>!
public:
	// generate one iteration of the matrix
	virtual void next();

	// wipe the entire board
	virtual void reset();

	// generate a random seed to start
	virtual void generate(int seed);

private:
	int u_thresh; // underpopulation threshold
	int o_thresh; // overpopulation threshold
	T health; 	// threshold of life / death
	int[8] row_offset = {-1, 1, 0, 0, -1, -1, 0, 0}; // for peksy
	int[8] col_offset = {0, 0, 1, -1, 1, -1, 1, -1}; // neighbors
};


class game_of_life : public ca_grid<int>
{
public:
	game_of_life(int r, int c, int u, int o, int t) : 
		ca_grid(r, c, u, o, t);
	game_of_life(int r, int c) : ca_grid(r, c);
	game_of_life(int u, int o, int t) : 
		ca_grid(u, o, t);
	game_of_life() : grid(), health(1);

	void init_cells() {
		for(size_t r = 0; r < rows; r++) {
			for(size_t c = 0; c < cols; c++) {
				grid[r][c].set_val(0);
				grid[r][c].set_val(0);
				// this is ass backwards, fix it soon silas
			}
		}
	}

public:
	/*
	 * Rules:
	 * - Live cells have a value above threshold
	 * - Dead cells have a value below threshold
	 * - cell state is in a cells grid.
	 * - live cells with fewer than two live neighbors dies
	 *   - underpopulation threshold (< threshold)
	 * - live cells with 2-3 live neighbors lives (between threshold)
	 * - live cells with more than 3 neighbors dies
	 *   - overpopulation threshold (> threshold)
	 * - dead cells with exactly 3 neighbors reproduce
	 *   - > underpopulation && < overpopulation == reproduce
	*/
	
	// generate one iteration of the matrix
	void next() {
		for(size_t i = 0; i < rows; i++) {
			for(size_t j = 0; j < cols; j++) {
				int alive_neigbors = 
					count_alive_neighbors(grid[i][j]);
	
				// live cells
				if(grid[i][j].is_alive(health) {
					// alive neighbors > o_thr. or < u_thresh die
					if( (alive_neighbors > o_thresh) || (alive_neighbors < u_thresh)) {
						grid[i][j].set_val(0);
					}
					// otherwise do nothing
					else { // alive_neighbors == o_threshold
						continue; // i don't think this needs to be here
					}
				} else {
					// reproduce
					if(alive_neighbors > o_thresh) {
						grid[i][j].set_val(1);
					}
				}
				// else we continue
				// implicitly, no changes are done otherwise

			}
		}
	}

	// wipe the entire board
	void reset() {
		init_cells();
	}
	// create a random seed to start
	void generate(int s) {

	}


// these are for handling the rules above
private:
	// gets number of alive neighbors
	int count_alive_neighbors(ca_cell<int> cell) {
		int out = 0;
      		size_t arr_len = std::size(row_offset);
		for(size_t i = 0; i < arr_len; i++) {
			int x = cell.get_x() + row_offset[i];
			int y = cell.get_y() + col_offset[i];

			// bounds check
			if(bounds_check(x, y))
				continue;

			if(grid[x][y].is_alive(health) {
				out++;
			}
		}
	}

	// return true if we are out of bounds
	bool bounds_check(int x, int y) {
		return (x < 0) || (y < 0) || (x >= rows) || (y >= cols);
	}
};
