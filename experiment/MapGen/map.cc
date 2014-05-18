// map.cc

#include <iostream>
#include <list>
#include <cstdlib>
#include <algorithm>
using namespace std;

void chooseLines(int line[], int size, int numchosen );

int main(int argc, const char* argv[]) {
	int numVlines;
	int numHlines;
	int height, width; // the height and width of the block

	if ( argc != 5 ) {
		cout << "Usage: map <height> <width> <numVerticalLines> <numHorizontallines>" << endl;
		return 1;
	} else {
		height = atoi( argv[1] );
		width = atoi( argv[2] );
		numVlines = atoi( argv[3] );
		numHlines = atoi( argv[4] );
	}

	srand( time(0) );

	int grid[height][width];

	int vlines[width];
	int hlines[height];

	chooseLines( vlines, width, numVlines );
	chooseLines( hlines, height, numHlines);

	bool vlinesChosen[width];
	bool hlinesChosen[height];

	for ( int i = 0; i < width; i++ ) {
		vlinesChosen[i] = false;
	}
	for ( int i = 0; i < height; i++ ) {
		hlinesChosen[i] = false;
	}

	for ( int i = 0; i < numVlines; i++ ) {
		vlinesChosen[ vlines[i] ] = true;
	}
	for ( int i = 0; i < numHlines; i++ ) {
		hlinesChosen[ hlines[i] ] = true;
	}

	for ( int i = 0; i < height; i++ ) {
		for ( int j = 0; j < width; j++ ) {
			if ( vlinesChosen[j] && hlinesChosen[i] ) {
				grid[i][j] = 3;
			} else if ( vlinesChosen[j] ) {
				grid[i][j] = 1;
			} else if ( hlinesChosen[i] ) {
				grid[i][j] = 2;
			} else {
				grid[i][j] = 0;
			}
		}
	}

	cout << height << endl << width << endl;

	for ( int i = 0; i < height; i++ ) {
		for ( int j = 0; j < width; j++ ) {
			cout << grid[i][j];
			if ( j < width - 1 ) cout << " ";
			else cout << endl;
		}
	}
	return 0;
}

void chooseLines(int line[], int size, int numchosen ) {
/*	for ( int i = 0; i < size; i++ ) {
		line[i] = i;
	}
	for ( int i = 0; i < numchosen; i++ ) {
		int chosen = rand() % ( size - i );
		int temp = line[i];
		line[i] = line[i + chosen];
		line[i + chosen] = temp;
	}
	sort( line, line + numchosen );
*/
	line[0] = 0;
	line[numchosen - 1 ] = size- 1;
	int step = size / (numchosen - 1);
	for ( int i = 1; i < numchosen - 1 ; i++ ) {
		line[i] = i * step;
	}
}
