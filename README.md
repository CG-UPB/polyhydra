# volumeshOS

## Logger
The Logger uses the singleton pattern, thus all classes interact with the same logger. A statement can be printed onto the logger console by use of the function 

> void addLog(const char* fmt, int level = 0);

where fmt is the statement to be printed, and level denotes the criticality. The criticality is seperated in the levels: information, warning, error, critical. The default value is information. It might be useful to write the calling classes' name into the print-statement.