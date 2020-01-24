### Choices and Relevent Implementation
The biggest aspect of our program is that we decided to separate relevant blocks
 of code into their own functions:

* `readExecute()` consists of the main `while` loop that runs our program; 
  it prints out the shell prompt and takes the command line, which it passes 
  into `parse()`.

* `parse(cmd)` is the function that parses through the command line to obtain 
  the separate commands. 
    - First, we use the `strstr` function to check the command line to check 
    which of the delimiters was encountered (or if there exists a `>&` or `|&` 
    within the command line), and set their respective boolean value to `true`. 
    - We chose to use `strtok` to separate the commands by using the delimiters 
    `>` and `|`, and added each command into the `commands` list. 
    - Depending on which of the booleans is true (or if none are), we call out 
    `execute()` function for that specific call; we pass the `commands` list and
     a string `type` (indicating the specific delimiter) into `execute()`.
    - We use a string pointer `originalCommand` to save our command line and use
    it later.

* `execute(commands, type)` holds all the function calls necessary to actually
   execute each system call. We check for several things:
    - `cd`: 
        - If the first command calls for `cd`, we tokenize the string once more 
        in order to get any arguments for `cd` (like a directory to go into).
        - We use `getcwd` to get the current working directory, then append 
        whatever additional arguments were included (like a path to a directory)
         with `strcat`. The `chdir` function allows us to actually go into the 
         indicated path.
    - `pwd`:
        - Simply put, all we do is call the `getcwd` function and print it to 
        the shell.
    - If the first command is neither, then we want to send our `commands` list
     directly to the `fork()+wait()+exec()` process. Here, we compare the string
      `type` with strings that had been initialized within that scope using 
      `strcmp`. Depending on the string `type` we receive, that specific 
      function (`redirect()` and `pipeline()`) is then called with the required 
      parameters. Should there be no specific `type`, then we call our `run()` 
      function immediately. (`run()` is also called within our other functions.)

* `run(cmd)` does the final parsing through the commands that are passed 
  through.
    - We chose to use `strtok` with the delimiter " " to separate the system 
    call from the arguments (if any) and insert them into another list, `args`. 
    We pass `args` into the `execvp()` function.
        - We chose to use `execvp()` to execute our commands since its arguments
         consist of a system call, and an array of arguments.

* `redirect(process1, filename, typeOfFile)` is called if the `execute()` 
  function finds that the boolean `isRedirect` or `isRedirectError` is `true`. 
  If so, then the third argument becomes the file descriptor for either `stderr`
   or `stdout`.
    - `filename` and `typeOfFile` are both passed to another function 
    (`writeToFile()`) to do the "redirecting" to a file.
    - After `writeToFile()` runs, we again call on our `run()` function to 
    execute our command, `process1`.

* `writeToFile(fileName, typeOfFile)` does the redirection of `stdout` (and 
  `stderr`) into the file we specify.
    - We use the `open()` function to get the file descriptor `filedesc` for the
     file that we pass in.
    - `typeOfFile` tells us whether the command line asked for normal 
    redirection or redirection error. If `typeOfFile` matches the file 
    descriptor for `stderr` (2), then we enter a if statement in which we use 
    `dup2()` twice: once to redirect `stdout` to the file we specify with 
    `filedesc`, and again to redirect `stderr` to that same file. If 
    `typeOfFile` does not match the file descriptor for `stderr`, then we only 
    call one `dup2()` to redirect `stdout` to the file.

* `pipeline(process1, process2)` implements the redirection from `process1`'s
  output to the input of `process2`.
    - We use another `fork()` here to ## somewords.
    - We have the parent process direct `stdin` to the read index in `pipe()`, 
    then we sent `process2` into our `run()` function. The child process directs
    `stdout` to the write index in `pipe()` and runs with `process1`.

### Extra Functions
* `removeWhiteSpace()` - called in `writeToFile()`; removes any whitespace
  within the passed string.
* `is_space()` - checks if the entire passed string is empty. Source:
https://stackoverflow.com/questions/3981510/getline-check-if-line-is-whitespace
* `printError()`- simple function to print out error statements.

### Testing
Most of our primary testing was on MAC terminals. After having the base program 
up and running, after each major change/addition, we tested with the various 
input/outputs from `project1.html`. After reaching the output redirection and 
piping, we tested on the CSIF environment and **_successfully_** ran the 
commands from `project1.html`. Once we implemented redirection error, we tested 
using the tester script on Gradescope.

**Sidenote1: We were not able to implement the `|&` system call.
  Sidenote2: If no possibility for regrading submissions, we would be grateful if
 after grading is done, we could have help truly understanding why our code did 
 not work in Gradescope.** 
