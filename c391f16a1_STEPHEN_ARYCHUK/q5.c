#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>
#include <string.h>

#define RADIUS (6371.009)
#define RAD_PER_DEGREE (M_PI/180)

/*
Q5 (15 pts)
Write a C program, in a file called q5.c that prints the list of types of airplanes 
capable of operating flights over 10,000 Km in length.
*/

int main(int argc, char **argv)
{
    sqlite3 *db; //the database
    sqlite3_stmt *stmt; //the select statement
    sqlite3_stmt *tempstmt;
    sqlite3_stmt *selstmt;

    // initialize the latitudes and longitudes
    double lat1, lng1, lat2, lng2;

    // initialize the char array for type
    char type[30];

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

    // Selects all airports that have source or destination
    char *sql_stmt = "select distinct r.Source_airport, a.Latitude, a.Longitude, " \
                        "r.Destination_airport, a2.Latitude, a2.Longitude, " \
                        "r.Airline, r.Equipment " \
                        "from airports a, airports a2, routes r " \
                        "where r.Source_airport = a.IATA_FAA and " \
                        "r.Destination_airport = a2.IATA_FAA;";

    // Create a temporary table to store the distances in
    char *temp_stmt = "CREATE TEMPORARY TABLE dist(Type TEXT, Distance REAL);";
    sqlite3_exec(db, temp_stmt, 0, 0, 0);

    rc = sqlite3_prepare_v2(db, sql_stmt, -1, &stmt, 0);

    if (rc != SQLITE_OK) 
    {   
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }    
      
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        // Assign the values from the select statement into their respective items
        lat1 = sqlite3_column_double(stmt, 1);
        lng1 = sqlite3_column_double(stmt, 2);
        lat2 = sqlite3_column_double(stmt, 4);
        lng2 = sqlite3_column_double(stmt, 5);
        strcpy(type, sqlite3_column_text(stmt, 7));
        
        // Convert Lat and Long into their Radian counterparts
        double rlat1 = lat1 * RAD_PER_DEGREE;
        double rlat2 = lat2 * RAD_PER_DEGREE;
        double rlng1 = lng1 * RAD_PER_DEGREE; 
        double rlng2 = lng2 * RAD_PER_DEGREE;

        // Calculate the Delta(lat) and Delta(long)
        double dlat = rlat2 - rlat1;
        double dlng = rlng2 - rlng1;

        // Calculate the Theta(m)
        double thtm = ((rlat1 + rlat2) / 2);

        //// OBSOLETE: double dist = (RADIUS * (sqrt(pow(dlat,2) + pow((cos(thtm) * dlng),2))));
        // Using Great-circle distance to calculate the distance from point to another on Earth
        double dist = (RADIUS * acos((sin(rlat1) * sin(rlat2)) + (cos(rlat1) * cos(rlat1) * cos(dlng))));

        // insert statement for the temporary table created
        char *insert_stmt = "INSERT INTO dist(Type, Distance) VALUES (?, ?)";

        rc = sqlite3_prepare_v2(db, insert_stmt, -1, &tempstmt, NULL);

        if (rc != SQLITE_OK) 
        {  
            fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return 1;
        }  

        // Bind the data that has been calculated into their respective "?" positions
        sqlite3_bind_text(tempstmt, 1, type, strlen(type), SQLITE_STATIC);
        sqlite3_bind_double(tempstmt, 2, dist);

        rc = sqlite3_step(tempstmt); 

        sqlite3_finalize(tempstmt);
    }

    // Final select to pull the Types of commercial airplanes that can fly more then 10000km
    char *final_sql = "select distinct Type " \
                        "from dist " \
                        "where Distance > 10000 " \
                        "order by Type;";

    rc = sqlite3_prepare_v2(db, final_sql, -1, &stmt, 0);

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