/*
 * GenericDBFile.h
 *
 *  Created on: Mar 6, 2013
 *      Author: gaurab
 */

#ifndef GENERICDBFILE_H_
#define GENERICDBFILE_H_


#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
//#include "DBFile.h"
#include "Defs.h"

//Added to Defs.h to make it generic for all (DBFile.h and DBFile.cc)
//typedef enum {heap, sorted, tree} fileType;

struct SortInfoGen
{
    OrderMaker *myOrder;
    int runLength;
};

class GenericDBFile {
public:
	GenericDBFile();
	virtual ~GenericDBFile();
	virtual int Create (char *fpath, fType file_type, void *startup) = 0;
	virtual int Open (char *fpath) = 0;
	virtual int Close () = 0;
	virtual void Load (Schema &myschema, char *loadpath) = 0;
	virtual void Add(Record& add) = 0;
	virtual int GetNext(Record &fetch) = 0;
	virtual int GetNext (Record &fetchme, CNF &cnf, Record &literal) = 0;
	virtual void MoveFirst() = 0;
	// A generic utility function, to convert .bin file names to .metadata file names
		static char* GetMetadataFileName(char* binFileName);

		// Metadata reading / writing functions, each derived class should implement their own specifics

		// Read metadata for bin file pointed by f_path
		virtual void ReadMetaData(char *f_path) = 0;

		// Write metadata to metadata file
		virtual void WriteMetaData(char* f_path, char* f_type, void* startup) = 0;

		// Stores the file name //
		static char* filename;
protected:
	// These are the instances of the types various base classes
	// representing their DBFile's current instance.
	File file;
	Page page;
	Record rec;

	//This shows the current page number in the current file
	off_t pageNum;

	OrderMaker* sortOrder;
	int runlength;
	SortInfoGen sortInfo;

};

#endif /* GENERICDBFILE_H_ */
