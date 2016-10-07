#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

/*
Q3 (5 pts)
Write a C program, in a file called q3.c that prints the list of top-10 countries with the most airlines flying to/from Canada.
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


  	char *sql_stmt = "select Country " \
                        "from " \
                        "(select distinct ai.Country, count(*) as number " \
                        "from airlines ai, routes r, airports ap " \
                        "where r.Airline_ID = ai.Airline_ID and " \
                        "(r.Source_airport_ID = ap.Airport_ID or r.Destination_airport_ID = ap.Airport_ID) and " \
                        "ap.Country = 'Canada' and " \
                        "ai.Country != '' and " \
                        "ai.Country!= '\\N' " \
                        "group by ai.Name order by count(*) desc) " \
                        "group by Country order by SUM(number) desc limit 10;";

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