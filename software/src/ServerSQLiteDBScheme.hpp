#pragma once

namespace horizon
{
	namespace dbscheme
	{
const char * simulations = "CREATE TABLE [solutions] ("
				"[id] INTEGER  NOT NULL PRIMARY KEY,"
				"[state] INTEGER DEFAULT '0' NULL,"
				"[current_wave] INTEGER DEFAULT '0' NULL,"
				"[name] TEXT NULL,"
				"[solution] TEXT NULL,"
				"[created] TIMESTAMP  NULL, "
				"[updated] TIMESTAMP  NULL,"
				"[completed] TIMESTAMP  NULL"
				");";

const char * tasks = "CREATE TABLE [tasks] ("
				"[id] INTEGER  NOT NULL PRIMARY KEY,"
				"[wave_id] iNTEGER  NULL,"
				"[type] iNTEGER DEFAULT '1' NULL,"
				"[state] integer DEFAULT '0' NULL,"
				"[part_num] integer  NULL,"
				"[metafile] text NULL,"
				"[node] text NULL,"
				"[created] timestamp  NULL,"
				"[updated] timestamp  NULL,"
				"[completed] timestamp  NULL"
				");";

const char * waves = "CREATE TABLE [waves] ("
				"[id] INTEGER  NOT NULL PRIMARY KEY,"
				"[solution_id] INTEGER  NOT NULL,"
				"[seq] INTEGER  NULL,"
				"[tasks] INTEGER  NULL,"
				"[state] INTEGER DEFAULT '0' NOT NULL,"
				"[created] TIMESTAMP  NULL,"
				"[updated] TIMESTAMP DEFAULT 'NOW()' NULL,"
				"[completed] TIMESTAMP  NULL"
				");";
}
}