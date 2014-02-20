/* Author: Gaurab Dey
 * UFID: 7802-9140
 * Email: gdey@cise.ufl.edu
 * Version: 1.4
 * Project: 1 (only Heap with test.cc)
 * */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"


// Global variable created to keep all methods on same status respect to records.
	File file;
	Page page;
	Record record;
	int pageNum;

DBFile::DBFile () {
	File file;
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

	if (f_type != heap)
		{
			cerr << "BAD File type " << f_type << "\n";
			return 0;
		}
	if (f_path == NULL || !strlen(f_path))
	{
		cerr << "No file name passed to create()\n";
		return 0;
	}

	if(f_type==heap){
			file.Open(0,f_path);
			string mfile;
			mfile.append(f_path);
			mfile.append(".header");
			//ofsteam  provides an interface to write data to files as output streams.

			ofstream dbF;
			dbF.open(mfile.c_str(),ios::out);
			//In this case, writes 0. Track details for the file created
			dbF << f_type;
			dbF.close();
			return 1;
		}
	return 0;

}

/* This method will load the records from the table file provided in the loadpath with the given schema f_schema.*/
void DBFile::Load (Schema &f_schema, char *loadpath)
 {
	FILE *F = fopen(loadpath,"r" );

   //pageNum is set to ZERO to load from starting of the file. File is opened in READ Mode "r"
	pageNum = 0;

   /* Records are fetched from opened file f, using while all records are fetched from file till all the records are added
    * in pages (return 1)or there some error getting the records.
    * Recorded are added to the file using the Add method*/
	while(record.SuckNextRecord(&f_schema,F)==1)
     {
       Add(record);
     }

   // Added records to the pages is added to file to complete the Load method.
   file.AddPage(&page,pageNum);

   //version:1.2 remove the older added pages to re-use the Page variable rather then creating new every time.
   page.EmptyItOut();

   //version:1.3 reseting the page value to 0 to avoid missing records because of reset in previous stage.
   pageNum=0;
   //Close the file to avoid locks on them and can be easily read in next call.
   //version: 1.4 TO BE implemented : check for file exceptions if it is closed or not.
   fclose(F);
}

/* This method  will open the file specified in the path given in f_path
 * Return value depend upon status of the file OPEN or NOT
 * TO BE IMPLEMENTED: exception opening files and handling them
 * */
int DBFile::Open (char *f_path)
{
  file.Open(1,f_path);
  return 0;
}

/* This method will put the page number counter (pageNum) to first position, to read the records from starting
 * Default postion of pageNum=0
 * Check File.cc-->GetPage
 *   */
void DBFile::MoveFirst ()
{
  pageNum = 0;
  file.GetPage(&page, 0);
}

/* This method will simply close thMoveFirste file opened during the process to avoid locks in next read/write
 * Return case: 0 for file close
 * 				System error when file not closed
 * */
int DBFile::Close ()
{
  file.Close();
  return 0;
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
//For page full return ZERO
  if(page.Append(&rec)==0)
    {
      file.AddPage(&page,pageNum);
      page.EmptyItOut();
      page.Append(&rec);
      pageNum++;
    }
}

/* This method will get the next record in the file and will return 0 for no record otherwise 1
 * (Return case is because of manipulation in test.cc)
 * Return case: 0 for no record
 * 				1 for next record
 * */
int DBFile::GetNext (Record &fetchme)
{
  if(page.GetFirst(&fetchme)==0)
    {
      pageNum++;
      //File check
      if((pageNum+1)== file.GetLength())
      {
    	  return 0;
      }
      file.GetPage(&page,pageNum);
      //Page check for leftover of records smaller then pagesize
      if(page.GetFirst(&fetchme)==0)
      {
    	  return 0;
      }
    }
  return 1;
}

/* This method have same features as GetNext but CNF as add-on. It will get next record based on CNF Query
 * All records are fetched and based on CNF using the comparison engine, records are filtered out
 * CASE: Need to check on optimization of the method in fetching and filter or filtering and fetching
 * Return case: 0 if not found as per CNF
 * 				1 if matching record found as per CNF
 * */
int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal)
{
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

  return 1;
}
