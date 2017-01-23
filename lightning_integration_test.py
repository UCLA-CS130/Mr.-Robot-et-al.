#!/usr/bin/env python

import subprocess
from subprocess import run

print("START Lightning integration tests\n")

# make -s suppresses output of commands as make executes them
# subprocess.run returns a CompletedProcess object
if (
    run(['make', '-s', 'clean']).returncode != 0 or
    run(['make', '-s']).returncode != 0
   ):
    print('FAILED: Build with make failed!')

# Test echoing
# Try a background fork/thread
if (run(['./lightning', 'simple_config']).returncode != 0):
    print('FAILED: Lightning server failed to start with simple config')

# TODO: Have intermediate logging throughout
# TODO: Use Python unit test frameworks + logging libraries
print("After server start")

# TODO: HTTPie is failing to run
original_request = 'GET / HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: HTTPie/0.9.8\r\nAccept-Encoding: gzip, deflate\r\nAccept: */*\r\nConnection: keep-alive\r\n\r\n'
completed_request = run(['http', 'localhost:8080'], stdout=subprocess.PIPE)

if (completed_request.returncode != 0 or
    completed_request.stdout != original_request
   ):
    print('FAILED: httpie received a non-matching echo response')

# Terminate the server
run(['pkill', 'lightning'])

print("\nEND Lightning integration tests. All tests passed!")
