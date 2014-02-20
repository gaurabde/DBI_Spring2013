#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include <vector>

#include "Pipe.h"
#include "File.h"
#include "Record.h"

using namespace std;

class BigQ {


	Pipe *input; //Input pipe
	Pipe *output; //Output pipe

	File file; //to Write data in file
	OrderMaker &orderSort; //creating sorting Order
	int runLen;

	vector<int> offSet; //Used to keep value offsets to the runs.
	pthread_t worker;
	char *metaFile;
public:

	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();

};
#endif
