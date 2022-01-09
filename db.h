#ifndef __DB_HANDLER__H__
#define __DB_HANDLER__H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <sqlite3.h>

#include "list.h"
#include "element.h"

List prev_track_list;

int callback(void * NotUsed, int argc, char ** argv, char ** azColName);

int open_db(char * file_path);
void close_db();

int create_table(char * table_name, int exist_drop, int param_count, ...);
int insert_values(char * table_name, int param_count, ...);
int select_table(char * table_name);
int db_available();
#endif