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


# 实验五：bonus部分
# 实验要求

欢迎来到bonus部分！

在这里，你需要根据刚刚在基础部分实现的接口，使用BS体系完成一个精简的图书管理系统。

[后端](https://www.yuque.com/wuchengunsuhuang/tup2as/yxutzyqq8thhgp2u#Fri9f)使用Java实现，接收来自前端的请求，调用你所实现的接口实现处理逻辑，并给出响应。

[前端](https://www.yuque.com/wuchengunsuhuang/tup2as/yxutzyqq8thhgp2u#QsRWH)使用Vue3实现，与用户进行交互，向后端发出请求并处理响应。

我们发现，在之后的课程、如B/S体系软件设计、软件工程、软件工程基础、软件工程管理、软件需求工程、大规模实验等许多课（软工导致的）中涉及到数据库应用的部分均为BS体系，因此相信完成这个bonus一定会令你有所收获！

# 可以参考的完成本bonus的顺序

为了引导你较为顺利地完成本bonus部分，我们提出几个小任务，或许你可以按照这样的顺序进行尝试：

1. 阅读下文教程
2. 阅读前端`src/compoents/Card.vue`内`QueryCards()`里的GET /card请求和后端的GET /card处理逻辑，理解网络请求和响应的写法
3. 启动后端，并使用浏览器访问`http://localhost:8000/card`查看后端GET /card响应结果
4. 启动前端，在浏览器打开控制台(右键菜单“检查”或F12)，选择“网络”菜单，之后在前端点击“借书证管理”，可以发现card网络包，阅读网络包的信息

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1709777030116-df25d382-f09c-4227-9c5e-305783aa7bbf.png)

5. 根据你的理解，修改GET /card的后端代码，随意修改传递的借书证对象的值，观察前端变化，理解后端是如何影响前端的

请注意，在修改后端代码后需要重新启动后端才能生效。

6. 根据你的理解，修改GET /card的后端代码，调用你基础部分实现的查询借书证接口，使得前端能够显示你数据库中的内容
7. 阅读前端`src/compoents/Borrow.vue`内`QueryBorrows()`里的GET /borrow请求，在后端完成一个GET /borrow请求，调用使得前端可以正常显示
8. 阅读前端`src/compoents/Card.vue`内`ConfirmNewCard()`里的POST /card请求，在后端修改POST /card请求，调用你基础部分实现的新增借书证接口，使得前端的相应操作可以影响你本地的数据库
9. 根据你的理解，完成修改借书证内容的前后端处理逻辑，并验证前端的操作可以正常影响你本地的数据库
10. 完成借书证管理页面和借书记录管理页面的所有逻辑

由于此时还没有实现图书管理，涉及到相关操作(比如说借书记录的存在依赖于图书的存在)时你可以通过本地的DBMS手动添加一些数据以供临时调试使用。

11. 理解`<input>`(或`<el-input>`)和`<button>`(或`<el-button>`)，在图书管理页面添加一些输入框和一个按钮，并利用这些输入框和按钮尝试实现一个图书入库功能，保证前端的操作可以正常影响你本地的数据库
12. 完善图书管理页面前端，实现[基础部分指导书](https://www.yuque.com/yingchengjun/ozqlqv/gnwbgi9my2ci7has?singleDoc#%20%E3%80%8A%E5%AE%9E%E9%AA%8C%E4%BA%94%EF%BC%9A%E5%9B%BE%E4%B9%A6%E7%AE%A1%E7%90%86%E7%B3%BB%E7%BB%9F%E3%80%8B)中要求的所有功能，完成本bonus

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1711595199100-d8430db3-b330-4e3a-b932-6581c31a842c.png)

# 开头：啥是前后端

## 首先，来点计网+OS

首先我们得知道，啥是进程（process）。字面上来看，进程就是一个进行中的程序，或许你可以这么理解，静止存储在磁盘上的一行行代码和数据在内存中跑起来，就产生了进程。比如，你下载了很多app，平常没运行的时候，他们的代码和数据在磁盘上，一旦启动，他们就被或多或少装入内存：

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1709698212285-734fc20a-245a-41ef-8a8f-a36cbf3e4b37.png)

再想想，平常你在使用微信的时候，信息从你的设备上的微信进程到了另一些设备上的微信进程中，也就是说，我们想让信息在不同设备上的进程之间流通，信息需要知道它从哪个进程来，到哪个进程去。于是你会想到：我们如何标识一台设备的进程呢？**IP+端口号**。例如，127.0.0.1:80很可能是运行在你的个人设备上的HTTP服务端。

