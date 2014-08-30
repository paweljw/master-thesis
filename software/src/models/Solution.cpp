#include "solution.hpp"

namespace pjw
{
	namespace models
	{

		Solution::Solution()
		{
			this->ID = 0;
			this->State = 0;
			this->CurrentWave = 0;
			this->Solution = "";
		}

		Solution::Solution(int _ID)
		{
			this->ID = _ID;
			this->State = 0;
			this->CurrentWave = 0;
			this->Solution = "";
		}
	}
}