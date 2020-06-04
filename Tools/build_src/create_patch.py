import os
import shutil
import distutils.dir_util
import argparse
import filecmp


def create_patch(modded_dir, original_dir, tgt_dir):
    for root, dirs, files in os.walk(modded_dir, topdown=False):
        root_without_dir = root[len(modded_dir):]
        if root_without_dir and root_without_dir[0] == os.path.sep:
            root_without_dir = root_without_dir[1:]
        original_dir_root = os.path.join(original_dir, root_without_dir)
        tgt_dir_root = os.path.join(tgt_dir, root_without_dir)
        for name in files:
            if (os.path.exists(os.path.join(original_dir_root, name)) and
                    not filecmp.cmp(os.path.join(original_dir_root, name), os.path.join(root, name))) or\
                    (not os.path.exists(os.path.join(original_dir_root, name))):
                os.makedirs(tgt_dir_root, exist_ok=True)
                shutil.copy2(
                    os.path.join(root, name),
                    os.path.join(tgt_dir_root, name)
                )
        for name in dirs:
            if (not os.path.exists(os.path.join(original_dir_root, name))):
                distutils.dir_util.copy_tree(
                    os.path.join(root, name),
                    os.path.join(tgt_dir_root, name)
                )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(add_help=False,
                                     formatter_class=argparse.RawTextHelpFormatter,
                                     description="Utility script to create patches.")
    parser.add_argument("-h", "--help", action="help", default=argparse.SUPPRESS,
                        help="Show this help message and exit.")
    parser.add_argument(
        "modded-dir", help="The directory that contains the modified sources.")
    parser.add_argument(
        "original-dir", help="The directory that contains the original sources.")
    parser.add_argument(
        "tgt-dir", help="The output directory with the patches.")
    args = parser.parse_args()
    create_patch(getattr(args, "modded-dir"),
                 getattr(args, "original-dir"),
                 getattr(args, "tgt-dir"))
