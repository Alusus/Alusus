import multiprocessing


class template_build:
    def build(deps_path, install_path, num_threads=multiprocessing.cpu_count(), target_system=None):
        raise NotImplementedError(
            "You need to implement the \"build\" method!")

    def get_dep_environ(deps_path, target_system=None):
        raise NotImplementedError(
            "You need to implement the \"get_deps_env\" method!")
