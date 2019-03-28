from __future__ import print_function
from termcolor import colored

def infoMsg(txt, newline=True):
    if newline:
        print('{0} {1}'.format(colored("INFO:", "blue"), txt))
    else:
        print('{0} {1}'.format(colored("INFO:", "blue"), txt), end='')


def successMsg(txt, newline=True):
    if newline:
        print('{0} {1}'.format(colored("DONE:", "green"), txt))
    else:
        print('{0} {1}'.format(colored("DONE:", "green"), txt), end='')


def warnMsg(txt, newline=True):
    if newline:
        print('{0} {1}'.format(colored("WARNING:", "yellow"), txt))
    else:
        print('{0} {1}'.format(colored("WARNING:", "yellow"), txt), end='')


def errMsg(txt, newline=True):
    if newline:
        print('{0} {1}'.format(colored("ERROR:", "red"), txt))
    else:
        print('{0} {1}'.format(colored("ERROR:", "red"), txt), end='')


def failMsg(txt, newline=True):
    if newline:
        print('{0} {1}'.format(colored("FAILED:", "red"), txt))
    else:
        print('{0} {1}'.format(colored("FAILED:", "red"), txt), end='')