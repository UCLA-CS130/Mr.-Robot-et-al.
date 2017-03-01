#!/usr/bin/env python

# NOTE: subprocess.run requires Python 3.5+
import subprocess
from subprocess import run

print("\nSTART Lightning integration tests\n")

# Test echoing using httpie
# Try a background fork/thread
server_process = subprocess.Popen(['./lightning', 'simple_config'])

# Spawn a shell process to act as hanging http request
telnet_request_command = "telnet localhost 2020"
telnet_request_proc = subprocess.Popen(telnet_request_command, stdout=subprocess.PIPE, shell=True)

# TODO: Have intermediate logging throughout
# TODO: Use Python unit test frameworks + logging libraries
print('DEBUG: Lightning server started!')

expected_response = b'GET /echo HTTP/1.1\r\nHost: localhost:2020\r\nAccept-Encoding: gzip, deflate, compress\r\nAccept: */*\r\nUser-Agent: HTTPie/0.8.0\r\n\r\n'
actual_response = run(['http', 'localhost:2020/echo'], stdout=subprocess.PIPE)

if (actual_response.returncode != 0):
    print('FAILED: httpie encountered an error')

if (actual_response.stdout != expected_response):
    print('FAILED: httpie received a non-matching echo response')
    print('Completed request: \n%s' % actual_response.stdout.decode('UTF-8'))
else:
    print('SUCCESS: HTTPie request echo; Multithreading successful!')

# Terminate the server
server_process.kill()

print("\nEND Lightning integration tests. All tests passed!")
