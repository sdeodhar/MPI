
#define ROW 0
#define COL 1
#define INF 202			// range of all graph inputs is 100
					// so an edge weight or sum of two 						// edges cannot be more than 200

/******
If using files with varying range, write this range in file along with array size and read it to use in the calculation
***********/

#define FILE_NOT_FOUND 404

#define SIDE_BORDER_TASK    1
#define BOTTOM_BORDER_TASK  2
#define CORNER_TASK         3

inline int min(int x, int y);
int get_rank_type(int r,int cur_size,int n);
int sqroot(int elt);
int read_graph(char *file_name, int n, int **A);
void write_graph (char *file_name, int n, int **A);
int print_graph(int n, char* filename);