计算机网络让设备联系在一起。目前的互联网采用一个分层架构。Web前后端开发涉及的更多是**TCP层**和**应用层**。TCP层可以做到点对点通信，让信息从一个设备的进程到另一个设备的进程；应用层负责规定点对点通信的内容长啥样、遵守什么规范等，最常见的应用层协议之一就是HTTP。

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1709698809160-ca84163c-b227-406c-bc36-286f850df5c8.png)

## HTTP协议

HTTP协议是一个请求——响应式协议。一个client向server发出request，server向client返回response。

HTTP的请求有请求头、请求体等信息。比较重要的有请求方式（GET、POST、DELETE、PUT、OPTIONS等）、URL（请求的资源在哪）、一些重要的header（比如跨域相关）。有可能请求会带有参数、表单。

HTTP的响应比较重要的有状态码（比如200代表成功，5开头服务器有问题等）、header、被请求的资源等。

## Web发展

最开始的Web1.0阶段，server端通常返回一个渲染好的静态页面就行（想想零几年的XX论坛Orz）。但是随着人们日渐增长的需求，Web开发分成了两个部分：前端和后端。其中，前端由HTML、CSS、JS三件套组成（网页的骨架、样式、行为），主要负责和用户交互的部分；后端向前端提供查询数据的API。

当然这只是大体架构，你也可以在开发的时候套一些中间件之类的，或者是把后端拆成微服务架构等等，前面的路以后再来探索吧（

# 后端部分

## 选啥语言

目前来看，用的多的Web后端技术栈有Java、Go、Node.js等（或许你很想试试Rust😈）。Java很早就入局Web开发，在写业务逻辑方面已经很成熟了，更不用说有Spring全家桶这种无敌的存在。Go是现在很多互联网企业转向的技术栈，比如某鹅、某度、某跳动，这个语言很适合写网络服务、中间件和云原生（找工也许没Java卷）。剩下个Node，由于它用的是单线程事件队列，它在IO密集型服务端很有优势（最有优势的是用JS写的，前端同学想转全栈的话走Node就很自然）。

因为整个图书管理系统是Java的，而且单元测试已经写的很好了，就接着**Java**吧Orz

目前来看，由于本实验重点还是数据库，所以只会用简单的自带类，不会用Spring全家桶或者MyBatis之类的，同学们在以后的软件工程基础/软件工程/BS体系结构设计课程上再用吧～

核心包：[https://docs.oracle.com/javase/8/docs/jre/api/net/httpserver/spec/com/sun/net/httpserver/package-summary.html](https://docs.oracle.com/javase/8/docs/jre/api/net/httpserver/spec/com/sun/net/httpserver/package-summary.html)

## 后端API制定

首先，我们可以在前后端之间传递[JSON](https://www.yuque.com/wuchengunsuhuang/tup2as/yxutzyqq8thhgp2u#rR49P)。

然后，我们的API制定需要遵从一定的规则，如RPC（可以试试gRPC）、SOAP（xml相关）、REST。在本实验中，我们采用**REST**风格的API制定模式。

如何制定RESTful的API呢，想想我们请求某某资源，是不是有个动作（请求）和动作承受者（资源），于是我们也可以把动词和名词分开：动词就是HTTP的请求方法，如GET、POST、PUT、DELETE；名词就是请求的资源。

例子：

```
GET 	 /user    			 # 获取用户
PUT 	 /user    			 # 更新用户
POST   /user    			 # 创建用户
DELETE /user    			 # 删除用户

GET    /user/history   # 获取用户历史信息
PUT    /school/{id}   # 更新主键为{id}的学校信息
...
```

如果真的是要进行团队内的前后端对接，你可以用API Fox等软件构建文档，写完之后直接测试也行（记得这个测试是不跨域的，最后所有接口测试完要测跨域问题）

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1709729242461-e7fbe649-c1b6-48d3-b386-dce7a3571920.png)

(不过如果你只是为了完成lab5这一部分，直接一个POST走天下也不是不行，虽然不太优雅（

## 跨域问题

这是个前后端分离的项目，前端是一个运行在localhost:5173的进程，后端是一个运行在localhost:8000的进程（也可以其他端口，比如8001），我们在处理所有用户发来的请求时，实际上是用户操作前端页面，触发事件之后，由前端向后端发送请求。

这就产生了跨域问题。典型的报错大概长这样：

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1713864486930-97e80c5b-0936-4505-bb95-41fc700760f4.png)

解决的方法很多，在本项目中我们采取后端加上请求头的方式处理：

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1713864544689-d957e239-a05b-4ae9-bd89-4afb9aa68344.png)

