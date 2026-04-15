实验3  SQL数据完整性
实验目的：
熟悉通过SQL进行数据完整性控制的方法。
实验平台：
数据库管理系统（推荐使用）：SQL Server、MySQL、OpenGauss、PolarDB
实验内容和要求：
1.        定义若干表，其中包括primary key, foreign key 和check的定义。
2.        表中插入数据，考察primary key如何控制实体完整性。
3.        删除被引用表中的行，考察foreign key 中on delete 子句如何控制参照完整性。
4.        修改被引用表中的行的primary key，考察foreign key 中on update 子句如何控制参照完整性。
5.        修改或插入表中数据，考察check子句如何控制校验完整性。
6.        定义一个trigger,并通过修改表中数据考察触发器如何起作用。
7.        完成实验报告，要求用截图+文字描述实验完成过程。


```
-- ============================================================
-- 数据库实验3：SQL数据完整性
-- 数据库：MySQL（同样适用于 MariaDB / OpenGauss / PolarDB）
-- 说明：按顺序执行每个 STEP，观察输出结果并截图
-- ============================================================


mysql -u root -p
SHOW DATABASES;
CREATE DATABASE lab2_banking;
USE lab2_banking;
 
-- 准备：重建实验数据库
DROP DATABASE IF EXISTS lab3_integrity;
CREATE DATABASE lab3_integrity CHARACTER SET utf8mb4;
USE lab3_integrity;


-- ===========================================================
-- STEP 1：建表（PRIMARY KEY / FOREIGN KEY / CHECK）
-- ===========================================================

-- 1-1 系部表（被引用的父表）
CREATE TABLE Department (
    dept_id   CHAR(4)      NOT NULL,
    dept_name VARCHAR(50)  NOT NULL,
    location  VARCHAR(100),
    PRIMARY KEY (dept_id)          
);

-- 1-2 学生表（引用 Department，并含 CHECK 约束）
CREATE TABLE Student (
    stu_id    CHAR(10)     NOT NULL,
    stu_name  VARCHAR(30)  NOT NULL,
    gender    CHAR(1)      NOT NULL,
    age       TINYINT      NOT NULL,
    dept_id   CHAR(4),
    PRIMARY KEY (stu_id),           
    FOREIGN KEY (dept_id)
        REFERENCES Department(dept_id)
        ON DELETE SET NULL                            
        ON UPDATE CASCADE,                             
    CHECK (gender IN ('M','F')), 
    CHECK (age BETWEEN 14 AND 60)
);

-- 1-3 课程表
CREATE TABLE Course (
    course_id   CHAR(6)     NOT NULL,
    course_name VARCHAR(60) NOT NULL,
    credit      TINYINT     NOT NULL,
    PRIMARY KEY (course_id),
    CHECK (credit > 0)
);

-- 1-4 选课表（引用 Student 和 Course，含 CHECK）
CREATE TABLE Enrollment (
    stu_id    CHAR(10) NOT NULL,
    course_id CHAR(6)  NOT NULL,
    score     DECIMAL(5,2),
    PRIMARY KEY (stu_id, course_id),
    FOREIGN KEY (stu_id)
        REFERENCES Student(stu_id)
        ON DELETE CASCADE                              
        ON UPDATE CASCADE,
    FOREIGN KEY (course_id)
        REFERENCES Course(course_id)
        ON DELETE RESTRICT                            
        ON UPDATE CASCADE,
    CHECK (score IS NULL OR score BETWEEN 0 AND 100)
);

SELECT '=== STEP 1 完成：表结构创建成功 ===' AS info;
SHOW TABLES;


-- ===========================================================
-- STEP 2：测试 PRIMARY KEY（实体完整性）
-- ===========================================================

-- 先插入合法数据
INSERT INTO Department VALUES ('CS01','计算机学院','A楼');
INSERT INTO Department VALUES ('EE02','电子工程学院','B楼');

INSERT INTO Student VALUES ('2024001001','张三','M',20,'CS01');
INSERT INTO Student VALUES ('2024001002','李四','F',21,'CS01');
INSERT INTO Student VALUES ('2024001003','王五','M',19,'EE02');

SELECT '--- 2-A 合法插入成功 ---' AS info;
SELECT * FROM Student;

-- 2-1 插入重复主键 → 违反实体完整性，应报错
SELECT '--- 2-B 插入重复主键（应报错）---' AS info;
INSERT INTO Student VALUES ('2024001001','赵六','F',22,'CS01');
-- 预期错误：Duplicate entry '2024001001' for key 'PRIMARY'

-- 2-2 插入主键为 NULL → 应报错
SELECT '--- 2-C 插入NULL主键（应报错）---' AS info;
INSERT INTO Student VALUES (NULL,'孙七','M',23,'CS01');
-- 预期错误：Column 'stu_id' cannot be null


-- ===========================================================
-- STEP 3：测试 FOREIGN KEY ON DELETE（参照完整性）
-- ===========================================================

SELECT '=== STEP 3：ON DELETE 测试 ===' AS info;
SELECT '删除前 Student 表（dept_id 应非空）:' AS info;
SELECT stu_id, stu_name, dept_id FROM Student;

-- Student.fk_stu_dept → ON DELETE SET NULL
-- 删除 CS01 系 → Student 中 dept_id 应自动变为 NULL
DELETE FROM Department WHERE dept_id = 'CS01';

SELECT '删除 CS01 后，Student 中原属 CS01 的学生 dept_id 变为 NULL:' AS info;
SELECT stu_id, stu_name, dept_id FROM Student;

-- Enrollment.fk_enroll_course → ON DELETE RESTRICT
-- 先插入选课数据
INSERT INTO Course VALUES ('C001','数据库原理',3);
INSERT INTO Course VALUES ('C002','操作系统',3);
INSERT INTO Enrollment VALUES ('2024001003','C001',85.5);

SELECT '--- 3-B 尝试删除已被选课引用的课程（应报错 RESTRICT）---' AS info;
DELETE FROM Course WHERE course_id = 'C001';
-- 预期错误：Cannot delete or update a parent row: a foreign key constraint fails

-- Enrollment.fk_enroll_stu → ON DELETE CASCADE
-- 删除学生，选课记录应级联删除
SELECT '--- 3-C 删除学生前，Enrollment 记录:' AS info;
SELECT * FROM Enrollment;
DELETE FROM Student WHERE stu_id = '2024001003';
SELECT '删除学生 2024001003 后，Enrollment（应已级联删除）:' AS info;
SELECT * FROM Enrollment;


-- ===========================================================
-- STEP 4：测试 FOREIGN KEY ON UPDATE（参照完整性）
-- ===========================================================

SELECT '=== STEP 4：ON UPDATE 测试 ===' AS info;

-- 重新插入测试数据
INSERT INTO Department VALUES ('CS01','计算机学院','A楼');
INSERT INTO Student VALUES ('2024001001','张三','M',20,'CS01');
INSERT INTO Student VALUES ('2024001004','周八','F',22,'CS01');
INSERT INTO Enrollment VALUES ('2024001001','C001',90);

SELECT '更新前 Student dept_id:' AS info;
SELECT stu_id, stu_name, dept_id FROM Student;

-- Student.fk_stu_dept → ON UPDATE CASCADE
-- 修改 Department 主键 → Student.dept_id 自动级联更新
UPDATE Department SET dept_id = 'CS99' WHERE dept_id = 'CS01';

SELECT 'CS01 → CS99 后，Student.dept_id 自动级联更新:' AS info;
SELECT stu_id, stu_name, dept_id FROM Student;

-- Enrollment.fk_enroll_stu → ON UPDATE CASCADE
SELECT '更新前 Enrollment.stu_id:' AS info;
SELECT * FROM Enrollment;
UPDATE Student SET stu_id = '2024001999' WHERE stu_id = '2024001001';
SELECT '更新学生主键后，Enrollment.stu_id 也级联更新:' AS info;
SELECT * FROM Enrollment;


-- ===========================================================
-- STEP 5：测试 CHECK（校验完整性）
-- ===========================================================

SELECT '=== STEP 5：CHECK 约束测试 ===' AS info;

-- 5-1 gender 不在 {'M','F'} → 应报错
SELECT '--- 5-A 插入非法 gender（应报错）---' AS info;
INSERT INTO Student VALUES ('2024002001','测试A','X',20,'CS99');
-- 预期错误：Check constraint 'chk_gender' is violated

-- 5-2 age 超出范围 → 应报错
SELECT '--- 5-B 插入非法 age（应报错）---' AS info;
INSERT INTO Student VALUES ('2024002002','测试B','M',200,'CS99');
-- 预期错误：Check constraint 'chk_age' is violated

-- 5-3 score 超出 [0,100] → 应报错
SELECT '--- 5-C 插入非法 score（应报错）---' AS info;
INSERT INTO Enrollment VALUES ('2024001999','C001',150);
-- 预期错误：Check constraint 'chk_score' is violated

-- 5-4 credit ≤ 0 → 应报错
SELECT '--- 5-D 插入非法 credit（应报错）---' AS info;
INSERT INTO Course VALUES ('C003','非法课程',-1);
-- 预期错误：Check constraint 'chk_credit' is violated

-- 5-5 合法数据，应成功
SELECT '--- 5-E 合法数据插入（应成功）---' AS info;
INSERT INTO Student VALUES ('2024002003','合法同学','F',18,'CS99');
SELECT * FROM Student WHERE stu_id='2024002003';


-- ===========================================================
-- STEP 6：定义并测试 TRIGGER（触发器）
-- ===========================================================

SELECT '=== STEP 6：TRIGGER 测试 ===' AS info;

-- 6-0 创建日志表
CREATE TABLE Score_Log (
    log_id      INT AUTO_INCREMENT PRIMARY KEY,
    stu_id      CHAR(10),
    course_id   CHAR(6),
    old_score   DECIMAL(5,2),
    new_score   DECIMAL(5,2),
    changed_at  DATETIME DEFAULT CURRENT_TIMESTAMP,
    operator    VARCHAR(30) DEFAULT CURRENT_USER()
);

-- 6-1 AFTER UPDATE 触发器：记录成绩修改日志
DROP TRIGGER IF EXISTS trg_score_update;
DELIMITER $$
CREATE TRIGGER trg_score_update
AFTER UPDATE ON Enrollment
FOR EACH ROW
BEGIN
    IF OLD.score <> NEW.score OR (OLD.score IS NULL AND NEW.score IS NOT NULL) THEN
        INSERT INTO Score_Log(stu_id, course_id, old_score, new_score)
        VALUES (OLD.stu_id, OLD.course_id, OLD.score, NEW.score);
    END IF;
END$$
DELIMITER ;

-- 6-2 BEFORE INSERT 触发器：成绩超过100自动截断为100
DROP TRIGGER IF EXISTS trg_score_cap;
DELIMITER $$
CREATE TRIGGER trg_score_cap
BEFORE INSERT ON Enrollment
FOR EACH ROW
BEGIN
    IF NEW.score > 100 THEN
        SET NEW.score = 100;
    END IF;
END$$
DELIMITER ;

-- 6-3 触发 AFTER UPDATE 触发器
SELECT '--- 6-A 修改成绩前，Enrollment:' AS info;
SELECT * FROM Enrollment WHERE stu_id='2024001999';

UPDATE Enrollment SET score = 95 WHERE stu_id='2024001999' AND course_id='C001';

SELECT '修改成绩后，Enrollment:' AS info;
SELECT * FROM Enrollment WHERE stu_id='2024001999';

SELECT '触发器写入的 Score_Log:' AS info;
SELECT * FROM Score_Log;

-- 6-4 再次修改，观察日志累积
UPDATE Enrollment SET score = 78 WHERE stu_id='2024001999' AND course_id='C001';
SELECT '再次修改后 Score_Log（应有2条）:' AS info;
SELECT * FROM Score_Log;

-- 6-5 触发 BEFORE INSERT 触发器（自动截断超出成绩）
-- 先删掉旧数据避免主键冲突
DELETE FROM Enrollment WHERE stu_id='2024002003';
-- course_id='C001' score=999，触发器应截断为100
INSERT INTO Enrollment VALUES ('2024002003','C001',999);
SELECT '--- 6-B BEFORE INSERT 触发器（999→截断为100）:' AS info;
SELECT * FROM Enrollment WHERE stu_id='2024002003';


-- ===========================================================
-- 最终汇总：展示所有表的当前数据
-- ===========================================================

SELECT '=== 最终数据汇总 ===' AS info;
SELECT '-- Department --' AS tbl; SELECT * FROM Department;
SELECT '-- Student --'    AS tbl; SELECT * FROM Student;
SELECT '-- Course --'     AS tbl; SELECT * FROM Course;
SELECT '-- Enrollment --' AS tbl; SELECT * FROM Enrollment;
SELECT '-- Score_Log --'  AS tbl; SELECT * FROM Score_Log;

SELECT '=== 实验3 全部完成！===' AS info;
```