/*
 * HeapFileType.cc
 *
 *  Created on: Mar 8, 2013
 *      Author: gaurab
 */


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

#include "HeapFileType.h"

HeapFileType::~HeapFileType() {
	// TODO Auto-generated destructor stub
}
//*****************************************************

/*
 * Heap file implementation
 */
HeapFileType::HeapFileType(){

}

// Write metadata file, for heap
void HeapFileType::WriteMetaData(char* f_path, char* f_type, void * startup)
{
	// Get the metadata file name
	char *metaFile = GetMetadataFileName(f_path);

	// create an output stream, that points to the metafile
	ofstream metafile(metaFile);

	// For heap file just write heap
	metafile << "heap\n";

	metafile.close();
	cout<<"Heap Write done"<<endl;
}

int HeapFileType::Create(char *fpath, fType file_type, void *startup){
	// We just need to open the file, type using the f_path variable
		// 0 is passed to create a completely new file, erasing old file
		// if any existed

		// write metadata file
		WriteMetaData(fpath, "heap", startup);
		  file.Open(0,fpath);
		  return 1;

}

int HeapFileType::Open (char *fpath){
	cout<<"HeapOpen: "<<fpath<<endl;
	file.Open(1,fpath);
	return 1;
}

int HeapFileType::Close(){
	file.Close();
	return 1;
}

void HeapFileType::MoveFirst(){
	page.EmptyItOut();
	pageNum = 0;
	file.GetPage(&page,pageNum);
}

void HeapFileType::Load(Schema &mySchema, char *loadpath){
	FILE *F = fopen(loadpath,"r" );

   //pageNum is set to ZERO to load from starting of the file. File is opened in READ Mode "r"
	pageNum = 0;

   /* Records are fetched from opened file f, using while all records are fetched from file till all the records are added
	* in pages (return 1)or there some error getting the records.
	* Recorded are added to the file using the Add method*/
	while(rec.SuckNextRecord(&mySchema,F)==1)
	 {
	   Add(rec);
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

void HeapFileType::Add(Record &addme){
	//For page full return ZERO

	  if(page.Append(&rec)==0)
	    {
	      file.AddPage(&page,pageNum);
	      page.EmptyItOut();
	      page.Append(&rec);
	      pageNum++;
	      cout<<"Heap Add rec=0"<<endl;
	      return;
	    }else{
	    	file.AddPage(&page,pageNum); //We overwrite the file's page with the new one
	    	//cout << "Page re-added" <<endl;
	    	pageNum++;
	    	cout<<"Heap Add rec=1"<<endl;
	    	return;
	    }
}

int HeapFileType::GetNext(Record &fetchme){
	//The first thing to do is fetch through the current page.
	//If the page returns 0, then we need to load the next page and get from there
	//If the GPI > f's size, then we've reached the end of the records

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
int HeapFileType::GetNext (Record &fetchme, CNF &cnf, Record &literal)
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


