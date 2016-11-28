import sys
import sqlite3

#cases:
# <http:...>, ...:..., "...", ..., "..."@en, "..."@it, "..."^^..:..., "..."^^<http:...>
#want:
# http:...
def replace_prefix(value, dict_prefix):
	newValue = value
	splitValue = value.split("\"")
	if(len(splitValue) == 3):
		if(not("@" in splitValue[2] and splitValue[2] != "@en")):
			newValue = splitValue[1]
		else:
			newValue = splitValue[2]
	else:
		splitValue = value.replace('<','>').split('>')
		if(len(splitValue) == 3):
			newValue = splitValue[1]
		else:
			splitValue = value.split(":")
			if(len(splitValue) == 2):
				try:
					newValue = dict_prefix[splitValue[0]]+splitValue[1]
				except:
					pass
	return newValue

#	dbo:motto	"Industry, Integrity, Progress" ;
#@prefix rdf:	<http://www.w3.org/1999/02/22-rdf-syntax-ns#> .
def fix_line(line):
	newLine = []
	newWord = ""
	quotes = False
	i = 0
	line = line.replace("\t", " ")
	line = line.replace("\n", "")
	while(i < len(line)):
		if((not quotes) and (line[i] == " ")):
			if(newWord != ""):
				newLine.append(newWord)
			newWord = ""
		else:
			if(line[i] == "\""):
				quotes = not quotes
			newWord += line[i]
		i+=1
		
	if(newWord != ""):
		newLine.append(newWord)
	return newLine

def main():

	if len(sys.argv) != 3:
		print("Expected 3 args: q8.py <database file> <rdf graph file>")
		sys.exit(2)

	dbConn = sqlite3.connect(sys.argv[1])
	rdfFile = open(sys.argv[2], "r")

	print("files opened")
	
	# Use dbConn.execute() to create the execution statement
	subj = ""
	pred = ""
	obj = ""
	dict_prefix = {}
	list_triples = []
	for line in rdfFile:
		splitLine = fix_line(line)
		if(splitLine[0] == "@prefix"):
			dict_prefix[splitLine[1][0:-1]] = splitLine[2][1:-1]
			continue
		if(len(splitLine) == 4):
			subj = replace_prefix(splitLine[0], dict_prefix)
			pred = replace_prefix(splitLine[1], dict_prefix)
			obj = replace_prefix(splitLine[2], dict_prefix)
		if(len(splitLine) == 3):
			pred = replace_prefix(splitLine[0], dict_prefix)
			obj = replace_prefix(splitLine[1], dict_prefix)
		if(len(splitLine) == 2):
			obj = replace_prefix(splitLine[0], dict_prefix)
		if("@" not in obj):
			list_triples.append([subj, pred, obj])
		
	for subj, pred, obj in list_triples:
		newInsert = "INSERT INTO rdf_triple VALUES (\""+subj+"\", \""+pred+"\", \""+obj+"\");"
		dbConn.execute(newInsert)

	# Use dbConn.commit to push the query.
	dbConn.commit()
	
	dbConn.close()
	rdfFile.close()

	print("files closed")

if __name__ == "__main__":
	main()
