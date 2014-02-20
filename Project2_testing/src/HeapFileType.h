/*
 * HeapFileType.h
 *
 *  Created on: Mar 8, 2013
 *      Author: gaurab
 */
#ifndef HEAPFILETYPE_H_
#define HEAPFILETYPE_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include "BigQ.h"

#include "GenericDBFile.h"


class HeapFileType :public GenericDBFile{
public:
	HeapFileType();
	~HeapFileType();

	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();

	void Load (Schema &myschema, char *loadpath);

	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

	//Custome methods:
	void WriteMetaData(char* f_path, char* f_type, void* startup);

			// Nothing to be done for Read Metadata
	void ReadMetaData(char *f_path){};
protected:
	File file;
	Page page;
	Record rec;

	//This shows the current page number in the current file
	off_t pageNum;

	OrderMaker* sortOrder;
	int runlength;
};


#endif /* HEAPFILETYPE_H_ */
