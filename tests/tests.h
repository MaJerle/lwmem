/**
 * \file            tests.h
 * \author          Tilen MAJERLE <tilen@majerle.eu>
 * \brief           
 * \version         0.1
 * \date            2025-03-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef TESTS_HDR_H
#define TESTS_HDR_H

/* Assert check */
#define ASSERT(x)                                                                                                      \
    do {                                                                                                               \
        if (!(x)) {                                                                                                    \
            printf("Assert in file %s and on line %d failed with condition (" #x ")\r\n", __FILE__, (int)__LINE__);    \
            return -1;                                                                                                 \
        }                                                                                                              \
    } while (0)

#endif /* TESTS_HDR_H */