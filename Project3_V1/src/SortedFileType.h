/*
 * SortedFileType.h
 *
 *  Created on: Mar 8, 2013
 *      Author: gaurab
 */

#ifndef SORTEDFILETYPE_H_
#define SORTEDFILETYPE_H_
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

struct SortInfoDB
{
    OrderMaker *myOrder;
    int runLength;
};

class SortedFileType :public GenericDBFile{
public:
	SortedFileType();
	~SortedFileType();
	// DB file functions moved here //
	int Create (char *fpath,fType file_type,void *startup);
	int Open (char *fpath);
	int Close ();
	void Load (Schema &myschema, char *loadpath);
	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

	// Reading / writing in the meta-data file plan to keep it Generic
	void ReadMetaData(char *f_path);
	void WriteMetaData(char* f_path, char* f_type, void* startup);


private:

	int GetRecord (Record &fetchme, Page &page);
	void AddRecord (Record& rec);
	void Merge(Pipe* out);
	int Compare(Record* lhs, Record* rhs);

	static const int PIPE_SIZE = 100;
	File tempfile;
	BigQ *bigQ;

	Pipe *input;
	Pipe *output;
	off_t pnumber;
	off_t temppnum;
	Page tempp;

	bool currentlyMerging;
	Mode mode;

};


#endif /* SORTEDFILETYPE_H_ */
