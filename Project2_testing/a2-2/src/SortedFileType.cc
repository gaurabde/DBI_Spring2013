/*
 * SortedFileType.cc
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

#include "GenericDBFile.h"
#include "SortedFileType.h"

SortedFileType::~SortedFileType() {
	// TODO Auto-generated destructor stub
}

//*****************************************************
/* SORTED FILE Section
 *
 * */
//*****************************************************
const int SortedFileType::PIPE_SIZE;

SortedFileType::SortedFileType(){
	// Input and Ouput Pipes created
		input = new Pipe(100);
		output = new Pipe(100);

	// default mode is readmode, Merge State set to false to avoid any work on data in pipe and BigQ intiallized to NULL
		mode = readMode;
		bigQ = NULL;
		currentlyMerging = false;
}


int SortedFileType::Create(char *fpath, fType file_type, void *startup){
	//Default file that should be used across the class as generic file name
		filename = fpath;
	// metadata file name from GenericDBFile used by both sorted and heap types
		char* metaDataFile = GetMetadataFileName(filename);
		cout<<"Sorted Create: "<<metaDataFile<<endl;
	//Writing the metedata details into the meta file
		WriteMetaData(metaDataFile, "sorted", startup);

		file.Open(0, filename);

}

int SortedFileType::Open (char *f_path){

//		GenericDBFile::GetMetadataFileName(f_path);
	cout<<"Sorted Open called"<<endl;
		file.Open(1,f_path);

}

int SortedFileType::Close (){
	file.Close();
	return 1;
}

void SortedFileType::Load (Schema &myschema, char *f_path){
	cout<<"Sorted Load: "<<mode<<endl;

	if (mode == readMode)
		{
			delete bigQ;
			//sortOrder.Print();

			bigQ = new BigQ(*input, *output, *sortOrder, runlength);
			cout<<"BigQ created"<<endl;
		}

		FILE *f = fopen(f_path,"r" );
		//Adding data into the pipe for processing
		while(rec.SuckNextRecord(&myschema,f)==1)
		{
		   //Adding Records into the input pipe
			input->Insert(&rec);
			cout<<"Intserd pipe"<<endl;
			rec.Print(&myschema);
		}

		//Write mode activated to avoid recreation of BigQ
		mode = writeMode;;


}

void SortedFileType::Add (Record &addme){

	if (mode == readMode)
		{
			if (bigQ != NULL)
			{
				delete bigQ;
				bigQ = NULL;
				// Create the new BigQ

			}
			bigQ = new BigQ(*input, *output, *sortOrder, runlength);
			cout<<"Sorted Add New BigQ created"<<endl;
		}

	//Write mode activated to avoid recreation of BigQ
		mode = writeMode;
	// Added records in the input pipe
	input->Insert(&addme);
	cout<<"sorted Add rec in-pipe"<<endl;
}
void SortedFileType::MoveFirst (){
	//mode=readMode;
	cout<<"Sort Move first called"<<mode<<endl;
	if (mode == writeMode)
		{
			Merge(output);
		}
		// Same implementation as the implementation for the GenericDBFile //
	page.EmptyItOut();
	cout << "Global page index now set to 0" << endl;
	pageNum = 0;
	file.GetPage(&page,pageNum);
	if(file.GetLength() > 0){
		file.GetPage(&page,pageNum);
	}else{
		cout<<"Current length is ZERO or less";
	}
}
int SortedFileType::GetNext (Record &fetchme){
	if(page.GetFirst(&fetchme) == 0){ //Check to see if anything is returned by our current page p
		pageNum++; //Update page to the next one
		if(pageNum < file.GetLength()-1){ //If nothing is returned, we check to see if p is the last page
			file.GetPage(&page,pageNum);
			page.GetFirst(&fetchme);
			return 1;
		}
		//	cout << "Nothing left in the file!" << endl;
		return 0;//No records left
	}
	return 1;
}

int SortedFileType::GetNext (Record &fetchme, CNF &cnf, Record &literal){
	ComparisonEngine comp;

	int ret = GetNext(fetchme);
	//cout << "Got past the initial ret" << endl;
	if(ret == 0){
		return 0; //Nothing in the file, so nothing to do here!
	}
	while(!comp.Compare (&fetchme, &literal, &cnf)){
	//	cout << "Comparing records!" << endl;
		ret = GetNext(fetchme);

		if(ret == 0){
			//cout << "Nothing found, time to leave. " << endl;
			return 0; //Nothing in the file, so nothing to do here!
		}
		/*if(p.GetFirst(&fetchme) == 0){
		 globalPageIndex++; //Update page to the next one
		 if(globalPageIndex < f.GetLength()-1){ //If nothing is returned, we check to see if p is the last page
		 f.GetPage(&p,globalPageIndex);
		 p.GetFirst(&fetchme);
		 }

		 return 0;//No records left
		 }*/
	}

	return 1;

}

