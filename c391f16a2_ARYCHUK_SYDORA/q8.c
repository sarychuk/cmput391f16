#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <math.h>
#include <stdbool.h>

/*
Q8 15pts

Write a C program, in a file called q8.c that takes a point (x1,y1)
and k as input, and prints a list of the k nearest neighbors. You
can do this, for instance, by implementing what is outlined in 
Section 3.3 of the Roussopoulos paper.

Marking breakdown: 5 pts for correctness, and 10 pts for efficiency. 
To get full marks, your program must implement an algorithm that is 
at least as fast (in asymptotic terms) as the one outlined in Section 
3.3 the Roussopoulos paper.

*/

struct Point
{
	double x;
	double y;
};

struct Neighbor
{
	double nodeno;
	double dist;
	struct Point rect;
};

double min(double x, double y)
{
  return (x) < (y) ? (x) : (y);
}

double max(double x, double y)
{
	return (x) > (y) ? (x) : (y);
}

double distance(struct Point p1, struct Point p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

double minDistance(struct Point point, double minLat, double maxLat, double minLon, double maxLon)
{
  double r1, r2;
  // check for r_i values
  if (point.x < minLat) 
  {
    r1 = minLat;
  }
  else if (point.x > maxLat)
  { 
    r1 = maxLat;
  }
  else 
  {
    r1 = point.x;
  }

  if (point.y < minLon) 
  {
    r2 = minLon;
  }
  else if (point.y > maxLon)
  { 
    r2 = maxLon;
  }
  else 
  {
    r2 = point.y;
  }

  // calculate each stage of the sums (2 because of 2 dimensions, x and y)
  double iter1 = pow(abs(point.x - r1),2);
  double iter2 = pow(abs(point.y - r2),2);

  // return the sum of the iterations
	return (iter1 + iter2);
}

double minMaxDistance(struct Point point, double minLat, double maxLat, double minLon, double maxLon)
{
	double rm1, rm2, rM1, rM2;
 	// check for rm_k values
	if (point.x <= ((minLat + maxLat)/2))
	{
		rm1 = minLat;
	}
	else 
	{
		rm1 = maxLat;
	}

	if (point.y <= ((minLon + maxLon)/2))
	{
		rm2 = minLon;
	}
	else 
	{
		rm2 = maxLon;
	}

	// check for rM_i values
	if (point.x >= ((minLat + maxLat)/2))
	{
		rM1 = minLat;
	}
	else 
	{
		rM1 = maxLat;
	}

	if (point.y >= ((minLon + maxLon)/2))
	{
		rM2 = minLon;
	}
	else 
	{
		rM2 = maxLon;
	}

	double partk1 = pow(abs(point.x - rm1),2);
	double partk2 = pow(abs(point.y - rm2),2);
	double parti1 = pow(abs(point.x - rM1),2);
	double parti2 = pow(abs(point.y - rM2),2);

	// calculate each stage of the sums (2 because of 2 dimensions, x and y)
	double iter1 = (partk1 + parti2);
	double iter2 = (partk2 + parti1);

	// return the sum of the iterations
	return min(iter1, iter2);
}

void nearestNeighborSearch(double node, struct Point point, sqlite3 *db, int k, struct Neighbor *kNearest)
{
	int rc;
	sqlite3_stmt *stmt;
	// Get all the children
	// Generate Active Branch List
	//genBranchList(Point, Node, branchList)	
	// Sort ABL based on ordering metric values
	//sortBranchList(branchList)
	char *sql_children = "SELECT pit.* FROM poi_index_temp pit, poi_index_parent pip WHERE pit.nodeno = pip.nodeno AND pip.parentnode = ? ORDER BY minLat, minLon;";
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
	bool first = true;
	double minMinMaxDist = 0;

	while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		hasChildren = 1;
		int newNode = sqlite3_column_int(stmt, 0);
		double minLat = sqlite3_column_double(stmt, 1);
		double maxLat = sqlite3_column_double(stmt, 2);
		double minLon = sqlite3_column_double(stmt, 3);
		double maxLon = sqlite3_column_double(stmt, 4);
		double minDist = minDistance(point, minLat, maxLat, minLon, maxLon);
		double minMaxDist = minDistance(point, minLat, maxLat, minLon, maxLon);
		if(first)
		{
			minMinMaxDist = minMaxDist;	
      		first = false;
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
	first = true;
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		// Perform Upward Pruning
		//last := pruneBranchList(Node, Point, Nearest, branchList)
		double minLat = sqlite3_column_double(stmt, 1);
		double maxLat = sqlite3_column_double(stmt, 2);
		double minLon = sqlite3_column_double(stmt, 3);
		double maxLon = sqlite3_column_double(stmt, 4);
		double minDist = minDistance(point, minLat, maxLat, minLon, maxLon);
		double minMaxDist = minMaxDistance(point, minLat, maxLat, minLon, maxLon);
		//double objDist = objDist(point, minLat, maxLat, minLon, maxLon);
		if(minDist > minMinMaxDist)
		{
			continue;
		}
		//if(objDist > minMinMaxDist)
		//{
		///	continue;
		//}
		if(!first)
		{			

			int i;
			for (i = 0; i < k; ++i)
			{
				if(minDist > kNearest[i].dist)
				{
					continue;
				}
			}
		}

		//newNode := Node.branch_branchList_i
		int newNode = sqlite3_column_int(stmt, 0);
	
		// Recursively visit child nodes
		//nearestNeighborSearch(newNode, Point, Nearest)

		struct Neighbor tempNearest[k];
		nearestNeighborSearch(newNode, point, db, k, &tempNearest);

	    if (first)
	    {
	    	int i;
	    	for (i = 0; i < k; ++i)
	    	{
		    	kNearest[i] = tempNearest[i];
	    	}
	      
	     	first = false;
	    }
	   	

  		double maxDist;
  		int iter;
  		int i;
  		for (i = 0; i < k; ++i)
  		{
  			int j;

  			// finds maxDist of kNearest[i]
  			for (j = 0; j < k; ++j) 
  			{
  				if (j == 0)
	  			{
	  				maxDist = kNearest[j].dist;
	  				iter = j;
	  			}
	  			else
	  			{
	  				if (maxDist < kNearest[j].dist)
	  				{
	  					iter = j;
	  				}
	  				maxDist = max(maxDist, kNearest[j].dist);
	  			}
  			}

  			if (maxDist > tempNearest[i].dist)
  			{
  				kNearest[iter] = tempNearest[i];
  			}
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
		int minLatP, minLonP, maxLatP, maxLonP;
		//FOR i := 1 to Node.count
    	first = true;
    	int kCounter = 0;
		while((rc = sqlite3_step(stmt_leaf)) == SQLITE_ROW)
		{
			//dist := objectDIST(Point,Node.branch_i.rect)
			double nodeno = sqlite3_column_double(stmt_leaf, 0);
			double rectx = sqlite3_column_double(stmt_leaf, 1);
			double recty = sqlite3_column_double(stmt_leaf, 3);
			struct Point rect = { rectx, recty };
			double dist = distance(point, rect);
	    	//If(dist < Nearest.dist)

	        struct Neighbor temp = { nodeno, dist, { rectx, recty } };

	      	if (kCounter < k)
	      	{
		        kNearest[kCounter] = temp;
		        ++kCounter;
	      	}
	      	else
	      	{
	      		int i;
	      		double maxDist;
	      		int iter;
	      		for (i = 0; i < k; ++i)
	      		{
	      			if (i == 0)
	      			{
	      				maxDist = kNearest[i].dist;
	      				iter = i;
	      			}
	      			else
	      			{
	      				if (maxDist < kNearest[i].dist)
	      				{
	      					iter = i;
	      				}
	      				maxDist = max(maxDist, kNearest[i].dist);
	      			}
	      		}

	      		if(dist < maxDist)
			    {
			    	kNearest[iter] = temp;
				}
	      	}
		}

		sqlite3_finalize(stmt_leaf);
	}
}

int main(int argc, char **argv)
{
	sqlite3 *db; //the database
	sqlite3_stmt *stmt; //the select statement
	
	int rc;
		
	if( argc!= 5 )
	{
		fprintf(stderr, "Usage: %s <database file, <x value>, <y value>, <k length> \n", argv[0]);
		return(1);
	}

  	rc = sqlite3_open(argv[1], &db);
  	if( rc )
    	{
      		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      		sqlite3_close(db);
      		return(1);
  	}
  	
  	double x = strtod(argv[2], NULL);
  	double y = strtod(argv[3], NULL);
  	int k = atoi(argv[4]);

  	struct Point point = { x , y };
  	struct Neighbor kNearest[k];
  	nearestNeighborSearch(1, point, db, k, &kNearest);
  	
  	int i;
  	for (i = 0; i < k; ++i)
  	{
  		printf("%d POI id = %.0f, POI dist = %f\n", i, kNearest[i].nodeno, kNearest[i].dist);
    	printf("POI lat = %f POI lon = %f\n\n", kNearest[i].rect.x, kNearest[i].rect.y);
  	}
  	
    return 0;
}
