from __future__ import print_function
import sys
import os
try:
    from termcolor import colored
except ImportError:
    from typing import Iterable

    # No colored output if "termcolor" is not present.
    def colored(text: str, color: str | None = ..., on_color: str | None = ..., attrs: Iterable[str] | None = ...) -> str:
        return text


def info_msg(txt, end=os.linesep, file=sys.stdout):
    print('{msg_type} {txt}'.format(msg_type=colored(
        "INFO:", "blue"), txt=txt), end=end, file=file)


def success_msg(txt, end=os.linesep, file=sys.stdout):
    print('{msg_type} {txt}'.format(msg_type=colored(
        "DONE:", "green"), txt=txt), end=end, file=file)


def warn_msg(txt, end=os.linesep, file=sys.stdout):
    print('{msg_type} {txt}'.format(msg_type=colored(
        "WARNING:", "yellow"), txt=txt), end=end, file=file)


def err_msg(txt, end=os.linesep, file=sys.stderr):
    print('{msg_type} {txt}'.format(msg_type=colored(
        "ERROR:", "red"), txt=txt), end=end, file=file)


def fail_msg(txt, end=os.linesep, file=sys.stderr):
    print('{msg_type} {txt}'.format(msg_type=colored(
        "FAILED:", "red"), txt=txt), end=end, file=file)
