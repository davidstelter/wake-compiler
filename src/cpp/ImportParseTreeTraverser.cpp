#include "ImportParseTreeTraverser.h"

void ImportParseTreeTraverser::traverse(Node* tree, ObjectSymbolTable& o, LibraryLoader& l, std::string importpath) {
	switch(tree->node_type) {

		case NT_PROGRAM:
		case NT_IMPORTSET:
		case NT_IMPORT:
			for(int i = 0; i < tree->subnodes; i++) {
				traverse(tree->node_data.nodes[i], o, l, importpath);
			}
			break;

		case NT_IMPORTPATH:
			// TODO use actual path
			l.loadImport(tree->node_data.string, importpath, o);
			break;

		//case NT_IMPORTTARGET:
			//throw "Not supported yet";
	}

}