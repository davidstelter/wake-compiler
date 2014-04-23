#ifndef HEADER_TYPE_PARAMETERIZER
#define HEADER_TYPE_PARAMETERIZER

#include <vector>

extern "C" {
	#include "type.h"
}

class TypeParameterizer {

	public:
		void applyParameterizations(Type** typeaddr, std::vector<Type*> parameters);
		void applyParameterizations(TypeArray* types, std::vector<Type*> parameters);

};

#endif