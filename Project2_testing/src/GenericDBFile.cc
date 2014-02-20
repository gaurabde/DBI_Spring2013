/*
 * GenericDBFile.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: gaurab
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "GenericDBFile.h"

GenericDBFile::GenericDBFile() {
	// TODO Auto-generated constructor stub

}

char* GenericDBFile::GetMetadataFileName(char* fpath)
{
	// Generate the name of metadatafile, for file named abc.bin, metadata file would be
	// named as abc.metadata

	// Take a large enough buffer to store the name
	char *metaDataFile = new char[100];

	// Copy the given file name into metadatafile name
	strncpy(metaDataFile, fpath, strlen(fpath) + 1);

	// Now truncate the trailing .bin from the metadata file name, 4 is the length of ".bin"
	int length = strlen(metaDataFile);

	// Ensure a valid string, should not be needed but still ...
	metaDataFile[length-4] = '\0';
	cout<<"Generic file name: "<<metaDataFile<<endl;
	// Add .metadata to it, 9 is the length of ".metadata"
	metaDataFile = strncat(metaDataFile, ".metadata", 9);
	cout<<"Generic file name (After Edit): "<<metaDataFile<<endl;
	return metaDataFile;
}


GenericDBFile::~GenericDBFile() {
	// TODO Auto-generated destructor stub
}

