import sys
import sqlite3

def main():

	if len(sys.argv) != 3:
		print("Expected 3 args: q8.py <database file> <rdf graph file>")
		sys.exit(2)

	dbConn = sqlite3.connect(sys.argv[1])
	rdfFile = open(sys.argv[2], "r")

	print("files opened")
	
	# Use dbConn.execute() to create the execution statement
	for line in rdfFile:
		splitLine = line.split()
		if(splitLine[0] == "@prefix"):
			newInsert = "INSERT INTO rdf_prefix VALUES ("+splitLine[1][0:-2]+", "+splitLine[2]+")"
			dbConn.execute(newInsert)
		

	# Use dbConn.commit to push the query.
	dbConn.commit()
	
	dbConn.close()
	rdfFile.close()

	print("files closed")

if __name__ == "__main__":
	main()
