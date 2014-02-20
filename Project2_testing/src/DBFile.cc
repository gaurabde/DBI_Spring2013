/* Author: Gaurab Dey
 * UFID: 7802-9140
 * Email: gaurabde@ufl.edu
 * Version: 2.2.3
 * Project: 2.2 (Heap and Sort file type implementation)
 * */
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
#include "HeapFileType.h"
#include "SortedFileType.h"

char* GenericDBFile::filename;

DBFile::DBFile () {
	File file;
	//genFileType=NULL;
}

/*This method will create new file from f_type of DBFile. using the file type that is provided
 * fileLen=0 if the file is created for the first time.
 * Return case: 1 if file created
 * 				System error if failed.
**/
int DBFile::Create (char *f_path, fType f_type, void *startup)
{
  // Open the file using the f_path variable
  // (Zero) is passed to create a new file, deleting old file (if any exist)
	//cout<<"Reached Create";
	type = f_type;

		// Decide on the basis of type, which concrete class object to create
		switch(type)
		{
			case heap:
				genFileType = new HeapFileType();
				break;

			case sorted:
				genFileType = new SortedFileType();
				break;
		}

		genFileType->Create(f_path, f_type,startup);
	return 1;

}

/* This method will load the records from the table file provided in the loadpath with the given schema f_schema.*/
void DBFile::Load (Schema &f_schema, char *loadpath)
{
	genFileType->Load(f_schema,loadpath);

}

/* This method  will open the file specified in the path given in f_path
 * Return value depend upon status of the file OPEN or NOT
 * TO BE IMPLEMENTED: exception opening files and handling them
 * */
int DBFile::Open (char *f_path)
{
	// Get the Metadata file name
		char *metaDataFile = GenericDBFile::GetMetadataFileName(f_path);

		cerr<<"Open DB: "<<metaDataFile<<endl;
		// Input Stream to Meta Data File
		ifstream inStream;

		// variable for text value holder from the file
		string str;

		inStream.open(metaDataFile);

		// Metadata file not found, exiting
		if(!inStream)
		{
		      cerr << "Error: Header file could not be opened" << endl;
		      exit(EXIT_FAILURE);
		}

		// Read from the stream
		inStream >> str;
		cout<<"Open DB file type: "<<str<<" "<<str.compare("heap")<<endl;
		inStream.close();
		// Deciding the type of genDB
		if(str.compare("sorted") == 0)
		{
			genFileType = new SortedFileType();
		}

		else{
			if(str.compare("heap") == 0)
			{
				genFileType = new HeapFileType();
				cout<<"Heap Open call "<<endl;

			}
			else
			{
				cerr << "Error: Invalid File Type in  metadata file Type - Permitted values( sorted/heap)" << endl;
				exit(EXIT_FAILURE);
			}

		}

		// now open relevent genDB instance
		genFileType->Open(f_path);

}

/* This method will put the page number counter (pageNum) to first position, to read the records from starting
 * Default postion of pageNum=0
 * Check File.cc-->GetPage
 *   */
void DBFile::MoveFirst ()
{
  /*
  page.EmptyItOut();
  pageNum = 0;
  file.GetPage(&page, 0);
  */
	genFileType->MoveFirst();
}

/* This method will simply close thMoveFirste file opened during the process to avoid locks in next read/write
 * Return case: 0 for file close
 * 				System error when file not closed
 * */
int DBFile::Close ()
{
  //file.Close();

  return genFileType->Close();
}

/* This method will add the records send to it in current page variable (Page) and
 * increment the page number (pageNum) to replicate record Number.
 * Once the page variable is full will add Page to the File
 *
 * Case: only when the page is full ADD will return 0, case handled and page added to file
 * current page is emptied and New Record added to new page.
 * */
void DBFile::Add (Record &rec)
{
	/*
//For page full return ZERO
  if(page.Append(&rec)==0)
    {
      file.AddPage(&page,pageNum);
      page.EmptyItOut();
      page.Append(&rec);
      pageNum++;
    }
    */

	genFileType->Add(rec);
}

/* This method will get the next record in the file and will return 0 for no record otherwise 1
 * (Return case is because of manipulation in test.cc)
 * Return case: 0 for no record
 * 				1 for next record
 * */
int DBFile::GetNext (Record &fetchme)
{
	/*
  if(page.GetFirst(&fetchme)==0)
    {
      pageNum++;
      //File check
      if((pageNum+1)== file.GetLength())
      {
    	  return 0;
      }f_schema, char *loadpath
      file.GetPage(&page,pageNum);
      //Page check for leftover of records smaller then pagesize
      if(page.GetFirst(&fetchme)==0)
      {
    	  return 0;
      }
    }
    */
	return genFileType->GetNext(fetchme);
}

/* This method have same features as GetNext but CNF as add-on. It will get next record based on CNF Query
 * All records are fetched and based on CNF using the comparison engine, records are filtered out
 * CASE: Need to check on optimization of the method in fetching and filter or filtering and fetching
 * Return case: 0 if not found as per CNF
 * 				1 if matching record found as per CNF
 * */
int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal)
{
	/*
  ComparisonEngine compE;
  int fileCase=1;

  while(fileCase)
    {
      //Using Recurrence to manipulate the return value
      if(GetNext(fetchme)==0)
      {
    	  return 0;
      }
      //Matching records as per the CNF
      if(compE.Compare(&fetchme,&literal,&cnf)==1)
      {
  	  	  fileCase=0;
      }
    }
	 */

  return genFileType->GetNext(fetchme,cnf,literal);
}

DBFile::~DBFile(){
	//delete *file;
	delete genFileType;
}
