#include <iostream>
#include <vector>
#include <algorithm>
#include "BigQ.h"

struct temCompStruct {
	OrderMaker& sorter;
	temCompStruct(OrderMaker &sortorder):sorter(sortorder)
 	{

	}
  //Default operator for replacing it with comparison Engine method
  	bool operator() (Record* rec1, Record* rec2)
	{
		ComparisonEngine cmp;
		//comparison as per the bit values of records
		 if(cmp.Compare(rec1, rec2, &sorter) < 0){
			 	 cout<<"Comparision true:"<<endl;
				return true;
		 }
		 cout<<"Comparision false:"<<endl;
		return false;
	}

};

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen):orderSort(sortorder){
	// read data from in pipe sort them into runlen pages
	this->input=&in;
	this->output=&out;
	this->runLen=runlen;
	//this->orderSort=sortorder;
	metaFile="meta.bin";
	file.Open(0,metaFile);
	Page page;
	Record read,copyRec;
	vector<Record *>run;
	int pageRunCount=0,offVal=0,n=0,recNo=1;

	ComparisonEngine cmp;


	offSet.push_back(offVal);
	cout<<"BigQ TPPMS phase 1"<<endl;
	//TPPMS Phase: 1

	while(input->Remove(&read) == 1){
			if(page.Append(&read) == 0)
			{
				while(page.GetFirst(&copyRec))
				{
					Record *vectorRec = new Record();
					vectorRec->Copy(&copyRec);
					run.push_back(vectorRec);
				}
				pageRunCount++;

				if(pageRunCount == runlen)
				{
					//used to compare the Records as per the comparison engine defined in comapsionEngine.cc
					std::sort(run.begin(),run.end(),temCompStruct(sortorder));
					for(n = 0; n < run.size(); n++){
						if(page.Append(run[n]) == 0){
							file.AddPage(&page, offVal);
							offVal++;

							page.EmptyItOut();
							page.Append(run[n]);
						}
						delete run[n];
					}

					file.AddPage(&page, offVal);
					offVal++;
					run.clear();
					int tracker = offVal;
					offSet.push_back(tracker);
					page.EmptyItOut();
					pageRunCount = 0;
				}
				page.Append(&read);
			}

		}

	while(page.GetFirst(&copyRec))
		{
				Record *vecRec = new Record();
				vecRec->Copy(&copyRec);
				run.push_back(vecRec);
		}
	//used to compare the Records as per the comparison engine defined in comapsionEngine.cc
	std::sort(run.begin(),run.end());

		for(int index = 0; index < run.size(); index++){
			if(page.Append(run[index]) == 0){
				file.AddPage(&page, offVal);
				offVal++;

				page.EmptyItOut();
				page.Append(run[index]);
			}
			delete run[index];
		}
		file.AddPage(&page, offVal);
		offVal++;
		page.EmptyItOut();
		run.clear();

		cout<<"BigQ phase-2"<<endl;
	//TPPMS Phase: 2
		int totalOffset;
		int initalSize = offSet.size();
		offSet.push_back(file.GetLength());

		vector<int> offSetUpdate;
		vector<int> skipCount;
		vector<Page *> pageVect;
		vector<Record *> recVect;

		pageVect.reserve(initalSize);
		recVect.reserve(initalSize);

		offSetUpdate.reserve(initalSize);
		skipCount.reserve(initalSize);

		for(int i = 0; i < initalSize; i++){
				Page *PTemp = new Page();
				file.GetPage(PTemp,offSet[i]);
				pageVect.push_back(PTemp);
				Record *RTemp = new Record();
				pageVect[i]->GetFirst(RTemp);
				recVect.push_back(RTemp);
				offSetUpdate.push_back(1);
				skipCount.push_back(0);
		}

		int outputCounter=0;
		int checker=-1;
		while(true){
			checker = -1;
			for(int i = 0; i< initalSize; i++){
				if(skipCount[i] == -1){
					continue;
				}
				//for first run
				if(checker == -1){
					checker = i;
				}
				else{
					//Test to see if the new record is smaller than the mindex
					if(cmp.Compare(recVect[checker],recVect[i],&sortorder) > 0){
						checker = i;
					}
				}
			}
			//break case at end or run
			if(checker == -1){
				break;
			}
			//input to the output pipe
			output->Insert(recVect[checker]);

			if(!pageVect[checker]->GetFirst(recVect[checker])){ //No record was found

				totalOffset = offSet[checker]+offSetUpdate[checker];
				if((totalOffset < file.GetLength()-1)&& ((totalOffset) < offSet[checker+1]))
				{
					file.GetPage(pageVect[checker], offSet[checker]+offSetUpdate[checker]);
					offSetUpdate[checker]++;
					pageVect[checker]->GetFirst(recVect[checker]);
				}
				else{
					cout << "Run Exhausted " << checker << "." << endl;

					skipCount[checker] = -1;
				}
			}
		}


	file.Close();
	out.ShutDown ();
}
/*		bool orderTest=true;
		ComparisonEngine cmp;
			 if(cmp.Compare(rec1, rec2, &sorter) < 0){
			 	 orderTest=true;
			 }
			 else{
			 	 orderTest=false;
			 }

			*/


BigQ::~BigQ () {
}
