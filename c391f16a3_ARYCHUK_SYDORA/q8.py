import sys
import sqlite3

def main():

	if len(sys.argv) != 3:
		print("Expected 3 args: q8.py <database file> <rdf graph file>")
		sys.exit(2)

	dbConn = sqlite3.connect(sys.argv[1])
	rdfFile = open(sys.argv[2], "r")

	# Use dbConn.execute() to create the execution statement
	# Use dbConn.commit to push the query.

	# Returning the rows of a select statement using a cursor
	#cursor = conn.execute("SELECT id, name, address, salary  from COMPANY")
	#for row in cursor:
	#	print "ID = ", row[0]
	#	print "NAME = ", row[1]
	#	print "ADDRESS = ", row[2]
	#	print "SALARY = ", row[3], "\n"

	print("files opened")

	dbConn.close()
	rdfFile.close()

	print("files closed")

if __name__ == "__main__":
	main()