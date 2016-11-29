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

def createSelect(selectStmt):
	print(selectStmt)

def main():

	prefixdict = {}
	sqlStr = ""
	selectStr = ""
	selectStart = False
	selectAll = False
	firstTriple = True
	subjPredObj = []
	selectParams = []
	queryData = []
	strLen = 0
	fromClause = "FROM rdf_triple tri"

	if len(sys.argv) != 3:
		print("Expected 3 args: q9.py <database file> <path to a file with a SPARQL query>")
		sys.exit(2)

	dbConn = sqlite3.connect(sys.argv[1])
	queryFile = open(sys.argv[2], "r")

	#sample select SPARQL Query
	#PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>
	#PREFIX schema: <http://schema.org/>
	#PREFIX dbr: <http://dbpedia.org/resource/>
	#PREFIX dbo: <http://dbpedia.org/ontology/>
	#SELECT * WHERE {
    	#?city rdf:type schema:City .
  		#?city dbo:country dbr:Canada .
  		#?city dbo:elevation ?elevation .
  		#?city dbo:motto ?motto .
  		#?city dbo:populationTotal ?pop .

	#} 

	#expected output

	#SELECT distinct c.subject as city, e.object as elevation, m.object as motto, p.object as population
	#FROM rdf_triple c 
	#left join rdf_triple ci
	#on c.subject = ci.subject
	#and ci.predicate = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
	#and ci.object =  "http://schema.org/City" 
	#left join rdf_triple co
	#on c.subject = co.subject
	#and co.predicate = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
	#and co.object = "http://dbpedia.org/resource/Canada"
	#left join rdf_triple m
	#on c.subject = m.subject
	#and m.predicate = "http://dbpedia.org/ontology/motto"
	#left join rdf_triple p
	#on c.subject = p.subject
	#and p.predicate = "http://dbpedia.org/ontology/populationTotal"
	#left join rdf_triple e
	#on c.subject = e.subject
	#and e.predicate  = "http://dbpedia.org/ontology/elevation"
	
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

					#temp change back
					##selectStr += "subject" + " "

					#add from tables 
					##selectStr += fromClause + " "

					#add where clause
					##selectStr += splitLine[2] + " "

				else:
					#handle individual items
					print(splitLine[strLen - strLen] + " " + splitLine[strLen - strLen + 1] + " " + splitLine[strLen - strLen + 2] + " " + splitLine[strLen - strLen + 3])

			elif(selectStart == True):
				#check for curly braces to read through again?
				#query commands
				if (splitLine[0] == "}"):
					tempStr = sqlStr
					sqlStr = tempStr[0:-10]
					continue

				
				if(splitLine[strLen - 1] == "."):
					subjPredObj.append(splitLine[0:-1])
				else:
					subjPredObj.append(splitLine)
				

				#sqlStr += selectStr
				if ("http" in splitLine[1]):
					sqlStr += "tri.predicate = \"" + splitLine[1] + "\" "
				else:
					predSplit = splitLine[1].split(":")
					sqlStr += "tri.predicate = \"" + prefixdict[predSplit[0]] +  predSplit[1] + "\" "
				sqlStr += "AND "
				if (":" in splitLine[2]):
					prefixInObject = True
					objSplit = splitLine[2].split(":")
				if (prefixInObject):
					sqlStr += "tri.object = \"" + prefixdict[objSplit[0]] +  objSplit[1] + "\" " 
				else:
					sqlStr += "tri.object = \"" + splitLine[2] + "\" " 
				sqlStr += "INTERSECT "
				prefixInObject = False
				
	#for key, value in prefixdict.items():
	#	print(key + " = " + value)

	print()
	#print(sqlStr)
	print()
	#left join rdf_triple ci
	#on c.subject = ci.subject
	#and ci.predicate = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
	#and ci.object =  "http://schema.org/City" 
	count = 0
	for val in subjPredObj:
		data = ""
		for item in val:
			if "?" in item:
				if item[1:] not in selectParams:
					selectParams.append(item[1:])
		print(val)
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
		queryData.append(data)
	#print(subjPredObj[0])
	print()
	print("Assign select string with variables")
	print(selectParams)
	print()
	#SELECT distinct c.subject as city, e.object as elevation, m.object as motto, p.object as population
	#FROM rdf_triple c
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
	print()
	for element in queryData:
		selectStr += element
	print(selectStr)
	print()

	dbConn.close()
	queryFile.close()

if __name__ == "__main__":
	main()
