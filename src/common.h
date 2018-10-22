/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <QMessageBox>
#include <cassert>
#include "../src/types.h"

const char PROGRAM_TITLE[] = "\"AV Scissors\" by Tarpeeksi Hyvae Soft";

#define k_assert(condition, error_string)   if (!(condition))\
                                            {\
                                                QMessageBox::critical(nullptr, "AV Scissors assertion failure", error_string); /*Notify in a user-friendly way.*/\
                                                assert(condition && error_string);\
                                            }

#define INFO(args)  (printf("[info ] {%s:%i} ", __FILE__, __LINE__), printf args, printf("\n"), fflush(stdout))
#define DEBUG(args) (printf("[debug] {%s:%i} ", __FILE__, __LINE__), printf args, printf("\n"), fflush(stdout))
#define NBENE(args) (fprintf(stderr, "[ERROR] {%s:%i} ", __FILE__, __LINE__), printf args, printf("\n"), fflush(stdout))

#endif
