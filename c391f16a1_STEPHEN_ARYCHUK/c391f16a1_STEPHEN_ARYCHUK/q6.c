#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/*
Q6 (5 pts)
Write a C program, in a file called q6.c that prints the list of airports that
can be reached from YEG with at most 3 connections.
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

    // this query will return airport names that can be reached from YEG within 3 connections. I am not considering user added airports or null airports
  	char *sql_stmt = "select distinct Name " \
                        "from airports " \
                        "where airports.IATA_FAA IN (select distinct r3.Destination_airport " \
                        "from routes r3 " \
                        "where r3.Source_airport IN " \
                          "(select distinct r2.Destination_airport " \
                          "from routes r2 " \
                          "where r2.Source_airport IN " \
                            "(select distinct r.Destination_airport " \
                            "from airports ai, routes r " \
                            "where r.Source_airport = 'YEG'))) group by Name;";

  	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {  
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }    

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        int col;
        for(col=0; col<sqlite3_column_count(stmt)-1; col++) 
        {
            printf("%s|", sqlite3_column_text(stmt, col));
        }

        printf("%s", sqlite3_column_text(stmt, col));
        printf("\n");
    }

    sqlite3_finalize(stmt); //always finalize a statement

    return 0;
}