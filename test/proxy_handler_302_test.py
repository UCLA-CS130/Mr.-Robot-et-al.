#!/usr/bin/env python

from subprocess import Popen
from subprocess import PIPE
import sys

# We start off with the assumption that the test will succeed
ec = 0

# Test proxy server
print('DEBUG: Testing 302 response for proxy server!')
proxy_server_process = Popen(['./lightning', 'proxy_config'])

# Store contents of request to www.ucla.edu
p = Popen(["curl", "www.ucla.edu"], stdout=PIPE, stderr=PIPE)
expected_proxy_response = p.stdout.read()
actual_proxy_response = Popen(['http', 'localhost:3030/reverse_proxy_redirect'], stdout=PIPE)

if (actual_proxy_response.communicate()[0].decode('utf-8') != expected_proxy_response.decode('utf-8')):
    print('FAILED: proxy server received a non-matching proxy response')
    ec = 1
else:
  print('SUCCESS: Received expected reverse proxy response!')

# Terminate the server
proxy_server_process.kill()

# Return 0 if the test succeeded or some other value on failure
if ec == 0:
    sys.stdout.write("Finished 302 Integration Test. ALL TESTS PASSED!\n")
    sys.exit(0)
else:
    sys.stdout.write("302 INTEGRATION TEST FAILED\n")
    sys.exit(ec)
