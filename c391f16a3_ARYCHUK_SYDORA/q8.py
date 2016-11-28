import sys
import sqlite3

# Takes in a value string and converts it to the proper form
#cases:
# <http:...>, ...:..., "...", ..., "..."@en, "..."@it, "..."^^..:..., "..."^^<http:...>, _:...
#want:
# http:...
def replace_prefix(value, dict_prefix):
	newValue = value
	# check if it has ""
	splitValue = value.split("\"")
	if(len(splitValue) == 3):
		if(not("@" in splitValue[2] and splitValue[2] != "@en")):
			#if yes then we return the value in the quotes
			newValue = splitValue[1]
		else:
			#here we return "@it" for example so that later we can
			#check and ignore this
			newValue = splitValue[2]
	else:
		#replace the <> and keep what is in it since its in the correct form
		splitValue = value.replace('<','>').split('>')
		if(len(splitValue) == 3):
			newValue = splitValue[1]
		else:
			#split by ":" and look up the prefix and replace it accordingly
			splitValue = value.split(":")
			if(len(splitValue) == 2):
				try:
					newValue = dict_prefix[splitValue[0]]+splitValue[1]
				except:
					#then the prefix didnt exist so we just
					#place it in the table as is (when we have _:...)
					pass
	return newValue

# Takes a line and splits it so that its grouped properly and ingnores spacing 
# only when the spacing is not in quotations
def fix_line(line):
	newLine = []
	newWord = ""
	quotes = False
	i = 0
	line = line.replace("\t", " ")
	line = line.replace("\n", "")
	while(i < len(line)):
		#check if we are in quotes or not and at a space
		if((not quotes) and (line[i] == " ")):
			#save the word and continue
			if(newWord != ""):
				newLine.append(newWord)
			newWord = ""
		else:
			#add the character to the string and continue
			if(line[i] == "\""):
				#we just entered or exited quotes
				quotes = not quotes
			newWord += line[i]
		i+=1
	
	#add the last word (the ",",":",".")
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
	
	#keep track of these values
	subj = ""
	pred = ""
	obj = ""
	dict_prefix = {}
	list_triples = []
	previous_symbol = "."
	error = False
	lineNum = 0
	# go over the lines and save the prefixes in the dictionary and the rest in triples
	# converting then saving them as we go and doing error checking based on the 
	# previous lines last symbol
	for line in rdfFile:
		lineNum += 1
		splitLine = fix_line(line)
		if(splitLine[0] == "@prefix"):
			dict_prefix[splitLine[1][0:-1]] = splitLine[2][1:-1]
			previous_symbol = splitLine[-1]
			continue
		if(len(splitLine) == 4):
			# previous_symbol should be a "."
			if(previous_symbol != "."):
				error = True
				break
			subj = replace_prefix(splitLine[0], dict_prefix)
			pred = replace_prefix(splitLine[1], dict_prefix)
			obj = replace_prefix(splitLine[2], dict_prefix)
		if(len(splitLine) == 3):
			# previous_symbol should be a ";"
			if(previous_symbol != ";"):
				error = True
				break
			pred = replace_prefix(splitLine[0], dict_prefix)
			obj = replace_prefix(splitLine[1], dict_prefix)
		if(len(splitLine) == 2):
			# previous_symbol should be a ","
			if(previous_symbol != ","):
				error = True
				break
			obj = replace_prefix(splitLine[0], dict_prefix)
		
		#add them to the list but get rid of the ones where object was an @.. != @en
		if("@" not in obj):
			list_triples.append([subj, pred, obj])
		previous_symbol = splitLine[-1]
		
	if(not error):	
		# do all the inserts to the db
		for subj, pred, obj in list_triples:
			newInsert = "INSERT INTO rdf_triple VALUES (\""+subj+"\", \""+pred+"\", \""+obj+"\");"
			# Use dbConn.execute() to create the execution statement
			dbConn.execute(newInsert)
	else:
		print("There was an Error at line: "+str(lineNum))

	# Use dbConn.commit to push the query.
	dbConn.commit()	
	dbConn.close()
	rdfFile.close()

	print("files closed")

if __name__ == "__main__":
	main()
