#ifndef HEADER_CODE_GENERATOR
#define HEADER_CODE_GENERATOR

#include <iostream>

extern "C" {
	#include "tree.h"
	#include "type.h"
}

#include "ScopeSymbolTable.h"
#include "ClassSpaceSymbolTable.h"
#include "ObjectFileHeaderData.h"
#include "TypeAnalyzer.h"

using namespace std;

class ObjectFileGenerator {

	public:
		ObjectFileGenerator(ostream& file, ClassSpaceSymbolTable* classes, ObjectFileHeaderData* header) : file(file) {this->classes = classes; this->header = header; forceArrayIdentifier = false; }
		void generate(Node* tree);

	private:
		bool forceArrayIdentifier;
		TypeAnalyzer typeanalyzer;
		ObjectFileHeaderData* header;
		ClassSpaceSymbolTable* classes;
		ScopeSymbolTable table;
		ostream& file;
		string classname;

};

#endif
