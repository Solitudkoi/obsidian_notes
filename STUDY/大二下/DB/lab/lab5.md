实验文档参考：

https://www.yuque.com/yingchengjun/ozqlqv/gnwbgi9my2ci7has

bonus代码框架可替换为新版本（已经使用老版本的同学可以不用换，两个版本差异不大）：

https://github.com/abilitywuchen/library-management-system/tree/master
https://github.com/abilitywuchen/library-management-system/tree/master
提交代码压缩文件和实验报告

## #1 实验指导书

### #1.1 实验目的与实验目标

#### 实验目的

- 设计并实现一个精简的图书管理程序，要求具有图书入库、查询、借书、还书、借书证管理等功能。

#### 最终目标

- 提供一个基于MySQL(或OpenGauss, SQL Server)的精简图书管理程序，该图书管理程序应具备较好的可扩展性、鲁棒性和安全性，并且在高并发场景下仍能正确运行。
- 本实验最基本的要求是完成类`_LibraryManagementSystemImpl_`中各功能模块的函数，并通过所有测试样例。Bonus是使用提供的前端框架，正确完成图书管理系统的前端页面，使其成为一个用户能真正使用的图书管理系统。->[前端框架文档](https://www.yuque.com/wuchengunsuhuang/tup2as/yxutzyqq8thhgp2u)
- 在实现图书管理程序后，提交实验成果，实验成果包括但不限于：

- 项目源代码：提交整个工程项目的**压缩**文件，至少应该包括`_src/_`目录下的所有文件以及`_pom.xml_`文件。提交工程项目时，**请勿**将以下文件提交：

- 隐藏的`_.git/_`文件夹；
- `_.gitignore_`中列出的那些需要被忽略的文件及文件夹，例如：`_target/_`、`_.idea/_`等。

- 实验报告：实验报告应包括实验目的、实验需求、实验环境、系统各模块的设计思路和实现、系统验证测试(正确性测试，如果实现了前端还应包括功能性测试)、遇到的问题及解决方法、思考题的解答等。

- 通过本实验，提高学生的系统编程能力，加深对数据库系统原理及应用的理解。

### #1.2 系统需求

#### 基本数据对象

所有的基本数据对象都被定义在`_entities_`包中，以下是这些对象的基本信息，其它信息请参考类中的注释。

|   |   |   |
|---|---|---|
|对象名称|类名|包含属性|
|书|Book|书号, 类别, 书名, 出版社, 年份, 作者, 价格, 剩余库存|
|借书证|Card|卡号, 姓名, 单位, 身份(教师或学生)|
|借书记录|Borrow|卡号, 书号, 借书日期, 还书日期|

#### 基本功能模块

图书管理系统中所有应具备的功能模块都在接口`_LibraryManagementSystem_`中被声明：

- 在实现交互界面之前，你首先需要为该接口做一个具体的实现(在实现类中重写接口中的方法)，具体实现位于类`_LibraryManagementSystemImpl_`中。
- 在完成所有的接口后，你可以在主类`_Main_`中通过`_LibraryManagementSystem_`的一个实例来实现与用户的交互。这样设计的好处在于它满足了(1) 封装性：对客户端(主类`_Main_`)屏蔽了图书管理系统底层的具体实现；(2) 可扩展性：可以很方便地对图书管理系统做不同的实现(例如再实现一个不依赖于数据库的基于内存的图书管理系统)；（3）可维护性：只需要在客户端中切换`_LibraryManagementSystem_`的实例即可使用不同实现方式的图书管理系统。

以下是`_LibraryManagementSystem_`中声明的模块，有关各模块的详细注释和说明已经在接口`_LibraryManagementSystem_`中给出，请自行阅读，本文档不做赘述：

- `_ApiResult storeBook(Book book)_`：图书入库模块。向图书库中注册(添加)一本新书，并返回新书的书号。如果该书已经存在于图书库中，那么入库操作将失败。当且仅当书的<类别, 书名, 出版社, 年份, 作者>均相同时，才认为两本书相同。请注意，book_id作为自增列，应该插入时由数据库生成。插入完成后，需要根据数据库生成的book_id值去更新book对象里的book_id。
- `_ApiResult incBookStock(int bookId, int deltaStock)_`：图书增加库存模块。为图书库中的某一本书增加库存。其中库存增量`_deltaStock_`可正可负，若为负数，则需要保证最终库存是一个非负数。
- `_ApiResult storeBook(List<Book> books)_`：图书批量入库模块。批量入库图书，如果有一本书入库失败，那么就需要回滚整个事务(即所有的书都不能被入库)。
- `_ApiResult removeBook(int bookId)_`：图书删除模块。从图书库中删除一本书。如果还有人尚未归还这本书，那么删除操作将失败。
- `_ApiResult modifyBookInfo(Book book)_`：图书修改模块。修改已入库图书的基本信息，该接口不能修改图书的书号和存量。
- `_ApiResult queryBook(BookQueryConditions conditions)_`：图书查询模块。根据提供的查询条件查询符合条件的图书，并按照指定排序方式排序。查询条件包括：类别点查(精确查询)，书名点查(模糊查询)，出版社点查(模糊查询)，年份范围查，作者点查(模糊查询)，价格范围差。如果两条记录排序条件的值相等，则按book_id升序排序。
- `_ApiResult borrowBook(Borrow borrow)_`：借书模块。根据给定的书号、卡号和借书时间添加一条借书记录，然后更新库存。若用户此前已经借过这本书但尚未归还，那么借书操作将失败。
- `_ApiResult returnBook(Borrow borrow)_`：还书模块。根据给定的书号、卡号和还书时间，查询对应的借书记录，并补充归还时间，然后更新库存。
- `_ApiResult showBorrowHistory(int cardId)_`：借书记录查询模块。查询某个用户的借书记录，按照借书时间递减、书号递增的方式排序。
- `_ApiResult registerCard(Card card)_`：借书证注册模块。注册一个借书证，若借书证已经存在，则该操作将失败。当且仅当<姓名, 单位, 身份>均相同时，才认为两张借书证相同。
- `_ApiResult removeCard(int cardId)_`：删除借书证模块。如果该借书证还有未归还的图书，那么删除操作将失败。
- `_ApiResult showCards()_`：借书证查询模块。列出所有的借书证。

#### 数据库(表)设计

以下是该图书管理系统的数据表定义：

```
create table `book` (
    `book_id` int not null auto_increment,
    `category` varchar(63) not null,
    `title` varchar(63) not null,
    `press` varchar(63) not null,
    `publish_year` int not null,
    `author` varchar(63) not null,
    `price` decimal(7, 2) not null default 0.00,
    `stock` int not null default 0,
    primary key (`book_id`),
    unique (`category`, `press`, `author`, `title`, `publish_year`)
);

create table `card` (
    `card_id` int not null auto_increment,
    `name` varchar(63) not null,
    `department` varchar(63) not null,
    `type` char(1) not null,
    primary key (`card_id`),
    unique (`department`, `type`, `name`),
    check ( `type` in ('T', 'S') )
);

create table `borrow` (
  `card_id` int not null,
  `book_id` int not null,
  `borrow_time` bigint not null,
  `return_time` bigint not null default 0,
  primary key (`card_id`, `book_id`, `borrow_time`),
  foreign key (`card_id`) references `card`(`card_id`) on delete cascade on update cascade,
  foreign key (`book_id`) references `book`(`book_id`) on delete cascade on update cascade
);
```

#### 系统功能验证

系统功能验证测试分为功能性测试和正确性测试。

- 正确性测试通过测试用例进行评判，以验收时通过的测试用例数量占总测试用例数量的百分比来评定正确性测试部分的得分。
- 当实现了前端，图书管理系统表现为一个完整可使用的程序时，进行功能性测试。功能性测试通过验收时随机运行模拟场景的结果进行评判，以软件使用时的交互友好程度、效率、正确性等指标来评定功能性测试部分的得分。

功能性测试的参考模拟场景如下：

|   |   |
|---|---|
|功能|描述|
|图书入库|输入<书号, 类别, 书名, 出版社, 年份, 作者, 价格, 初始库存>，入库一本新书B|
|增加库存|将书B的库存增加到X，然后减少到1|
|修改图书信息|随机抽取N个字段，修改图书B的图书信息|
|批量入库|输入图书导入文件的路径U，然后从文件U中批量导入图书|
|添加借书证|输入<姓名, 单位, 身份>，添加一张新的借书证C|
|查询借书证|列出所有的借书证|
|借书|用借书证C借图书B，再借一次B，然后再借一本书K|
|还书|用借书证C还掉刚刚借到的书B|
|借书记录查询|查询C的借书记录|
|图书查询|从查询条件<类别点查(精确查询)，书名点查(模糊查询)，出版社点查(模糊查询)，年份范围查，作者点查(模糊查询)，价格范围差>中随机选取N个条件，并随机选取一个排序列和顺序|

### #1.3 思考题

1. 绘制该图书管理系统的E-R图。
2. 描述SQL注入攻击的原理(并简要举例)。在图书管理系统中，哪些模块可能会遭受SQL注入攻击？如何解决？
3. 在InnoDB的默认隔离级别(RR, Repeated Read)下，当出现并发访问时，如何保证借书结果的正确性？下面是一个在该场景下可能会出现非预期结果的例子：

4. A启动事务，通过JDBC查询到余量1，代码逻辑判定借书成功，修改库存；

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673698473973-7de5ad02-5228-487a-9488-45b32a5cad3f.png)

