#include "ParseTreeTraverser.h"
#include "CompilationExceptions.h"
#include "ClassParseTreeTraverser.h"
#include <string.h>

void ParseTreeTraverser::traverse(Node* tree) {
	switch(tree->node_type) {
		case NT_PROGRAM:
			traverse(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) traverse(tree->node_data.nodes[1]);

			if(!passesForCompilation()) return;
			secondPass(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) secondPass(tree->node_data.nodes[1]);

			if(!passesForCompilation()) return;
			objectsymtable->propagateInheritance();

			thirdPass(tree->node_data.nodes[0]);
			if(tree->subnodes > 1) thirdPass(tree->node_data.nodes[1]);
			break;

		case NT_INHERITANCESET:
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					traverse(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_CLASS:
			traverse(tree->node_data.nodes[0]);
			traverse(tree->node_data.nodes[1]);
			break;

		case NT_CLASSNAME:
			{
				errors.pushContext("In declaration of 'every " + string(tree->node_data.string) + "'");
				boost::optional<SemanticError*> error = objectsymtable->addClass(tree->node_data.string);
				if(error) {
					(*error)->token = tree;
					errors.addError(*error);
				}
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			{
				boost::optional<SemanticError*> error = objectsymtable->addInheritance(tree->node_data.string, tree->node_type == NT_SUBCLASS);
				if(error) {
					(*error)->token = tree;
					errors.addError((*error));
				}
			}
	}
}

void ParseTreeTraverser::secondPass(Node* tree) {
	switch(tree->node_type) {
		case NT_INHERITANCESET:
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					secondPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_CLASS:
			{
				string classname = tree->node_data.nodes[0]->node_data.string;
				errors.pushContext("In declaration of 'every " + classname + "'");
				ClassParseTreeTraverser classtraverser(&errors, objectsymtable, &scopesymtable, classname, &typechecker, &methodanalyzer);

				secondPass(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) classtraverser.firstPass(tree->node_data.nodes[2]);

				errors.popContext();
			}
			break;

		case NT_INTERFACE:
		case NT_SUBCLASS:
			try {
				objectsymtable->find(tree->node_data.string);
			} catch(SymbolNotFoundException* e) {
				errors.addError(new SemanticError(CLASSNAME_NOT_FOUND, e->errormsg, tree));
				delete e;
			}
			break;

	}
}

void ParseTreeTraverser::thirdPass(Node* tree) {
	switch(tree->node_type) {
		case NT_CLASSSET:
			{
				int i = 0;
				while(i < tree->subnodes) {
					thirdPass(tree->node_data.nodes[i]);
					i++;
				}
			}
			break;

		case NT_CLASS:
			{
				string classname = tree->node_data.nodes[0]->node_data.string;
				errors.pushContext("In declaration of 'every " + classname + "'");
				ClassParseTreeTraverser classtraverser(&errors, objectsymtable, &scopesymtable, classname, &typechecker, &methodanalyzer);

				thirdPass(tree->node_data.nodes[1]);
				if(tree->subnodes > 2) classtraverser.secondPass(tree->node_data.nodes[2]);

				errors.popContext();
			}
			break;
	}
}

bool ParseTreeTraverser::passesForCompilation() {
	return errors.passesForCompilation();
}

void ParseTreeTraverser::printErrors(SemanticErrorPrinter& printer) {
	errors.printErrors(printer);
}
