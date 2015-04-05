#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "utilhead.hpp"
#include "globals.hpp"
using namespace logging::trivial;
			

#pragma once

#ifndef __solution_hpp
#define __solution_hpp

namespace horizon
{
	namespace models
	{
		/** @brief	The solution unpublished. */
		const uchr SOLUTION_UNPUBLISHED	= 0;
		/** @brief	The solution available. */
		const uchr SOLUTION_AVAILABLE = 1;
		/** @brief	The solution complete. */
		const uchr SOLUTION_COMPLETE = 2;
		/** @brief	The solution broken. */
		const uchr SOLUTION_BROKEN = 9;

		/**
		 * @class	Solution
		 *
		 * @brief	A solution.
		 *
		 * @author	PJW
		 * @date	2014-09-20
		 */

		class Solution
		{
		private:
			src::severity_logger< severity_level > lg;

			/** @brief	The identifier. */
			int ID;

			/** @brief	The state. */
			uchr State;

			/** @brief	The current wave. */
			int CurrentWave;

			/** @brief	The solution file reference. */
			std::string _Solution;

			/** @brief	The solution name. */
			std::string _Name;

			/** @brief	The created time. Constant after creation. */
			boost::posix_time::ptime Created;

			/** @brief	The update time. Updated on every change to the structure. */
			boost::posix_time::ptime Updated;

			/** @brief	The completion time. Updated only once. */
			boost::posix_time::ptime Completed;

			/**
			 * @fn	void Solution::touch();
			 *
			 * @brief	Touches the update time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 */
			void touch();
		public:

			/**
			 * @fn	void Solution::setState(uchr);
			 *
			 * @brief	Sets the object state.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	parameter1	The first parameter.
			 */
			void setState(uchr);

			/**
			 * @fn	Solution::Solution();
			 *
			 * @brief	Default constructor.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 */
			Solution();

			/**
			 * @fn	Solution::Solution(int);
			 *
			 * @brief	Constructor from numeric ID.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	ID	Numeric identifier.
			 */
			Solution(int);

			/**
			 * @fn	int Solution::getID();
			 *
			 * @brief	Gets the identifier.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The identifier.
			 */
			int getID();

			/**
			 * @fn	void Solution::setID(int);
			 *
			 * @brief	Sets the identifier.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	ID The identifier
			 */
			void setID(int);

			/**
			 * @fn	uchr Solution::getState();
			 *
			 * @brief	Gets the state.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The state.
			 */
			uchr getState();

			/**
			 * @fn	int Solution::getWave();
			 *
			 * @brief	Gets the current wave in this solution.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The wave.
			 */
			int getWave();

			/**
			 * @fn	void Solution::setWave(int);
			 *
			 * @brief	Sets the current wave.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	Wave The wave ID.
			 */
			void setWave(int);

			/**
			 * @fn	std::string Solution::getSolution();
			 *
			 * @brief	Gets the solution file reference.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The solution file.
			 */
			std::string getSolution();

			/**
			 * @fn	void Solution::setSolution(std::string);
			 *
			 * @brief	Sets the solution file.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	Solution Solution file.
			 */
			void setSolution(std::string);

			/**
			 * @fn	void Solution::markAvailable();
			 *
			 * @brief	Mark available.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 */
			void markAvailable();

			/**
			 * @fn	void Solution::markComplete(std::string);
			 *
			 * @brief	Mark complete.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	parameter1	The first parameter.
			 */
			void markComplete(std::string);

			/**
			 * @fn	void Solution::markBroken();
			 *
			 * @brief	Mark broken.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 */
			void markBroken();

			/**
			 * @fn	boost::posix_time::ptime Solution::getCreated();
			 *
			 * @brief	Gets the creation time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The creation time.
			 */
			boost::posix_time::ptime getCreated();

			/**
			 * @fn	boost::posix_time::ptime Solution::getUpdated();
			 *
			 * @brief	Gets the update time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The update time.
			 */
			boost::posix_time::ptime getUpdated();

			/**
			 * @fn	boost::posix_time::ptime Solution::getCompleted();
			 *
			 * @brief	Gets the completion time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The completion time.
			 */
			boost::posix_time::ptime getCompleted();

			/**
			 * @fn	boost::posix_time::time_duration Solution::getDuration();
			 *
			 * @brief	Gets the total solution duration.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The duration.
			 */

			boost::posix_time::time_duration getDuration();

			/**
			 * @fn	boost::posix_time::time_duration Solution::getElapsed();
			 *
			 * @brief	Gets the elapsed time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The elapsed.
			 */

			boost::posix_time::time_duration getElapsed();

			/**
			 * @fn	void Solution::setCreated(std::string);
			 *
			 * @brief	Sets created time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	parameter1	The first parameter.
			 */

			void setCreated(std::string);

			/**
			 * @fn	void Solution::setUpdated(std::string);
			 *
			 * @brief	Sets update time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	parameter1	The first parameter.
			 */

			void setUpdated(std::string);

			/**
			 * @fn	void Solution::setCompleted(std::string);
			 *
			 * @brief	Sets completion time.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	solution	Solution file reference.
			 */

			void setCompleted(std::string);

			/**
			 * @fn	std::string Solution::getName();
			 *
			 * @brief	Gets the name.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @return	The name.
			 */

			std::string getName();

			/**
			 * @fn	void Solution::setName(std::string);
			 *
			 * @brief	Sets a name.
			 *
			 * @author	PJW
			 * @date	2014-09-20
			 *
			 * @param	name	The name to be set.
			 */

			void setName(std::string);

		};
	}
}

#endif