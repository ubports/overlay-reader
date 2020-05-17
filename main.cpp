#include "ResourcesParser.h"
#include "ResourcesParserInterpreter.h"

#include <iostream>
#include <sstream>

using namespace std;

int findArgvIndex(const char* argv, char *argvs[], int count);
const char* getArgv(const char* argv, char *argvs[], int count);
void printHelp();

int main(int argc, char *argv[]) {
	const char* path = getArgv("-p", argv, argc);
	const char* type = getArgv("-t", argv, argc);
	const char* id = getArgv("-i", argv, argc);
	const char* name = getArgv("-n", argv, argc);
	int all = findArgvIndex("-a", argv, argc);
	int apk = findArgvIndex("-s", argv, argc);

	if(nullptr == path) {
		printHelp();
		return -1;
	} else if(getArgv("-h", argv, argc) != nullptr) {
		printHelp();
	}

	ResourcesParser parser;
	int err = 0;
	if (apk >= 0)
	{
		zip *z = zip_open(path, 0, &err);
		zip_file *f = zip_fopen(z, "resources.arsc", 0);
		parser.SetResourcesZip(f);
	} else {
		FILE *stream = fopen(path, "rb");
		parser.SetResourcesBin(stream);
	}
	parser.SetupResourcesParser();
	ResourcesParserInterpreter interpreter(&parser);

	if(all >= 0) {
		interpreter.parserResource(ResourcesParserInterpreter::ALL_TYPE);
	}

	if(type) {
		interpreter.parserResource(type);
	}

	if(id) {
		interpreter.parserId(id);
	}

	if (name) {
		interpreter.parserName(name);
	}
	return 0;
}

int findArgvIndex(const char* argv, char *argvs[], int count) {
	for(int i = 0 ; i<count ; i++) {
		if(strcmp(argv, argvs[i])==0) {
			return i;
		}
	}
	return -1;
}

const char* getArgv(const char* argv, char *argvs[], int count) {
	int index = findArgvIndex(argv, argvs, count);
	if(index>=0 && index+1 < count) {
		return argvs[index+1];
	}
	return nullptr;
}

void printHelp() {
	cout <<"rp -p path [-a] [-t type] [-i id]" <<endl<<endl;
	cout <<"-p : set path of resources.arsc or app.apk" <<endl;
	cout <<"-a : show all resources" <<endl;
	cout <<"-t : select the type in resources.arsc to show" <<endl;
	cout <<"-i : select the id of resource to show" <<endl;
	cout <<"-n : select the name of resource to show" << endl;
	cout <<"-s : Input file is apk" << endl;
}