## 咋写呢

### 主类

从上面的内容可以看出，后端需要解析来自前端的请求，分配路由（具体的请求用哪个handler去处理），然后查询数据库、返回数据等。

**Java是一个Pure OOP语言，我们需要实现一个含main方法的主类。下面是主类的main方法，Java程序从这里启动，Lab5之前单元测试过的代码可以在主类中调用。**首先，我们在本地的8000端口创建了一个服务器（也就是127.0.0.1:8000），在/card处绑定一个我们的CardHandler（也就是说，URL为127.0.0.1:8000/card的请求都会由我们的CardHandler处理）。启动服务器即可。

```
public class Main {
    // main方法，代码从这开始
    public static void main(String[] args) throws IOException {
        // 创建HTTP服务器，监听指定端口
        // 这里是8000，建议不要80端口，容易和其他的撞
        HttpServer server = HttpServer.create(new InetSocketAddress(8000), 0);

        // 添加handler，这里就绑定到/card路由
        // 所以localhost:8000/card是会有handler来处理
        server.createContext("/card", new CardHandler());

        // 启动服务器
        server.start();

        // 标识一下，这样才知道我的后端启动了（确信
        System.out.println("Server is listening on port 8000");
    }
    
    // 剩下的Main class...
```

接下来就是我们的CardHandler类。

首先，第一行implements非常重要，实现了HttpHandler接口的类才能作为Handler。

Handle部分，首先在Header带上Access的三个玩意儿，这是为了解决跨域问题（不带上的话，出于安全性考虑，前端无法调用后端API）。然后解析请求方法，这里我们主要处理GET和POST方法。

```
static class CardHandler implements HttpHandler {
        // 关键重写handle方法
        @Override
        public void handle(HttpExchange exchange) throws IOException {
            // 允许所有域的请求，cors处理
            Headers headers = exchange.getResponseHeaders();
            headers.add("Access-Control-Allow-Origin", "*");
            headers.add("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            headers.add("Access-Control-Allow-Headers", "Content-Type");
            // 解析请求的方法，看GET还是POST
            String requestMethod = exchange.getRequestMethod();
            // 注意判断要用equals方法而不是==啊，java的小坑（
            if (requestMethod.equals("GET")) {
                // 处理GET
                handleGetRequest(exchange);
            } else if (requestMethod.equals("POST")) {
                // 处理POST
                handlePostRequest(exchange);
            } else if (requestMethod.equals("OPTIONS")) {
                // 处理OPTIONS
                handleOptionsRequest(exchange);
            } else {
                // 其他请求返回405 Method Not Allowed
                exchange.sendResponseHeaders(405, -1);
            }
        }
}
```

### GET

然后我们处理GET的请求。其实操作就是先加个application/json的header，然后通过一个OutputStream把一个字节序列发过去。最后记得关闭OutputStream。

当然，这里只是很简单地传了一个固定的JSON，实际操作中，我们可以在这里查询数据库，获取到数据库中的实际数据，并转成字节流传递给前端。

如果处理请求时出了错误，记得try-catch处理掉，并且对应状态码也不能是200。在[https://developer.mozilla.org/zh-CN/docs/Web/HTTP](https://developer.mozilla.org/zh-CN/docs/Web/HTTP)里你能找到很多状态码和它们的应用场景。

如果GET请求带参数，一般是在URL上，一种是附带的`Query`，例如`/api/users?id=12345&name=John`，可以通过一些Java方法获取id和name的值；一种是路径上的`Param`，形式上是`/api/users/{id}`，实际请求中是`/api/users/5`，这个5就是id。获取这些参数后，后端程序可以进行一些额外处理。

```
 private void handleGetRequest(HttpExchange exchange) throws IOException {
            // 响应头，因为是JSON通信
            exchange.getResponseHeaders().set("Content-Type", "application/json");
            // 状态码为200，也就是status ok
            exchange.sendResponseHeaders(200, 0);
            // 获取输出流，java用流对象来进行io操作
            OutputStream outputStream = exchange.getResponseBody();
            // 构建JSON响应数据，这里简化为字符串
            // 这里写的一个固定的JSON，实际可以查表获取数据，然后再拼出想要的JSON
            String response = "[{\"cardId\": 1, \"name\": \"John Doe\", \"department\": \"Computer Science\", \"type\": \"Student\"}," +
                    "{\"cardId\": 2, \"name\": \"Jane Smith\", \"department\": \"Electrical Engineering\", \"type\": \"Faculty\"}]";
            // 写
            outputStream.write(response.getBytes());
            // 流一定要close！！！小心泄漏
            outputStream.close();
        }
```

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1709717518034-c9358203-07fa-43d8-89b2-3f21ede32a39.png)

