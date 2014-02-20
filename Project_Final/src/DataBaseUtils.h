/*
 * DataBaseUtils.h
 *      Author: gaurab
 */

#ifndef DATABASEUTILS_H_
#define DATABASEUTILS_H_


#include <iostream>
#include "ParseTree.h"
#include <fstream>
#include "DBFile.h"
#include <time.h>

class DataBaseUtils {
public:
	DataBaseUtils();
	virtual ~DataBaseUtils();
	void create(char* tableName,char* catalog,AttList* newAtt);
	void insert(char* tableName, char* fileName, char* catalog);
	int find(char* tableName,char* catalog);
	void drop(char* tablename);

	int returnType;

};

#endif /* DATABASEUTILS_H_ */
