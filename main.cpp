#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include "platform.h"
#include "input.h"
#include "print.h"
#include <io.h>
#include <algorithm>
#include <stdio.h>
#include <cstring>
//
struct entries_context {
    FILE *file = 0;
    long entry_count = 0;
    entries_context() : file(fopen("entries.bin", "rb+")) {
        if (!file) {
            file = fopen("entries.bin", "wb+");
        }
        fseek(file, 0, SEEK_END);
        entry_count = ftell(file) / sizeof(entry);
        fseek(file, 0, SEEK_SET);
    }
    entries_context(entries_context const &) = delete;
    ~entries_context() { fclose(file); }

    entry get_entry(int index) const {
        entry result;
        fseek(file, index*sizeof(entry), SEEK_SET);
        fread(&result, sizeof(entry), 1, file);
        return result;
    }
    void set_entry(int index, entry entry) const {
        fseek(file, index*sizeof(entry), SEEK_SET);
        fwrite(&entry, sizeof(entry), 1, file);
    }
};

//
struct account {
    char login[16];
    char password[16];
};

bool authorize(char *login = 0, char *password = 0) {
    char login_buf[16];
    char password_buf[16];

    if (!login) {
        login = login_buf;
        puts("Введите логин:");
        scanf("%15s", login);
    }

    if (!password) {
        password = password_buf;
        puts("Введите пароль:");
        scanf("%15s", password);
    }

    auto file = fopen("users.bin", "rb");
    if (!file)
        return false;

    fseek(file, 0, SEEK_END);
    auto file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto account_count = file_size / sizeof(account);

    while (account_count--) {
        account account = {};

        fread(&account, sizeof(account), 1, file);

        if (strcmp(login, account.login) == 0 && strcmp(password, account.password) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

entry *read_all_entries(entries_context const &context) {
    auto entries = (entry *)malloc(sizeof(entry) * context.entry_count);
    for (int i = 0; i < context.entry_count; ++i) {
        entries[i] = context.get_entry(i);
    }
    return entries;
}

void print_entries(entries_context const &context) {
    for (int i = 0; i < context.entry_count; ++i) {
        printf("%d:\n", i);
        print(context.get_entry(i));
    }
    printf("\n");
}

void print_db() {
    clear_screen();
    print_entries({});
}

void add_entry() {
    clear_screen();
    auto entry = input_entry();
    auto file = fopen("entries.bin", "ab");
    fwrite(&entry, sizeof(entry), 1, file);
    fclose(file);
    clear_screen();
}

void edit_entry() {
    clear_screen();

    entries_context context;
    print_entries(context);

    printf("Введите номер билета, для изменения информации: ");
    int index = input_int(0, context.entry_count-1);

    entry entry = input_entry();

    context.set_entry(index, entry);

    clear_screen();
}

void delete_entry() {
    clear_screen();

    entries_context context;
    print_entries(context);

    printf("Введите номер билета, для удаления: ");
    int index = input_int(0, context.entry_count-1);

    context.set_entry(index, context.get_entry(context.entry_count - 1));

    _chsize(_fileno(context.file), (context.entry_count-1) * sizeof(entry));

    clear_screen();
}

void heapify(entry arr[], int n, int i)
{
    int largest = i;
// Инициализируем наибольший элемент как корень
    int l = 2*i + 1; // левый = 2*i + 1
    int r = 2*i + 2; // правый = 2*i + 2

    // Если левый дочерний элемент больше корня
    if (l < n && arr[l].cost > arr[largest].cost)
        largest = l;

    // Если правый дочерний элемент больше, чем самый большой элемент на данный момент
    if (r < n && arr[r].cost > arr[largest].cost)
        largest = r;

    // Если самый большой элемент не корень
    if (largest != i)
    {
        std::swap(arr[i], arr[largest]);

// Рекурсивно преобразуем в двоичную кучу затронутое поддерево
        heapify(arr, n, largest);
    }
}

// Основная функция, выполняющая пирамидальную сортировку
void heap_sort(entry arr[], int n)
{
    // Построение кучи (перегруппируем массив)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Один за другим извлекаем элементы из кучи
    for (int i=n-1; i>=0; i--)
    {
        // Перемещаем текущий корень в конец
        std::swap(arr[0], arr[i]);

        // вызываем процедуру heapify на уменьшенной куче
        heapify(arr, i, 0);
    }
}
void sort_entries() {
    clear_screen();

    entries_context context;

    auto entries = read_all_entries(context);

    heap_sort(entries, context.entry_count);


    for (int i = 0; i < context.entry_count; ++i) {
        printf("%d:\n", i);
        print(entries[i]);
    }
    printf("\n");
}

void filter_entries() {
    printf("Введите максимальную стоимость: ");
    int threshold = input_int(10, 99999);

    bool found = false;
    entries_context context;

    clear_screen();
    printf("Информация о билетах:\n");
    for (int i = 0; i < context.entry_count; ++i) {
        auto entry = context.get_entry(i);
        if (entry.cost <= threshold) {
            printf("%d:\n", i);
            print(entry);
            found = true;
        }
    }
    if (!found) {
        printf("Не найдены\n");
    }
    printf("\n");
}

void search_entries() {
    char source[256];
    printf("Введите название города отправления: ");
    input_string(source, sizeof(source));
    char destination[256];
    printf("Введите название города назначения: ");
    input_string(destination, sizeof(destination));

    bool found = false;
    entries_context context;

    clear_screen();
    printf("Билеты:\n");
    for (int i = 0; i < context.entry_count; ++i) {
        auto entry = context.get_entry(i);
        if (_stricmp(entry.start.name, source) == 0 && _stricmp(entry.end.name, destination) == 0) {
            printf("%d:\n", i);
            print(entry);
            found = true;
        }
    }
    if (!found) {
        printf("Не найдены\n");
    }
    printf("\n");
}

void add_manager() {
    clear_screen();

    account account = {};

    printf("Введите логин: ");
    scanf("%15s", account.login);

    printf("Введите пароль: ");
    scanf("%15s", account.password);

    if (authorize(account.login, account.password)) {
        puts("Менеджер с таким логином и паролем уже существует");
    } else {
        auto file = fopen("users.bin", "ab");
        fwrite(&account, sizeof(account), 1, file);
        fclose(file);
    }

    clear_screen();
}

void edit_manager() {
    clear_screen();

    char login[16] = {};
    printf("Введите логин: ");
    scanf("%15s", login);

    auto file = fopen("users.bin", "rb+");

    fseek(file, 0, SEEK_END);
    auto file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto account_count = file_size / sizeof(account);

    while (account_count--) {
        account account = {};

        fread(&account, sizeof(account), 1, file);

        if (strcmp(login, account.login) == 0) {

            printf("Введите новый пароль: ");
            scanf("%15s", account.password);
            fseek(file, -(long)sizeof(account), SEEK_CUR);
            fwrite(&account, sizeof(account), 1, file);

            fclose(file);
            clear_screen();
            return;
        }
    }

    fclose(file);

    printf("Менеджера с таки логином не существует\n");
    read_character();
    clear_screen();
}

int main() {
    init_locale();

    enum class menu {
        main,
        admin,
        manager,
        user,
    };

    auto current_menu = menu::main;
    while (1) {
        change_menu:
        switch (current_menu) {
            case menu::main: {
                while (1) {
                    enum class choice {
                        user = '1',
                        manager,
                        admin,
                        exit,
                    };
                    puts(R"(Добро пожаловать в систему учёта продаж авиабилетов.
Нажмите Ctrl+C в любой момент для выхода из программы.
Введите пункт меню для входа с соответстующими правами:
    1. Пользователь
    2. Менеджер
    3. Администратор
    4. Выход из программы)");
                    switch ((choice)read_character()) {
                        case choice::admin: {
                            clear_screen();
                            account account;
                            printf("Введите логин: ");
                            scanf("%15s", account.login);

                            printf("Введите пароль: ");
                            scanf("%15s", account.password);

                            if (strcmp(account.login, "admin") == 0 && strcmp(account.password, "admin") == 0) {
                                clear_screen();
                                current_menu = menu::admin;
                                goto change_menu;
                            } else  {
                                puts("Неверный логин или пароль");
                                puts("Нажмите любую клавишу для выхода в меню");
                                read_character();
                                clear_screen();
                            }

                            break;
                        }
                        case choice::manager: {
                            clear_screen();
                            if (authorize()) {
                                clear_screen();
                                current_menu = menu::manager;
                                goto change_menu;
                            } else  {
                                puts("Неверный логин или пароль");
                                puts("Нажмите любую клавишу для выхода в меню");
                                read_character();
                                clear_screen();
                            }
                            break;
                        }
                        case choice::user: {
                            clear_screen();
                            current_menu = menu::user;
                            goto change_menu;
                        }
                        case choice::exit:
                            return 0;
                        default:
                            clear_screen();
                            continue;
                    }
                    break;
                }
                break;
            }
            case menu::user: {
                while (1) {
                    enum class choice {
                        print_db = '1',
                        sort,
                        filter,
                        search,
                        exit
                    };
                    puts(R"(Добро пожаловать Пользователь.
Введите пункт меню:
    1. Просмотр информации о билетах
    2. Сортировка по времени продажи билета
    3. Добавление фильтрации
    4. Поиск билетов
    5. Назад)");
                    switch ((choice)read_character()) {
                        case choice::print_db: print_db(); break;
                        case choice::sort: sort_entries(); break;
                        case choice::filter: filter_entries(); break;
                        case choice::search: search_entries(); break;
                        case choice::exit: {
                            clear_screen();
                            current_menu = menu::main;
                            goto change_menu;
                        }
                        default: clear_screen(); break;
                    }
                }
                break;
            }
            case menu::manager: {
                while (1) {
                    enum class choice {
                        print_db = '1',
                        add_entry,
                        edit_entry,
                        delete_entry,
                        sort,
                        filter,
                        search,
                        exit
                    };
                    puts(R"(Добро пожаловать Менеджер.
Введите пункт меню:
    1. Просмотр информации о билетах
    2. Добавление информации о билете
    3. Изменение информации о билете
    4. Удаление информации о билете
    5. Сортировка по времени продажи билета
    6. Добавление фильтрации
    7. Поиск билетов
    8. Назад)");
                    switch ((choice)read_character()) {
                        case choice::print_db: print_db(); break;
                        case choice::add_entry: add_entry(); break;
                        case choice::edit_entry: edit_entry(); break;
                        case choice::delete_entry: delete_entry(); break;
                        case choice::sort: sort_entries(); break;
                        case choice::filter: filter_entries(); break;
                        case choice::search: search_entries(); break;
                        case choice::exit: {
                            clear_screen();
                            current_menu = menu::main;
                            goto change_menu;
                        }
                        default: clear_screen(); break;
                    }
                }
                break;
            }
            case menu::admin: {
                while (1) {
                    enum class choice {
                        print_db = '1',
                        add_entry,
                        edit_entry,
                        delete_entry,
                        sort,
                        filter,
                        search,
                        add_manager,
                        edit_manager,
                        exit = '0'
                    };
                    puts(R"(Добро пожаловать Администратор.
Введите пункт меню:
    1. Просмотр информации о билетах
    2. Добавление информации о билете
    3. Изменение информации о билете
    4. Удаление информации о билете
    5. Сортировка по времени продажи билета
    6. Добавление фильтрации
    7. Поиск билетов
    8. Добавление аккаунта менеджера
    9. Изменение аккаунта менеджера
    0. Назад)");
                    switch ((choice)read_character()) {
                        case choice::print_db: print_db(); break;
                        case choice::add_entry: add_entry(); break;
                        case choice::edit_entry: edit_entry(); break;
                        case choice::delete_entry: delete_entry(); break;
                        case choice::sort: sort_entries(); break;
                        case choice::filter: filter_entries(); break;
                        case choice::search: search_entries(); break;
                        case choice::add_manager: add_manager(); break;
                        case choice::edit_manager: edit_manager(); break;
                        case choice::exit: {
                            clear_screen();
                            current_menu = menu::main;
                            goto change_menu;
                        }
                        default: clear_screen(); break;
                    }
                }
                break;
            }
        }
    }
}
