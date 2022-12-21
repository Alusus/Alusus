import os
import sys

# fmt: off
# Alusus import(s).
sys.path.insert(0, os.path.dirname(__file__))
from common import subprocess_run_hidden_except_on_error
# fmt: on


def make_git_path(path):
    if os.path.basename(path) != ".git":
        return os.path.join(path, ".git")
    return path


def generate_git_cmd(cmd, repo_path):
    return ["git", "--git-dir", make_git_path(repo_path)] + cmd


def restore_git_tree_to_path(repo_path, git_hash: str, path: str) -> None:
    os.makedirs(path, exist_ok=True)
    ret = subprocess_run_hidden_except_on_error(
        generate_git_cmd(["restore", "-q", "-s", git_hash, "."], repo_path),
        verbose_output=False, cwd=path)
    if ret.returncode:
        raise OSError(ret)


def get_current_branch(repo_path) -> str:
    ret = subprocess_run_hidden_except_on_error(
        generate_git_cmd(["rev-parse", "--abbrev-ref", "HEAD"], repo_path),
        verbose_output=False
    )
    if ret.returncode:
        raise OSError(ret)
    return ret.stdout.decode().strip()


def describe_tags(repo_path) -> str:
    ret = subprocess_run_hidden_except_on_error(
        generate_git_cmd(["describe", "--tags"], repo_path),
        verbose_output=False
    )
    if ret.returncode:
        raise OSError(ret)
    return ret.stdout.decode().strip()
