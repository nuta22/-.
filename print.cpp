#include "print.h"
#include <stdio.h>

char const *month_string(uint32_t month) {
    switch (month) {
        case 1: return "Января";
        case 2: return "Февраля";
        case 3: return "Марта";
        case 4: return "Апреля";
        case 5: return "Мая";
        case 6: return "Июня";
        case 7: return "Июля";
        case 8: return "Августа";
        case 9: return "Сентября";
        case 10: return "Октября";
        case 11: return "Ноября";
        case 12: return "Декабря";
    }
    return "";
}

void print(date date) {
    printf("%d:%d %d %s %d", date.minute, date.hour, date.day, month_string(date.month), date.year);
}

void print(endpoint endpoint) {
    printf("%s", endpoint.name);
    print(endpoint.date);
}

void print(entry entry) {
    printf("%s ", entry.start.name);
    print(entry.start.date);
    printf(" -> %s ", entry.end.name);
    print(entry.end.date);
    printf("\nСтоимость: %d\n\n", entry.cost);
}
