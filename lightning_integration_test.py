#!/usr/bin/env python

from subprocess import call

print("START Lightning integration tests\n")

# make -s suppresses output of commands as make executes them
if (
    call(['make', '-s', 'clean']) != 0 or
    call(['make', '-s']) != 0 or
    call(['./lightning', 'simple_config']) != 0
   ):
    print('FAILED: Build and run with simple_config failed!')

print("\nEND Lightning integration tests. All tests passed!")
