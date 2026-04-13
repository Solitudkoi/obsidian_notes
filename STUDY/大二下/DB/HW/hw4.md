4.7

Consider the employee database of Figure 4.12. Give an SQL DDL definition of this database. Identify referential-integrity constraints that should hold, and include them in the DDL definition.

![[Pasted image 20260412203653.png]]

```
create table employee(
    ID integer,
    person_name varchar(20),
    street varchar(20),
    city varchar(20),
    primary key (ID)
);

create table works(
    ID integer,
    company_name varchar(20),
    salary integer,
    primary key (ID),
    foreign key (ID) references employee(ID),
    foreign key (company_name) references company(company_name)
);

create table company(
    company_name varchar(20),
    city varchar(20),
    primary key (company_name)
);

create table manages(
    ID integer,
    manager_id integer,
    primary key (ID),
    foreign key (ID) references employee(ID),
    foreign key (manager_id) references employee(ID)
);
```
![[Pasted image 20260412203653.png]]
4.18

For the database of Figure 4.12, write a query to find the ID of each employee with no manager. Note that an employee may simply have no manager listed or may have a null manager. Write your query using an outer join and then write it again using no outer join at all.


5.4

Describe the circumstances in which you would choose to use embedded SQL rather than SQL alone or only a general-purpose programming language.

5.13

Suppose you were asked to define a class MetaDisplay in Java, containing a method static void printTable(String r); the method takes a relation name r as input, executes the query “select * from r”, and prints the result out in tabular format, with the attribute names displayed in the header of the table.
a. What do you need to know about relation r to be able to print the result in the specified tabular format?
b. What JDBC methods(s) can get you the required information?
c. Write the method printTable(String r) using the JDBC API.


5.14

Repeat Exercise 5.13 using ODBC, defining void printTable(char *r) as a function instead of a method.


5.19

Suppose there are two relations r and s, such that the foreign key B ofr references the primary key A of s. Describe how the trigger mechanism can be used to implement the on delete cascade option when a tuple is deleted from s.






```
create trigger overdraft_trigger 
after update of balance on account      ← 【什么时候】balance 被更新后
referencing new row as nrow              ← 小工具：给"新值"起个名字叫 nrow
for each row                             ← 小工具：每改一行跑一次
when nrow.balance < 0                    ← 【什么条件】新余额是负数
begin atomic
    insert into borrower                 
        values (nrow.customer, -nrow.balance);   ← 【做什么】①记欠款
    update account set balance = 0               ← 【做什么】②拉回 0
        where account_number = nrow.account_number;
end
```
>在 employee 表上建触发器，只要有人把某员工的 salary 改成比原来低的值，就把这次降薪记录插到 salary_log 表（记员工ID、旧工资、新工资）。

```
create trigger employee_trigger
after update of salary on employee
referencing new row as nrow 
for each row
when nrow.salary<employee.salay
begin atomic
	insert into salary_log
		values(nrow.id,employee.salary,nrow.salary)
end
```

- 把 `instructor` 表的查询权限给 Alice，并允许她转授。
- **Alice 把这个权限授给 Bob。**（提示：现在是 Alice 在执行这条语句，不是 DBA）
- **DBA 想一次性把 Alice 和 Bob 的权限都撤掉——写一条语句。**（提示：关键词是"一次性"和"连锁"）
```
GRANT SELECT ON instructor TO Alice with grant option;
GRANT SELECT ON instructor TO Bob;
REVOKE SELECT ON instructor FROM Alice CASCADE;
```


我来帮你完成这次数据库作业。我会先用中文解释思路，然后给出英文答案。

---

## 4.7 SQL DDL 定义

**中文思路：** 根据图4.12的四个关系模式，需要定义主键（下划线标注的属性）和外键（参照完整性约束）：

- `employee` 的主键是 ID
- `works` 中 ID 参照 employee，company_name 参照 company
- `company` 的主键是 company_name
- `manages` 中 ID 和 manager_id 都参照 employee（manager_id 是经理的员工 ID）

