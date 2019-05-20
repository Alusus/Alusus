cat > main_empty_dll.c <<END
#include <windows.h>
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	return TRUE;
}
END

echo "create libLLVM.a" > export.mri
rm tmp-libLLVM.a
for entry in *.a
do
	echo "addlib $entry" >> export.mri
done
echo "save" >> export.mri
echo "end" >> export.mri

gcc -c main_empty_dll.c
ar -M < export.mri
dlltool -z libLLVM.def --export-all-symbols libLLVM.a
g++ main_empty_dll.o libLLVM.def -shared -o "$1.dll" -L. -lLLVM -lole32 -luuid -lLLVMSupport -lz

rm main_empty_dll.c
rm main_empty_dll.o
rm export.mri
rm libLLVM.a
rm libLLVM.def
