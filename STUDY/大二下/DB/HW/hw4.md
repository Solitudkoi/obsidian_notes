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


```