#include "utilhead.hpp"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>

std::string formattedNow()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer [16];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer,16,"%Y%m%d_%H%M%S",timeinfo);
  return std::string(buffer);
}

void loggerhead(std::string where)
{
	boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");
	logging::add_common_attributes();
	logging::add_console_log(std::cout, keywords::format = "[%TimeStamp%] <%Severity%>: %Message%");

	logging::add_file_log
    (
        keywords::file_name = where + "_%N.log",                                        /*< file name pattern >*/
		keywords::auto_flush = true,
        keywords::open_mode = (std::ios::out | std::ios::app),
        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::format = "[%TimeStamp%] <%Severity%>: %Message%"
    );
}

namespace horizon
{

	bool is_numeric(std::string const& str)
	{
		std::string::const_iterator first(str.begin()), last(str.end());
		return boost::spirit::qi::parse(first, last,
			boost::spirit::double_ >> *boost::spirit::qi::space)
			&& first == last;
	}

	std::string sqlite3_time(boost::posix_time::ptime bpt)
	{
		std::string ret = boost::posix_time::to_iso_extended_string(bpt);

		if (ret == "not-a-date-time")
			return "NULL";

		boost::algorithm::replace_first(ret, ",", ".");

		// Because apparently if you don't provide it with a character,
		// Resize decides it's a cool thing to fill it up with nulls.
		ret.resize(23, ' ');

		boost::algorithm::trim(ret);
		
		return "datetime('" + ret + "')";
	}
}