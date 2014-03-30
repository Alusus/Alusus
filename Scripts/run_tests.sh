currDir=`pwd`
newDir="$(git rev-parse --show-toplevel)/Builds/"
echo "Changing directory to $newDir"
cd $newDir
echo "Making project!"
#make -j8
make test
echo "Changing directory to $currDir"
cd $currDir

