Owner: Stephen Arychuk


Collaborations: Spoke to two classmates about Question 4, 7, and 9
				William Kwan and Christoph Sydora


Compilation: Simply type "make all" without the quotation marks in terminal to compile all of the executables. If you want to look at 				the command line arguments used please look inside the Makefile.
			  Question 4 and 5 have an additional compilation parameter -lm
			  Alternatively, you can also type "make q#" without quotations and where # is the associated file number.


Execution: Question 9 has an additional parameter on running "./q9 databasefile airport" where "databasefile" is the db and "airport"				is the desired airport to search for.
		   All other files can be run with "./q# databasefile" where # is the associated file number and databasefile is the db
			  

Libraries: 	<stdio.h> 
			<stdlib.h> 
			<math.h> 
			<string.h>
			<sqlite3.h>