**English Answer:**

```sql
CREATE TABLE employee (
    ID          VARCHAR(20),
    person_name VARCHAR(50) NOT NULL,
    street      VARCHAR(50),
    city        VARCHAR(30),
    PRIMARY KEY (ID)
);

CREATE TABLE company (
    company_name VARCHAR(50),
    city         VARCHAR(30),
    PRIMARY KEY (company_name)
);

CREATE TABLE works (
    ID           VARCHAR(20),
    company_name VARCHAR(50),
    salary       NUMERIC(8,2) CHECK (salary >= 0),
    PRIMARY KEY (ID),
    FOREIGN KEY (ID) REFERENCES employee(ID)
        ON DELETE CASCADE,
    FOREIGN KEY (company_name) REFERENCES company(company_name)
        ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE manages (
    ID         VARCHAR(20),
    manager_id VARCHAR(20),
    PRIMARY KEY (ID),
    FOREIGN KEY (ID)         REFERENCES employee(ID) ON DELETE CASCADE,
    FOREIGN KEY (manager_id) REFERENCES employee(ID) ON DELETE SET NULL
);
```

---

## 4.18 查找没有经理的员工

**中文思路：** "没有经理"有两种情况：(1) 在 manages 表里根本没这条记录；(2) 有记录但 manager_id 为 NULL。外连接能把两种情况都抓出来。

**English Answer:**

Using an outer join:

```sql
SELECT E.ID
FROM   employee E LEFT OUTER JOIN manages M ON E.ID = M.ID
WHERE  M.manager_id IS NULL;
```

Without any outer join:

```sql
SELECT ID FROM employee
WHERE  ID NOT IN (SELECT ID FROM manages WHERE manager_id IS NOT NULL);
```

---

## 5.4 何时使用嵌入式 SQL

**中文思路：**

- 纯 SQL 不够：SQL 是声明式的，没有循环、条件分支、复杂用户交互这些能力，做不了完整应用程序。
- 纯通用语言不够：自己写数据访问逻辑非常麻烦，还不能利用 DBMS 的查询优化器、事务、并发控制等能力。
- 嵌入式 SQL 适用场景：需要在应用程序中和数据库交互，且查询结构在编译时就已知（静态）。相比动态 SQL（如 JDBC），嵌入式 SQL 能在编译期做语法和类型检查，性能也更好。当查询在运行时才能决定时，才用动态 SQL。

**English Answer:**

Embedded SQL is the right choice when an application needs both (a) the data-manipulation power of SQL and (b) the control structures, I/O, user-interaction, and computational abilities of a host language that SQL by itself does not provide. Pure SQL cannot build a full application — it has no loops, branches, or UI — and writing data access purely in a general-purpose language forces you to reimplement query optimization, transaction management, and concurrency control that the DBMS already provides.

Embedded SQL is preferred over dynamic SQL (e.g., JDBC/ODBC) when the SQL statements are known at compile time, because the precompiler can check syntax and types at compile time, catching errors early, and the queries can be optimized once instead of on every execution. Typical cases: business applications with fixed, well-defined queries (payroll, inventory, banking transactions) where performance and compile-time safety matter. Use dynamic SQL (JDBC/ODBC) instead only when the query itself is not known until runtime (e.g., an ad-hoc query tool).

---

## 5.13 JDBC printTable

**中文思路：**

**(a)** 要按表格格式输出，需要知道：关系 r 的**属性数量**和每个属性的**名称**（用作表头）。

**(b)** 执行查询后得到 `ResultSet`，调用 `getMetaData()` 得到 `ResultSetMetaData`，然后用 `getColumnCount()` 和 `getColumnName(i)`。

**(c)** 代码如下：

**English Answer:**

