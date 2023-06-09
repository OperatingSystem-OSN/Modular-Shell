# Operating Systems C-Shell
### Name:Yash Shivhare
### Roll NUmber:2021101105


* > Running Shell
- - Run the command make
- -  then do ./main

* > main funtion
- - This contains the main Shell Loop
- - Extract Arguments from command
- - All the functions have been made sperately for various commands(mantaining modularity) however the entire code is in one file.

* > echo function
- - Prints user input after removing excess tabs and spaces.

* > pwd function
- - Prints the present working directory.
- - Uses the getcwd function to extract the current working directory of the terminal.

* > ls function
- - Prints all the files and directories according to the flags given.
- - Flags can be:
- - ls, ls -l, ls -al, ls -a, ls -la, ls <directory_name>,ls    <directory_path>,ls <file_name> , ls <flag_name> <directory_name>
- - Also multiple combos / repition are handled as well

* > fg function
- - Brings a process to the state of running.
- - Runs when & exists in input  or when some other command of bash is given

* > bg function
- - Change the status of background process from stop to running.
- - Can handle multiple ";" and "&"

* > cd function
- - Navigates through different directories.
- - Functions can be: cd,cd ..,cd .,cd -,cd ~,cd <directory_name>

* > history function
- - Prints the last 10 commands we executed on the terminal.
- - The implementation goes as follows.first we read the file and store it's contents in an array and checks if the number of commands is greater,less or equal to 20.Then accordingly commands are deleted and new commnads are added and finally written in the "history.txt" file.

* > pinfo function
- - pinfo
- - pinfo <pid_no_1> <pid_no_2> <pid_no_3> and so on..

* >discover function
- - This prints all the directories and files present in the arguement.
- - All the possible conditions have been handled.
- - This works on a recursive approach.(sort of DFS).
* >jobs
- - implemented the basic functionings
* > signalling
- - works great for all three ctrl + c,ctrl + d, ctrl + z
- - Yet to apply through raw mode.
