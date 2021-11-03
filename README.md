# iCalendar-ics-filter
Command line interface application using C to read and store data from ICS files to print a formatted list of calendar events sorted chronologically.

To demo this program, 3 files ics files are included. This program uses command line input to specify what ics file to read as well as a start and end date which specifies the range of dates to be printed. To run the program have C installed and enter the following into the command line.
"./process_cal --start=YYYY/DD/MM --end=YYYY/DD/MM --file=FILE.ics"

Customize the YYYY, MM, DD, FILE portions to suit the output you are looking for.
For example if I wanted to fiter semester-schedule.ics from January 1st, 2021 to January 20th 2021 the following command should be entered.
"./process_cal --start=2021/1/1 --end=2021/1/20 --file=semester-schedule.ics"

The ouput of the previous command is as follows.

January 11, 2021 (Mon)
----------------------
 2:30 PM to  4:00 PM: CSC 115 {{ECS 123}}

January 14, 2021 (Thu)
----------------------
 2:30 PM to  4:00 PM: CSC 115 {{ECS 123}}

January 18, 2021 (Mon)
----------------------
 8:30 AM to 10:00 AM: MATH 102 {{HSD A240}}
 
10:00 AM to 11:30 AM: WRIT 120 {{CLE A210}}

 2:30 PM to  4:00 PM: CSC 115 {{ECS 123}}
 
 4:30 PM to  6:30 PM: CSC 115 lab {{ECS 248}}
 

January 19, 2021 (Tue)
----------------------
10:30 AM to 11:30 AM: ECON 104 {{DSB C112}}
11:30 AM to 12:30 PM: ASTR 101 {{ELL 067}}

January 20, 2021 (Wed)
----------------------
10:30 AM to 11:30 AM: ECON 104 {{DSB C112}}
11:30 AM to 12:30 PM: ASTR 101 {{ELL 067}}
