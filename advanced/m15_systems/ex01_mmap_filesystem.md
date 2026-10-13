# m15_systems/ex01_mmap_filesystem — файлы, отображённые в память

> Модуль m15_systems · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m15_systems/ex01_mmap_filesystem`
>
> Windows API (`CreateFileMapping`/`MapViewOfFile`). POSIX-аналог: `open` +
> `mmap` + `ftruncate`. Используется временный файл, который удаляется в конце.

## Теория

Обычный ввод-вывод копирует данные: диск → буфер ядра → буфер процесса.
**Отображение файла в память (memory-mapped file)** связывает файл с диапазоном
виртуальной памяти: чтение/запись страниц идёт напрямую, ядро подгружает их по
требованию (demand paging). Преимущества:

- нет копирования и явных `read`/`write` — доступ через указатель;
- **разделяемая память между процессами**: одно и то же отображение видит
  изменения обоих (IPC);
- удобно для больших и «разреженных» файлов (затрагиваются только нужные
  страницы).

Windows-путь: `CreateFileW` (дескриптор файла) → `CreateFileMappingW`
(объект-отображение) → `MapViewOfFile` (указатель) → работа → `UnmapViewOfFile` →
`CloseHandle` ×2. POSIX: `open` → `ftruncate` (задать размер) → `mmap` → `munmap`.

## Синтаксис

```cpp
HANDLE file = CreateFileW(path.wstring().c_str(), GENERIC_READ | GENERIC_WRITE,
                          0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READWRITE, 0, 0, nullptr);
auto* view = static_cast<unsigned char*>(MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0));

view[offset] = new_value;                 // zapis' idet pryamo na stranicu
UnmapViewOfFile(view);
CloseHandle(mapping);
CloseHandle(file);
```

POSIX-эквивалент:

```c
int fd = open(path, O_RDWR);
ftruncate(fd, size);
void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
```

## Подводные камни

- **Порядок закрытия**: сначала `UnmapViewOfFile`, потом `CreateFileMapping`,
  потом `CreateFile`. Файл нельзя удалить/усечь, пока есть отображение
  (`ERROR_USER_MAPPED_FILE`).
- **Размер отображения не может превышать файл** (для `CreateFileMappingW` без
  явного размера — берётся размер файла). Для расширения нужен `SetFilePointer` +
  `SetEndOfFile` или именованные отображения.
- **`PAGE_READWRITE` требует прав файла** `GENERIC_READ | GENERIC_WRITE`.
- **Флаги доступа должны согласовываться**: `FILE_MAP_READ` с `PAGE_READONLY`,
  иначе `ERROR_ACCESS_DENIED`.
- **Отображение — не транзакция**: изменения попадают на диск позже; для
  гарантии используйте `FlushViewOfFile` + `FlushFileBuffers`.
- **Выравнивание смещения**: при частичном отображении смещение обязано быть
  кратно гранулярности выделения (`dwAllocationGranularity`, обычно 64 КиБ).
- **Указатели на отображённую память нельзя хранить дольше**, чем живёт
  отображение.

## Задача

Создать временный файл 1 МиБ с детерминированным шаблоном, отобразить его в
память, проверить совпадение байта на смещении 12345, изменить его и убедиться,
что изменение видно на диске. Затем закрыть/удалить.

## Где применяется

- СУБД (mmap-индексы), редакторы (большие файлы), разделяемая память сервисов,
  загрузчики (ELF/PE), кэши, обработка датасетов больше RAM.

## Проверка

Вывод: `file size = 1048576`, `mapped view = ok`,
`modified offset 12345: old = 238, new = 99`, `verified on disk = 99`,
`mmap filesystem done`. Код возврата `0` при совпадении, иначе `1`.

## Решение

См. `ex01_mmap_filesystem.cpp`. Обратите внимание на RAII-подобный порядок
освобождения ресурсов и удаление файла в конце.
