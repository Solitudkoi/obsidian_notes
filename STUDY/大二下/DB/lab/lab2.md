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

DESCRIBE branch;

ALTER TABLE branch
ADD manager VARCHAR(50);

DESCRIBE branch;


```


# Step 5 创建索引（截图⑤）

CREATE INDEX idx_customer_city  
ON customer(customer_city);

查看索引：

SHOW INDEX FROM customer;

---

# Step 6 删除索引（截图⑥）

DROP INDEX idx_customer_city  
ON customer;

验证：

SHOW INDEX FROM customer;

---

# Step 7 插入数据（截图⑦）

插入 branch

INSERT INTO branch VALUES  
('ICBC','Beijing',500000,'Zhang'),  
('CCB','Shanghai',400000,'Li');

---

插入 customer

INSERT INTO customer VALUES  
('Alice','ZJU Street','Hangzhou'),  
('Bob','PKU Street','Beijing');

---

插入 account

INSERT INTO account VALUES  
(101,'ICBC',3000),  
(102,'CCB',5000);

---

插入 loan

INSERT INTO loan VALUES  
(201,'ICBC',10000),  
(202,'CCB',20000);

---

插入 depositor

INSERT INTO depositor VALUES  
('Alice',101),  
('Bob',102);

---

插入 borrow

INSERT INTO borrow VALUES  
('Alice',201),  
('Bob',202);

截图：

SELECT * FROM account;

---

# Step 8 UPDATE操作（截图⑧）

UPDATE account  
SET balance = 8000  
WHERE account_number = 101;

验证：

SELECT * FROM account;

---

# Step 9 DELETE操作（截图⑨）

DELETE FROM account  
WHERE account_number = 102;

验证：

SELECT * FROM account;

---

# Step 10 单表查询（截图⑩）

SELECT * FROM customer;

---

# Step 11 多表查询 JOIN（截图⑪ ⭐重点）

SELECT customer.customer_name, account.balance  
FROM customer, depositor, account  
WHERE customer.customer_name = depositor.customer_name  
AND depositor.account_number = account.account_number;

老师最看重这个查询之一 ⭐

---

# Step 12 子查询（截图⑫ ⭐重点）

查询余额最高账户：

SELECT *  
FROM account  
WHERE balance = (  
SELECT MAX(balance)  
FROM account  
);

---

# Step 13 创建视图 VIEW（截图⑬ ⭐重点）

CREATE VIEW account_view AS  
SELECT customer.customer_name, account.balance  
FROM customer, depositor, account  
WHERE customer.customer_name = depositor.customer_name  
AND depositor.account_number = account.account_number;

验证：

SELECT * FROM account_view;

---

# Step 14 通过视图更新数据（截图⑭ ⭐加分）

UPDATE account_view  
SET balance = 9000  
WHERE customer_name = 'Alice';

验证：

SELECT * FROM account;

---

# Step 15 删除视图（截图⑮）

DROP VIEW account_view;

验证：

SHOW FULL TABLES  
WHERE TABLE_TYPE='VIEW';

---

#  删除表

实验要求包含删除表：

DROP TABLE borrow;

验证：

SHOW TABLES;