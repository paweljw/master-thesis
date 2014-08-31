#include <string>

#pragma once

#ifndef __solution_hpp
#define __solution_hpp

namespace horizon
{
	namespace models
	{
		const int SOLUTION_NOT_STARTED	= 0;
		const int SOLUTION_STARTED		= 1;
		const int SOLUTION_COMPLETE		= 2;
		const int SOLUTION_BROKEN		= 9;

		class Solution
		{
		private:
			int ID;
			int State;
			int CurrentWave;
			std::string _Solution;

		public:
			Solution();
			Solution(int);
			int getID();
			void setID(int);
			int getState();
			void setState(int);
			int getWave();
			void setWave(int);
			std::string getSolution();
			void setSolution(std::string);
				
		};
	}
}

#endif