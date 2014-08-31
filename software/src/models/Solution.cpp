#include "Solution.hpp"

namespace horizon
{
	namespace models
	{

		Solution::Solution()
		{
			setID(0);
			setState(SOLUTION_NOT_STARTED);
			setWave(0);
			setSolution(0);
		}

		Solution::Solution(int _ID)
		{
			setID(_ID);
			setState(SOLUTION_NOT_STARTED);
			setWave(0);
			setSolution(0);
		}

		int Solution::getID()
		{
			return this->ID;
		}

		void Solution::setID(int _ID)
		{
			this->ID = _ID;
		}

		uchr Solution::getState()
		{
			return this->State;
		}

		void Solution::setState(uchr _State)
		{
			this->State = _State;
		}

		int Solution::getWave()
		{
			return this->CurrentWave;
		}

		void Solution::setWave(int _Wave)
		{
			this->CurrentWave = _Wave;
		}

		std::string Solution::getSolution()
		{
			return this->_Solution;
		}

		void Solution::setSolution(std::string __Solution)
		{
			this->_Solution = __Solution;
		}
	}
}