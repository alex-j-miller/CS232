"""
Name: Alex Miller - ajm94
Date: 2/3/2023
Class: CS232

This is a command shell that uses child processes

"""
import os

def main():
    while True:
        # Display a prompt
        command = input(f"{os.getcwd()}$ ")
        if command != "":
            # Split the command into arguments
            args = command.split()

            # Exit condition
            if args[0] == 'exit':
                break
            
            # cd
            if args[0] == 'cd':
                # Handle the 'cd' command
                if len(args) > 1:
                    os.chdir(args[1])
                else:
                    os.chdir(os.environ['HOME'])
                continue

            # pwd
            elif args[0] == 'pwd' and len(args) == 1:
                print( os.getcwd() )
            else:
                childProcess(args)
  
            
def childProcess(args):
    # Try to find the executable in the PATH environment variable
            for path in os.environ["PATH"].split(os.pathsep):
                executable_path = os.path.join(path, args[0])
                if os.path.isfile(executable_path) and os.access(executable_path, os.X_OK):
                    break
            else:
                # Executable not found in PATH
                print(f"{args[0]}: command not found")

            # Fork a new process to run the command
            # https://www.geeksforgeeks.org/python-os-fork-method/
            pid = os.fork()

            # https://blog.devgenius.io/execvp-system-call-in-python-everything-you-need-to-know-c402fe6886eb
            if pid == 0:
                # In the child process
                os.execv(executable_path, args)
            else:
                # In the parent process
                print(f"[Child pid: {pid}]")
                print(f"[{pid} -> {os.waitpid(pid, 0)[1]}]")

main()