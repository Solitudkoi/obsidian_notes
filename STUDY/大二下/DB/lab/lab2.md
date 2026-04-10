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




## 三、索引的建立与删除

```sql
CREATE INDEX idx_account_branch ON account(branch_name);
CREATE INDEX idx_loan_branch ON loan(branch_name);
CREATE INDEX idx_customer_city ON customer(customer_city);

SHOW INDEX FROM account;

DROP INDEX idx_customer_city ON customer;


CREATE INDEX idx_customer_city ON customer(customer_city);
SHOW INDEX FROM customer\G
DROP INDEX idx_customer_city ON customer;
SHOW INDEX FROM customer\G

```

## 四、插入数据

```sql
INSERT INTO branch VALUES
('Downtown', 'Brooklyn', 9000000),
('Perryridge', 'Horseneck', 1700000),
('Mianus', 'Horseneck', 400000),
('Round Hill', 'Horseneck', 8000000),
('Brighton', 'Brooklyn', 7100000);

INSERT INTO customer VALUES
('Adams', 'Spring', 'Pittsfield'),
('Brooks', 'Senator', 'Brooklyn'),
('Curry', 'North', 'Rye'),
('Glenn', 'Sand Hill', 'Woodside'),
('Green', 'Walnut', 'Stamford'),
('Hayes', 'Main', 'Harrison'),
('Johnson', 'Alma', 'Palo Alto'),
('Jones', 'Main', 'Harrison'),
('Lindsay', 'Park', 'Pittsfield'),
('Smith', 'North', 'Rye'),
('Turner', 'Putnam', 'Stamford'),
('Williams', 'Nassau', 'Princeton');

INSERT INTO account VALUES
(101, 'Downtown', 500),
(215, 'Mianus', 700),
(102, 'Perryridge', 400),
(305, 'Round Hill', 350),
(201, 'Brighton', 900),
(217, 'Brighton', 750),
(222, 'Downtown', 700);

INSERT INTO loan VALUES
(17, 'Downtown', 1000),
(23, 'Perryridge', 2000),
(15, 'Perryridge', 1500),
(14, 'Downtown', 1500),
(93, 'Mianus', 500),
(11, 'Round Hill', 900),
(16, 'Perryridge', 1300);

INSERT INTO depositor VALUES
('Johnson', 101),
('Smith', 215),
('Hayes', 102),
('Turner', 305),
('Johnson', 201),
('Jones', 217),
('Lindsay', 222);

INSERT INTO borrow VALUES
('Jones', 17),
('Smith', 23),
('Hayes', 15),
('Jackson', 14),  -- 这一行会因外键失败，演示用，可去掉
('Curry', 93),
('Smith', 11),
('Williams', 17),
('Adams', 16);
```

把那行 `Jackson` 删掉再执行（或者用它来演示外键约束报错，作为实验报告里的一个亮点）。

## 五、数据更新

```sql
-- UPDATE：所有 Perryridge 支行的账户余额加 5%
UPDATE account SET balance = balance * 1.05 WHERE branch_name = 'Perryridge';

-- UPDATE：余额大于 600 的账户加 6%，否则加 5%（CASE）
UPDATE account
SET balance = CASE
  WHEN balance > 600 THEN balance * 1.06
  ELSE balance * 1.05
END;

-- DELETE：删除 Mianus 支行的所有贷款
DELETE FROM borrow WHERE loan_number IN (SELECT loan_number FROM loan WHERE branch_name = 'Mianus');
DELETE FROM loan WHERE branch_name = 'Mianus';

SELECT * FROM account;
SELECT * FROM loan;
```

## 六、数据查询

**单表查询：**

```sql
-- 1. 查询所有支行
SELECT * FROM branch;

-- 2. 查询资产大于 500 万的支行
SELECT branch_name, assets FROM branch WHERE assets > 5000000;

-- 3. 按资产降序排列
SELECT branch_name, assets FROM branch ORDER BY assets DESC;

-- 4. 统计每个支行的账户数
SELECT branch_name, COUNT(*) AS account_count, AVG(balance) AS avg_balance
FROM account GROUP BY branch_name;
```

