# volumeshOS

## LogWindow
The LogWindow uses the singleton pattern, thus all classes interact with the same logger. A statement can be printed onto the logger console by use of the function

> void addLog(const char* fmt, int level = 0);

where fmt is the statement to be printed, and level denotes the criticality. The criticality is seperated in the levels: information, warning, error, critical. The default value is information. It might be useful to write the calling classes' name into the print-statement.
In the created Logwindow we provide some buttons for the User of the program. There is a clear button, which clears the
Logwindow, so that every line is deleted and the buffer is empty. The next button copy, copies the buffer into the clipboard
of the User. The button options provides some options for the Logwindow (at the moment only the autoscroll-feature). At least the User
can use the filter-field. This filters every line of the Logwindow, with the given string, and only shows the line in which the string is include.

To use the LogWindow the programmer has to create log-messages as follows:
> include "../panels/LogWindow.h"
>
> ...
>
> LogWindow::getInstance()->addLog(const char* fmt, int level = 0);

Every log-message will be given in a buffer, and printed in the next iteration of the show-method