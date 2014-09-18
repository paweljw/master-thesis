#include "utilhead.hpp"

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
}
