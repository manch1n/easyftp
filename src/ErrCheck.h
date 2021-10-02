#pragma once
#include <stdio.h>
#include <stdlib.h>
#define ASSERT_EXIT(expresion, ...)                                           \
    do                                                                        \
    {                                                                         \
        if (!(expresion))                                                     \
        {                                                                     \
            fprintf(stderr, "error in file:%s line:%d ", __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__);                                     \
            exit(-1);                                                         \
        }                                                                     \
    } while (0)

#define EASY_LOG(...)                 \
    do                                \
    {                                 \
        fprintf(stdout, __VA_ARGS__); \
    } while (0)
