# proiect_SO CITY MANAGER

Here i will write my AI usage report for the Phase 1 of the cityManager project.

--Tool used:
    Google Gemini

--Where did I use AI:
    I used AI as a concept explainer and debugging tool (NOT as a writing tool!)
    Even though I used sites like https://www.codequoi.com/en/sending-and-intercepting-a-signal-in-c/ (a colleague suggested this as a great site to use for understanding signals), linux.die.net, pubs.opengroup.org or stackoverflow.com more, for some ideas I didn't quiet understand, I used AI. For coding I used it to indentify typos and corner cases I've missed. Some ideas like using a macro to stop my IDE from considering sigaction an incomplete type that's not allowed or using sigemptyset instead of memset (which I did not use since I did not quiet understand it's purpose) were stuff I found out during debugging, while WIFEXITED(status) and WEXITSTATUS(status) were checks I incorporated from stackoverflow after going in depth through the manual page. Also I used it to help me run my monitor_reports.c code independently to check whether it works before incorporating it to my main function.