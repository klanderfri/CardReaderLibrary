#pragma once
#include <vector>
//Class holding algorithm methods.
class OcvAlgorithmHelper
{
public:
	OcvAlgorithmHelper();
	~OcvAlgorithmHelper();

	//Tells if a vector contains the specified object.
	template<typename T> static bool VectorContains(std::vector<T> heystack, T needle);
};

template<typename T>
inline bool OcvAlgorithmHelper::VectorContains(std::vector<T> heystack, T needle)
{
	return find(heystack.begin(), heystack.end(), needle) != heystack.end();
}
