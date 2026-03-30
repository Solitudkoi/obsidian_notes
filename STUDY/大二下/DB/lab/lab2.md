实验2  SQL数据定义和操作
实验目的：
1. 掌握关系数据库语言SQL的使用。
2. 面向某个应用场景定义数据模式和操作数据。
实验平台：
数据库管理系统（推荐使用）： SQL Server, MySQL, OpenGauss, PolarDB
实验内容和要求：
1. 以某个应用场景（如Banking）为例，建立数据库。
2. 数据定义：表的建立、删除、修改；索引的建立、删除；视图的建立、删除。
3. 数据更新：用insert/delete/update语句插入/删除/更新表数据。
4. 数据查询：单表查询、多表查询、嵌套子查询等。
5. 视图操作：通过视图进行数据查询和数据更新。
6. 完成实验报告，要求用截图+文字描述实验完成过程。

![[Pasted image 20260330101958.png]]


```
mysql -u root -p

SHOW DATABASES;

 CREATE DATABASE lab2_banking;
 
 USE lab2_banking;

CREATE TABLE branch(
branch_name VARCHAR(50) PRIMARY KEY,
branch_city VARCHAR(50),
assets INT
);

CREATE TABLE depositor(
customer_name VARCHAR(50),
account_number INT,
PRIMARY KEY(customer_name, account_number),
FOREIGN KEY(customer_name) REFERENCES customer(customer_name),
FOREIGN KEY(account_number) REFERENCES account(account_number)
);
    
CREATE TABLE account(
account_number INT PRIMARY KEY,
branch_name VARCHAR(50),
balance INT,
FOREIGN KEY(branch_name) REFERENCES branch(branch_name)
);
    
CREATE TABLE customer(
customer_name VARCHAR(50) PRIMARY KEY,
customer_street VARCHAR(50),
customer_city VARCHAR(50)
);
    
CREATE TABLE borrow(
customer_name VARCHAR(50),
loan_number INT,
PRIMARY KEY(customer_name, loan_number),
FOREIGN KEY(customer_name) REFERENCES customer(customer_name),
FOREIGN KEY(loan_number) REFERENCES loan(loan_number)
);
    
 CREATE TABLE loan(
loan_number INT PRIMARY KEY,
branch_name VARCHAR(50),
amount INT,
FOREIGN KEY(branch_name) REFERENCES branch(branch_name)
);

SHOW TABLES;

DESCRIBE account;

DROP TABLE account;

SHOW TABLES;

ALTER TABLE branch
ADD manager VARCHAR(50);

DESCRIBE branch;


```