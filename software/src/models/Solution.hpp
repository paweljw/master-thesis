#pragma once

#ifndef __solution_hpp
#define __solution_hpp

#include <string.h>

namespace horizon
{
	namespace models
	{
		class Solution
		{
			public:
				Solution();
				Solution(int);
				int ID;
				int State;
				int CurrentWave;
				std::string _Solution;
		};
	}
}

#endif