void SortedFileType::Merge(Pipe* out)
{
	cout<<"Merge started"<<endl;
	// Merging Currently
	currentlyMerging = true;

	// Close the input pipe
	input->ShutDown();

	// Merge The Records into temporary file, using temporary page number //
	temppnum = 0;
	tempfile.Open(0, "temp.bin");

	//Records needed for comparisons //
	Record *rec1 = new Record;
	Record *rec2 = new Record;
	Page p;


	// Get the records from pipe and file, for comparisons //
	int frompipe = out->Remove(rec1);
	//cout << "I am here  2" <<endl;
	int fromfile = GetNext(*rec2);

	// Set current page number to 0
	pageNum = 0;

	// Keep on getting, the records from file and pipe for merging with comparisons //
	while(frompipe || fromfile)
	{
		// Record present in both file and pipe //
		if (frompipe && fromfile)
		{
			//Compare the records to find sorting order //
			int result = Compare(rec1, rec2);

			// rec2 is smaller as per the sorting order //
			if (result)
			{
				AddRecord(*rec2);
				rec2 = new Record;

				// Get the next record from the file //
				fromfile = GetNext(*rec2);
			}

			// rec1 is greater //
			else
			{
				AddRecord(*rec1);
				rec1=new Record;

				// Get the next record from the pipe //
				frompipe=out->Remove(rec1);
			}
		}

		// Record present only in file //
		else if (fromfile > 0)
		{
			// Add the record and fetch the next record from file //
			AddRecord(*rec2);
			rec2=new Record;
			fromfile=GetNext(*rec2);
		}

		// Record Present only in pipe //
		else if (frompipe > 0)
		{
			// Add the record and fetch the next record from file //
			AddRecord(*rec1);
			rec1=new Record;
			frompipe=out->Remove(rec1);
		}
	}

	// Add this page to temp file //
	tempfile.AddPage(&tempp,temppnum);
	tempp.EmptyItOut();
	temppnum++;
	file.Close();
	tempfile.Close();

	// Change the name of file from temp to correct file name, using cstdio function rename //
	rename ("temp",filename );

	// Now open this file.
	file.Open(1, filename);

	// Merging done
	currentlyMerging = false;
}

void SortedFileType::AddRecord(Record& rec)
{
	// Try to append the record in temporaray Page //
	if(tempp.Append(&rec)==0)
	{
		// If full than add this page to file and empty it //
		tempfile.AddPage(&tempp,temppnum);
		tempp.EmptyItOut();

		// Append the record to the empty page now //
		tempp.Append(&rec);
		temppnum++;
	}
}
int SortedFileType::Compare(Record* lhs, Record* rhs)
{
	ComparisonEngine engine;
	int result;

	// Check the current sort order
	// sortOrder.Print();

	// Get the result from Comparison Engine
	result = engine.Compare(lhs, rhs, sortOrder);

	// If result < 0 return 0 else return 1.
	return ((result < 0) ? 0 : 1) ;
}

void SortedFileType::WriteMetaData(char* f_path, char* f_type, void* startup)
{
	cout<<"Sort Write method: "<<f_path<<endl;

	SortInfoDB* order = reinterpret_cast<SortInfoDB*>(startup);
	int runlen = order->runLength;
	sortOrder = order->myOrder;

	// These attributes needs to be written from the orderinfo to metadata file
	int numAtts=sortOrder->NumAtt();
	int* whichAtts=sortOrder->GetAtts();
	Type* whichTypes=sortOrder->GetTypes();

	// write the type of metadatafile
	char *metaFile = f_path;
	ofstream metafile(metaFile);
	metafile << f_type << endl;
	metafile << runlen << endl;
	metafile << numAtts << endl;

	for (int i = 0; i < numAtts; i++)
	{
		metafile <<whichAtts[i] << endl;
		if (whichTypes[i] == Int)
			metafile << "Int" << endl;
		else if (whichTypes[i] == Double)
				metafile << "Double" << endl;
			else
				metafile << "String" << endl;
	}
	// close the file
	metafile.close();
}

void SortedFileType::ReadMetaData(char* f_path)
{
	int numAtts;
	int whichAtts[MAX_ANDS];
	Type whichTypes[MAX_ANDS];
	filename = f_path;


	char* metaDataFile = GenericDBFile::GetMetadataFileName(f_path);


	// Input Stream to Meta Data File
	ifstream inStream;
	int num;
	string str;
	inStream.open(metaDataFile);

	if(!inStream){
	  cerr << "Error: Header file could not be opened" << endl;
	  exit(EXIT_FAILURE);
	}
	// getting type of file and other variables
	inStream >> str;
	inStream >> num;
	runlength = num;

	inStream >> num;
	numAtts = num;
	for(int i=0; i<numAtts; i++){
		// Get the number of the attribute
		inStream >> num;
		whichAtts[i] = num;
		inStream >> str;

		if (str.compare("Int") == 0)
			whichTypes[i] = Int;
		else if (str.compare("Double") == 0)
				whichTypes[i] = Double;
			else
				whichTypes[i] = String;
	}

	printf("NumAtts = %5d\n", numAtts);
		for (int i = 0; i < numAtts; i++)
		{
			printf("%3d: %5d ", i, whichAtts[i]);
			if (whichTypes[i] == Int)
				printf("Int\n");
			else if (whichTypes[i] == Double)
				printf("Double\n");
			else
				printf("String\n");
		}
	inStream.close();
}