**多表查询（连接）：**

```sql
-- 5. 查询所有在 Perryridge 支行有账户的客户姓名
SELECT DISTINCT d.customer_name
FROM depositor d JOIN account a ON d.account_number = a.account_number
WHERE a.branch_name = 'Perryridge';

-- 6. 查询每个客户的姓名、城市和账户余额
SELECT c.customer_name, c.customer_city, a.account_number, a.balance
FROM customer c
JOIN depositor d ON c.customer_name = d.customer_name
JOIN account a ON d.account_number = a.account_number;
```

**嵌套子查询：**

```sql
-- 7. 查询在 Perryridge 支行有账户的所有客户
SELECT customer_name FROM depositor
WHERE account_number IN (
  SELECT account_number FROM account WHERE branch_name = 'Perryridge'
);

-- 8. 查询既有账户又有贷款的客户
SELECT DISTINCT customer_name FROM depositor
WHERE customer_name IN (SELECT customer_name FROM borrow);

-- 9. 查询余额高于 Downtown 支行平均余额的账户
SELECT * FROM account
WHERE balance > (
  SELECT AVG(balance) FROM account WHERE branch_name = 'Downtown'
);

-- 10. 使用 EXISTS：查询所有有账户的客户
SELECT customer_name FROM customer c
WHERE EXISTS (SELECT 1 FROM depositor d WHERE d.customer_name = c.customer_name);
```

## 七、视图的建立、查询、更新与删除

```sql
-- 创建视图：每个支行的总存款
CREATE VIEW branch_total_deposit AS
SELECT branch_name, SUM(balance) AS total_balance
FROM account GROUP BY branch_name;

-- 创建视图：客户及其账户信息
CREATE VIEW customer_account_view AS
SELECT c.customer_name, c.customer_city, a.account_number, a.branch_name, a.balance
FROM customer c
JOIN depositor d ON c.customer_name = d.customer_name
JOIN account a ON d.account_number = a.account_number;

-- 通过视图查询
SELECT * FROM branch_total_deposit;
SELECT * FROM customer_account_view WHERE customer_city = 'Harrison';

-- 通过视图更新（注意：聚合视图不可更新，简单视图可以）
CREATE VIEW perryridge_account AS
SELECT account_number, branch_name, balance FROM account WHERE branch_name = 'Perryridge';

UPDATE perryridge_account SET balance = balance + 100 WHERE account_number = 102;
SELECT * FROM account WHERE account_number = 102;

-- 删除视图
DROP VIEW perryridge_account;
DROP VIEW branch_total_deposit;
DROP VIEW customer_account_view;
```

## 实验报告建议结构

1. **实验目的与环境**（MySQL 版本可以用 `SELECT VERSION();` 截图）
2. **数据库与表的创建** —— 附 ER 图说明 6 张表的关系，截图 `SHOW TABLES;` 和 `DESCRIBE`
3. **表的修改** —— ALTER ADD/MODIFY/DROP 各一次截图
4. **索引操作** —— CREATE INDEX、SHOW INDEX、DROP INDEX
5. **数据插入** —— INSERT 后 SELECT 验证
6. **数据更新与删除** —— UPDATE/DELETE 前后对比截图
7. **查询实验** —— 单表 / 多表 / 嵌套子查询各 2-3 个，每个截图 + 文字解释
8. **视图操作** —— 创建、查询、通过视图更新、删除
9. **遇到的问题与解决** —— 比如建表顺序导致的外键错误（你最初的脚本就是个很好的例子，可以写进去）
10. **实验小结**

按这个走完，所有 lab 要求点都覆盖到了。每执行一组命令就截一张图，最后拼到报告里就行。需要我帮你把这些整理成一份 Word 实验报告模板吗？