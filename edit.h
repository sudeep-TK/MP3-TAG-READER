#ifndef EDIT_H
#define EDIT_H

#include "common.h"

Status read_and_validation_for_edit(char *argv[]);
Status to_edit(char *filename, char *option, char *value);

#endif