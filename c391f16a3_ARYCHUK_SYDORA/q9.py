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

# This will find the various parameters listed if there is a "*" in the select
def parseSelectParams(queryData):
	selectParams = []
	for val in queryData:
		data = ""
		for item in val:
			if "?" in item:
				if item[1:] not in selectParams:
					selectParams.append(item[1:])

	return selectParams

# This function will go through the list of queryData, which is in the form of
# subjm predm objm, and create the joins.
def parseData(queryData, selectParams, prefixdict):
	returnData = []
	count = 0
	for val in queryData:
		data = ""
		count += 1
		data = "left join rdf_triple r" + str(count)
		data += " on r0.subject = r" + str(count) + ".subject "

		if ("http" in val[1]):
			data += "and r" + str(count) + ".predicate = \"" + val[1] + "\" ", count
		else:
			predSplit = val[1].split(":")
			data += "and r" + str(count) + ".predicate = \"" + prefixdict[predSplit[0]] +  predSplit[1] + "\" "

		if (":" in val[2]):
			objSplit = val[2].split(":")
			data += "and r" + str(count) + ".object = \"" + prefixdict[objSplit[0]] +  objSplit[1] + "\" "
		elif ("?" in val[2]):
			mt = True
		else:
			data += "and r" + str(count) + ".object = \"" + val[2] + "\" "
		returnData.append(data)

	return returnData

def main():
	# Declarations
	prefixdict = {}
	sqlStr = ""
	selectStr = ""
	selectStart = False
	selectAll = False
	firstTriple = True
	subjPredObj = []
	strLen = 0

	# Check arguments
	if len(sys.argv) != 3:
		print("Expected 3 args: q9.py <database file> <path to a file with a SPARQL query>")
		sys.exit(2)

	dbConn = sqlite3.connect(sys.argv[1])
	queryFile = open(sys.argv[2], "r")
	
	# Use dbConn.execute() to create the execution statement
	for line in queryFile:
		splitLine = line.split()
		if(splitLine):
			strLen = len(splitLine)
			if(splitLine[0] == "PREFIX"):
				prefixdict[splitLine[1][0:-1]] = splitLine[2][1:-1]
			elif(splitLine[0] == "SELECT"):
				selectStart = True
				if(splitLine[1] == "*"):
					selectStr = splitLine[0:-1]
					selectAll = True

			elif(selectStart == True):
				#check for curly braces to read through again?
				#query commands
				if (splitLine[0] == "}"):
					continue
				if(splitLine[strLen - 1] == "."):
					subjPredObj.append(splitLine[0:-1])
				else:
					subjPredObj.append(splitLine)
				
	selectParams = parseSelectParams(subjPredObj)
	queryData = parseData(subjPredObj, selectParams, prefixdict)

	selectStr = "SELECT distinct "
	for item in selectParams:
		for val in subjPredObj:
			if item == selectParams[0]:
				selectStr += "r" + str(selectParams.index(item)) + ".subject as " + item + ", "
				break
			else:
				if item in val[2]:
					selectStr += "r" + str(subjPredObj.index(val) + 1) + ".object as " + item + ", "

	tempSelect = selectStr
	selectStr = tempSelect[0:-2]
	selectStr += " FROM rdf_triple r0 "
	for element in queryData:
		selectStr += element
	cursor = dbConn.execute(selectStr)

	for item in selectParams:
		print(item + " | ", end="")

	print()

	for row in cursor:
		for item in selectParams:
			print(row[selectParams.index(item)] + " | ", end="")
		print()

	dbConn.close()
	queryFile.close()

if __name__ == "__main__":
	main()
