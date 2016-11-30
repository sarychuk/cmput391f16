Owner: Stephen Arychuk & Christoph Sydora

Collaborations: N/A

Compilation: Since q8.py and q9.py are python there is no compilation

Execution: 	Question 8 -- execute with the command "python3 q8.py <database> <rdf graph>" without the quotation marks and the database
				is the sqlite database while the rdf graph is the file that contains the graph.

			Question 9 -- execute with the command "python3 q9.py <database> <sparql query>" without the quotation marks, while the
				database is the sqlite database and the query is the sparql query.
				The test_with_error.ttl file should not show any results as the motto that has been attempted to filter
				contains an extra "s" in the last part, thus removing it because it doesn't match.
			  
Python
Libraries: 	sys
			sqlite3

Assumptions for reading in RDF (Question 8):
1) That the triples are all on the same line.
2) Each line must end with a ",", ".", ";".
3) There is some space or tab between the triple items

Assumptions for reading in the query (Question 9):
1) The "Select" to the first "{" is all placed in one line. e.g. "Select * Where {"
