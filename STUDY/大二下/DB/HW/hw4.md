![[a3ae3fc7043613da3e6152ce843e03fc.jpg]]
![[05a18e9b02e0212e1d61268eb8be58b0.jpg]]
![[197c587c0173f3be843c3f258ba17971.jpg]]
![[25b2962879866316dd1dc0e5fd60ee75.jpg]]
![[98ec8e5ec0fc9a6256f6ef0ea38f68b9.jpg]]
### (c) 

```c
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>

void printTable(char *r) {
    SQLHENV  env;
    SQLHDBC  conn;
    SQLHSTMT stmt;
    char query[256];
    SQLSMALLINT numCols;
    SQLCHAR colName[64];
    SQLSMALLINT nameLen, dataType, decDigits, nullable;
    SQLULEN colSize;
    char buf[256];
    SQLLEN indicator;

    SQLAllocHandle(SQL_HANDLE_ENV,  SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
    SQLAllocHandle(SQL_HANDLE_DBC,  env, &conn);
    SQLConnect(conn, (SQLCHAR *)"DSN", SQL_NTS,
                     (SQLCHAR *)"user", SQL_NTS,
                     (SQLCHAR *)"pass", SQL_NTS);
    SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);

    sprintf(query, "select * from %s", r);
    SQLExecDirect(stmt, (SQLCHAR *)query, SQL_NTS);

    SQLNumResultCols(stmt, &numCols);

    /* print header */
    for (int i = 1; i <= numCols; i++) {
        SQLDescribeCol(stmt, i, colName, sizeof(colName), &nameLen,
                       &dataType, &colSize, &decDigits, &nullable);
        printf("%s%s", colName, (i < numCols) ? "\t" : "\n");
    }

    /* print rows */
    while (SQLFetch(stmt) == SQL_SUCCESS) {
        for (int i = 1; i <= numCols; i++) {
            SQLGetData(stmt, i, SQL_C_CHAR, buf, sizeof(buf), &indicator);
            printf("%s%s",
                   (indicator == SQL_NULL_DATA) ? "NULL" : buf,
                   (i < numCols) ? "\t" : "\n");
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    SQLDisconnect(conn);
    SQLFreeHandle(SQL_HANDLE_DBC, conn);
    SQLFreeHandle(SQL_HANDLE_ENV, env);
}
```
![[5591ad2674e0727810fd4e22cedfa19b.jpg]]