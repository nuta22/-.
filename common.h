#pragma once
#include <stdint.h>

struct date {
    uint32_t year   : 12; // 4096
    uint32_t month  : 4;  // 16
    uint32_t day    : 5;  // 32
    uint32_t hour   : 5;  // 32
    uint32_t minute : 6;  // 64
};

struct endpoint {
    date date;
    char name[256];};


struct entry {
    endpoint start;
    endpoint end;
    uint32_t cost;
};
