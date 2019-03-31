from __future__ import print_function
try:
    import install_dep
except ImportError:
    import sys
    if os.path.dirname(os.path.realpath(__file__)) not in sys.path:
        sys.path.append(os.path.dirname(os.path.realpath(__file__)))
    import install_dep
install_dep.install_package('termcolor'); from termcolor import colored

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
