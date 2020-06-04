import argparse
import os
import sys
SOURCE_LOCATION = os.path.abspath(__file__)
sys.path.insert(0, os.path.dirname(os.path.dirname(SOURCE_LOCATION)))
from utils import eprint  # noqa
from create_shared_lib import create_dylib  # noqa


if __name__ == "__main__":
    # Parse args.
    parser = argparse.ArgumentParser()
    parser.add_argument("--file", action="append")
    parser.add_argument("--dir", action="append")
    parser.add_argument("--dir-recurs", action="append")
    parser.add_argument("--output-dir", default=os.getcwd())
    parser.add_argument("--output-name", default="tmp")
    parser.add_argument("--link-lib", action="append")
    parser.add_argument("--link-lib-path", action="append")
    args = parser.parse_args()

    ret = create_dylib(arg_file=args.file, arg_dir=args.dir, arg_dir_recurs=args.dir_recurs,
                       arg_output_dir=args.output_dir, arg_output_name=args.output_name,
                       arg_link_lib=args.link_lib, arg_link_lib_path=args.link_lib_path)
    if ret[0]:
        eprint(ret[1])
    os._exit(ret[0])
