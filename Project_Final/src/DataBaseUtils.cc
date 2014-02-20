/*
 * DataBaseUtils.cc

 *      Author: gaurab
 */

#include "DataBaseUtils.h"

#include <iostream>
#include "ParseTree.h"
#include <fstream>
#include "DBFile.h"
#include <time.h>

DataBaseUtils::DataBaseUtils() {
	// TODO Auto-generated constructor stub
	returnType=0;

}

DataBaseUtils::~DataBaseUtils() {
	// TODO Auto-generated destructor stub
}


//delete Table --- opens catalog and writes into the catalog file.. keeps iterating over the file till it finds table
//name = the one to drop ..then skip those while rewriting into the file
void DataBaseUtils::drop(char* tbl_name){
	string line,tmpline;
	int i=0;
	bool flag=false;
	bool found=false;
	bool ffound=false;

	ifstream infile("catalog");
	ofstream outtmpfile("tmp");

	while(!infile.eof()){
		getline(infile,line);	//check line by line for existance of table name
		if (strcmp((char*)line.c_str(),"BEGIN")==0){
			tmpline = line;
			getline(infile,line);
			flag = true;
		}
		if (strcmp(tbl_name,(char*)line.c_str())==0){	//if table found ..hav to drop it
			found=true;									//dont write in file
			ffound=true;
			flag = false;
			while(strcmp((char*)line.c_str(),"END")!=0){
				getline(infile,line);
			}
		}
		else{
			if (flag == true){				//dont write in file
				outtmpfile.write((char*)tmpline.c_str(),tmpline.	length());
				outtmpfile.put('\n');
				flag = false;
			}
		}
		if (found==true && (strcmp((char*)line.c_str(),"END")==0)){			//if not found in entire file
			found=false;			//error
			continue;
		}
		outtmpfile.write((char*)line.c_str(),line.length());
		outtmpfile.put('\n');
	}
	infile.close();
	outtmpfile.close();

	if(!ffound){
		cout<< "\nTable not found"<<endl;
		remove("tmp");
		returnType= -1;
	}

	remove("catalog");
	rename("tmp","catalog");
	if (returnType ==-1)
	{
		cout<<"\nDrop table failed"<<endl;
		return;
	}
	else
	{
		strcat(tbl_name,".bin");
		remove(tbl_name);
		strcat (tbl_name,".header");
		remove(tbl_name);

		cout << "\nDrop table completed"<<endl;
	}
}

//gets catalog and writes table details into the catalog
void DataBaseUtils::insert(char* tbl_name,char* file_name,char* catalog)
{
	DBFile *myDBFile = new DBFile();
	Schema *mysch = new Schema (catalog, tbl_name);
	if (find(tbl_name,catalog)==-1){
	//	CreateTable();
		strcat(tbl_name,".bin");

		myDBFile->Create(tbl_name, heap, mysch);
		myDBFile->Close();

		myDBFile->Open(tbl_name);
		myDBFile->Load(*mysch,file_name);
		myDBFile->Close();
		cout << "INSERT table successful"<<endl;
	}
	else
		cout<< "INSERT table failed"<<endl;
}


void DataBaseUtils::create(char* tbl_name,char* catalog,AttList* newAtt){
	string line;
	char *tmp = tbl_name;
	AttList *tmpAttList = newAtt;

	if(find(tbl_name,catalog)== -1)
	{
		returnType = -1;

	}
	ifstream infile (catalog);
	ofstream outfile(catalog,ios_base::app);

	//write in specified format
	outfile.write("BEGIN\n",6);	//begin tablename aatributes with attribute type end
	while(*(tmp)!='\0'){
		outfile.put(*(tmp));
		tmp++;
	}
	outfile.put('\n');
	tmp = tbl_name;
	while(*(tmp)!='\0'){
		outfile.put(*(tmp));
		tmp++;
	}
	outfile.write(".tbl\n",5);
	while(tmpAttList != NULL){	//put all attr in file
		tmp = tmpAttList->AttInfo->name;
		while(*(tmp)!='\0'){			//attribute name
			outfile.put(*(tmp));
			tmp++;
		}
		if (tmpAttList->AttInfo->code == INT)	//attribute type
			outfile.write(" Int\n",4);
		else if (tmpAttList->AttInfo->code == DOUBLE)
			outfile.write(" Double\n",7);
		else if (tmpAttList->AttInfo->code == STRING)
			outfile.write(" String\n",7);
		outfile.put('\n');
		tmpAttList = tmpAttList->next;
	}
	outfile.write("END\n",4);
	infile.close();
	outfile.close();
	DBFile *DBobj = new DBFile();
	if (returnType ==-1)
	{
		cout << "Create table failed\n";
		return;
	}
	Schema *mysch = new Schema (catalog, tbl_name);
	strcat(tbl_name,".bin");

	DBobj->Create(tbl_name,heap,NULL);
	DBobj->Close();

	cout << "\nCreated table successfully"<<endl;
}

int DataBaseUtils::find(char* tbl_name,char* catalog)
{
	string line;
	char *tmp = tbl_name;
	ifstream infile ("catalog");
	ofstream outfile("catalog",ios_base::app);

	while(!infile.eof()){
		getline(infile,line);
		if (line.compare((string)tbl_name)==0){
			cout << "Table already exists\n";
			infile.close();
			outfile.close();
			return -1;
		}
	}
	infile.close();
	outfile.close();

	return 0;
}
