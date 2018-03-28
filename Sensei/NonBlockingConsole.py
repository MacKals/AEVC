## Class for taking console input frmo user without blocking execution
## Found implementation foun here: https://stackoverflow.com/questions/2408560/python-nonblocking-console-input

import sys
import select
import tty
import termios
import time


class NonBlockingConsole(object):

    def __enter__(self):
        self.old_settings = termios.tcgetattr(sys.stdin)
        tty.setcbreak(sys.stdin.fileno())
        return self

    def __exit__(self, type, value, traceback):
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.old_settings)

    def get_data(self):
        if select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
            return sys.stdin.read(1)
        return False


## Example:

# with NonBlockingConsole() as nbc:
#     i = 0
#     while 1:
#         print i
#         i += 1
#         if nbc.get_data() == '\x1b':  # x1b is ESC
#             break


with NonBlockingConsole() as nbc:
    i = 0
    while 1:
    #    print i
        time.sleep(0.1)
    #    i += 1
        d = nbc.get_data()
        if d: print d
        if d == 'w':
            print 'up'
        elif d == 's':
            print 'down'
        elif d == 'a':
            print 'right'
        elif d == 'd':
            print 'left'