2. B启动事务，在A尚未提交事务时，通过JDBC同样查询到余量为1，此时代码逻辑同样判定借书成功，修改库存；
3. A、B分别提交事务，最后结果显示余量为-1，不符合预期。

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673698871837-f586bda3-13b0-49fb-a8a0-7b27ae129594.png)

【提示】

- 了解InnoDB默认隔离级别(RR, Repeated Read)实现的原理，以及快照读和当前读的区别，能够帮助你解决这个问题。
- 在实际应用（例如电商系统）中，“秒杀”、“团购”是频繁出现的一些活动。“秒杀”活动通常伴随着高并发、访问量激增等特点。当并发请求数过多时，“秒杀”系统又是如何防止库存超卖的呢？可以作为课外拓展去了解一下。
- 测试用例`_LibraryTest::parallelBorrowBookTest()_`将会测试在该场景下你的程序是否能够执行得到预期的结果。

  

## #2 框架使用指南

### #2.0 代码仓库

[https://git.zju.edu.cn/zjucsdb/librarymanagementsystem](https://git.zju.edu.cn/zjucsdb/librarymanagementsystem)

### #2.1 环境配置

#### 开发机环境要求

**若使用**[**方案2**](https://www.yuque.com/yingchengjun/ozqlqv/gnwbgi9my2ci7has#r7YXk)**作为集成开发环境**，请确保你的电脑中已经安装了独立的JDK和Maven：

- [JDK](https://jdk.java.net/19/) 1.8.0及以上，可在终端中通过`_java -version_`命令查看
- [Apache Maven](https://maven.apache.org/download.cgi) 3.6.3及以上，可在终端中通过`_mvn -v_`命令查看

MacOS下可通过上面的链接安装JDK([MacOS下JDK安装教程](https://segmentfault.com/a/1190000040495961))，使用`_brew install maven_`命令安装Maven，成功安装后运行命令应显示：

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673805555381-54d2b9cd-9473-4bda-a713-593c4748633e.png)

Windows下可以通过上面的链接安装JDK和Maven，具体安装教程参考：

- [JDK安装教程](https://blog.csdn.net/Marvin_996_ICU/article/details/106240065) (PS: 安装完成后需要设置环境变量才可以正确执行上述命令)
- [Maven安装教程](https://learnku.com/articles/67402) (PS: 安装完成后需要设置环境变量才可以正确执行上述命令)

成功安装后，通过CMD运行命令应显示：

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673872563849-39716548-3c30-4637-961f-cf44021bb398.png)

#### 方案1：使用 [IntelliJ IDEA](https://www.jetbrains.com.cn/idea/) 作为集成开发环境【推荐】

下载 [IntelliJ IDEA](https://www.jetbrains.com.cn/idea/) 的方式参考本链接，下载后会有30天的免费试用期，可以使用ZJU的学生认证申请在校期间免费使用。

进入IDEA后，通过上方导航栏进入项目的架构设置界面(`File --> Project Structure`)

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673870942597-62e352a6-fde9-4c5f-9d2c-bfb8bcacd6c3.png)

在项目的架构设置界面中，可以选择项目使用的SDK，若你的开发机中没有安装JDK，那么需要点击下方Add SDK下载一个JDK或是自行安装一个独立的JDK。

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673870875037-fb3ac389-1af8-40a8-9651-69b10abd802f.png)

然后打开`pom.xml`，在右边侧栏处选择Maven，点击刷新按钮重新导入该项目，此时IDEA会在后台自动下载项目所需要的依赖(如JDBC)，这些依赖都在`pom.xml`文件中被定义。

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673874434262-4ccddd80-7788-4de7-b5fb-3e6b95d1a53b.png?x-oss-process=image%2Fcrop%2Cx_0%2Cy_0%2Cw_463%2Ch_281)

依赖下载完成后，即可运行主程序(`main/java/Main.java`)或运行测试(`test/java/LibraryTest.java`)。注意，整个过程并不需要运行mvn命令，让IDEA帮你自动进行包管理即可。混合使用可能导致找不到依赖。

#### 方案2：使用 VSCode + Java + Maven 作为开发环境

使用VSCode打开项目文件夹时，若没有安装Java扩展，右下角会提示推荐安装Java扩展，点击Install即可安装：

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673887652881-9b8e5fb9-d1b3-4a80-b985-41a044fc7684.png)

当然也可以在VSCode的扩展商店里面安装：

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673887741010-54e365f6-5a8e-46b2-a724-03739d15df67.png)

接下来就可以在VSCode提供的开发环境里面进行开发了。

### #2.2 项目开发提示

1. `src/main/resources`目录下存放了数据库连接的相关配置以及SQL脚本，**在开始开发代码前，请先将**`**src/main/resources/application_template.yaml**`**文件拷贝一份，并重命名为**`**application.yaml**`，然后按需修改该文件内的配置参数，各配置参数的含义如下：

- `host`：主机名，默认值为`"localhost"`
- `port`：端口号，默认值为`"3306"`
- `user`：用户名，默认值为`"root"`
- `password`：密码，默认值为`""`
- `db`：数据库名，默认值为`"library"`，注意需要先在数据库里创建
- `type`：数据库类型，可选值为`["mysql", "postgresql", "sqlserver"]`，默认值为`"mysql"`。备注：如果需要使用OpenGauss，请修改为`"postgresql"`。

2. 在实现`LibraryManagementImpl`后，需要在Main中利用`LibraryManagementImpl`实现后续的用户交互功能。
3. JDBC的使用可参考[JDBC常用API](https://blog.csdn.net/fhuqw/article/details/120095166)或查阅其它资料，下面是JDBC结合本框架使用的一个例子：

```
@Override
public ApiResult resetDatabase() {
    Connection conn = connector.getConn();
    try {
        Statement stmt = conn.createStatement();
        stmt.addBatch(Common.SQL_DROP_BORROW);
        stmt.addBatch(Common.SQL_DROP_BOOK);
        stmt.addBatch(Common.SQL_DROP_CARD);
        stmt.addBatch(Common.SQL_CREATE_BOOK);
        stmt.addBatch(Common.SQL_CREATE_CARD);
        stmt.addBatch(Common.SQL_CREATE_BORROW);
        stmt.executeBatch();
        // 注意：(1)执行成功后一定要记得提交事务
        // (2)如果在函数执行时提前返回，需要及时将事务回滚或提交掉
        commit(conn);	
    } catch (Exception e) {
        rollback(conn);
        return new ApiResult(false, e.getMessage());
    }
    return new ApiResult(true, null);
}
```

### #2.3 运行测试用例

#### IDEA下运行测试用例

所有的测试用例都在`test/java/LibraryTest.java`文件中：

- 点击类边上的运行按钮将会运行所有测试

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673874746760-99293e72-3f8b-4de1-93e6-9d871c611930.png)

- 点击方法边上的运行按钮将会运行单个测试

![](https://cdn.nlark.com/yuque/0/2023/png/25540491/1673874724781-ad116cb3-f017-4d7c-b5f0-bda4828b5fb0.png)

#### VSCode下运行测试用例

在VSCode中的终端中输入相应的指令即可编译/运行/测试代码，以下是一些常用指令：

- `_mvn clean compile_`：清理输出目录并编译项目主代码
- `_mvn exec:java -Dexec.mainClass="Main" -Dexec.cleanupDaemonThreads=false_`：运行主代码

- **注意：**在**Windows**下，需要使用`_mvn exec:java -D"exec.mainClass"="Main" -D"exec.cleanupDaemonThreads"=false_`命令，来源参考[Unknown lifecycle phase on Maven](https://stackoverflow.com/questions/64299956/unknown-lifecycle-phase-on-maven)

- `_mvn -Dtest=LibraryTest clean test_`：运行所有的测试
- `_mvn -Dtest=LibraryTest#_子测试名 _clean test_`：运行某个特定的测试

- eg：`_mvn -Dtest=LibraryTest#parallelBorrowBookTest clean test_`

#### 单元测试使用提示

1. 在连接远程数据库时，可能会出现测试用例执行缓慢的情况，此时可以在测试用例中调小测试的规模，如将1000次循环测试调整成100次。

### #2.4 可能遇到的问题

1. 使用VSCode作为集成开发环境时，执行`_mvn compile_`命令提示`No compiler is provided in this environment. Perhaps you are running on a JRE rather than a JDK?`。

- 解决方案参考：[https://blog.csdn.net/LJFPHP/article/details/89341345](https://blog.csdn.net/LJFPHP/article/details/89341345)

- Mac下配置`JAVA_HOME`可参考：[https://cloud.tencent.com/developer/article/1597666](https://cloud.tencent.com/developer/article/1597666)

2. 使用IDEA作为集成开发环境时，出现以下错误`Cannot resolve plugin org.apache.maven.plugins:maven-compiler-plugin:3.1`。

- 解决方案参考：[https://blog.csdn.net/weixin_51091274/article/details/128054182](https://blog.csdn.net/weixin_51091274/article/details/128054182)

3. ConnectConfig.java 25行报错找不到文件，可能是因为有中文路径，请确保你的项目路径里没有中文字符
4. 如出现以下错误，是getResource函数未找到application.yaml文件，可以将路径替换成绝对路径，如：

new Yaml().load(new BufferedReader(new FileReader("D:\\lab5\\application.yaml")));

![](https://cdn.nlark.com/yuque/0/2023/png/29437275/1683513405869-a017b65b-3c24-4592-9d91-9cd465bc474e.png)