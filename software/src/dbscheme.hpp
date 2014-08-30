#include <string.h>

std::string dbscheme = R"CREATE TABLE [simulations] (
[id] INTEGER  NOT NULL PRIMARY KEY,
[state] INTEGER DEFAULT '0' NULL,
[current_wave] INTEGER DEFAULT '0' NULL,
[solution] TEXT  NULL
);

CREATE TABLE [tasks] (
[id] INTEGER  NOT NULL PRIMARY KEY,
[wave_id] iNTEGER  NULL,
[type] iNTEGER DEFAULT '1' NULL,
[state] iNTEGER DEFAULT '0' NULL,
[mcx] TEXT  NULL,
[metafile] TEXT  NULL,
[node] teXT  NULL,
[created] timesTAMP  NULL,
[updated] timesTAMP  NULL,
[deleted] timesTAMP  NULL
);

CREATE TABLE [waves] (
[id] INTEGER  NOT NULL PRIMARY KEY,
[solution_id] INTEGER  NOT NULL,
[seq] INTEGER  NULL,
[tasks] INTEGER  NULL,
[state] INTEGER DEFAULT '0' NOT NULL,
[created] TIMESTAMP  NULL,
[updated] TIMESTAMP DEFAULT 'NOW()' NULL,
[completed] TIMESTAMP  NULL
);";