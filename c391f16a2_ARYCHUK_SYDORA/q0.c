#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

/*
Q0 10pts

Data Conversion. Most spatial indexes, including the R-Tree, are designed 
for Cartesian coordinates and the Euclidian distance, and not for 
lat/lon coordinates. For the purpose of this assignment, you will take 
a simplifying assumption and project all lat/lon coordinates 
into a square 1000x1000 grid.

  lat: (48.06000, 48.24900)
  lon: (11.35800, 11.72400)

Your can perform the projection with SQL or with a program in C or Python. 
If you write/borrow code, please submit it in a file called q0.c or q0.py 
with clear instructions on how to run it.
*/

int main(int argc, char **argv)
{
	  sqlite3 *db; //the database
	  sqlite3_stmt *stmt; //the select statement

  	int rc;

  	if( argc!=2 )
    {
    	  fprintf(stderr, "Usage: %s <database file>\n", argv[0]);
    	  return(1);
  	}

  	rc = sqlite3_open(argv[1], &db);
  	if( rc )
    {
      	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      	sqlite3_close(db);
      	return(1);
  	}


    // This select is to check to see if the lat,lon have been projected or not.
    //char *sql_select = "select lat,lon from poi limit 1";

    // This SQL statement updates the lat and lon to their projected values on a 1000,1000 grid
    char *sql_stmt = "update poi " \
                      "set lat=((((lat - 48.06000) * (1000 - 0)) / (48.24900 - 48.06000)) + 0), " \
                      "lon=((((lon - 11.35800) * (1000 - 0)) / (11.72400 - 11.35800)) + 0);";

  	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {  
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }    

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) { }

    sqlite3_finalize(stmt); //always finalize a statement

    return 0;
}
