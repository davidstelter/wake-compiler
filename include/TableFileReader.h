#ifndef HEADER_TABLE_FILE_READER
#define HEADER_TABLE_FILE_READER

#include <istream>
#include <string>
#include "PropertySymbolTable.h"

extern "C" {
	#include "type.h"
}

class TableFileReader {

	public:
		void read(PropertySymbolTable* table, istream& s);

	private:
		std::string readString(istream& s);
		char* readCString(istream& s);
		unsigned char readUInt8(istream& s);
		void readMethod(PropertySymbolTable* table, istream& s);
		void readInheritance(PropertySymbolTable* table, istream& s);
		Type* readType(istream& s);
		void readTypeCommon(Type* type, istream& s);
		Type* readTypeByTag(int tag, istream& s);
		Type* readClassType(istream& s);
		Type* readLambdaType(istream& s);
		Type* readParameterizedType(istream& s);

};

#endif
