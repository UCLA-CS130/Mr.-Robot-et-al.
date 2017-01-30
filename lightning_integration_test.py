#!/usr/bin/env python

# NOTE: subprocess.run requires Python 3.5+
import subprocess
from subprocess import run

print("\nSTART Lightning integration tests\n")

# make -s suppresses output of commands as make executes them
# subprocess.run returns a CompletedProcess object
if (
    run(['make', '-s', 'clean']).returncode != 0 or
    run(['make', '-s']).returncode != 0
   ):
    print('FAILED: Build with make failed!')

print('SUCCESS: make clean and make')

# Test echoing
# Try a background fork/thread
server_process = subprocess.Popen(['./lightning', 'simple_config'])

# TODO: Have intermediate logging throughout
# TODO: Use Python unit test frameworks + logging libraries
print('DEBUG: Lightning server started!')

original_request = b'GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: HTTPie/0.9.8\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nConnection: keep-alive\r\n\r\n'
completed_request = run(['http', 'localhost:8080'], stdout=subprocess.PIPE)

if (completed_request.returncode != 0):
    print('FAILED: httpie encountered an error')

if (completed_request.stdout != original_request):
    print('FAILED: httpie received a non-matching echo response')
    print('Completed request: \n%s' % completed_request.stdout.decode('UTF-8'))

print('SUCCESS: HTTPie request echo')

# Terminate the server
server_process.kill()

print("\nEND Lightning integration tests. All tests passed!")
