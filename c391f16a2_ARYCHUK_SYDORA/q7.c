#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <math.h>

/*
Q7 30pts

Write a C program, in a file called q7.c that takes a point (x1,y1) as input, 
and prints the coordinates of the object that is its nearest neighbor, breaking ties arbitrarily.

Marking breakdown: 15 pts for correctness, and 15 pts for efficiency. 
To get full marks, your program must implement an algorithm that is at least as 
fast (in asymptotic terms) as the one in the Roussopoulos paper. You can get 
full marks even if your program is not the fastest among all submissions.

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

struct Point
{
	int x;
	int y;
};
struct Neighbour
{
	int nodeno;
	double dist
	struct Point rect;
};

double distance(Point p1, Point p2)
{
	return math.sqrt(math.pow(p1.x - p2.x, 2), math.pow(p1.y - p2.y, 2));
}

double minDist(struct Point point, double minLat, double maxLat, double minLon, double maxLon)
{
	return math.pow(math.abs(),2)
}

double minMaxDist(struct Point point, double minLat, double maxLat, double minLon, double maxLon)
{
	
}

struct Neighbour nearestNeighborSearch(int node, struct Point point)
{
	int rc;
	struct Neighbour newNearest;
	sqlite3_stmt *stmt;
	// Get all the children
	// Generate Active Branch List
	//genBranchList(Point, Node, branchList)	
	// Sort ABL based on ordering metric values
	//sortBranchList(branchList)
	char *sql_children = "SELECT * FROM poi_index_temp WHERE nodeno = ? ORDER BY minLat, minLon;";
	rc = sqlite3_prepare_v2(db, sql_children, -1, &stmt, 0);
	if (rc != SQLITE_OK) 
	{  
		fprintf(stderr, "Preparation failed 1: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_bind_int(stmt, 1, node);
	
	// Perform Downward Pruning
	// (may discard all branches)
	//last = pruneBranchList(Node, Point, Nearest, branchList)
	int hasChildren = 0;
	int first = 0;
	double minMinMaxDist = 0;
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		hasChildren = 1;
		int newNode = sqlite3_column_int(stmt, 0);
		double minLat = sqlite3_column_double(stmt, 1);
		double maxLat = sqlite3_column_double(stmt, 2);
		double minLon = sqlite3_column_double(stmt, 3);
		double maxLon = sqlite3_column_double(stmt, 4);
		double minDist = minDist(point, minLat, maxLat, minLon, maxLon);
		double minMaxDist = minDist(point, minLat, maxLat, minLon, maxLon);
		if(first == 0)
		{
			minMinMaxDist = minMaxDist;	
		}
		else
		{
			if(minMinMaxDist > minMaxDist)
			{
				minMinMaxDist = minMaxDist;
			}
		}
	}
	
	sqlite3_reset(stmt);
	
	// Iterate through the Active Branch List
	//For i := 1 to last
	first = 0;
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		// Perform Upward Pruning
		//last := pruneBranchList(Node, Point, Nearest, branchList)
		double minLat = sqlite3_column_double(stmt, 1);
		double maxLat = sqlite3_column_double(stmt, 2);
		double minLon = sqlite3_column_double(stmt, 3);
		double maxLon = sqlite3_column_double(stmt, 4);
		double minDist = minDist(point, minLat, maxLat, minLon, maxLon);
		double minMaxDist = minMaxDist(point, minLat, maxLat, minLon, maxLon);
		double objDist = objDist(point, minLat, maxLat, minLon, maxLon);
		if(minDist > minMinMaxDist)
		{
			continue;
		}
		if(objDist > minMinMaxDist)
		{
			continue;
		}
		if(first == 1)
		{			
			if(mindist > newNearest.dist)
			{
				continue;
			}
		}
		else
		{
			first = 1;
		}
		
		//newNode := Node.branch_branchList_i
		int newNode = sqlite3_column_int(stmt, 0);
	
		// Recursively visit child nodes
		//nearestNeighborSearch(newNode, Point, Nearest)
		struct Neighbour tempNearest = nearestNeighborSearch(newNode, point);
		if(tempNearest.dist < newNearest.dist)
		{
			newNearest = tempNearest;
		}
	}
	
	sqlite3_finalize(stmt);
	
	// At leaf level - compute distance to actual objects
	//If Node.type == LEAF
	if(hasChildren == 0)
	{
		sqlite3_stmt *stmt_leaf;
		char *sql_leaf = "SELECT pi.* "\
				 "FROM poi_index_rowid rid, poi_index pi, poi_index_parent pip "\
				 "WHERE pip.parentnode = ? AND "\
				       "rid.rowid = pi.id AND "\
				       "pip.nodeno = rid.nodeno;";
		rc = sqlite3_prepare_v2(db, sql_leaf, -1, &stmt_leaf, 0);
		if (rc != SQLITE_OK) 
		{  
			fprintf(stderr, "Preparation failed 2: %s\n", sqlite3_errmsg(db));
		}
		sqlite3_bind_int(stmt_leaf, 1, node);
		int hasChildren = 0;
		int first = 0;
		int minLatP, minLonP, maxLatP, maxLonP;
		//FOR i := 1 to Node.count
		while((rc = sqlite3_step(stmt_leaf)) == SQLITE_ROW)
		{
			//dist := objectDIST(Point,Node.branch_i.rect)
			int nodeno = sqlite3_column_int(stmt, 0);
			double rectx = sqlite3_column_double(stmt_leaf, 1);
			double recty = sqlite3_column_double(stmt_leaf, 3);
			struct Point rect = { rectx, recty };
			double dist = distance(point, rect);
	      		//If(dist < Nearest.dist)
	      		if(dist < newNearest.dist)
	      		{
	      			newNearest.nodeno = nodeno;
				//Nearest.dist := dist
				newNearest.dist = dist;
				//Nearest.rect := Node.branch_i.rect
				newNearest.rect = rect;
			}
		}
		sqlite3_finalize(stmt_leaf);
	}
	
	return newNearest;
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
  	int y = argv[3];
  	struct Point point = { x , y };
  	struct Neighbour neighbour = { 0 , 99999999 , { 0 , 0 } };  	
  	struct Neighbour nearestNeighbour = nearestNeighborSearch(1, point, neighbour);
  	
      	fprintf("%d\n", nearestNeighbour.nodeno);
  	
    	return 0;
}
