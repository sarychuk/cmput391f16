#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

// Gets the mbr for the node and inserts it into the table poi_index_temp
void get_min_max_values(sqlite3 *db, int node, int* minLat, int* maxLat, int* minLon, int* maxLon)
{
	int rc;
	sqlite3_stmt *stmt;
	// Get all the children
	char *sql_children = "SELECT nodeno FROM poi_index_parent WHERE parentnode = ?;";
	rc = sqlite3_prepare_v2(db, sql_children, -1, &stmt, 0);
	if (rc != SQLITE_OK) 
	{  
		fprintf(stderr, "Preparation failed 1: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_bind_int(stmt, 1, node);
	int hasChildren = 0;
	int first = 0;
	int minLatP, minLonP, maxLatP, maxLonP;
	//go over each child and get the bounding boxes and use those to determine
	// this bounding box
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		hasChildren = 1;
		int minLatC, minLonC, maxLatC, maxLonC;
		int childNum = sqlite3_column_int(stmt, 0);
		get_min_max_values(db, childNum, &minLatC, &minLonC, &maxLatC, &maxLonC);
		if(first == 0)
		{
			minLatP = minLatC;
			minLonP = minLonC;
			maxLatP = maxLatC;
			maxLonP = maxLonC;
		}
		else
		{
			if(minLatP > minLatC) {minLatP = minLatC;}
			if(minLonP > minLonC) {minLonP = minLonC;}
			if(maxLatP < maxLatC) {maxLatP = maxLatC;}
			if(maxLonP < maxLonC) {maxLonP = maxLonC;}		
		}
		
		first = 1;
	}
	sqlite3_finalize(stmt);
	
	// if it was a leaf then we return that leafs bounding box
	if(hasChildren == 0)
	{
		sqlite3_stmt *stmt_leaf;
		char *sql_leaf = "SELECT pi.minLat, pi.minLon, pi.maxLat, pi.maxLon "\
				 "FROM poi_index_rowid rid, poi_index pi "\
				 "WHERE rid.nodeno = ? AND rid.rowid = pi.id;";
		rc = sqlite3_prepare_v2(db, sql_leaf, -1, &stmt_leaf, 0);
		if (rc != SQLITE_OK) 
		{  
			fprintf(stderr, "Preparation failed 2: %s\n", sqlite3_errmsg(db));
		}
		sqlite3_bind_int(stmt_leaf, 1, node);
		while((rc = sqlite3_step(stmt_leaf)) == SQLITE_ROW)
		{
			minLatP = sqlite3_column_double(stmt_leaf, 0);
			minLonP = sqlite3_column_double(stmt_leaf, 1);
			maxLatP = sqlite3_column_double(stmt_leaf, 2);
			maxLonP = sqlite3_column_double(stmt_leaf, 3);
			break;
		}
		sqlite3_finalize(stmt_leaf);
	}
	else
	{	
		// insert the box into the table
		sqlite3_stmt *stmt_insert;
		char *sql_insert = "INSERT INTO poi_index_temp VALUES (?, ?, ?, ?, ?);";
		rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt_insert, 0);
		if (rc != SQLITE_OK) 
		{  
			fprintf(stderr, "Preparation failed 3: %s\n", sqlite3_errmsg(db));
		}
		sqlite3_bind_int(stmt_insert, 1, node);
		sqlite3_bind_double(stmt_insert, 2, minLatP);
		sqlite3_bind_double(stmt_insert, 3, maxLatP);
		sqlite3_bind_double(stmt_insert, 4, minLonP);
		sqlite3_bind_double(stmt_insert, 5, maxLonP);
		while((rc = sqlite3_step(stmt_insert)) == SQLITE_ROW){ }
		sqlite3_finalize(stmt_insert);
	}
	
	// return the bounding box:
	*minLat = minLatP;
	*minLon = minLonP;
	*maxLat = maxLatP;
	*maxLon = maxLonP;
}

int main(int argc, char **argv)
{
	sqlite3 *db; //the database
	sqlite3_stmt *stmt; //the select statement

	int rc;
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
	
	// drop the table
	char *sql_drop = "DROP TABLE IF EXISTS poi_index_temp;";
	rc = sqlite3_prepare_v2(db, sql_drop, -1, &stmt, 0);
	if (rc != SQLITE_OK) 
	{  
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) { }
	sqlite3_finalize(stmt);

	// Create temp table
	char *sql_create = "CREATE TABLE poi_index_temp "\
			   "(nodeno INTEGER, minLat REAL, maxLat REAL, "\
			  		   "minLon REAL, maxLon REAL);";	
	rc = sqlite3_prepare_v2(db, sql_create, -1, &stmt, 0);
	if (rc != SQLITE_OK) 
	{  
		fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}	
	while((rc = sqlite3_step(stmt)) == SQLITE_ROW) { }
	sqlite3_finalize(stmt);
	
	// start the recurtion at the first node "1":
	int minLatP, minLonP, maxLatP, maxLonP;
	get_min_max_values(db, 1, &minLatP, &minLonP, &maxLatP, &maxLonP);
		
	sqlite3_close(db);
	return 0;
}
