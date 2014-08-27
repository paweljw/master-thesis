/* strftime example */
#include <stdio.h>      /* puts */
#include <time.h>       /* time_t, struct tm, time, localtime, strftime */
#include <string.h>

#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>

#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

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