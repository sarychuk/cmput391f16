#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/*
Q7 (15 pts)
Write a C program, in a file called q7.c that prints the highest 
airport(s) that one can reach from YEG, regardless of the number of connections.
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
    

    // Recursively checks connections until all have been found that have a connection to YEG
    // Finds highest airport

    char *sql_stmt = "WITH RECURSIVE test(Destination_airport) AS " \
                        "(select distinct routes.Destination_airport " \
                          "from airports, routes " \
                          "where routes.Source_airport = 'YEG' " \
                          "UNION " \
                          "select r.Destination_airport " \
                          "from routes r " \
                          "JOIN test t ON r.Source_airport = t.Destination_airport) " \
                        "select Name, MAX(Altitude) " \
                        "from airports " \
                        "where (airports.IATA_FAA IN test or airports.ICAO in test);";

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