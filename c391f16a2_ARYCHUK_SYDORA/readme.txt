Owner: Stephen Arychuk & Christoph Sydora


Collaborations: N/A


Compilation: Simply type "make all" without the quotation marks in terminal to compile all of the executables. If you want to look at 				the command line arguments used please look inside the Makefile.
			  Alternatively, you can also type "make q#" without quotations and where # is the associated file number.

			  Specifically for q7, after "make all", you need to run q7_first to create poi_index_temp. This is simple done by typing
			  "./a.out <databasefile>". This will do some initial setup for q7 and q8. q7_first will not return or print anything to stdout.


Execution: 	Question 4 requires parameters "./q4 <databasefile> <x1> <y1> <x2> <y2> <POI Class>" where databasefile is the db, x1, y1,
				x2 and y2 are the bounding rectangle and POI Class is the class you are searching for.
			Question 5 requires parameters "./q5 <databasefile> <side length>" where databasefile is the db and "side length" is the 	length of the bounding square's side.
			Question 7 requires parameters "./q7 <databasefile> <x> <y>" where databasefile is the db, x and y are the point you want
				to find the nearest neighbors to.
			Question 8 requires parameters "./q8 <databasefile> <x> <y> <k>" where databasefile is the db, x and y are the point you
				want, and k is the number of Nearest Neighbors you want returned.
				to find the nearest neighbors to.
		   All other files can be run with "./q# databasefile" where # is the associated file number and databasefile is the db
			  

Libraries: 	<stdio.h> 
			<stdlib.h> 
			<math.h> 
			<string.h>
			<sqlite3.h>
			<stdbool.h>
			<time.h>