#### JSON相关

可以看出，我们GET时传递的是一个写死的字符串JSON：

![](https://cdn.nlark.com/yuque/0/2024/png/34246589/1713865163434-a2cfc1b3-97f9-468d-abea-d5a5ca682392.png)

也就是说，你可以通过学习JSON规范[https://developer.mozilla.org/zh-CN/docs/Learn/JavaScript/Objects/JSON](https://developer.mozilla.org/zh-CN/docs/Learn/JavaScript/Objects/JSON)，**手写字符串**来解析JSON，具体可以实现在你想解析成JSON的对象类内的toString()方法里（或者写个toJSONString()方法）。

但是这样的效率肯定有点低，究其原因是Java没有自带JSON相关库（连Go都有）。你可以使用阿里的`fastjson`、开源库`jackson`等。你可以在maven官方库找到它们：

[https://mvnrepository.com/artifact/com.alibaba.fastjson2/fastjson2](https://mvnrepository.com/artifact/com.alibaba.fastjson2/fastjson2)

[https://mvnrepository.com/artifact/org.codehaus.jackson/jackson-core-asl](https://mvnrepository.com/artifact/org.codehaus.jackson/jackson-core-asl)

然后，在你项目的`pom.xml`的`<dependencies>`下面复制粘贴网站上的`<dependency>`标签，等待远程库被拉过来即可。

具体使用方法也可以参照国内网站。

### POST

然后，我们尝试处理POST请求。首先解析request body（因为POST请求附带的数据都在请求体里），然后读取POST附带的数据。

实际操作中，解析到POST发来的数据，你可以利用数据对表进行增删改查。

```
private void handlePostRequest(HttpExchange exchange) throws IOException {
    // 读取POST请求体
    InputStream requestBody = exchange.getRequestBody();
    // 用这个请求体（输入流）构造个buffered reader
    BufferedReader reader = new BufferedReader(new InputStreamReader(requestBody));
    // 拼字符串的
    StringBuilder requestBodyBuilder = new StringBuilder();
    // 用来读的
    String line;
    // 没读完，一直读，拼到string builder里
    while ((line = reader.readLine()) != null) {
        requestBodyBuilder.append(line);
    }

    // 看看读到了啥
    // 实际处理可能会更复杂点
    System.out.println("Received POST request to create card with data: " + requestBodyBuilder.toString());

    // 响应头
    exchange.getResponseHeaders().set("Content-Type", "text/plain");
    // 响应状态码200
    exchange.sendResponseHeaders(200, 0);

    // 剩下三个和GET一样
    OutputStream outputStream = exchange.getResponseBody();
    outputStream.write("Card created successfully".getBytes());
    outputStream.close();
}
```

### 处理请求的参数

GET方法，请求体一般为空，参数在URL上，比如`https://www.test.org/card?id=1&name=3`，请求的路径是`https://www.test.org/card`，参数是`id`和`name`，值为`1`和`3`。

POST或其他方法，一般参数在请求体，有表单、JSON等形式。如果像本实验中一样，前后端已经约定好了JSON格式的API，直接读取转换成字符串处理也是可以的。

具体的处理方法可以查网上的文档或官方文档：[https://docs.oracle.com/javase/8/docs/jre/api/net/httpserver/spec/com/sun/net/httpserver/package-summary.html](https://docs.oracle.com/javase/8/docs/jre/api/net/httpserver/spec/com/sun/net/httpserver/package-summary.html)

### OPTIONS

在前端发请求时，如果请求比较复杂，例如非GET、非简单POST的XHR请求，浏览器的`XMLHttpRequest`会先发一次预检请求，方法为`OPTIONS`，**然后才会发之后的请求**。所以我们需要实现一个函数去处理这个预检请求，参考上面的`POST`处理，返回状态码`204`(status no content)，然后把请求结束即可。

## 扩展

仅针对这个实验而言，我们可以实现更多的类，实现HttpHandler接口，这样就可以在特定路由上绑定处理器。每个处理器可以针对不同的请求方法调用不同的业务逻辑处理函数。在本实验中，不要求严格遵守REST的API规范，甚至全GET/POST请求也行（当然如果是正式程序肯定不行x）

如果是很大的高并发程序之类的，等到后端足够复杂，接口太多，效率太低的时候，可以尝试进行微服务拆分。如果你有构建分布式系统的需求，可以试试gRPC。数据库方面，可以加一层Redis作为缓存，可以使用ORM代替JDBC等。

## 注意的坑

1. 跨域问题需要前后端联调发现，如果你用API Fox或者Postman单独调试后端，很可能没啥问题，前端后端部署后因为跨域问题挂了。除了上文中加上请求头的方式，还可以在前端/后端用一些代理（搜搜http-proxy-middleware中间件、Nginx应用）。
2. 与C不同，在Java中，Object通过引用的方式传递，而不是值。

# 前端部分

## #1 前端部分介绍

本项目使用[Vue3](https://cn.vuejs.org/)作为框架，并使用了[Element Plus](https://element-plus.org/zh-CN/)组件库，你可以点击链接了解它们的使用方法。

当然，你也可以使用任何你喜欢的组件库。

在本实验中，你只需要完善`src/components/`下的`Book.vue`、`Borrow.vue`和`Card.vue`文件。

其中，`Borrow.vue`和`Card.vue`的页面已经写好，你只需完善其中的逻辑，向后端发起网络请求并处理响应。

`Book.vue`的页面留待你来探索，祝你收获从头开始搭建一个页面的成就感。

[#3 基础知识](https://www.yuque.com/wuchengunsuhuang/tup2as/yxutzyqq8thhgp2u#RR2q2)部分提供了一些或许对实现前端部分有所帮助的教程。

## #2 环境配置

为了完成这个bonus的前端部分，首先需要具备[Node.js](https://nodejs.org/en)环境，你可以自行参考相关教程进行安装。

在安装完成后，在终端输入`npm -v`，如果看到版本信息，说明安装成功。

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1709654293919-14412304-d935-4d23-887f-0ae6b1b41d9e.png)

将前端部分代码下载后，在其所在文件夹打开终端，输入`npm install`，将自动下载项目所需依赖，稍作等待，当显示以下信息时安装完成。

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1709654689803-e4fc59cb-298f-4d0c-8d35-5cdac949d494.png)

此时，项目已经可以运行，在其所在文件夹打开终端，输入`npm run dev`即可运行，当显示如下信息时运行成功，此时在浏览器中输入终端显示的URL（本例中为`http://localhost:5173/`）即可打开。

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1709654769170-f64fb994-9180-454c-8984-006544744a8a.png)

**如果使用VS Code开发，建议安装**[**Vue Official**](https://marketplace.visualstudio.com/items?itemName=Vue.volar)**扩展。**

Vue支持热重载，当你修改 `.vue` 文件时，该组件的所有实例将在不刷新页面的情况下被替换，也就是说，在修改文件后你可以实时在页面中查看效果，而不需要重新打开、刷新页面等操作。

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1709654909966-a2dbb6db-8b44-4657-862e-d48b094a332a.png)

## #3 基础知识

以下对与本项目相关的一些重要基础知识进行介绍。

关于项目目录的文件组织，你可以通过[此处](https://www.runoob.com/vue2/vue-directory-structure.html)了解。

### #3.1 `Vue`文件结构

总得来说，**Vue文件由template、style、script三部分组成**，可以暂时理解为它们与“前端三件套”HTML、CSS、JavaScript一一对应，分别控制页面的结构（页面中有什么）、样式（页面中的元素长什么样）与行为（页面中的元素可以做什么）。

```
<template>
  ...
</template>
<style>
  ...
</style>
<script>
  ...
</script>
```

本项目对展示的美观程度不做要求，因此只对template和script做讲解，感兴趣的同学可以自行了解[CSS](https://www.runoob.com/css/css-tutorial.html)部分。

#### #3.1.1 template块

template块支持[HTML](https://www.runoob.com/html/html-tutorial.html)语法，即`<element>`和`</element>`标签标注了一个元素的开始和结束，其中element代表那个元素的类型。例如，`<p>数据库系统</p>`表示一个段落，其内容为“数据库系统”。在一个元素的开始标签内部，你可以追加一些内容来表示这个元素的属性，例如`<a href="http://cspo.zju.edu.cn">浙江大学计算机科学与技术学院</a>`表示一个链接，点击后将跳转到`http://cspo.zju.edu.cn`这个网址。

在HTML的基础上，Vue增加了一些属性，在本项目中你可能会用到：

- `v-model`属性

这个属性为template中的表单内容和script中的内容建立了双向绑定，使得你在表单中输入的数据能够实时更新并且显示出来。

例如`<input v-model="text"></input>`表示这个输入框的值为script中`text`这一变量的值，当template和script中任意一方变化时另一方会同步变化。(熟悉HTML的同学可以理解为它与`<input :value="text" @input="event => text = event.target.value"></input>`的写法效果相同)

- `v-if`&`v-else`属性

这个属性控制一个元素显示出来的条件。

例如，`<p v-if="condition">这个段落将在condition值为true时显示，否则不会显示</p>`。

- `v-for`属性

这个属性可以基于script中的数据多次渲染元素，其值为`alias in iterable`。

例如，`<div v-for="book in books"></div>`将为`books`数组中的每一个元素都创建一个块。

- `v-on / @`属性

这个属性可以为元素绑定一个监听器，一般为`@event`格式，其中本项目中可能会用到`@click`。

例如，

- `<button @click="Increment">这个按钮被点击时会调用Increment函数</button>`
- `<button @click="Increment(1)">这个按钮被点击时会调用Increment函数并传递参数1</button>`
- `<button @click="i=1, Increment(1)">这个按钮被点击时会将i赋值为1，之后调用Increment函数并传递参数1</button>`

除了这些属性外，你还需要了解双大括号语法，双大括号及其所括起来的内容类似于一个占位符，它将被替换为它括起来的内容的值。例如有一个`text`变量值为`Hello Database`，你使用`<p>{{ text }}</p>`时将得到一个内容为`Hello Database`的段落。

以下给出了一个综合使用以上用法的例子：

```
<div v-for="student in students" v-if="student.department=='CS'">
  修改{{student.name}}的成绩：
  <input v-model="student.grade"></input>
  <button @click="ModifyGrade(student.id,student.grade)">修改</button>
</div>
```

我们假设在script块中存在一个`students`数组，其中的每个元素都具备`id`,`name`,`department`, `grade`属性，以上代码的功能是：为每一个部门为CS的学生显示一块内容，这块内容首先包括一段“修改XXX的成绩：”的文本，其中XXX是这个学生的名字，接着是一个显示着他的成绩的输入框，接着是一个显示“修改”二字的按钮，点击按钮后会将这个学生的学号和成绩作为参数，调用`ModifyGrade`函数。

如果能够理解以上示例，你已经具备了完成本项目template部分的基本技能，如果想要了解更多，可以阅读Vue3官方文档。

最后简要介绍一下Element Plus组件库，它或许可以使你的开发更为便捷和美观。你可以在组件库中找到你想要使用的组件，并通过查看源代码功能得到它的代码，将它复制到你的项目中并作适当修改便可以使用。组件的属性可以在Element Plus提供的官方文档中查询。

当然，只使用HTML原生元素同样可以完成本项目。

![](https://cdn.nlark.com/yuque/0/2024/png/33575985/1709695982669-20476e62-1209-45d3-83aa-c790d7c59b24.png)

#### #3.1.2 script块

script块使用[JavaScript](https://www.runoob.com/js/js-tutorial.html)语法，本框架中使用的是Vue的**选项式API风格**，其结构大致为：

```
<script>
import ...
export default(){
  data(){
    return{
      variable1: value,
      variable2: value,
      ...
    }
  },
  methods:{
    function1(params){
      ...
    },
    function2(params){
      ...
    },
      ...
  },
  (lifecycles...)
}
</script>
```

- import部分导入了其他模块导出的内容，例如在框架中你将会看到`import axios from 'axios'`。
- export default部分导出了本文件的接口，在本次实验中暂时不需要了解其中的原理。
- data部分给出了本文件中用到的数据，你可以理解为这里用来定义本组件的全局变量、供其他部分（比如template块）使用。
- methods部分定义了本文件中用到的函数。
- lifecycles是Vue的生命周期钩子函数，将会在特定阶段自动执行，比较常用的有：mounted()在页面被渲染后执行，updated()在页面数据发生变化时执行...

关于这部分内容，我们将在稍后介绍完axios后给出示例。

### #3.2 网络请求

#### #3.2.1 axios

[axios](https://www.axios-http.cn/)是一个基于promise的网络请求库，本项目中我们通过axios向后端发送网络请求(request)并接收响应(response)，其中主要使用GET和POST方法，在此作简要介绍。

我们需要使用到的基本格式如下：

```
axios.get(url[, config])
  .then(response=>{
    ...
  })
```

```
axios.post(url[, data[, config]])
  .then(response=>{
    ...
  })
```

- url

它们都具有url参数，我们在`src/main.js`里配置了`baseURL`：

```
axios.defaults.baseURL = 'http://localhost:8000';
```

它将会与你在调用axios函数时给出的url参数一起拼成即将接受请求的URL，例如在url给出`'/book'`时将向`http://localhost:8000/book`发出网络请求。

如果你不使用 8000端口，请到`src/main.js`修改。

- GET参数/POST请求负载

axios函数的第二个参数有所不同，当你使用GET时需要通过`{params:{...}}`的方式给出参数，而使用POST时可以直接通过`{...}`给出请求负载。

- then

`.then()`的参数是一个函数，当发出的请求收到后端的响应后，axios函数自动调用这个函数。

我们在这里使用`response=>{...}`的形式被称作箭头函数，其中`response`是参数，`{...}`部分是函数体，这种写法与`func(response){...}`的区别是这样不会改变`this`，即仍然可以通过`this`来操作`script`块中`data`部分的变量。

- response

这个参数接收到的是后端给出的网络响应(response)，其中它的`data`成员是响应的内容，也就是说，如果后端发送给前端一个整数，那么`response.data`就是这个整数，而如果后端发送给前端一个对象，那么`response.data`也就是这个对象。(注意不是`response`，而是`response.data`)

#### #3.2.2 JSON

[JSON](https://www.runoob.com/json/json-tutorial.html)是JavaScript的对象表示法（**J**ava**S**cript **O**bject **N**otation），是一种轻量级的文本数据交换格式，我们以此来在前后端之间交换数据。使用方括号`[...]`表示数组，使用花括号`{...}`表示对象，使用键值对`key: value`来表示数据，键需要用引号括起来，数据之间用逗号分隔。例如，前后端之间可以传递这样的数据：

```
[
  {
    "id":1,
    "name":"王小明",
    "department": "CS",
    "type": "学生"
  },
  {
    "id":2,
    "name":"王老师",
    "department": "CS",
    "type": "教师"
  }
]
```

这表示一个包含两个对象的数组，其中每个对象都有id、name、department和type四个成员。

### #3.3 示例介绍

我们就框架中的一段具体代码，来帮助你深入理解以上内容。

```
<el-button @click="newCardInfo.name = '', 
  newCardInfo.department = '', newCardInfo.type = '学生', newCardVisible = true">
</el-button>
...
<el-dialog v-model="newCardVisible" title="新建借书证">
  <div>
    姓名：<el-input v-model="newCardInfo.name" clearable />
  </div>
  <div>
    部门：<el-input v-model="newCardInfo.department" clearable />
  </div>
  <div>
    类型：
    <el-select v-model="newCardInfo.type">
    <el-option v-for="type in types" :key="type.value" :label="type.label" :value="type.value" />
    </el-select>
  </div>

  <template #footer>
  <span class="dialog-footer">
    <el-button @click="newCardVisible = false">取消</el-button>
    <el-button type="primary" @click="ConfirmNewCard"
  :disabled="newCardInfo.name.length === 0 || newCardInfo.department.length === 0">确定</el-button>
      </span>
  </template>
</el-dialog>
  
<script>
import { ElMessage } from 'element-plus'
import axios from 'axios'
export default {
  data() {
    return {
      types: [
        {
          value: '教师',
          label: '教师',
        },
        {
          value: '学生',
          label: '学生',
        }
      ],
      newCardVisible: false,
      newCardInfo: {
        name: '',
        department: '',
        type: '学生'
      },
    }
  },
  methods: {
    ConfirmNewCard() {
      axios.post("/card",
                 {
                   name: this.newCardInfo.name,
                   department: this.newCardInfo.department,
                   type: this.newCardInfo.type
                 })
        .then(response => {
            ElMessage.success("借书证新建成功")
            this.newCardVisible = false
            this.queryCards()
        })
    },
  }
}
</script>
```

以上是涉及新建借书证功能的主体代码，我们分段分析：

```
<el-button @click="newCardInfo.name = '', 
  newCardInfo.department = '', newCardInfo.type = '学生', newCardVisible = true">
</el-button>
...
<el-dialog v-model="newCardVisible" title="新建借书证">
  <div>
    姓名：<el-input v-model="newCardInfo.name" clearable />
  </div>
  <div>
    部门：<el-input v-model="newCardInfo.department" clearable />
  </div>
  <div>
    类型：
    <el-select v-model="newCardInfo.type">
    <el-option v-for="type in types" :key="type.value" :label="type.label" :value="type.value" />
    </el-select>
  </div>
  <template #footer>
  <span class="dialog-footer">
    <el-button @click="newCardVisible = false">取消</el-button>
    <el-button @click="ConfirmNewCard"
  :disabled="newCardInfo.name.length === 0 || newCardInfo.department.length === 0">确定</el-button>
      </span>
  </template>
</el-dialog>
```

第1行至第3行设置了一个按钮，当它被点击时将设置`newCardInfo`的`name`和`department`为空串，将`type`设置为学生，并将`newCardVisible`设置为true。

之后的部分定义了一个对话框，

- 第5行的v-model属性表示它在`newCardVisible = true`时可见，`title`给定了对话框的标题
- 第6行至第11行定义了姓名、部门的输入框，他们分别与`newCardInfo.name`和`newCardInfo.department`双向绑定，并设置了`clearable`属性表示这个输入框是可以一键清空的。
- 第12行至第17行给出了借书证类型的下拉选择框，与`newCardInfo.type`绑定，

- 第15行给出了选择框，其中`v-for`要求遍历`types`中的每项作为下拉选择框的一个选项，其键和值都是`type.value`，显示出来的文字为`type.label`。

- 第18行表示接下来的部分在对话框底部。
- 第20行设置了一个取消按钮，点击后将设置`newCardVisible`为false。
- 第21行至第22行设置了一个确定按钮，被点击时将调用`ConfirmNewCard`函数。这个按钮在`newCardInfo.name`和`newCardInfo.department`这两项的任意一项为空时将被禁用。

```
<script>
import { ElMessage } from 'element-plus'
import axios from 'axios'
export default {
  data() {
    return {
      types: [
        {
          value: '教师',
          label: '教师',
        },
        {
          value: '学生',
          label: '学生',
        }
      ],
      newCardVisible: false,
      newCardInfo: {
        name: '',
        department: '',
        type: '学生'
      },
    }
  },
```

在script块中，先导入了几个需要用到的接口，在其`data`中定义了以下内容：

- types，是一个列表，里面有两个对象，每个对象都有一个value和一个label，用来表示借书证类型。
- newCardVisible，布尔值，表示新建借书证对话框的可见性。
- newCardInfo，是一个对象，里面有name、department、type三个成员，用于存储新借书证信息。

```
  methods: {
    ConfirmNewCard() {
      axios.post("/card",
                 {
                   name: this.newCardInfo.name,
                   department: this.newCardInfo.department,
                   type: this.newCardInfo.type
                 })
        .then(response => {
            ElMessage.success("借书证新建成功")
            this.newCardVisible = false
            this.queryCards()
        })
    }
  }
}
</script>
```

methods里面定义了一个函数，名为ConfirmNewCard，无参数，在被调用时向`http://localhost:8000/card`发送一个POST请求，同时向后端传递一个对象，这个对象具有name、department、type三个成员，其值分别为newCardInfo的对应值。在收到后端的响应后，通过ElMessage在页面上弹出一条消息提示借书证新建成功，将对话框可见性设为false，并调用queryCards()函数重新查询现有借书证，以达到更新页面的目的。

```
async queryCards() {
    let response = await axios.get('/card')
    this.cards = []
    let cards = response.data
    cards.forEach(card => {
        this.cards.push(card)
    })
}
```

queryCards()使用了axios的另一种写法，它向`http://localhost:8000/card`发送GET请求，不携带参数。得到响应后，将this.cards清空，创建一个临时变量cards来获取响应负载，将其中的每个元素加到this.cards中。