#define _CRT_SECURE_NO_WARNINGS
#include "input.h"
#include <stdio.h>

int input_int(int min, int max) {
    int result;
retry:
    while (scanf("%d", &result) != 1) {
        printf("Неверный ввод. Повторите: ");
        char c;
        while ((c = getchar()) != EOF && c != '\n');
    }
    if (min <= result && result <= max) {
        return result;
    }
    printf("Число не входит в диапазон [%d, %d]. Повторите: ", min, max);
    goto retry;
}

void input_string(char *buf, int count) {
    char format[16];
    sprintf(format, "%%%ds", count-1);
    scanf(format, buf);
}

date input_date() {
    date result = {};

    printf("Введите год: ");    result.year   = input_int(1970, 9999);
    printf("Введите месяц: ");  result.month  = input_int(1, 12);
    printf("Введите день: ");   result.day    = input_int(1, 31);
    printf("Введите час: ");    result.hour   = input_int(0, 23);
    printf("Введите минуту: "); result.minute = input_int(0, 59);

    return result;
}

endpoint input_endpoint() {
    endpoint result = {};
    printf("Введите дату:\n");     result.date = input_date();
    printf("Введите название города: "); input_string(result.name, sizeof(result.name));
    return result;
}

entry input_entry() {
    entry result = {};
    printf("Введите город отправления:\n"); result.start = input_endpoint();
    printf("Введите город назначения:\n"); result.end   = input_endpoint();
    printf("Введите стоимость:\n");      result.cost  = input_int(1, INT32_MAX);
    return result;
}
