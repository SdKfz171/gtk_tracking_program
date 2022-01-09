#include "db.h"

sqlite3 * prev_track_db;

int callback(void * NotUsed, int argc, char ** argv, char ** azColName)
{    
    NotUsed = 0;

    for (int i = 0; i < argc; i++)
    {
        TrackingOption * option = (TrackingOption *)malloc(sizeof(TrackingOption));
        if(strcmp(azColName[i], "Invoice") == 0){
            option->tracking_number = (char *)malloc(sizeof(argv[i]));
            strcpy(option->tracking_number, argv[i]);
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

            i++;

            option->carrier_id = (char *)malloc(sizeof(argv[i]));
            strcpy(option->carrier_id, argv[i]);
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            
            if(!list_size(&prev_track_list)) list_insert_next(&prev_track_list, NULL, option);
            else list_insert_next(&prev_track_list, list_tail(&prev_track_list), option);
        }
    }

    printf("\n");
    
    return 0;
}

int open_db(char * file_path)
{
    char * err_msg = 0;
    int rc = sqlite3_open(file_path, &prev_track_db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(prev_track_db));
        sqlite3_close(prev_track_db);
        
        return 0;
    }
    return 1;
}

void close_db()
{
    sqlite3_close(prev_track_db);
}

int create_table(char * table_name, int exist_drop, int param_count, ...)
{
    va_list ap;
    va_start(ap, param_count);

    char * temp;
    int count = 0;
    char sql[256] = "";

    char create[BUFSIZ];
    sprintf(create, "create table %s(", table_name);

    for(count = 0; count < param_count; count++){
        temp = va_arg(ap, char*);
        
        printf("param%d: %s\r\n", count, temp);
        strcat(create, temp);
        strcat(create, ", ");
    }
    va_end(ap);

    if (count < 1){
        return 0;
    }
    else {
        // drop table
        if(exist_drop){
            char drop[64];
            sprintf(drop, "drop table if exists %s;", table_name);

            strcat(sql, drop);
        }

        // create table
        int len = 0;
        while(create[len] != NULL) len++;
        create[len-2] = ')';
        create[len-1] = ';';
        create[len] = NULL;

        strcat(sql, create);
        
        len = 0;
        while(sql[len] != NULL) len++;
        sql[len] = NULL;

        puts(sql);

        char * err_msg = 0;
        int rc = sqlite3_exec(prev_track_db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);        
            sqlite3_close(prev_track_db);
            
            return 0;
        }
    }

    return 1;
}

int insert_values(char * table_name, int param_count, ...)
{
    // insert into TrackData values(4, '6078990235408', 'kr.epost');

    va_list ap;
    va_start(ap, param_count);

    char * temp;
    int count = 0;
    char sql[256] = "";

    char insert[BUFSIZ];
    sprintf(insert, "insert into %s(", table_name);
    for(count = 0; count < param_count; count++){
        temp = va_arg(ap, char*);
        
        strcat(insert, temp);
        strcat(insert, ", ");
    }

    if (count < 1){
        return 0;
    }
    else {
        // insert values
        int len = 0;
        while(insert[len] != NULL) len++;
        insert[len-2] = ')';
        insert[len-1] = NULL;

        strcat(insert, " values(");
    }

    for(count = 0; count < param_count; count++){
        temp = va_arg(ap, char*);
        
        strcat(insert, temp);
        strcat(insert, ", ");
    }
    va_end(ap);

    if (count < 1){
        return 0;
    }
    else {
        // insert values
        int len = 0;
        while(insert[len] != NULL) len++;
        insert[len-2] = ')';
        insert[len-1] = ';';
        insert[len] = NULL;

        printf("insert: %s\r\n", insert);

        strcat(sql, insert);
        
        len = 0;
        while(sql[len] != NULL) len++;
        sql[len] = NULL;

        puts(sql);

        char * err_msg = 0;
        int rc = sqlite3_exec(prev_track_db, sql, 0, 0, &err_msg);

        if (rc != SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);        
            sqlite3_close(prev_track_db);
            
            return 0;
        }
    }

    return 1;
}

int delete_values_by_id(char * table_name, int * id)
{
    // delete from TrackData where Id=1;

    char * temp;
    int count = 0;

    char sql[BUFSIZ];
    sprintf(sql, "delete from %s where Id=%d;", table_name, id);

    // delete
    printf("delete: %s\r\n", sql);
    
    // puts(sql);

    char * err_msg = 0;
    int rc = sqlite3_exec(prev_track_db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(prev_track_db);
        
        return 0;
    }

    return 1;
}

int select_table(char * table_name)
{
    // select * from table_name;
    list_init(&prev_track_list);

    char sql[64];
    char * err_msg = 0;

    sprintf(sql, "select * from %s;", table_name);
        
    int rc = sqlite3_exec(prev_track_db, sql, callback, 0, &err_msg);
     
    if (rc != SQLITE_OK )
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(prev_track_db);
        
        return 0;
    } 
    return 1;
}

int db_available()
{
    char * err_msg = 0;
    int rc = sqlite3_exec(prev_track_db, "", 0, 0, &err_msg);
    if(rc != SQLITE_OK) return 0;
    else return 1;
}

#if 0
int main(int argc, char *argv[])
{
    open_db("table.db");
    create_table("test_table", 1, 3, "Id integer primary key autoincrement", "Invoice text", "Carrier text");
    insert_values("test_table", 2, "Invoice", "Carrier", "'6078990235408'", "'kr.epost'");
    insert_values("test_table", 2, "Invoice", "Carrier", "'1234567890'", "'kr.lotte'");
    delete_values_by_id("test_table", 1);
    select_table("test_table");
    close_db();

    printf("%d\r\n", db_available());

    return 0;
}
#endif