### 实验目的：
---
1. 通过安装某个数据库管理系统，初步了解DBMS的运行环境。
2. 了解DBMS交互界面、图形界面和系统管理工具的使用。
3. 搭建实验平台。
### 实验平台：
--- 
1.操作系统： Windows
2.数据库管理系统：MySQL
### 实验内容和要求：
---
   1.根据某个DBMS的安装说明等文档，安装DBMS。
	   1.1在MySQL官网https://dev.mysql.com/downloads/mysql/5.7.html#downloads上下载最新版本：MYSQL Community Server 9.6.0 
	   ![[屏幕截图 2026-03-11 112547.png]]
	   1.2登录账号，并设置合适的配置后完成下载并应用
	   ![[屏幕截图 2026-03-11 114638.png]]
	   1.3设置环境变量，便于调用mysql命令
		   在系统属性的环境变量中，将MYSQL的可执行命令路径（.../mysql/bin)添加至系统变量的 #PATH 中
	   1.4使用命令开启mysql服务并登录账号
	  ![[Pasted image 20260323130122.png]]
	   
   2.了解基本的DBMS管理功能和操作，创建数据库表。
	 基本交互命令：
	 - 数据库查询：SHOW DATABASES;
	   ![[Pasted image 20260323132231.png]]   
	 - 创建数据库:CREATE DATABASE database_name;
	![[Pasted image 20260323133717.png]]
	 - 进入数据库：USE database_name;
	 ![[Pasted image 20260323133826.png]]
	 - 在数据库中创建表格的schema:CREATE TABLE table_schema();
	 ![[Pasted image 20260323134320.png]]
		 table_schema是table的表格名，括号内为表结构定义，每一行通常表示一个字段。每个字段一般包含字段名、数据类型以及[约束]（没有额外约束时可以默认为空）三个部分。
	 - 显示该database中的表格：SHOW TABLES;
	 ![[Pasted image 20260323134942.png]]
	 - 显示该表格的schema:DESCRIBE table_name;
	 ![[Pasted image 20260323135107.png]]
	 - 插入表格数据：INSERT INTO table VALUES(...);
	 
   3.查询表中的内容。
