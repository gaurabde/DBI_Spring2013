#ifndef DBFILE_H
#define DBFILE_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "BigQ.h"
#include "Pipe.h"
//
#include "GenericDBFile.h"

//typedef enum {heap, sorted, tree} fType;



class DBFile {

public:
	DBFile (); 
	~DBFile();

	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();

	void Load (Schema &myschema, char *loadpath);

	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);
private:
	GenericDBFile *genFileType;
	// Global variable created to keep all methods on same status respect to records.
		File file;
		Page page;
		Record record;
		int pageNum;
		char fileChecker;
		fType type;
	//Global Variables for Sorted File types will use the same variable for the file,page,record
		Pipe *in,*out;
		OrderMaker ordermaker;
		BigQ *bigQ;
		//fType fileChecker=NULL;
	//SortDBFile sortFile;

};


#endif
