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

int nearestNeighborSearch(node, point, nearest)
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

    srand(time(NULL));

  	int rc

  	if( argc!=2 )
    {
    	  fprintf(stderr, "Usage: %s <database file> \n", argv[0]);
    	  return(1);
  	}

  	rc = sqlite3_open(argv[1], &db);
  	if( rc )
    {
      	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      	sqlite3_close(db);
      	return(1);
  	}

    // Selects all poi from poi_comp_index 
    char *sql_index = "SELECT * " \
                          "FROM poi indexed BY poi_comp_index " \
                          "WHERE lat >= ? and lon >= ? " \
                          "and lat <= ? and lon <= ?;";

    rc = sqlite3_prepare_v2(db, sql_index, -1, &stmti, 0);

    if (rc != SQLITE_OK) 
      {  
          fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
          sqlite3_close(db);
          return 1;
      }   

    sqlite3_bind_double(stmt, 1, lstRandX[i]);
    sqlite3_bind_double(stmt, 2, lstRandY[i]);
    sqlite3_bind_double(stmt, 3, lstRandX[i] + length);
    sqlite3_bind_double(stmt, 4, lstRandY[i] + length);      

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) { }

    sqlite3_reset(stmt);

    sqlite3_finalize(stmt);

    sqlite3_close(db);

    return 0;
}
