@echo off
rem taskkill /F /FI "STATUS eq RUNNING"
taskkill /F /T /FI "STATUS eq RUNNING"
timeout /t 3 /nobreak > nul


goto comments

- `taskkill`: This is a command that allows you to end one or more tasks or processes.

- `/F`: This option forcefully terminates the process(es).

- `/T`: This option ends the specified process and any child processes started by it.

- `/FI "STATUS eq RUNNING"`: This is a filter that selects only the processes that are currently running.



This line will be skipped over.
will not be executed because of the goto END_COMMENT command. 
The script will jump to the :END_COMMENT label and continue execution from there. 

rem goto lable_name
rem :lable_name

:comments