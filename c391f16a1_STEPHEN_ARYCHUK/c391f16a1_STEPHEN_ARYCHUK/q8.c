#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/*
Q8 (15 pts)
Write a C program, in a file called q8.c that prints the list of 
airports with commercial flights into (or out of) them, but yet cannot 
be reached flying from YEG regardless of the number of connections.
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

    // Recursive select to find airports that connect to YEG, then NEGATE to find all others.
    // Reduce results by filtering for commercial and making sure that they have flights going into or out of.

  	char *sql_stmt = "WITH RECURSIVE test(Destination_airport) AS " \
                    "(select distinct routes.Destination_airport " \
                      "from airports, routes " \
                      "where routes.Source_airport = 'YEG' " \
                      "UNION " \
                      "select r.Destination_airport " \
                      "from routes r " \
                      "JOIN test t ON r.Source_airport = t.Destination_airport) " \
                    "select distinct ap.Name " \
                    "from airports ap, airlines a " \
                    "where (ap.IATA_FAA NOT IN test) and " \
                    "ap.IATA_FAA IN (select Source_airport from routes) and " \
                    "ap.IATA_FAA IN (select destination_airport from routes) and " \
                    "a.IATA != '' and a.ICAO != '' " \
                    "and a.Country != '' and a.Callsign != '' " \
                    "and a.Active = 'Y' and ap.IATA_FAA != '' and ap.ICAO != '';";

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