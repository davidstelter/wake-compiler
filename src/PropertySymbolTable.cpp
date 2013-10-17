#include "PropertySymbolTable.h"
#include "SemanticError.h"

PropertySymbolTable::PropertySymbolTable(TypeAnalyzer* analyzer, AddressAllocator* allocator) {
	this->analyzer = analyzer;
	alloc = allocator;
	abstract = false;
}

boost::optional<SemanticError*> PropertySymbolTable::addMethod(Type* returntype, vector<pair<string, TypeArray*> >* segments_arguments, Node* body) {
	string name = getSymbolNameOf(segments_arguments);

	if(properties.count(name)) {
		string temp = "duplicate method signature is " + name;
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_METHOD_DEFINITION, temp));
	}

	if(body != NULL && body->node_type == NT_ABSTRACT_METHOD) {
		body = NULL;
		abstract = true;
	}

	Type* method = MakeType(TYPE_LAMBDA);
	method->typedata.lambda.returntype = copyType(returntype);
	method->typedata.lambda.body = body;

	TypeArray* conglomerate = MakeTypeArray();
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		int i;
		for(i = 0; i < it->second->typecount; i++)
			AddTypeToTypeArray(copyType(it->second->types[i]), conglomerate);
	}

	method->typedata.lambda.arguments = conglomerate;

	properties[name] = pair<Type*,string>(method, string());

	return boost::optional<SemanticError*>();
}

boost::optional<SemanticError*> PropertySymbolTable::addProvision(Type* provided, Node* body) {
	string name = analyzer->getNameForType(provided) + "<-";
	if(provided->specialty != NULL) name += provided->specialty;

	if(properties.count(name)) {
		string temp = "duplicate method signature is " + name;
		return boost::optional<SemanticError*>(new SemanticError(MULTIPLE_PROVISION_DEFINITION, temp));
	}

	Type* method = MakeType(TYPE_LAMBDA);
	method->typedata.lambda.returntype = copyType(provided);
	method->typedata.lambda.body = body;

	method->typedata.lambda.arguments = MakeTypeArray(); //TODO injections with curried ctors or arguments!

	properties[name] = pair<Type*,string>(method, string());
	return boost::optional<SemanticError*>();
}

boost::optional<Type*> PropertySymbolTable::find(string name) {
	if(!properties.count(name))
		//throw new SemanticError(PROPERTY_OR_METHOD_NOT_FOUND, "Cannot find method with signature " + name);
		return boost::optional<Type*>();

	return boost::optional<Type*>(properties.find(name)->second.first);
}

string PropertySymbolTable::getAddress(string name) {
	return properties.find(name)->second.second;
}

string PropertySymbolTable::getProvisionAddress(Type* provided) {
	string name = analyzer->getNameForType(provided) + "<-";
	if(provided->specialty != NULL) name += provided->specialty;
	return properties.find(name)->second.second;
}

string PropertySymbolTable::getSymbolNameOf(vector<pair<string, TypeArray*> >* segments_arguments) {
	string name;
	for(vector<pair<string, TypeArray*> >::iterator it = segments_arguments->begin(); it != segments_arguments->end(); ++it) {
		name += it->first;
		name += "(";
		int i;
		for(i = 0; i < it->second->typecount; i++) {
			if(i) name += ",";
			name += analyzer->getNameForType(it->second->types[i]);
		}
		name += ")";
	}

	return name;
}

void PropertySymbolTable::printEntryPoints(EntryPointAnalyzer* entryanalyzer) {
	for(map<string, pair<Type*, string> >::iterator it = properties.begin(); it != properties.end(); ++it) {
		if(entryanalyzer->checkMethodCanBeMain(it->first, it->second.first))
			entryanalyzer->printMethod(it->first);
	}
}

void PropertySymbolTable::addNeed(Type* needed) {
	needs.push_back(needed);
}

vector<Type*>* PropertySymbolTable::getNeeds() {
	return &needs;
}

void PropertySymbolTable::assignAddresses() {
	for(map<string, pair<Type*, string> >::iterator it = properties.begin(); it != properties.end(); ++it)
	if(it->second.second == "")
		it->second.second = alloc->allocate();
}

bool PropertySymbolTable::isAbstract() {
	return abstract;
}

PropertySymbolTable::~PropertySymbolTable() {
	for(map<string, pair<Type*, string> >::iterator it = properties.begin(); it != properties.end(); ++it) {
		freeType(it->second.first);
	}
}

void propagateInheritanceTables(PropertySymbolTable* child, PropertySymbolTable* parent, bool extend) {
	for(map<string, pair<Type*, string> >::iterator it = parent->properties.begin(); it != parent->properties.end(); ++it) {
		map<string, pair<Type*, string> >::iterator searcher = child->properties.find(it->first);
		if(searcher == child->properties.end()) {
			Type* inheritedtype = copyType(it->second.first);
			if(!extend) {
				inheritedtype->typedata.lambda.body = NULL;
				child->abstract = true;
			} else if(inheritedtype->typedata.lambda.body == NULL) {
				child->abstract = true;
			}
			child->properties[it->first] = pair<Type*, string>(inheritedtype, it->second.second);
		} else {
			searcher->second.second = it->second.second;
		}
	}
}
