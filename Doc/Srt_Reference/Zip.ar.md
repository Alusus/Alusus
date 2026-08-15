# دليل مكتبة `ضـغط` (Zip)

تحتوي وحدة `ضـغط` على الدالات التالية:

### استخرج_من_ملف (extractFromFile)

```
دالة استخرج_من_ملف (
  اسم_الملف: مؤشر[مصفوفة[مـحرف]]، اسم_المجلد: مؤشر[مصفوفة[مـحرف]]،
  مستلم_الاشعارات: مؤشر[دالة (مؤشر[مصفوفة[مـحرف]]، مؤشر) => صـحيح]، معطيات: مؤشر
) => صـحيح؛
```

```
func extractFromFile (
    filename: ptr[array[Char]], folderName: ptr[array[Char]],
    callback: ptr[func (ptr[array[Char]], ptr): Int], arguments: ptr
): Int;
```

تستخرج ملفًا مضغوطا إلى المجلد المحدد.

### اضغط_إلى_ملف (compressToFile)

```
دالة اضغط_إلى_ملف (
  اسم_المف: مؤشر[مصفوفة[مـحرف]]، ملفات: مؤشر[مصفوفة[مؤشر[مصفوفة[مـحرف]]]]،
  عدد_الملفات: صـحيح،
  نوع_الاستخراج: مؤشر[مصفوفة[مـحرف]]
) => صـحيح؛
```

```
func compressToFile (
    filename: ptr[array[Char]], files: ptr[array[ptr[array[Char]]]],
    fileCount: Int,
    extractType: ptr[array[Char]]
): Int;
```

تنشئ ملفًا مضغوطا يحتوي على الملفات المعطاة.

