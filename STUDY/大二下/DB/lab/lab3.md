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