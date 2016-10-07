#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/*
Q2 (5 pts)
Write a C program, in a file called q2.c that prints a list of all international flights by 
active and commercial (i.e., not user-added) airlines that do not have a reciprocal return.
(A reciprocal return flight for YEG->LHR would be LHR->YEG by the same airline).
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

    // This SQL statement collects all the non-reciprocal flights
    char *sql_stmt = "select distinct " \
                      "r.Source_airport, r.Destination_airport, r.Airline " \
                      "from routes r, airports ap1, airports ap2, airlines ai " \
                      "where r.Source_airport_ID = ap1.Airport_ID and " \
                      "ap1.Country != ap2.Country and " \
                      "r.Destination_airport_ID = ap2.Airport_ID and " \
                      "ap1.Airport_ID != ap2.Airport_ID and " \
                      "ap1.Airport_ID < ap2.Airport_ID and " \
                      "r.Airline_ID = ai.Airline_ID and " \
                      "ai.Active = 'Y' and " \
                      "ai.IATA != '' and ai.ICAO != '' and " \
                      "ai.Country != '' and ai.Callsign != '' and " \
                      "ai.IATA != '\\N' and ai.ICAO != '\\N' and " \
                      "ai.Country != '\\N' and ai.Callsign != '\\N';";

  	rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {  
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }    

    int count = 0;
      
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        int col;
        for(col=0; col<sqlite3_column_count(stmt)-1; col++) 
        {
            printf("%s|", sqlite3_column_text(stmt, col));
        }

        ++count;

        printf("%s", sqlite3_column_text(stmt, col));
        printf("\n");
    }

    printf("airlines = %d\n", count);

    sqlite3_finalize(stmt); //always finalize a statement

    return 0;
}
