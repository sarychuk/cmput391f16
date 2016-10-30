#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <time.h>

/*
Q7 30pts

Write a C program, in a file called q7.c that takes a point (x1,y1) as input, 
and prints the coordinates of the object that is its nearest neighbor, breaking ties arbitrarily.

Marking breakdown: 15 pts for correctness, and 15 pts for efficiency. 
To get full marks, your program must implement an algorithm that is at least as 
fast (in asymptotic terms) as the one in the Roussopoulos paper. You can get 
full marks even if your program is not the fastest among all submissions.

*/

int nearestNeighborSearch(node, pointx, pointy, nearest)
{
	/*
		
	//local vars
	Node        newNode
	Brancharray branchlist
	integer     dist,last,i
	
	// At leaf level - compute distance to actual objects
	If Node.type == LEAF
		FOR i := 1 to Node.count
			dist := objectDIST(Point,Node.branch_i.rect)
	      		If(dist < Nearest.dist)
				Nearest.dist := dist
				Nearest.rect := Node.branch_i.rect
	
	// Non-leaf level - order, prune and visit nodes
	Else
	
		// Generate Active Branch List
		genBranchList(Point, Node, branchList)
		
		// Sort ABL based on ordering metric values
		sortBranchList(branchList)
	
		// Perform Downward Pruning
		// (may discard all branches)
		last = pruneBranchList(Node, Point, Nearest, branchList)
	
		// Iterate through the Active Branch List
		For i := 1 to last
			newNode := Node.branch_branchList_i
	
			// Recursively visit child nodes
			nearestNeighborSearch(newNode, Point, Nearest)
	
			// Perform Upward Pruning
			last := pruneBranchList(Node, Point, Nearest, branchList)
	
	*/
  
}

int main(int argc, char **argv)
{
	sqlite3 *db; //the database
	sqlite3_stmt *stmt; //the select statement
	
	int rc;
		
	if( argc!=4 )
	{
		fprintf(stderr, "Usage: %s <database file, x, y> \n", argv[0]);
		return(1);
	}

  	rc = sqlite3_open(argv[1], &db);
  	if( rc )
    	{
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		sqlite3_close(db);
      		return(1);
  	}
  	
  	int x = argv[2];
  	int y = argv[2];
  	int inf = 999999999;
  	
  	int nearest_node = nearestNeighborSearch(1, x, y, inf);
  	
      	fprintf("%d\n", nearest_node);
  	
    	return 0;
}
