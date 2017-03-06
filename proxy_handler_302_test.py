#!/usr/bin/env python3.6

# NOTE: subprocess.run requires Python 3.5+
import subprocess
from subprocess import run

# Test proxy server
print('DEBUG: Testing 302 response for proxy server!')
proxy_server_process = subprocess.Popen(['./lightning', 'proxy_config'])

# Store contents of request to www.ucla.edu
p = subprocess.Popen(["curl", "www.ucla.edu"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
expected_proxy_response = p.stdout.read()
actual_proxy_response = run(['http', 'localhost:3030/reverse_proxy_redirect'], stdout=subprocess.PIPE)

if (actual_proxy_response.returncode != 0):
    print('FAILED: proxy server encountered an error')

if (actual_proxy_response.stdout != expected_proxy_response):
    print('FAILED: proxy server received a non-matching proxy response')
    print('Expected proxy response: \n%s' % expected_proxy_response)
    print('Completed proxy response: \n%s' % actual_proxy_response.stdout.decode('UTF-8'))
else:
  print('SUCCESS: Received expected reverse proxy response!')

# Terminate the server
proxy_server_process.kill()
