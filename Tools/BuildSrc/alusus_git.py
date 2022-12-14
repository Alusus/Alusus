import subprocess
import os
import inspect
import types
from typing import cast
import json

# Alusus import(s).
from alusus_common import subprocess_run_hidden_except_on_error


# This abstract class has some of the common Git operations used by Alusus build scripts.
class AlususGit:
    @staticmethod
    def _generate_method_not_implemented_error():
        caller_function_name = cast(
            types.FrameType, inspect.currentframe()).f_back.f_code.co_name
        return NotImplementedError("{caller_function_name} is not implemented.".format(
            caller_function_name=json.dumps(caller_function_name)
        ))

    def restore_git_tree_to_path(self, git_hash, path):
        raise AlususGit._generate_method_not_implemented_error()

    def get_current_branch(self) -> str:
        raise AlususGit._generate_method_not_implemented_error()

    def describe_tags(self) -> str:
        raise AlususGit._generate_method_not_implemented_error()


# AlususGit backend that operates on a repository path and calls the Git binary.
class AlususGitFromRepoPathWithGitBinary(AlususGit):
    _repo_path = None
    _git_bin = None
    _environ = None

    def __init__(self, repo_path, git_bin="git", environ=os.environ.copy(), verbose_output=False) -> None:
        super().__init__()
        self.set_repo_path(repo_path)
        self.set_git_bin(git_bin)
        self.set_environ(environ)
        self.set_verbose_output(verbose_output)

    @property
    def git_bin(self):
        return self._git_bin

    def set_git_bin(self, git_bin):
        self._git_bin = git_bin

    @property
    def repo_path(self):
        return self._repo_path

    def set_repo_path(self, repo_path):
        self._repo_path = repo_path
        if os.path.basename(self._repo_path) != ".git":
            self._repo_path = os.path.join(self._repo_path, ".git")
    
    @property
    def verbose_output(self):
        return self._verbose_output

    def set_verbose_output(self, verbose_output):
        self._verbose_output = verbose_output

    @property
    def environ(self):
        return self._environ

    def set_environ(self, environ):
        self._environ = environ

    def _exec_git_cmd(self, git_args, cwd=os.getcwd()) -> subprocess.CompletedProcess[str]:
        return subprocess_run_hidden_except_on_error(
            [self.git_bin, "--git-dir", self.repo_path] + git_args,
            env=self.environ,
            cwd=cwd
        )

    # Interface implementation methods.

    def restore_git_tree_to_path(self, git_hash: str, path: str) -> None:
        os.makedirs(path, exist_ok=True)
        ret = self._exec_git_cmd(
            ["restore", "-q", "-s", git_hash, "."], cwd=path)
        if ret.returncode:
            raise OSError(ret)

    def get_current_branch(self) -> str:
        ret = self._exec_git_cmd(["rev-parse", "--abbrev-ref", "HEAD"])
        if ret.returncode:
            raise OSError(ret)
        return ret.stdout.decode().strip()

    def describe_tags(self) -> str:
        ret = self._exec_git_cmd(["describe", "--tags"])
        if ret.returncode:
            raise OSError(ret)
        return ret.stdout.decode().strip()
