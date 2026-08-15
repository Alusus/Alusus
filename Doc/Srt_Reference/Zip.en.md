# Zip Library Reference

`Zip` library contains the following functions:

### extractFromFile

```
func extractFromFile (
    filename: ptr[array[Char]], folderName: ptr[array[Char]],
    callback: ptr[func (ptr[array[Char]], ptr): Int], arguments: ptr
): Int;
```

Extracts a zip compressed file to the specified directory.

### compressToFile

```
func compressToFile (
    filename: ptr[array[Char]], files: ptr[array[ptr[array[Char]]]],
    fileCount: Int,
    extractType: ptr[array[Char]]
): Int;
```

Creates a zip compressed file containing the given files.
