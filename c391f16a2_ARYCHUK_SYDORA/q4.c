#include "stdio.h"
#include "sqlite3.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
	{
	//the database
	sqlite3 *db;

	//A value to check if sqlite function worked or not
  	int rc;

	//Check if the number of arguments is correct
  	if( argc!=6 )
	{
		//To many arguments
    		fprintf(stderr, "Expected 5 args: ./a.out X1 Y1 X2 Y2 value\n");
    		return(1);
  	}

	//try opening the database
  	rc = sqlite3_open("streets.db", &db);
  	if( rc )
	{
		//Could not open
	    	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	    	sqlite3_close(db);
	    	return(1);
  	}

	//statement handle
	sqlite3_stmt *stmt;

	//the statments
  	char *sql_stmt = "SELECT * "\
  			 "FROM poi_index pi, poi_tag pt "\
  			 "WHERE (pt.id = pi.id AND pt.value LIKE ? AND pt.key like 'class') "\
  			 	"AND pi.minLat>=? AND pi.maxLat<=? "\
  			   	"AND pi.minLon>=? AND pi.maxLon<=?;";

	//prepare the statment:
  	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);
	if (rc != SQLITE_OK)
	{  
		//error in the stament
	        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
	        sqlite3_close(db);
	        return 1;
	}
	
	sqlite3_bind_text(stmt, 1, argv[5], -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 2, strtod(argv[1], NULL));
	sqlite3_bind_double(stmt, 3, strtod(argv[3], NULL));
	sqlite3_bind_double(stmt, 4, strtod(argv[2], NULL));
	sqlite3_bind_double(stmt, 5, strtod(argv[4], NULL));
    
	//step through the output rows:
    	while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
        	int col;
        	//print results:
	        for(col=0; col<sqlite3_column_count(stmt)-1; col++)
	        {
	          printf("%s|", sqlite3_column_text(stmt, col));
	        }
	        printf("%s", sqlite3_column_text(stmt, col));
	        printf("\n");
	}

	//always finalize a statement
	sqlite3_finalize(stmt);

	//close the database:
	sqlite3_close(db);
}
