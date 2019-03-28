currDir=`pwd`
newDir="$(git rev-parse --show-toplevel)/Builds/"
echo "Changing directory to $newDir"
cd $newDir
echo "Cleaning project!"
make clean
echo "Changing directory to $currDir"
cd $currDir

