# Создание RPM для Show

Решение [задания 03_Packaging](https://www.uneex.ru/LecturesCMC/LinuxApplicationDevelopment2026/03_Packaging).

- `show-1.0.tar.gz` содержит обновлённые `Makefile` и `Show.c` из `01_TerminalProject`.
- `show.spec` собирает пакет `show`, устанавливающий команду `/usr/bin/Show`.

В Makefile добавлена цель `install` с переменными `DESTDIR`, `PREFIX`
(по умолчанию `/usr/local`) и `BINDIR` (по умолчанию `$(PREFIX)/bin`).
`DESTDIR` задаёт временный корень установки, а `PREFIX` — путь внутри него:

```sh
make -C 01_TerminalProject
make -C 01_TerminalProject install DESTDIR="$PWD/staging" PREFIX=/usr
```

Программа окажется в `staging/usr/bin/Show`. При сборке RPM spec передаёт
`DESTDIR` через `%makeinstall_std`, а каталог установки — через `%_bindir`.
Зависимости готового пакета от ncurses и libc определяются автоматически.

В исходнике также исправлены две ошибки, мешавшие проверке: восстановлен
вызов `initscr()` и добавлено сохранение прочитанных строк через `text_add()`
с проверкой выделения памяти.

## Сборка и установка в ALT Linux

От администратора установи сборочные зависимости:

```sh
sudo apt-get update
sudo apt-get install rpm-build gcc make libncurses-devel
```

Из корня репозитория, обычным пользователем:

```sh
mkdir -p "$HOME/RPM/SOURCES" "$HOME/RPM/SPECS"
cp 03_Packaging/show-1.0.tar.gz "$HOME/RPM/SOURCES/"
cp 03_Packaging/show.spec "$HOME/RPM/SPECS/"
rpmbuild --define "_topdir $HOME/RPM" -ba "$HOME/RPM/SPECS/show.spec"
```

Результат: исходный пакет `~/RPM/SRPMS/show-1.0-alt1.src.rpm`,
двоичный пакет и пакет отладочной информации в `~/RPM/RPMS/<архитектура>/`.
Установка и запуск:

```sh
arch=$(rpm --eval '%_target_cpu')
sudo apt-get install "$HOME/RPM/RPMS/$arch/show-1.0-alt1.$arch.rpm"
rpm -ql show
rpm -V show
Show 01_TerminalProject/Show.c
```

`rpm -ql show` должен вывести `/usr/bin/Show`, а `rpm -V show` — ничего.
Стрелки прокручивают текст, пробел сдвигает его на строку вниз,
Page Up / Page Down — на страницу; Esc завершает программу.

## Результат проверки

Проверено 12 октября 2026 года в существующей VM **ALT Starterkit 11,
ветка p11, aarch64**: сборка обоих RPM, установка двоичного пакета,
его состав и целостность. В псевдотерминале проверены отображение текста,
прокрутка пробелом, стрелками и Page Up / Page Down, выход по Esc.
Также проверены сообщения при запуске без аргументов и с отсутствующим файлом.
На macOS проверена установка через `DESTDIR` в каталог с пробелами.

Отдельная VM с дистрибутивом практикума или основной регулярной сборкой,
указанными в пункте 0 задания, не устанавливалась; использована готовая ALT p11.
Проверки на x86_64 не выполнялись.

Поле `License: Unspecified` отражает отсутствие указанной лицензии
в исходном проекте; новая лицензия исходникам не назначалась.

## Обновление архива после изменения исходников

Из корня репозитория:

```sh
workdir=$(mktemp -d)
mkdir "$workdir/show-1.0"
cp 01_TerminalProject/Makefile 01_TerminalProject/Show.c "$workdir/show-1.0/"
COPYFILE_DISABLE=1 tar -czf 03_Packaging/show-1.0.tar.gz -C "$workdir" show-1.0
rm -r "$workdir"
```