**(a)** To print the result in tabular format, we need to know: the **number of attributes** in relation r, and the **name of each attribute** (for the header row). We may also want each attribute's type/display width for nicer formatting, but the count and names are the essentials.

**(b)** After executing the query, call `ResultSet.getMetaData()` to obtain a `ResultSetMetaData` object, then use `getColumnCount()` to get the number of columns and `getColumnName(i)` to get each column's name (also `getColumnDisplaySize(i)` if you want widths).

**(c)**

```java
import java.sql.*;

public class MetaDisplay {
    public static void printTable(String r) {
        String url  = "jdbc:oracle:thin:@//host:1521/db";
        String user = "username";
        String pass = "password";

        try (Connection conn = DriverManager.getConnection(url, user, pass);
             Statement  stmt = conn.createStatement();
             ResultSet  rs   = stmt.executeQuery("select * from " + r)) {

            ResultSetMetaData md = rs.getMetaData();
            int n = md.getColumnCount();

            // print header
            for (int i = 1; i <= n; i++) {
                System.out.print(md.getColumnName(i));
                if (i < n) System.out.print("\t");
            }
            System.out.println();

            // print rows
            while (rs.next()) {
                for (int i = 1; i <= n; i++) {
                    System.out.print(rs.getString(i));
                    if (i < n) System.out.print("\t");
                }
                System.out.println();
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
}
```

---

好问题。题目说 "Repeat Exercise 5.13 using ODBC"，意思是**同样的三问都要做一遍**，只不过用 ODBC 回答。a 和 b 的**结论本质一样**（都是要知道列数和列名），但 b 要换成**ODBC 对应的函数**。

---

## 5.14 完整答案

### (a) 需要知道什么？

和 5.13 一样：关系 r 的**列数**和每个**列名**（用作表头）。

### (b) ODBC 用哪些函数？

- `SQLNumResultCols()` — 获取结果集的列数
- `SQLDescribeCol()` — 获取每一列的列名（以及类型、长度等）
- 配合 `SQLFetch()` 取行、`SQLGetData()` 取字段值

**对比 5.13**：

|功能|JDBC（5.13）|ODBC（5.14）|
|---|---|---|
|列数|`ResultSetMetaData.getColumnCount()`|`SQLNumResultCols()`|
|列名|`ResultSetMetaData.getColumnName(i)`|`SQLDescribeCol()`|
|取下一行|`ResultSet.next()`|`SQLFetch()`|
|取列值|`ResultSet.getString(i)`|`SQLGetData()`|

### (c) 代码

就是我之前给你的那段 C 代码，这里再贴一遍方便你整合：

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

---

## 5.19 用触发器实现 ON DELETE CASCADE

**中文思路：** 当 s 中某元组被删除时，需要自动删除 r 中所有 B 值等于被删除元组 A 值的行。用 AFTER DELETE 触发器监听 s 上的删除操作，然后在 r 上执行对应删除。注意 r 自身也可能被其他表参照，删除 r 的行可能递归触发更多触发器——这是期望的级联行为。

**English Answer:**

Create an `AFTER DELETE` trigger on s. For each row deleted from s, the trigger deletes from r all tuples whose foreign key B matches the deleted tuple's A value.

```sql
CREATE TRIGGER cascade_delete
AFTER DELETE ON s
REFERENCING OLD ROW AS orow
FOR EACH ROW
    DELETE FROM r
    WHERE  r.B = orow.A;
```

Notes:

- `AFTER` (not `BEFORE`) is used so the deletion on s is already in place before r is touched, avoiding constraint-check conflicts.
- If r itself has foreign keys referenced by other tables with their own cascade triggers, the deletions in r will recursively fire those triggers — producing the full cascading effect expected of `ON DELETE CASCADE`.
- If B allowed `ON DELETE SET NULL` behavior instead, the trigger body would be `UPDATE r SET B = NULL WHERE r.B = orow.A;`.

---

需要我把这些整理成一个可下载的 PDF 或 Word 文档吗？