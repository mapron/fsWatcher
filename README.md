# fsWatcher
Qt модуль для отслеживания изменения в файловой системе.

Зависимости:
- Qt4 >=4.8.0 или Qt5 >= 5.2
На текущий момент реализация только под Windows, минимум NT 5.1.

Проверенные конфигурации сборки:
-Windows-msvc2010-x86, Qt4
-Windows-msvc2010-x86, Qt5
-linux-gcc4.9-x86_64, Qt4

Сборка:
Для сборки требуется система сборки QBS. Ее можно получить, установив Qt Creator (2.8 или выше).
Документация: http://doc.qt.io/qbs/index.html
Ссылка на скачивание (QtCreator): https://www.qt.io/download-open-source/

после настройки профиля QBS, перейти в директорию  с fsWatcher.qbs
и выполнить
qbs build fsWatcher.qbs

Либо, просто открыть fsWatcher.qbs через QtCreator и выполнить сборку.