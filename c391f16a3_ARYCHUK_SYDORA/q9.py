#   Write a program, in a file called q9.XYZ, where XYZ is the appropriate
#   extension of your chosen programming language, that:

#1) reads in two parameters from the command line: (1) the name of a SQLite
#   database file containing an RDF graph and (2) a path to a file with a SPARQL query
#2) prints the output of that query against that graph

#  For the sake of this assignment, you will deal with conjunctive SPARQL queries,
#  without negation and without aggregations. At a minimum, your program must
#  handle a fully specified query such as the one below, where the output is
#  fully specified by variables that are used among the graph patterns.

#PREFIX p:<URI>
#SELECT ?v1 ?v2 ... ?vn
#WHERE {
#subj1 pred1 obj1 .
#...
#subjm predm objm
#}

#You can assume that:
    #-There can be blank lines in the query file, consisting of zero or more
    # space characters (e.g., \s or \t)
    #-There can be zero or many PREFIX declarations, and that there will always
    # be one SELECT ... WHERE statement in each query
    #-Every PREFIX declaration comes in a separate line and before the SELECT 
    # statement
    #-Every non-blank line inside the WHERE clause contains a single subj pred 
    # obj pattern or a single FILTER constraint
    #-Every sub, pred, obj will be one of: a variable, a fully prefixed resource 
    # (predicate or entity), or a literal.
    #-All literals in any query will be enclosed with double quotes, and only 
    # strings, integers, and decimal literals will be used.

#If it helps, you may require that every variable in the SELECT appears in at 
#least one pattern in the WHERE clause, and throw an error otherwise. Also, you 
#can assume that every regex filter will be of the form:
#FILTER (regex(?v, "<text>")), without any other operators.


import sys
import sqlite3

def main():

	if len(sys.argv) != 3:
		print("Expected 3 args: q9.py <database file> <path to a file with a SPARQL query>")
		sys.exit(2)

	dbConn = sqlite3.connect(sys.argv[1])
	queryFile = open(sys.argv[2], "r")
	
	print("files opened")

	# Use dbConn.execute() to create the execution statement

	# Returning the rows of a select statement using a cursor
	#cursor = conn.execute("SELECT id, name, address, salary  from COMPANY")
	#for row in cursor:
	#	print "ID = ", row[0]
	#	print "NAME = ", row[1]
	#	print "ADDRESS = ", row[2]
	#	print "SALARY = ", row[3], "\n"

	dbConn.close()
	queryFile.close()

	print("files closed")

if __name__ == "__main__":
	main()
