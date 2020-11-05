#include <type_traits>
#include "LAYER.h"

LAYER begin(LAYER)
{
	return LAYER::BG;
}

LAYER end(LAYER)
{
	return LAYER::MAX;
}

LAYER operator++(LAYER & key)
{
	return key = static_cast<LAYER>(std::underlying_type<LAYER>::type(key) + 1);
}

LAYER operator*(LAYER key)
{
	return key;
}
