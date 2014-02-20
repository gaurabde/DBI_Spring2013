
#include <iostream>

#include "ParseTree.h"
#include "QueryPlan.h"
#include "DataBaseUtils.h"

#include <fstream>
#include "DBFile.h"
#include <time.h>


using namespace std;

extern "C" {
int yyparse(void);   // defined in y.tab.c
}

/*

*/


const char *settings = "test.cat";
char *catalog_path, *dbfile_dir, *tpch_dir = NULL;


bool flag = true;

void setup();
bool startMenu();

int main () {

	setup();
	streambuf *sb,*tmp;
	while(flag == true)
	{
		startMenu();
	}
}
void setup () {
	FILE *fp = fopen (settings, "r");
	if (fp) {
		char *mem = (char *) malloc (80 * 3);
		catalog_path = &mem[0];
		dbfile_dir = &mem[80];
		tpch_dir = &mem[160];
		char line[80];
		fgets (line, 80, fp);
		sscanf (line, "%s\n", catalog_path);
		fgets (line, 80, fp);
		sscanf (line, "%s\n", dbfile_dir);
		fgets (line, 80, fp);
		sscanf (line, "%s\n", tpch_dir);
		fclose (fp);
		if (! (catalog_path && dbfile_dir && tpch_dir)) {
			cerr << " Test settings file 'test.cat' not in correct format.\n";
			free (mem);
			exit (1);
		}
	}
	else {
		cerr << " Test settings files 'test.cat' missing \n";
		exit (1);
	}
	cout << " \n** IMPORTANT: MAKE SURE THE INFORMATION BELOW IS CORRECT **\n";
	cout << " catalog location: \t" << catalog_path << endl;
	cout << " tpch files dir: \t" << tpch_dir << endl;
	cout << " heap files dir: \t" << dbfile_dir << endl;
	cout << " \n\n";

}

bool startMenu(){

	cout<< "\t Enter SQL query :\n"<<endl;

	int x=yyparse();

	DataBaseUtils dbu;//=new DataBaseUtils();

	clock_t init, final;

	init = clock();

	switch (commandType)
	{
		case 1:
		{
			dbu.create(createName,"catalog",newAttList);
			break;
		}
		case 2:
		{
			dbu.insert(createName,fileName,"catalog");
			break;
		}
		case 3:
		{
			dbu.drop(createName);
			break;
		}
		case 4:
			break;
		case 5:
		{
			QueryPlan Q;		//select and execute/print query
			Q.start();
			break;
		}
		case 6:
		{
			flag = false;
			break;
		}
		default:
			break;

	}

	final = clock() - init;
	cout<<"Final Time for Execution : "<<(double)final / ((double)CLOCKS_PER_SEC)<<endl;

	return flag;
}
