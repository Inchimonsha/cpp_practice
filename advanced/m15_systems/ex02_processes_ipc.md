# m15_systems/ex02_processes_ipc — процессы, каналы (pipes) и сигналы

> Модуль m15_systems · Сложность ★★★ · C++23 · Запуск: `./run.ps1 m15_systems/ex02_processes_ipc`
>
> Windows: `CreateProcessW`/`CreatePipe`/`ReadFile`. POSIX: `fork`/`exec`/`pipe`.
> Дочерний процесс — тот же исполняемый файл с аргументом `--child`.

## Теория

**Процесс** — изолированное адресное пространство с точкой входа. Родитель
создаёт ребёнка (`CreateProcessW`/`fork`+`exec`), при желании перенаправляет
его стандартные потоки. **Канал (pipe)** — односторонний буфер ядра: запись в
один конец читается с другого; так реализуют IPC и захват вывода.

**Сигнал** — асинхронное уведомление процесса (прерывание). В C — `std::signal`
и `std::raise`. Обработчик должен быть **асинхронно-безопасным**: только запись
в `volatile std::sig_atomic_t`, без `malloc`/`printf`/блокировок. Windows-специфика:
`SetConsoleCtrlHandler` (Ctrl+C/Close) и `GenerateConsoleCtrlEvent`; POSIX-модель
богаче (`SIGTERM`/`SIGHUP`/`SIGCHLD`, `sigaction`, маскирование).

Передача вывода в работе: `CreatePipe` с наследуемым дескриптором записи →
`STARTUPINFO.hStdOutput` → `CreateProcessW(..., TRUE /*inherit*/, ...)`.

## Синтаксис

```cpp
volatile std::sig_atomic_t g_stop = 0;
extern "C" void handle_signal(int) { g_stop = 1; }   // tol'ko flag, bez vyzovov
std::signal(SIGINT, handle_signal);
std::raise(SIGINT);

SECURITY_ATTRIBUTES sa{ sizeof(attributes), nullptr, TRUE };
HANDLE read_pipe = nullptr, write_pipe = nullptr;
CreatePipe(&read_pipe, &write_pipe, &attributes, 0);
SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);   // rebenok ne nasleduet chitatel'

STARTUPINFOW si{};
si.dwFlags = STARTF_USESTDHANDLES;
si.hStdOutput = write_pipe;
CreateProcessW(nullptr, command_line.data(), nullptr, nullptr, TRUE, 0, nullptr,
               nullptr, &si, &process);
CloseHandle(write_pipe);                  // parentu pisat' ne nuzhno
std::string output = read_all(read_pipe);
WaitForSingleObject(process.hProcess, INFINITE);
GetExitCodeProcess(process.hProcess, &exit_code);
```

## Подводные камни

- **Утечка дескрипторов**: родитель обязан закрыть свою копию *записывающего*
  конца, иначе `ReadFile` никогда не увидит EOF (чтение заблокируется).
- **Флаг наследования**: `bInheritHandle = TRUE` у pipe и `TRUE` в
  `CreateProcessW`, иначе детей не получат `stdout`. Но читающий конец нельзя
  делать наследуемым.
- **Консоль vs pipe**: при перенаправлении в pipe вывод буферизуется иначе —
  дочерний код должен явно сбрасывать буфер (`std::flush`), иначе данные зависнут.
- **Асинхронная безопасность обработчика сигнала**: `std::println` внутри
  обработчика — UB (не reentrant).
- **Windows-сигналы ограничены**: `SIGTERM`/`SIGHUP`/`SIGKILL` и `kill()` для
  чужих процессов недоступны; для контроля используются события/`TerminateProcess`.
- **Код возврата**: `GetExitCodeProcess` даёт `DWORD`; отличайте нормальный код
  от статусов исключений (`0xC0000005` и т.п.).
- Дождитесь ребёнка (`WaitForSingleObject`) — иначе дескрипторы и коды могут не
  успеть освободиться.

## Задача

Родитель регистрирует обработчик `SIGINT`, вызывает `raise(SIGINT)`, затем
запускает себя же как дочерний процесс с аргументом `--child`, читает его вывод
через pipe, дожидается завершения и проверяет код возврата `7`.

## Где применяется

- Запуск внешних утилит и CI-раннеров, shell-подобные инструменты, пулы
  воркеров, supervisor-процессы, обработка Ctrl+C и graceful shutdown.

## Проверка

Вывод: `signal handled = yes`, `child output:` + три строки `child: ...`,
`child exit code = 7`, `processes signals done`. Код возврата `0` при совпадении,
иначе `1`.

## Решение

См. `ex02_processes_signals.cpp`. Тот же бинарник «переиспользуется» как ребёнок
через `argv[1] == "--child"`.
