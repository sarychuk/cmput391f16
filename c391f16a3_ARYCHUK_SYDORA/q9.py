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

# This function will go through the Filters and append them to the final statement
def parseFilter(queryData, selectParams, regexdict):
	returnData = []
	data = ""

	if regexdict:
		returnData.append("WHERE ")
		for value in regexdict:
			#reset data string
			data = ""
			for type in queryData:
				if value == selectParams[0]:
					data += "r" + str(selectParams.index(value)) + ".subject like \"" + regexdict[value] + "\" and "
					break
				else:
					if value in type[2]:
						data += "r" + str(queryData.index(type) + 1) + ".object like \"" + regexdict[value] + "\" and "
			returnData.append(data)
	
	#remove the trailing "and"
	tempData = data
	data = tempData[0:-4]
	returnData[-1] = data

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
	regexdict = {}
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
				if (splitLine[0] == "FILTER"):
					quoteSplit = line.split("\"")
					regexdict[splitLine[1][8:-1]] = quoteSplit[1]
				elif(splitLine[strLen - 1] == "."):
					subjPredObj.append(splitLine[0:-1])
				else:
					subjPredObj.append(splitLine)
				

	# get the select parameters
	selectParams = parseSelectParams(subjPredObj)

	# get the query data with left joins
	queryData = parseData(subjPredObj, selectParams, prefixdict)

	# get the filter data with Where clauses
	filterData = parseFilter(subjPredObj, selectParams, regexdict)

	# create the start of the select statement
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
	for filt in filterData:
		selectStr += filt

	#execute the statement
	cursor = dbConn.execute(selectStr)


	# print out the headers
	for item in selectParams:
		print(item + " | ", end="")

	print()

	# print out the data from the select statement
	for row in cursor:
		for item in selectParams:
			print(row[selectParams.index(item)] + " | ", end="")
		print()

	# close the files
	dbConn.close()
	queryFile.close()

if __name__ == "__main__":
	main()