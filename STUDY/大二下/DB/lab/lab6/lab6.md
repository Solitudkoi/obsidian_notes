实验文档：

https://www.yuque.com/yingchengjun/minisql

每组仅需一位同学提交代码压缩包和小组总体设计报告；

其他同学仅需提交个人详细设计报告

access token：hrjg8f2GhJnV1YngDKeQYm86MQp1OjN1dwk.01.0z0vmm5hv

git clone https://3240103533:hrjg8f2GhJnV1YngDKeQYm86MQp1OjN1dwk.01.0z0vmm5hv@git.zju.edu.cn/3240103533/minisql.git

**2024年5月31日：修复了框架的一些问题，请大家重新拉取代码**

### 框架链接：[ZJU GitLab链接，请使用内网访问](https://git.zju.edu.cn/zjucsdb/minisql)

### 一、实验目的

1. 设计并实现一个精简型单用户SQL引擎MiniSQL，允许用户通过字符界面输入SQL语句实现基本的增删改查操作，并能够通过索引来优化性能。
2. 通过对MiniSQL的设计与实现，提高学生的系统编程能力，加深对数据库管理系统底层设计的理解。

### 二、实验需求

1. 数据类型：要求支持三种基本数据类型：`integer`，`char(n)`，`float`。
2. 表定义：一个表可以定义多达32个属性，各属性可以指定是否为`unique`，支持单属性的主键定义。
3. 索引定义：对于表的主属性自动建立B+树索引，对于声明为`unique`的属性也需要建立B+树索引。
4. 数据操作: 可以通过`and`或`or`连接的多个条件进行查询，支持等值查询和区间查询。支持每次一条记录的插入操作；支持每次一条或多条记录的删除操作。
5. 在工程实现上，使用源代码管理工具（如Git）进行代码管理，代码提交历史和每次提交的信息清晰明确；同时编写的代码应符合代码规范，具有良好的代码风格。

### 三、系统架构与模块概述

#### 3.1 系统架构示意图

- 在系统架构中，解释器`SQL Parser`在解析SQL语句后将生成的语法树交由执行器`Executor`处理。执行器则根据语法树的内容对相应的数据库实例（`DB Storage Engine Instance`）进行操作。
- 每个`DB Storage Engine Instance`对应了一个数据库实例（即通过`CREATE DATABSAE`创建的数据库）。在每个数据库实例中，用户可以定义若干表和索引，表和索引的信息通过`Catalog Manager`、`Index Manager`和`Record Manager`进行维护。目前系统架构中已经支持使用多个数据库实例，不同的数据库实例可以通过`USE`语句切换（即类似于MySQL的切换数据库），在初步实现时，可以先考虑单个数据库实例的场景，在单个实例跑通后再支持多个实例。

![](https://cdn.nlark.com/yuque/0/2024/png/29437275/1713753763916-ba42e7f6-6b61-4afd-920f-4acdf54a083e.png)

#### 3.2 系统模块概述

##### 3.2.1 Disk Manager

- Database File（DB File）是存储数据库中所有数据的文件，其主要由记录（Record）数据、索引（Index）数据和目录（Catalog）数据组成（即共享表空间的设计方式）。与书上提供的设计（每张表通过一个文件维护，每个索引也通过一个文件维护，即独占表空间的设计方式）有所不同。共享表空间的优势在于所有的数据在同一个文件中，方便管理，但其同样存在着缺点，所有的数据和索引存放到一个文件中将会导致产生一个非常大的文件，同时多个表及索引在表空间中混合存储会导致做了大量删除操作后可能会留有有大量的空隙。在本实验中，为了方便同学们实现，我们采取共享表空间的设计方式，即将所有的数据和索引放在同一个文件中。学有余力的同学可以额外尝试使用独占表空间的设计方式进行设计。
- Disk Manager负责DB File中数据页的分配和回收，以及数据页中数据的读取和写入。
- 对应实验：[#1 DISK AND BUFFER POOL MANAGER](https://www.yuque.com/yingchengjun/pcp6qx/ldgod4)。

##### 3.2.2 Buffer Pool Manager

- Buffer Manager 负责缓冲区的管理，主要功能包括：

1. 根据需要，从磁盘中读取指定的数据页到缓冲区中或将缓冲区中的数据页转储（Flush）到磁盘；
2. 实现缓冲区的替换算法，当缓冲区满时选择合适的数据页进行替换；
3. 记录缓冲区中各页的状态，如是否是脏页（Dirty Page）、是否被锁定（Pin）等；
4. 提供缓冲区页的锁定功能，被锁定的页将不允许替换。

- 为提高磁盘 I/O 操作的效率，缓冲区与文件系统交互的单位是数据页（Page），数据页的大小应为文件系统与磁盘交互单位的整数倍。在本实验中，数据页的大小默认为 4KB。
- 对应实验：[#1 DISK AND BUFFER POOL MANAGER](https://www.yuque.com/yingchengjun/pcp6qx/ldgod4)。

##### 3.2.3 Record Manager

- Record Manager 负责管理数据表中记录。所有的记录以堆表（Table Heap）的形式进行组织。Record Manager 的主要功能包括：记录的插入、删除与查找操作，并对外提供相应的接口。其中查找操作返回的是符合条件记录的起始迭代器，对迭代器的迭代访问操作由执行器（Executor）进行。
- 堆表是由多个数据页构成的链表，每个数据页中包含一条或多条记录，支持非定长记录的存储。不要求支持单条记录的跨页存储（即保证所有插入的记录都小于数据页的大小）。堆表中所有的记录都是无序存储的。
- 需要额外说明的是，堆表只是记录组织的其中一种方式，除此之外，记录还可以通过顺序文件（按照主键大小顺序存储所有的记录）、B+树文件（所有的记录都存储在B+树的叶结点中，MySQL中InnoDB存储引擎存储记录的方式）等形式进行组织。学有余力的同学可以尝试着使用除堆表以外的形式来组织数据。
- 对应实验：[#2 RECORD MANAGER](https://www.yuque.com/yingchengjun/pcp6qx/cnz3sh)。

##### 3.2.4 Index Manager

- Index Manager 负责数据表索引的实现和管理，包括：索引（B+树等形式）的创建和删除，索引键的等值查找，索引键的范围查找（返回对应的迭代器），以及插入和删除键值等操作，并对外提供相应的接口。
- B+树索引中的节点大小应与缓冲区的数据页大小相同，B+树的叉数由节点大小与索引键大小计算得到。
- 对应实验：[#3 INDEX MANAGER](https://www.yuque.com/yingchengjun/pcp6qx/vol7gq)。

##### 3.2.5 Catalog Manager

- Catalog Manager 负责管理数据库的所有模式信息，包括：

1. 数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。
2. 表中每个字段的定义信息，包括字段类型、是否唯一等。
3. 数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。

- Catalog Manager 还必需提供访问及操作上述信息的接口，供执行器使用。
- 对应实验：[#4 CATALOG MANAGER](https://www.yuque.com/yingchengjun/pcp6qx/yu3rrg)。

##### 3.2.6 **Planner and** Executor

- Planner（执行计划生成器）的主要功能是根据解释器（Parser）生成的语法树，通过Catalog Manager 提供的信息检查语法树中的信息是否正确，如表、列是否存在，谓词的值类型是否与column类型对应等等，随后将这些词语转换成可以理解的各种 c++ 类。解析完成后，Planner根据改写语法树后生成的Statement结构，生成对应的Plannode，并将Plannode交由Executor进行执行。
- Executor（执行器）的主要功能是遍历Planner生成的计划树，将树上的 PlanNode 替换成对应的 Executor，并调用 Record Manager、Index Manager 和 Catalog Manager 提供的相应接口进行执行。Executor采用的是火山模型，提供迭代器接口，每次调用时会返回一个元组和相应的 RID，直到执行完成。
- 语法树的相关结构请参考[#5 PLANNER AND EXECUTOR](https://www.yuque.com/yingchengjun/pcp6qx/vi1zsfn8y7se2r6q)。

##### 3.2.7 SQL Parser

- 程序流程控制，即“启动并初始化 → ‘接收命令、处理命令、显示命令结果’循环 → 退出”流程。
- 接收并解释用户输入的命令，生成命令的内部数据结构表示，同时检查命令的语法正确性和部分语义正确性，对正确的命令生成语法树，然后调用执行器层提供的函数执行并显示执行结果，对不正确的命令显示错误信息。

### 四、评分标准

本实验有一定的规模，实现上有一定的复杂度，推荐1~3人一组**（请各小组于夏学期第1周结束前完成组队信息填写）**，完成一个完整的系统，推荐每个同学都参与到每个模块的设计中（但也可以每个同学完成单独的模块），分工合作完成模块功能实现、测试用例编写、模块功能测试、性能调优以及文档撰写工作。具体的评分标准如下：

|   |   |   |
|---|---|---|
|评分项|   |说明|
|小组总体设计报告（20%）|   |1. 总体报告得分各小组成员相同，在总体报告中应给出各成员负责的模块。详细报告根据各人的任务单独给分；<br>2. 报告及时提交则根据报告的质量进行给分；未及时提交则在报告质量的基础上降一级；未提交报告或抄袭，相应报告得分为`0`。|
|个人详细设计报告（20%）|   |
|系统测试与验收<br><br>（60%）|功能性测试<br><br>（50%）|1. 多人协作完成一个完整的系统，经验收功能完善且几乎没有错误，则组内各成员验收等级为`A`；如某模块功能不完善或有较多错误，则对负责该模块的成员进行扣分；<br>2. 若只有单人参与实现，但无法进行系统演示，根据完成度评定等级为`B`、`C`、`D`或`0`，原则上不评定为`A`；若单人完成，且能够进行系统演示，完成度较高，可获得加分，但平时分不会溢出50分<br>3. 多人协作完成一个系统，但最后系统无法联合运行，则按各人完成他负责的模块进行处理；<br>4. 若程序编写工作基本完成，但无法运行或无法进行测试，则根据程序质量给评定验收等级为`C`、`D`；<br>5. 若基本上未编写程序或程序纯属抄袭，验收得分为`0`。|
|正确性测试<br><br>（10%）|1. 对于系统中每个模块，除了提供的参考测试代码外，每个小组需要自行设计测试代码并运行通过，测试代码中的测试用例应尽可能涵盖所有可能出现的情况。测试代码的设计说明也应体现在最终的设计文档中；|
|附加项（Bonus）|   |1. 完成实验中要求的附加项并通过验收可获得一定的加分；<br>2. 对MiniSQL代码框架提出建设性意见的，经采纳可获得一定的加分；<br>3. Bonus直接加在平时分上，但平时分总分不超过50分。|

附：验收等级与得分对照

|     |        |
| --- | ------ |
| `A` | 90~100 |
| `B` | 80~89  |
| `C` | 70~79  |
| `D` | 60~69  |
| `E` | <60    |

# 1 DISK AND BUFFER POOL MANAGER
## 1.1 实验概述

在MiniSQL的设计中，Disk Manager和Buffer Pool Manager模块位于架构的最底层。Disk Manager主要负责数据库文件中数据页的分配和回收，以及数据页中数据的读取和写入。其中，数据页的分配和回收通过位图（Bitmap）这一数据结构实现，位图中每个比特（Bit）对应一个数据页的分配情况，用于标记该数据页是否空闲（`0`表示空闲，`1`表示已分配）。当Buffer Pool Manager需要向Disk Manager请求某个数据页时，Disk Manager会通过某种映射关系，找到该数据页在磁盘文件中的物理位置，将其读取到内存中返还给Buffer Pool Manager。而Buffer Pool Manager主要负责将磁盘中的数据页从内存中来回移动到磁盘，这使得我们设计的数据库管理系统能够支持那些占用空间超过设备允许最大内存空间的数据库。

Buffer Pool Manager中的操作对数据库系统中其他模块是透明的。例如，在系统的其它模块中，可以使用数据页唯一标识符`page_id`向Buffer Pool Manager请求对应的数据页。但实际上，这些模块并不知道该数据页是否已经在内存中还是需要从磁盘中读取。同样地，Disk Manager中的数据页读写操作对Buffer Pool Manager模块也是透明的，即Buffer Pool Manager使用逻辑页号`logical_page_id`向Disk Manager发起数据页的读写请求，但Buffer Pool Manager并不知道读取的数据页实际上位于磁盘文件中的哪个物理页（对应页号`physical_page_id`）。

注意：对于下面需要实现的每个模块，我们提供了相关的类与一组公共接口。在编写代码时，请勿修改这些类与公共接口的签名，否则将无法通过测试代码。但你可以选择向类中添加数据成员或是辅助函数以实现所需的功能。为了简化工程，目前我们只考虑单线程下的设计，但在程序中我们仍然保留了并发控制相关的锁，供对感兴趣实现事务、并发控制、故障恢复相关功能模块的同学使用。

  

## 1.2 热身: 实现一个简单的位图页

为了帮助同学们更快地熟悉MiniSQL代码的编写、调试与测试工作，我们设计了一个简单的小任务：实现一个简单的位图页（Bitmap Page），位图页是Disk Manager模块中的一部分，是实现磁盘页分配与回收工作的必要功能组件。位图页与数据页一样，占用`PAGE_SIZE`（4KB）的空间，标记一段连续页的分配情况。

Bitmap Page由两部分组成，一部分是用于加速Bitmap内部查找的元信息（Bitmap Page Meta），它可以包含当前已经分配的页的数量（`page_allocated_`）以及下一个空闲的数据页(`next_free_page_`)，元信息所包含的内容可以由同学们根据实际需要自行定义。除去元信息外，页中剩余的部分就是Bitmap存储的具体数据，其大小`BITMAP_CONTENT_SIZE`可以通过`PAGE_SIZE - BITMAP_PAGE_META_SIZE`来计算，自然而然，这个Bitmap Page能够支持最多纪录`BITMAP_CONTENT_SIZE * 8`个连续页的分配情况。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1648371054209-c0dd543c-8ca2-4be0-b0b9-e4a505b0c2de.png)

与Bitmap Page相关的代码位于`src/include/page/bitmap_page.h`和`src/page/bitmap_page.cpp`中，以下函数需要被实现：

- `BitmapPage::AllocatePage(&page_offset)`：分配一个空闲页，并通过`page_offset`返回所分配的空闲页位于该段中的下标（从`0`开始）；
- `BitmapPage::DeAllocatePage(page_offset)`：回收已经被分配的页；
- `BitmapPage::IsPageFree(page_offset)`：判断给定的页是否是空闲（未分配）的。

此外，与该模块相关的测试代码位于`test/storage/disk_manager_test.cpp`中。

注意：

  

## 1.3 磁盘数据页管理

在实现了基本的位图页后，我们就可以通过一个位图页加上一段连续的数据页（数据页的数量取决于位图页最大能够支持的比特数）来对磁盘文件（DB File）中数据页进行分配和回收。但实际上，这样的设计还存在着一点点的小问题，假设数据页的大小为4KB，一个位图页中的每个字节都用于记录，那么这个位图页最多能够管理32768个数据页，也就是说，这个文件最多只能存储`4K * 8 * 4KB = 128MB`的数据，这实际上很容易发生数据溢出的情况。

为了应对上述问题，一个简单的解决思路是，把上面说的一个位图页加一段连续的数据页看成数据库文件中的一个分区（Extent），再通过一个额外的元信息页来记录这些分区的信息。通过这种“套娃”的方式，来使磁盘文件能够维护更多的数据页信息。其主要结构如下图所示：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1648370611392-3116a928-60ef-4df3-b0fa-5903a431729f.png)

Disk Meta Page是数据库文件中的第`0`个数据页，它维护了分区相关的信息，如分区的数量、每个分区中已经分配的页的数量等等。接下来，每一个分区都包含了一个位图页和一段连续的数据页。在这样的设计下，我们假设Disk Meta Page能够记录`4K/4=1K`个分区的信息，那么整个数据库能够维护的数据页的数量以及能够存储的数据数量与之前的设计相比，扩大了1000倍。与Disk Meta Page相关的代码定义在`src/include/page/disk_file_meta_page.h`中。

不过，这样的设计还存在着一个问题。由于元数据所占用的数据页实际上是不存储数据库数据的，而它们实际上又占据了数据库文件中的数据页，换而言之，实际上真正存储数据的数据页是不连续的。举一个简单例子，假设每个分区能够支持3个数据页，那么实际上真正存储数据的页只有：2, 3, 4, 6, 7, 8...

|   |   |   |   |   |   |   |   |   |
|---|---|---|---|---|---|---|---|---|
|物理页号|0|1|2|3|4|5|6|...|
|职责|磁盘元数据|位图页|数据页|数据页|数据页|位图页|数据页||
|逻辑页号|/|/|0|1|2||3||

但实际上，对于上层的Buffer Pool Manager来说，希望连续分配得到的页号是连续的（0, 1, 2, 3...），为此，在Disk Manager中，需要对页号做一个映射（映射成上表中的逻辑页号），这样才能使得上层的Buffer Pool Manager对于Disk Manager中的页分配是无感知的。

因此，在这个模块中，需要实现以下函数，与之相关的代码位于`src/include/storage/disk_manager.h`和`src/storage/disk_manager.cpp`。

- `DiskManager::AllocatePage()`：从磁盘中分配一个空闲页，并返回空闲页的**逻辑页号**；
- `DiskManager::DeAllocatePage(logical_page_id)`：释放磁盘中**逻辑页号**对应的物理页。
- `DiskManager::IsPageFree(logical_page_id)`：判断该**逻辑页号**对应的数据页是否空闲。
- `DiskManager::MapPageId(logical_page_id)`：可根据需要实现。在`DiskManager`类的私有成员中，该函数可以用于将逻辑页号转换成物理页号。

此外，为了确保系统的其余部分正常工作，我们将在Disk Manager中提供一些已经实现的功能，如磁盘中数据页内容的读取和写入等等。

Note：`DiskManager`类中的`meta_data_`成员实际上是`MetaPage`在内存中的缓存（类似于`BufferPool`缓存`Page`的作用）。使用时，只需通过`reinterpret_cast`将`meta_data_`转换成`MetaPage`类型的对象即可。

  

## 1.4 LRU替换策略

Buffer Pool Replacer负责跟踪Buffer Pool中数据页的使用情况，并在Buffer Pool没有空闲页时决定替换哪一个数据页。在本节中，你需要实现一个基于LRU替换算法的`LRUReplacer`，`LRUReplacer`类在`src/include/buffer/lru_replacer.h`中被定义，其扩展了抽象类`Replacer`（在`src/include/buffer/replacer.h`中被定义）。`LRUReplacer`的大小默认与Buffer Pool的大小相同。

因此，在这个模块中，需要实现以下函数，与之相关的代码位于`src/buffer/lru_replacer.cpp`中。

- `LRUReplacer::Victim(*frame_id)`：替换（即删除）与所有被跟踪的页相比最近最少被访问的页，将其页帧号（即数据页在Buffer Pool的Page数组中的下标）存储在输出参数`frame_id`中输出并返回`true`，如果当前没有可以替换的元素则返回`false`；

- `LRUReplacer::Pin(frame_id)`：将数据页固定使之不能被`Replacer`替换，即从`lru_list_`中移除该数据页对应的页帧。`Pin`函数应当在一个数据页被Buffer Pool Manager固定时被调用；
- `LRUReplacer::Unpin(frame_id)`：将数据页解除固定，放入`lru_list_`中，使之可以在必要时被`Replacer`替换掉。`Unpin`函数应当在一个数据页的引用计数变为`0`时被Buffer Pool Manager调用，使页帧对应的数据页能够在必要时被替换；
- `LRUReplacer::Size()`：此方法返回当前`LRUReplacer`中能够被替换的数据页的数量。

**Bonus：**除LRU Replacer外，实现一种新的缓冲区替换算法（如Clock Replacer）。需要为新的算法实现测试用例，以证明算法的正确性。

## 1.5 缓冲池管理

在实现Buffer Pool的替换算法`LRUReplacer`后，你需要实现整个`BufferPoolManager`，与之相关的代码位于`src/include/buffer/buffer_pool_manager.h`和`src/buffer/buffer_pool_manager.cpp`中。Buffer Pool Manager负责从Disk Manager中获取数据页并将它们存储在内存中，并在必要时将脏页面转储到磁盘中（如需要为新的页面腾出空间）。

数据库系统中，所有内存页面都由`Page`对象（`src/include/page/page.h`）表示，每个`Page`对象都包含了一段连续的内存空间`data_`和与该页相关的信息（如是否是脏页，页的引用计数等等）。注意，`Page`对象并不作用于唯一的数据页，它只是一个用于存放从磁盘中读取的数据页的容器。这也就意味着同一个`Page`对象在系统的整个生命周期内，可能会对应很多不同的物理页。`Page`对象的唯一标识符`page_id_`用于跟踪它所包含的物理页，如果`Page`对象不包含物理页，那么`page_id_`必须被设置为`INVALID_PAGE_ID`。每个`Page`对象还维护了一个计数器`pin_count_`，它用于记录固定(Pin)该页面的线程数。Buffer Pool Manager将不允许释放已经被固定的`Page`。每个`Page`对象还将记录它是否脏页，在复用`Page`对象之前必须将脏的内容转储到磁盘中。

在`BufferPoolManager`的实现中，你需要用到此前已经实现的`LRUReplacer`或是其它的`Replacer`，它将被用于跟踪`Page`对象何时被访问，以便`BufferPoolManager`决定在Buffer Pool中没有空闲页可以用于分配时替换哪个数据页。

因此，在这个模块中，需要实现以下函数：

- `BufferPoolManager::FetchPage(page_id)`：根据逻辑页号获取对应的数据页，如果该数据页不在内存中，则需要从磁盘中进行读取；
- `BufferPoolManager::NewPage(&page_id)`：分配一个新的数据页，并将逻辑页号于`page_id`中返回；
- `BufferPoolManager::UnpinPage(page_id, is_dirty)`：取消固定一个数据页；
- `BufferPoolManager::FlushPage(page_id)`：将数据页转储到磁盘中；
- `BufferPoolManager::DeletePage(page_id)`：释放一个数据页；
- `BufferPoolManager::FlushAllPages()`：将所有的页面都转储到磁盘中。

对于`FetchPage`操作，如果空闲页列表（`free_list_`）中没有可用的页面并且没有可以被替换的数据页，则应返回 `nullptr`。`FlushPage`操作应该将页面内容转储到磁盘中，无论其是否被固定。

  

## 1.6 模块相关代码

需要完成的部分，可以通过搜索**TODO**来快速寻找，以下是Clion中的一个范例：

![](https://cdn.nlark.com/yuque/0/2023/png/29437275/1682239427370-fe12a44a-d210-4316-a9e1-c1d377dbe793.png)

- `src/include/page/bitmap_page.h`
- `src/page/bitmap_page.cpp`
- `src/include/storage/disk_manager.h`
- `src/storage/disk_manager.cpp`
- `src/include/buffer/lru_replacer.h`
- `src/buffer/lru_replacer.cpp`
- `src/include/buffer/buffer_pool_manager.h`
- `src/buffer/buffer_pool_manager.cpp`
- `test/buffer/buffer_pool_manager_test.cpp`
- `test/buffer/lru_replacer_test.cpp`
- `test/storage/disk_manager_test.cpp`

  

## 1.7 开发提示

1. 推荐在**夏学期第2周前**完成本模块的设计。
2. 我们推荐使用`glog`模块（`glog`是谷歌提供的日志模块，代码已在`thirdparty`目录下提供）来打印必要的调试和日志信息来代替`printf`。在程序入口处调用`google::InitGoogleLogging`初始化日志模块后，即可通过`LOG`宏（通过`#include "glog/logging.h"`引入）来打印不同级别的日志，下面是一个示例：

```
#include "glog/logging.h"

void InitGoogleLog(char *argv) {
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;
  google::InitGoogleLogging(argv);
}

int main(int argc, char **argv) {
  InitGoogleLog(argv[0]);
  LOG(INFO) << "This is an info log!";
  LOG(WARNING) << "This is a warning log!" << std::endl;
  LOG(ERROR) << "This is an error log!";
  return 0;
}
```

3. 运行测试时，出现`YOU HAVE 1 DISABLED TEST`是正常现象，如需开启，可以在测试用例的用例名中去掉`DISABLED`以开启测试，或是自行拟定测试用例。

  

## 1.8 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为`0`；
2. 请勿将代码发布到公共Github存储库上。


  
# 2 RECORD MANAGER

## 2.1 实验概述

在MiniSQL的设计中，Record Manager负责管理数据表中所有的记录，它能够支持记录的插入、删除与查找操作，并对外提供相应的接口。

与记录（Record）相关的概念有以下几个：

- 列（`Column`）：在`src/include/record/column.h`中被定义，用于定义和表示数据表中的某一个字段，即包含了这个字段的字段名、字段类型、是否唯一等等；
- 模式（`Schema`）：在`src/include/record/schema.h`中被定义，用于表示一个数据表或是一个索引的结构。一个`Schema`由一个或多个的`Column`构成；
- 域（`Field`）：在`src/include/record/field.h`中被定义，它对应于一条记录中某一个字段的数据信息，如存储数据的数据类型，是否是空，存储数据的值等等；
- 行（`Row`）：在`src/include/record/row.h`中被定义，与元组的概念等价，用于存储记录或索引键，一个`Row`由一个或多个`Field`构成。

此外，与数据类型相关的定义和实现位于`src/include/record/types.h`中。

  

## 2.2 记录与模式

在实现通过堆表来管理记录之前，先做一个小的热身项目，这是一个有关数据的序列化和反序列化操作的任务。为了能够持久化存储上面提到的`Row`、`Field`、`Schema`和`Column`对象，我们需要提供一种能够将这些对象序列化成字节流（`char*`）的方法，以写入数据页中。与之相对，为了能够从磁盘中恢复这些对象，我们同样需要能够提供一种反序列化的方法，从数据页的`char*`类型的字节流中反序列化出我们需要的对象。总而言之，序列化和反序列化操作实际上是将数据库系统中的对象（包括记录、索引、目录等）进行内外存格式转化的过程，前者将内存中的逻辑数据（即对象）通过一定的方式，转换成便于在文件中存储的物理数据，后者则从存储的物理数据中恢复出逻辑数据，两者的目的都是为了实现数据的持久化。

```
// 逻辑对象
class A {
    int id;
    char *name;
};

// 以下是序列化和反序列化的伪代码描述
void SerializeA(char *buf, A &a) {
    // 将id写入到buf中, 占用4个字节, 并将buf向后推4个字节
    WriteIntToBuffer(&buf, a.id, 4);
    WriteIntToBuffer(&buf, strlen(a.name), 4);
    WriteStrToBuffer(&buf, a.name, strlen(a.name));
}

void DeserializeA(char *buf, A *&a) {
    a = new A();
    // 从buf中读4字节, 写入到id中, 并将buf向后推4个字节
    a->id = ReadIntFromBuffer(&buf, 4);
    // 获取name的长度len
    auto len = ReadIntFromBuffer(&buf, 4);
    a->name = new char[len];
    // 从buf中读取len个字节拷贝到A.name中, 并将buf向后推len个字节
    ReadStrFromBuffer(&buf, a->name, len);
}
```

为了确保我们的数据能够正确存储，我们在上述提到的`Schema`和`Column`对象中都引入了魔数`MAGIC_NUM`，它在序列化时被写入到字节流的头部并在反序列化中被读出以验证我们在反序列化时生成的对象是否符合预期。

在本节中，你需要完善`Row`、`Schema`和`Column`对象各自的`SerializeTo`、`DeserializeFrom`和`GetSerializedSize`方法，具体以何种方式进行序列化（即需要序列化类中的哪些数据）由你自行决定，我们在测试代码中只会验证序列化前后的对象是否匹配。为了避免同学们对这块内容毫无头绪，我们保留了`Field`类型对象的序列化和反序列化操作，用于提供参考。

在本节中你需要完成如下函数：

- `Row::SerializeTo(*buf, *schema)`
- `Row::DeserializeFrom(*buf, *schema)`
- `Row::GetSerializedSize(*schema)`
- `Column::SerializeTo(*buf)`
- `Column::DeserializeFrom(*buf, *&column)`
- `Column::GetSerializedSize()`
- `Schema::SerializeTo(*buf)`
- `Schema::DeserializeFrom(*buf, *&schema)`
- `Schema::GetSerializedSize()`

其中，`SerializeTo`和`DeserializeFrom`函数的返回值为`uint32_t`类型，它表示在序列化和反序列化过程中`buf`指针向前推进了多少个字节。

对于`Row`类型对象的序列化，对于为`null`的`Field`，可以通过位图的方式标记(即 _Null Bitmaps_)；对于`Row`类型对象的反序列化，在反序列化每一个`Field`时，反序列化出来的`Field`的内存都由该`Row`对象维护。对于`Column`和`Schema`类型对象的反序列化，分配后新生成的对象于参数`column`和`schema`中返回，以下是一个简单的例子：

```
uint32_t Column::DeserializeFrom(char *buf,
                                 Column *&column){
  if (column != nullptr) {
    LOG(WARNING) << "Pointer to column is not null in column deserialize." 									 << std::endl;
  }
  /* deserialize field from buf */
	......

  /* allocate object */
  if (type == kTypeChar) {
    column = new Column(column_name, type, col_len, col_ind, nullable, unique);
  } else {
    column = new Column(column_name, type, col_ind, nullable, unique);
  }
  return ofs;
}
```

此外，在序列化和反序列化中可以用到一些宏定义在`src/include/common/macros.h`中，可根据实际需要使用：

```
#define MACH_WRITE_TO(Type, Buf, Data) \
           do { \
              *reinterpret_cast<Type *>(Buf) = (Data); \
           } while (0)
#define MACH_WRITE_UINT32(Buf, Data) MACH_WRITE_TO(uint32_t, (Buf), (Data))
#define MACH_WRITE_INT32(Buf, Data) MACH_WRITE_TO(int32_t, (Buf), (Data))
#define MACH_WRITE_STRING(Buf, Str)      \
           do {                                       \
              memcpy(Buf, Str.c_str(), Str.length()); \
           } while (0)

#define MACH_READ_FROM(Type, Buf) (*reinterpret_cast<const Type *>(Buf))
#define MACH_READ_UINT32(Buf) MACH_READ_FROM(uint32_t, (Buf))
#define MACH_READ_INT32(Buf) MACH_READ_FROM(int32_t, (Buf))

#define MACH_STR_SERIALIZED_SIZE(Str) (4 + Str.length())
```

  

## 2.3 通过堆表管理记录

### 2.3.1 RowId

对于数据表中的每一行记录，都有一个唯一标识符`RowId`（`src/include/common/rowid.h`）与之对应。`RowId`同时具有逻辑和物理意义，在物理意义上，它是一个64位整数，是每行记录的唯一标识；而在逻辑意义上，它的高32位存储的是该`RowId`对应记录所在数据页的`page_id`，低32位存储的是该`RowId`在`page_id`对应的数据页中对应的是第几条记录（详见#2.3.2）。`RowId`的作用主要体现在两个方面：一是在索引中，叶结点中存储的键值对是索引键`Key`到`RowId`的映射，通过索引键`Key`，沿着索引查找，我们能够得到该索引键对应记录的`RowId`，也就能够在堆表中定位到该记录；二是在堆表中，借助`RowId`中存储的逻辑信息（`page_id`和`slot_num`），可以快速地定位到其对应的记录位于物理文件的哪个位置。

### 2.3.2 堆表

堆表（`TableHeap`，相关定义在`src/include/storage/table_heap.h`）是一种将记录以无序堆的形式进行组织的数据结构，不同的数据页（`TablePage`）之间通过双向链表连接。堆表中的记录通过`RowId`进行定位。`RowId`记录了该行记录所在的`page_id`和`slot_num`，其中`slot_num`用于定位记录在这个数据页中的下标位置。

堆表中的每个数据页（与课本中的`Slotted-page Structure`给出的结构基本一致，见下图，能够支持存储不定长的记录）都由表头（Table Page Header）、空闲空间（Free Space）和已经插入的数据（Inserted Tuples）三部分组成，与之相关的代码位于`src/include/page/table_page.h`中，表头在页中从左往右扩展，记录了`PrevPageId`、`NextPageId`、`FreeSpacePointer`以及每条记录在`TablePage`中的偏移和长度；插入的记录在页中从右向左扩展，每次插入记录时会将`FreeSpacePointer`的位置向左移动。具体的实现细节请自行参考实现代码。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649165584868-b8768a94-7287-4ffa-8283-126368851db6.png)

当向堆表中插入一条记录时，一种简单的做法是，沿着`TablePage`构成的链表依次查找，直到找到第一个能够容纳该记录的`TablePage`（_First Fit_ 策略）。当需要从堆表中删除指定`RowId`对应的记录时，框架中提供了一种逻辑删除的方案，即通过打上Delete Mask来标记记录被删除，在之后某个时间段再从物理意义上真正删除该记录（本节中需要完成的任务之一）。对于更新操作，需要分两种情况进行考虑，一种是`TablePage`能够容纳下更新后的数据，另一种则是`TablePage`不能够容纳下更新后的数据，前者直接在数据页中进行更新即可，后者的实现方式留给同学们自行思考。此外，在堆表中还需要实现迭代器`TableIterator`(`src/include/storage/table_iterator.h`)，以便上层模块遍历堆表中的所有记录。

更新：在`TablePage::UpdateTuple`函数中，返回的是`bool`类型的结果，其中返回`true`表示更新成功，返回`false`表示更新失败。但更新失败可能由多种原因造成，只用一个`false`无法区分更新失败的原因。可以采取以下两种做法：（1）更改返回值为`int`类型；（2）参数列表中增加一个参数表示返回状态；（不太理解的同学可以移步评论区看细节）

综上，在本节中，你需要实现堆表的插入、删除、查询以及堆表记录迭代器的相关的功能，具体需要实现的函数如下：

- `TableHeap:InsertTuple(&row, *txn)`: 向堆表中插入一条记录，插入记录后生成的`RowId`需要通过`row`对象返回（即`row.rid_`）；
- `TableHeap:UpdateTuple(&new_row, &rid, *txn)`：将`RowId`为`rid`的记录`old_row`替换成新的记录`new_row`，并将`new_row`的`RowId`通过`new_row.rid_`返回；
- `TableHeap:ApplyDelete(&rid, *txn)`：从物理意义上删除这条记录；
- `TableHeap:GetTuple(*row, *txn)`：获取`RowId`为`row->rid_`的记录；
- `TableHeap:FreeHeap()`：销毁整个`TableHeap`并释放这些数据页；
- `TableHeap::Begin()`：获取堆表的首迭代器；
- `TableHeap::End()`：获取堆表的尾迭代器；
- `TableIterator`类中的成员操作符

- `TableIterator::operator++()`：移动到下一条记录，通过`++iter`调用；
- `TableIterator::operator++(int)`：移动到下一条记录，通过`iter++`调用；
- ……

**提示：**一个使用迭代器的例子

```
for (auto iter = table_heap.Begin(); iter != table_heap.End(); iter++) {
    Row &row = *iter;
    /* do some things */
}
```

**Bonus:** 优化堆表（`TableHeap`）以及数据页（`TablePage`）的实现，通过使用额外的空间记录一些元信息来加速`Row`的插入、查找和删除操作。

  

## 2.4 模块相关代码

- `src/include/record/row.h`
- `src/record/row.cpp`
- `src/include/record/schema.h`
- `src/record/schema.cpp`
- `src/include/record/column.h`
- `src/record/column.cpp`
- `src/include/storage/table_iterator.h`
- `src/storage/table_iterator.cpp`
- `src/include/storage/table_heap.h`
- `src/storage/table_heap.cpp`
- `test/record/tuple_test.cpp`
- `test/storage/table_heap_test.cpp`

  

## 2.5 开发提示

1. 推荐在**夏学期第3周前**完成本模块的设计。
2. `Linux`系统下可以使用性能测试工具`perf`来剖析性能，找到运行热点（即运行时开销较大的函数或指令）。一个基本的例子：`perf top -a -g -p <进程PID>`，可以看到在插入大量数据时`InsertTuple`是一个性能热点，为此可以通过优化插入算法来提升系统的整体性能。对性能调优感兴趣的同学可以自行上网学习有关`perf`工具的具体用法。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1648392831254-52ef84dd-22e6-415b-ae7b-57a7c4859acf.png)

3. 对于大型c++项目，因为指针的存在，如果程序中存在内存申请后没有释放就会造成内存泄漏，从而产生严重的问题。因此，在申请内存并把它交给一个指针后，需要思考这个指针指向的内存何时会被释放掉。使用智能指针可以帮助你进行内存管理，如果不想用智能指针，那么new和delete必须配对使用。可以采用valgrind和Asan等内存检查工具，帮助你检查内存泄漏等问题（可参考 [配置valgrind和asin来检查内存泄漏](https://www.zhihu.com/question/382668081/answer/2481122094?utm_id=0)）。如果使用Asan，配置好后可打开CMakeLists.txt中的-fsanitize=address 编译开关。
4. 在插入大量记录时，如果运行缓慢，可以使用`release`模式编译代码，即在编译时指定`CMAKE_BUILD_TYPE=Release`，但`release`模式下可能会丢失一些调试信息。

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

  

## 2.6 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为`0`；
2. 请勿将代码发布到公共Github存储库上。


  
# 3 INDEX MANAGER
## 3.1 实验概述

Index Manager 负责数据表索引的实现和管理，包括：索引的创建和删除，索引键的等值查找，索引键的范围查找（返回对应的迭代器），以及插入和删除键值等操作，并对外提供相应的接口。

在上一个实验中，同学们应该能够发现，通过遍历堆表的方式来查找一条记录是十分低效的。为了能够快速定位到某条记录而无需搜索数据表中的每一条记录，我们需要在上一个实验的基础上实现一个索引，这能够为快速随机查找和高效访问有序记录提供基础。索引有很多种实现方式，如B+树索引，Hash索引等等。在本实验中，需要同学们实现一个基于磁盘的B+树动态索引结构。

  

## 3.2 B+树数据页

B+树中的每个结点（Node）都对应一个数据页，用于存储B+树结点中的数据。因此在本节中，你需要实现以下三种类型的B+树结点数据页：

### 3.2.1 BPlusTreePage

`BPlusTreePage`是`BPlusTreeInternalPage`和`BPlusTreeLeafPage`类的公共父类，它包含了中间结点和叶子结点共同需要的数据：

- `page_type_`: 标记数据页是中间结点还是叶子结点；
- `key_size_`: 当前索引键的长度；
- `lsn_`: 数据页的日志序列号，该模块中不会用到；
- `size_`: 当前结点中存储Key-Value键值对的数量；
- `max_size_`: 当前结点最多能够容纳Key-Value键值对的数量；
- `parent_page_id_`: 父结点对应数据页的`page_id`;
- `page_id_`: 当前结点对应数据页的`page_id`。

你需要在`src/include/page/b_plus_tree_page.h`和`src/page/b_plus_tree_page.cpp`中实现`BPlusTreePage`类。

### 3.2.2 BPlusTreeInternalPage

中间结点`BPlusTreeInternalPage`不存储实际的数据，它只按照顺序存储![](https://cdn.nlark.com/yuque/__latex/4760e2f007e23d820825ba241c47ce3b.svg)个键和![](https://cdn.nlark.com/yuque/__latex/d6f147b9f168e0b5fbec1db2ccaa315b.svg)个指针（这些指针记录的是子结点的`page_id`）。由于键和指针的数量不相等，因此我们需要将第一个键设置为INVALID，也就是说，顺序查找时需要从第二个键开始查找。在任何时候，每个中间结点至少是半满的（Half Full）。当删除操作导致某个结点不满足半满的条件，需要通过合并（Merge）相邻两个结点或是从另一个结点中借用（移动）一个元素到该结点中（Redistribute）来使该结点满足半满的条件。当插入操作导致某个结点溢出时，需要将这个结点分裂成为两个结点。

你需要在`src/include/page/b_plus_tree_internal_page.h`和`src/page/b_plus_tree_internal_page.cpp`中实现`BPlusTreeInternalPage`类。

Note: 为了便于理解和设计，我们将键和指针以`pair`的形式顺序存储，但由于键和指针的数量不一致，我们不得已牺牲一个键的空间，将其标记为INVALID。也就是说对于B+树的每一个中间结点，我们都付出了一个键的空间代价。实际上有一种更为精细的设计选择：定义一个大小为![](https://cdn.nlark.com/yuque/__latex/4760e2f007e23d820825ba241c47ce3b.svg)的数组连续存放键，然后定义一个大小为![](https://cdn.nlark.com/yuque/__latex/d6f147b9f168e0b5fbec1db2ccaa315b.svg)的数组连续存放指针，这样设计的好处在于，一是没有空间上的浪费，二是在键值查找时CPU缓存的命中率较高（局部性原理）。学有余力的同学可以尝试着使用这种方式去实现。

### 3.2.3 BPlusTreeLeafPage

叶结点`BPlusTreeLeafPage`存储实际的数据，它按照顺序存储![](https://cdn.nlark.com/yuque/__latex/4760e2f007e23d820825ba241c47ce3b.svg)个键和![](https://cdn.nlark.com/yuque/__latex/4760e2f007e23d820825ba241c47ce3b.svg)个值，其中键由一个或多个`Field`序列化得到（参考#3.2.4），在`BPlusTreeLeafPage`类中用模板参数`KeyType`表示；值实际上存储的是`RowId`的值，它在`BPlusTreeLeafPage`类中用模板参数`ValueType`表示。叶结点和中间结点一样遵循着键值对数量的约束，同样也需要完成对应的合并、借用和分裂操作。

你需要在`src/include/page/b_plus_tree_leaf_page.h`和`src/page/b_plus_tree_leaf_page.cpp`中实现`BPlusTreeLeafPage`类。

### 3.2.4 Key、Value & KeyManager

`Key`: 索引键是索引列的值序列化后得到的字符串。如BPlusTreeIndexGenericKeyTest中所示，对于一个有三列（id，name，account）的表，索引（id，name）的键即是两列的值（例如27，“minisql”）序列化后的字符串。索引列的长度作为参数在构造BPlusTreeIndex时作为参数传入，保存在各个节点中，方便根据`key_size`确定每个键值对在模板中的位置，从而读写。

`Value`: 值类型可能不同，叶结点存储`RowId`，而非叶结点存储`page_id`

`KeyManager`: 负责对`GenericKey`进行序列化/反序列化和比较，注意比较时传入的是`GenericKey*`指针，指针指向的内容可能在插入删除时随着B+树结构变动被修改。

```
TEST(BPlusTreeTests, BPlusTreeIndexGenericKeyTest) {
  DBStorageEngine engine(db_name);
  std::vector<Column *> columns = {new Column("id", TypeId::kTypeInt, 0, false, false),
                                   new Column("name", TypeId::kTypeChar, 64, 1, true, false),
                                   new Column("account", TypeId::kTypeFloat, 2, true, false)};
  std::vector<uint32_t> index_key_map{0, 1};
  const TableSchema table_schema(columns);
  auto *key_schema = Schema::ShallowCopySchema(&table_schema, index_key_map);
  std::vector<Field> fields{Field(TypeId::kTypeInt, 27),
                            Field(TypeId::kTypeChar, const_cast<char *>("minisql"), 7, true)};
  KeyManager KP(key_schema, 128);
  Row key(fields);
  GenericKey *k1 = KP.InitKey();
  KP.SerializeFromKey(k1, key, key_schema);
  GenericKey *k2 = KP.InitKey();
  Row copy_key(fields);
  KP.SerializeFromKey(k2, copy_key, key_schema);
  ASSERT_EQ(0, KP.CompareKeys(k1, k2));
}
```

对于B+树中涉及到的索引键的比较，由于`GenericKey`对象并不是基本数据类型，因此不能够直接使用比较运算符`>`、`<`等进行比较（除非对传入的对象的比较运算符进行重载，但这种设计方式难以应对需要不同比较方式的场景）。为此，我们需要借助`KeyManager`中的`CompareKeys`方法对两个索引键进行比较。以下是一个例子：

```
void Example(GenericKey *k1, GenericKey *k2, KeyManager &KM) {
    if (KM.CompareKeys(k1, k2) > 0) {
        // k1 > k2
    } else if (KM.CompareKeys(k1, k2) < 0) {
        // k1 < k2
    } else {
        // k1 == k2
    }
} 
```

`CompareKeys`的实现在框架中已经给出（在`src/include/index/generic_key.h`中定义），其基本原理是，对于两个待比较的索引键`GenericKey`（为了将索引键存储到B+树数据页中，需要将索引键进行序列化，也就是说`GenericKey`内部实际上存储的是索引键序列化后得到的字符串，参考下面代码中`GenericKey`类的定义），首先将其按照索引键定义的模式`key_schema_`进行反序列化，然后对反序列化得到的每一个域`Field`，调用`Field`的比较函数进行比较。`Field`类型的比较函数已经在代码框架中给出，具体细节请同学们自行学习了解。

```
class GenericKey {
  	friend class KeyManager;
    // actual location of data, extends past the end.
    char data[0];
}

inline void SerializeFromKey(GenericKey *key_buf, const Row &key) const;

inline void DeserializeToKey(const GenericKey *key_buf, Row &key) const;

inline int GenericComparator::CompareKeys(const GenericKey *lhs, const GenericKey *rhs) const
{
    uint32_t column_count = key_schema_->GetColumnCount();
    Row lhs_key(INVALID_ROWID);
    Row rhs_key(INVALID_ROWID);
    DeserializeToKey(lhs, lhs_key);
    DeserializeToKey(rhs, rhs_key);

    for (uint32_t i = 0; i < column_count; i++)
    {
      Field *lhs_value = lhs_key.GetField(i);
      Field *rhs_value = rhs_key.GetField(i);
        if (lhs_value->CompareLessThan(*rhs_value) == CmpBool::kTrue)
        return -1;

      if (lhs_value->CompareGreaterThan(*rhs_value) == CmpBool::kTrue)
        return 1;
    }
    // equals
    return 0;
}
```

### 3.2.5 Some Tips

- `BPlusTreePage::GetMinSize()`所返回的值通常情况下为`max_size_/2`，但它实际上对于叶子结点/非叶结点/根结点/非根结点可能会有所不同。且`size`的概念通常情况下表示的是指针的数量（即结点中键值对的数量），换而言之，在中间结点中，包含![](https://cdn.nlark.com/yuque/__latex/35e9ad4e8d4ed4a0df12c72b01f18024.svg)个键和![](https://cdn.nlark.com/yuque/__latex/df976ff7fcf17d60490267d18a1e3996.svg)个指针的`size`为![](https://cdn.nlark.com/yuque/__latex/df976ff7fcf17d60490267d18a1e3996.svg)。
- `BPlusTreePage`中的内容实际上存储于`Page`中的`data_`，每当需要对B+树的数据页进行读写时，首先需要从`BufferPoolManager`中获取（`Fetch`）这个页，此时拿到的数据页为`Page`类型，但我们需要用到的数据页`BPlusTreeInternalPage`和`BPlusTreeLeafPage`是`BPlusTreePage`类的子类，`BPlusTreePage`类和`Page`类的`data_`域在内存分布上是相同的（通俗来说，`data_`域中`PAGE_SIZE`个字节存放的就是`BPlusTreePage`对象），因此需要通过`reinterpret_cast`将`Page`中的`data_`重新解释成为我们需要使用的类。最后，在使用完毕后需要将该页释放（`Unpin`），以下是一个使用`reinterpret_cast`将`Page`类的`data_`域重新解释成`BPlusTreeInternalPage`对象例子：

```
auto *page = buffer_pool_manager->FetchPage(page_id);
if (page != nullptr) {
    auto *node = reinterpret_cast<BPlusTreeInternalPage *>(page->GetData());
    /* do something */
    buffer_pool_manager->UnpinPage(page_id, true);
}
```

- 在不需要使用数据页时，请务必将其释放，我们将会在测试代码中加入`CheckAllUnpinned()`机制检查所有的数据页最终是否被释放。
- 在`UpdateRootPageId`函数中，有关root page的定义在`include/page/index_roots_page.h`中
- `BPlusTree::BPlusTree`函数中，如果传入的`leaf_max_size`和`internal_max_size`是默认值0，即`UNDEFINED_SIZE`，那么需要自己根据keysize进行计算

## 3.3 B+树索引

在完成B+树结点的数据结构设计后，接下来需要完成B+树的创建、插入、删除、查找和释放等操作。注意，所设计的B+树只能支持`Unique Key`，这也意味着，当尝试向B+树插入一个重复的Key-Value键值对时，将不能执行插入操作并返回`false`状态。当一些写操作导致B+树索引的根结点发生变化时，需要调用`BPLUSTREE_TYPE::UpdateRootPageId`完成`root_page_id`的变更和持久化。

**Note：在**`**UpdateRootPageId**`**函数中，有关root page的定义在**`**include/page/index_roots_page.h**`**中**

你需要在`src/include/index/b_plus_tree.h`和`src/index/b_plus_tree.cpp`中实现整个`BPlusTree`类。其中一些方法如Coalesce、Redistribute根据传入参数类型不同（LeafPage or InternalPage）需要实现两个方法，看起来很多，但大体逻辑是类似的，细微处需要根据是叶子结点还是内部节点作出修改。

在实现`BPlusTree`时，你无需考虑`GenericKey`、`KeyManager`的实现，与它们相关的类已经实现，位于`src/include/index/generic_key.h`中。`KeyManager`的实例将会随`BPlusTreeIndex`一起构造。

## 3.4 B+树索引迭代器

与堆表`TableHeap`对应的迭代器类似，在本节中，你需要为B+树索引也实现一个迭代器。该迭代器能够将所有的叶结点组织成为一个单向链表，然后沿着特定方向有序遍历叶结点数据页中的每个键值对（这在范围查询时将会被用到）。

你需要在`src/include/index/index_iterator.h`和`src/index/index_iterator.cpp`中实现B+树索引的迭代器`IndexIterator`。同样地，你需要在`BPlusTree`类中实现`Begin()`和`End()`函数以获取B+树索引的首迭代器和尾迭代器。

  

## 3.5 模块相关代码

- `src/include/page/b_plus_tree_page.h`
- `src/page/b_plus_tree_page.cpp`
- `src/include/page/b_plus_tree_internal_page.h`
- `src/storage/page/b_plus_tree_internal_page.cpp`
- `src/include/page/b_plus_tree_leaf_page.h`
- `src/storage/page/b_plus_tree_leaf_page.cpp`
- `src/include/storage/index/b_plus_tree.h`
- `src/storage/index/b_plus_tree.cpp`
- `src/include/storage/index/index_iterator.h`
- `src/storage/index/index_iterator.cpp`
- `test/index/b_plus_tree_index_test.cpp`
- `test/index/b_plus_tree_test.cpp`
- `test/index/index_iterator_test.cpp`

  

## 3.6 开发提示

1. 推荐在**夏学期第4周前**完成本模块的设计。
2. 这是一个展现B+树插入和删除操作的可视化网站，可以帮助熟悉B+树的相关操作：[链接](https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html)
3. 在调试时，可以通过`BPlusTree::PrintTree(std::ofstream &out)`将B+树的结构以DOT格式输出到输出流中，然后可以通过一个可视化网站：[链接](http://dreampuf.github.io/GraphvizOnline/)，查看当前B+树的状态。具体的使用方法可以参考测试模块中给出的代码。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1648395761641-530a96fa-6cc6-46b4-94e9-c3c032a0c7f2.png)

  

## 3.7 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为`0`；
2. 请勿将代码发布到公共Github存储库上。

  
# 4 CATALOG MANAGER
  

## 4.1 实验概述

Catalog Manager 负责管理和维护数据库的所有模式信息，包括：

- 数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。
- 表中每个字段的定义信息，包括字段类型、是否唯一等。
- 数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。

这些模式信息在被创建、修改和删除后还应被持久化到数据库文件中。此外，Catalog Manager还需要为上层的执行器Executor提供公共接口以供执行器获取目录信息并生成执行计划。

  

## 4.2 目录元信息

数据库中定义的表和索引在内存中以`TableInfo`和`IndexInfo`的形式表现，它们分别定义于`src/include/catalog/table.h`和`src/include/catalog/indexes.h`，其维护了与之对应的表或索引的元信息和操作对象。以`IndexInfo`为例，它包含了这个索引定义时的元信息`meta_data_`，该索引对应的表信息`table_info_`，该索引的模式信息`key_schema_`和索引操作对象`index_`。除元信息`meta_data_`外，其它的信息（如`key_schema_`、`table_info_`等）都是通过反序列化后的元信息生成的。也就是说，为了能够将所有表和索引的定义信息持久化到数据库文件并在重启时从数据库文件中恢复，我们需要为表和索引的元信息`TableMetadata`和`IndexMetadata`实现序列化和反序列化操作。它们与`TableInfo`和`IndexInfo`定义在相同文件中。在序列化时，为了简便处理，我们为每一个表和索引都分配一个单独的数据页用于存储序列化数据。因此，在这样的设计下，我们同样需要一个数据页和数据对象`CatalogMeta`（定义在`src/include/catalog/catalog.h`）来记录和管理这些表和索引的元信息被存储在哪个数据页中。`CatalogMeta`的信息将会被序列化到数据库文件的第`CATALOG_META_PAGE_ID`号数据页中（逻辑意义上），`CATALOG_META_PAGE_ID`默认值为0。

在本节中，你需要了解与Catalog相关的元信息的序列化和反序列化操作，并实现GetSerializedSize()函数：

- `CatalogMeta::GetSerializedSize()`
- `IndexMetadata::GetSerializedSize()`
- `TableMetadata::GetSerializedSize()`
- `IndexInfo::Init(*index_meta_data, *table_info, *buffer_pool_manager)`: 传入事先创建好的`IndexMetadata`和从`CatalogManager`中获取到的`TableInfo`，创建索引本身的`key_schema_`和`Index`对象。这里的`key_schema_`可以通过`Schema::ShallowCopySchema`来创建，且`key_schema_`中包含的列与`TableSchema`中的列共享同一份存储。

**提示：**

- 与之前`RecordManager`中的序列化和反序列化类似，你需要通过魔数`MAGIC_NUM`来确保序列化和反序列化的正确性。

  

## 4.3 表和索引的管理

在实现目录、表和索引元信息的持久化后，你需要在`src/include/catalog/catalog.h`和`src/catalog/catalog.cpp`中实现整个`CatalogManager`类。`CatalogManager`类应具备维护和持久化数据库中所有表和索引的信息。`CatalogManager`能够在数据库实例（`DBStorageEngine`）初次创建时（`init = true`）初始化元数据；并在后续重新打开数据库实例时，从数据库文件中加载所有的表和索引信息，构建`TableInfo`和`IndexInfo`信息置于内存中。此外，`CatalogManager`类还需要对上层模块提供对指定数据表的操作方式，如`CreateTable`、`GetTable`、`GetTables`、`DropTable`、`GetTableIndexes`；对上层模块提供对指定索引的操作方式，如`CreateIndex`、`GetIndex`、`DropIndex`。

**Note:** 完成这部分时，需要注意对象内存的管理问题。我们在申请每一块内存的时候，都需要思考在什么时候进行释放。比如`Schema`类中，有深拷贝、浅拷贝函数，并且注释里写了什么时候该用深拷贝。如果没有正确使用，那么测试用例中`shared_ptr`会发生二次析构的报错。

**Note:** 在目前这种架构设计下，Catalog和实际数据存放在同一个数据库文件中。这使得Catalog Manager既依赖于Disk Manager 、Buffer Pool Manager，又为Record Manager、Index Manager、Executor(甚至Parser)提供接口支持，从而导致整个MiniSQL项目的内聚度相当地高。为此，一种妥协式的方案是：Catalog Manager不依赖于Disk Manager和Buffer Pool Manager，而是存放在独立的文件中，通过持久化手段直接落盘，但这种做法同样存在缺点，即Catalog的信息完全不受事务管理控制，存在一致性和可恢复性的问题。同学们可以根据实际需要自行选择Catalog Manager的实现方式。

  

## 4.4 模块相关代码

- `src/include/catalog/catalog.h`
- `src/catalog/catalog.cpp`
- `src/include/catalog/indexes.h`
- `src/catalog/indexes.cpp`
- `src/include/catalog/table.h`
- `src/catalog/table.cpp`
- `test/catalog/catalog_test.cpp`

  

## 4.5 开发提示

1. 推荐在**夏学期第5周前**完成本模块的设计。

  

## 4.6 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为`0`；
2. 请勿将代码发布到公共Github存储库上。


# 5 PLANNER AND EXECUTOR
## 5.1 实验概述

本实验主要包括Planner和Executor两部分。Planner的主要功能是将解释器（Parser）生成的语法树，改写成数据库可以理解的数据结构。在这个过程中，我们会将所有sql语句中的标识符（Identifier）解析成没有歧义的实体，即各种C++的类，并通过Catalog Manager 提供的信息生成执行计划。Executor遍历查询计划树，将树上的 PlanNode 替换成对应的 Executor，随后调用 Record Manager、Index Manager 和 Catalog Manager 提供的相应接口进行执行，并将执行结果返回给上层模块。

## 5.2 Parser生成语法树

考虑到同学们尚未接触到编译原理的相关知识，在本实验中，我们已经为同学们设计好MiniSQL中的Parser模块，与Parser模块的相关代码如下：

- `src/include/parser/minisql.l`：SQL的词法分析规则；
- `src/include/parser/minisql.y`：SQL的文法分析规则；
- `src/include/parser/minisql_lex.h`：`flex(lex)`根据词法规则自动生成的代码；
- `src/include/parser/minisql_yacc.h`：`bison(yacc)`根据文法规则自动生成的代码；
- `src/include/parser/parser.h`：Parser模块相关的函数定义，供词法分析器和语法分析器调用存储分析结果，同时可供执行器调用获取语法树根结点；
- `src/include/parser/syntax_tree.h`：语法树相关定义，语法树各个结点的类型同样在`SyntaxNodeType`中被定义。

### 5.2.1 语法树数据结构

以下是语法树（结点）的数据结构定义，每个结点都包含了一个唯一标识符`id_`，唯一标识符在调用`CreateSyntaxNode`函数时生成（框架中已经给出实现）。`type_`表示语法树结点的类型，`line_no_`和`col_no_`表示该语法树结点对应的是SQL语句的第几行第几列，`child_`和`next_`分别表示该结点的子结点和兄弟结点，`val_`用作一些额外信息的存储（如在`kNodeString`类型的结点中，`val_`将用于存储该字符串的字面量）。

```
/**
 * Syntax node definition used in abstract syntax tree.
 */
struct SyntaxNode {
  int id_;    /** node id for allocated syntax node, used for debug */
  SyntaxNodeType type_; /** syntax node type */
  int line_no_; /** line number of this syntax node appears in sql */
  int col_no_;  /** column number of this syntax node appears in sql */
  struct SyntaxNode *child_;  /** children of this syntax node */
  struct SyntaxNode *next_;   /** siblings of this syntax node, linked by a single linked list */
  char *val_; /** attribute value of this syntax node, use deep copy */
};
typedef struct SyntaxNode *pSyntaxNode;
```

举一个简单的例子，`select * from t1 where id = 1 and name = "str";`这一条SQL语句生成的语法树如下。以根结点为例说明，`kNodeSelect`为结点的类型，`(1,47)`表示该结点在规约（_reduce_，编译原理中的术语）后位于行的第1行第47列（语句末），`id(9)`表示该结点的`id_`为`9`。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649154043421-00f6a10a-a8fe-45c5-b02f-7fe1895e42c1.png)

### 5.2.2 Parser支持的SQL语句

Parser模块中目前能够支持以下类型的SQL语句。其中包含了一些在语法定义上正确，但在语义上错误的SQL语句（如Line 8～10）需要同学们在执行器中对这些特殊情况进行处理。此外涉及到事务开启、提交和回滚相关的`begin`、`commit`和`rollback`命令可以不做实现。

```
create database db0;
drop database db0;
show databases;
use db0;
show tables;
create table t1(a int, b char(20) unique, c float, primary key(a, c));
create table t1(a int, b char(0) unique, c float, primary key(a, c));
create table t1(a int, b char(-5) unique, c float, primary key(a, c));
create table t1(a int, b char(3.69) unique, c float, primary key(a, c));
create table t1(a int, b char(-0.69) unique, c float, primary key(a, c));
create table student(
  sno char(8),
  sage int,
  sab float unique,
  primary key (sno, sab)
);
drop table t1;
create index idx1 on t1(a, b);
-- "btree" can be replaced with other index types
create index idx1 on t1(a, b) using btree;
drop index idx1;
show indexes;
select * from t1;
select id, name from t1;
select * from t1 where id = 1;
-- note: use left association
select * from t1 where id = 1 and name = "str";
select * from t1 where id = 1 and name = "str" or age is null and bb not null;
insert into t1 values(1, "aaa", null, 2.33);
delete from t1;
delete from t1 where id = 1 and amount = 2.33;
update t1 set c = 3;
update t1 set a = 1, b = "ccc" where b = 2.33;
begin;
commit;
rollback;
quit;
execfile "a.txt";
```

在Parser模块调用`yyparse()`（一个示例在`src/main.cpp`中）完成SQL语句解析后，将会得到语法树的根结点`pSyntaxNode`。将语法树根结点传入`ExecuteEngine`（定义于`src/include/executor/execute_engine.h`）后，`ExecuteEngine`将会根据语法树根结点的类型，决定是否需要传入`Planner`生成执行计划。

## 5.3 Planner生成查询计划

对于简单的语句例如show databases，drop table 等，生成的语法树也非常简单。以show databases为例，对应的语法树只有单节点kNodeShowDB，表示展示所有数据库。此时无需传入Planner生成执行计划，我们直接调用对应的执行函数执行即可。

![](https://cdn.nlark.com/yuque/0/2023/png/29437275/1675914520871-d2fbd06c-0439-4f7e-a75c-8755d42d1d60.png)

而复杂的语句例如select，insert，update，deleete，生成的语法树也较为复杂。以一条select的sql为例，生成的对应语法树如下：其中 kNodeSelect标识了语句的类型，kNodeColumnList标识了有哪些列，kNodeIdentifier标识了是哪张表，kNodeConditions标识了where语句的条件。

```
select id, name from t1 where id = 1;
```

![](https://cdn.nlark.com/yuque/0/2023/svg/29437275/1675913206316-11521d7f-f1df-4681-acd5-33210ce8a286.svg)

对于复杂的语句，生成的语法树需传入Planner生成执行计划，并交由Executor进行执行。Planner需要先遍历语法树，调用Catalog Manager 检查语法树中的信息是否正确，如表、列是否存在，谓词的值类型是否与column类型对应等等，随后将这些词语抽象成相应的表达式（表达式在`src/include/planner/expressions/`），即可以理解的各种 c++ 类。解析完成后，Planner根据改写语法树后生成的可以理解的Statement结构，生成对应的Plannode，并将Planndoe交由executor进行执行。该模块相关的代码如下：

- `src/include/planner/statement/abstract_statement.h`
- `src/include/planner/statement/select_statement.h`
- `src/include/planner/statement/insert_statement.h`
- `src/include/planner/statement/delete_statement.h`
- `src/include/planner/statement/update_statement.h`

`Statement`中的函数`SyntaxTree2Statement`将解析语法树，并将各种Identifier转化为可以理解的表达式，存储在Statement结构中。Planner再根据Statement，生成对应的执行计划，相关代码如下：

- `src/include/executor/plans/abstract_plan.h`
- `src/include/executor/plans/delete_plan.h`
- `src/include/executor/plans/insert_plan.h`
- `src/include/executor/plans/seq_scan_plan.h`
- `src/include/executor/plans/update_plan.h`
- `src/include/executor/plans/value_plan.h`

本模块中你不用实现任何代码。

**Tips：**在成熟的数据库中，Planner一般和优化器Optimizer一起，称为查询优化器。通常，查询优化器会通过如下三个典型组件协同来完成查询优化。优化后，能将原本根据语法树直接生成的查询计划改写成效率更高的查询计划，例如经典的join order问题。

- **Plan space enumeration:** 根据一系列的等价变换规则，生成与查询等价的多个执行计划；
- **cardinality estimation:** 根据查询表的分布情况，估计查询执行过程中的数据量/数据分布等；
- **cost model:** 根据执行计划以及数据库内部的状态，计算按照各个执行计划执行所需要的代价。

## 5.4 Executor执行查询计划

在拿到 Planner 生成的具体的查询计划后，就可以生成真正执行查询计划的一系列算子了。生成算子的步骤很简单，遍历查询计划树，将树上的 PlanNode 替换成对应的 Executor。算子的执行模型也大致分为三种：

1. Iterator Model，即经典的火山模型。执行引擎会将整个 SQL 构建成一个 Operator 树，查询树自顶向下的调用接口，数据则自底向上的被拉取处理。每一种操作会抽象为一个 Operator，每个算子都有 Init() 和 Next() 两个方法。Init() 对算子进行初始化工作。Next() 则是向下层算子请求下一条数据。当 Next() 返回 false 时，则代表下层算子已经没有剩余数据，迭代结束。

2. 该方法的优点是其计算模型简单直接，通过把不同物理算子抽象成一个个迭代器。每一个算子只关心自己内部的逻辑即可，让各个算子之间的耦合性降低，从而比较容易写出一个逻辑正确的执行引擎。
3. 缺点是火山模型一次调用请求一条数据，占用内存较小，但函数调用开销大，特别是虚函数调用造成 cache miss 等问题。同时，逐行地获取数据会带来过多的 I/O，对缓存也不友好。

4. Materialization Model，算子计算出所有结果后一起返回。这种模型的弊端显而易见，当数据量较大时，内存占用很高。但该模型减少了函数调用的开销。比较适合查询数据量较小的 OLTP workloads。
5. Vectorization Model. 对上面两种模型的中和，输入和输出都以Batch为单位。在Batch的处理模式下，计算过程还可以使用SIMD指令进行加速。目前比较先进的 OLAP 数据库都采用这种模型。

本任务采用的是最经典的 Iterator Model。在本次任务中，我们将实现5个算子，分别是select，Index Select，insert，update，delete。 对于每个算子，都实现了 Init 和 Next 方法。 Init 方法初始化运算符的内部状态，Next 方法提供迭代器接口，并在每次调用时返回一个元组和相应的 RID。对于每个算子，我们假设它在单线程上下文中运行，并不需要考虑多线程的情况。每个算子都可以通过访问 `ExecuteContext`来实现表的修改，例如插入、更新和删除。 为了使表索引与底层表保持一致，插入删除时还需要更新索引。

最后，我们在`test/execution/executor_test.cpp`中提供了一些执行器的测试样例。这些样例非常简单，但可以通过它们了解PlanNode是如何构建的，以及算子是如何执行PlanNode并得到期望的结果。

### 5.3.1 SeqScan

SeqScanExecutor 对表执行一次顺序扫描，一次Next()方法返回一个符合谓词条件的行。顺序扫描的表名和谓词（Predicate）由SeqScanPlanNode 指定。

提示：遍历表时可使用 TableIterator 对象，并了解清楚 ++iter 和 iter++ 在底层是怎样实现的。

提示：如果Scan存在谓词，需要通过调用谓词表达式的Evaluate方法来判断该行是否满足条件。建议了解清楚不同表达式的AbstractExpression::Evaluate方法是如何实现的。目前底层没有Bool类型，所以Evaluate方法返回的结果可以和Field(kTypeInt, 1)进行比较，相等则为True。

提示：顺序扫描的输出是每个匹配行及其标识符（RID）的副本。

### 5.3.2 IndexScan

IndexScanExecutor 对表执行一次带索引的扫描，一次Next()方法返回一个符合谓词条件的行。为简单起见，IndexScan仅支持单列索引。当Planner检测到select的谓词中的列上存在索引，而且索引只包含该列时，会生成IndexScanPlan，其他情况则生成SeqScanPlan。同时，为简单起见，IndexScan不支持谓词中存在or的情况（例如a=1 or b=2，此时a列存在索引，b列不存在索引，我们仍需顺序扫描表来找出符合b=2条件的列）。因此，在IndexScanExecutor中你只需要考虑单列索引，并且不用考虑谓词中存在or的情况。

提示：indexes_中存放了所有的索引，我们需要求出满足每个索引的rowid集合，并对这些集合取交集（因为只用考虑and情况）。之后根据rowid，去取相应的记录。取交集可以用库函数set_intersection方法。

提示：need_filter_表示是否谓词中所有的列上都有索引。如果只有部分列上有索引，那么我们用索引筛选出符合条件的记录后，仍需再调用evaluate方法判断是否符合条件。

### 5.3.3 Insert

InsertExecutor 将行插入表中并更新索引。目前，Parser只支持直接插入`insert into t1 values(1, "aaa", null, 2.33);`，不支持 INSERT INTO .. SELECT ...的语法。要插入的值通过ValueExecutor生成对应的行，随后被拉取到InsertExecutor中。

提示：注意unique对插入的影响。因为建表时会对所有unique列建立索引，所以查询插入值是否已经存在可以走索引。

提示：由于可以对不同的字段建立 index，一个 table 可能对应多个 index，所有的 index 都需要更新。

提示：返回空的 tuple ，用于统计 table 中有多少行受到了影响。

### 5.3.4 Value

ValueExecutor主要用于`insert into t1 values(1, "aaa", null, 2.33);`语句。插入的值以vector形式存储在ValuesPlanNode中，ValueExecutor调用Next()方法一次返回一个新的行。本算子已经实现。

### 5.3.5 Update

UpdateExecutor 修改指定表中的现有行并更新其索引。UpdatePlanNode 将有一个 SeqScanPlanNode 作为其子节点，要更新的值通过SeqScanExecutor提供。

提示：Update可理解为先删除旧行，再插入新行。

提示：GenerateUpdatedTuple方法将根据PlanNode中提供的更新属性构造出一个新的元组。

### 5.3.6 Delete

DeleteExecutor 删除表中符合条件的行。和Update一样，DeletePlanNode 将有一个 SeqScanPlanNode 作为其子节点，要删除的值通过SeqScanExecutor提供。

提示：注意索引的删除。

提示：返回空的 tuple ，用于统计 table 中有多少行受到了影响。

在本节中，你需要实现seqscan，insert，update，delete, indexscan这五个算子。算子的头文件定义如下：

- `src/include/executor/executors/abstract_executor.h`
- `src/include/executor/executors/delete_executor.h`
- `src/include/executor/executors/insert_executor.h`
- `src/include/executor/executors/seq_scan_executor.h`
- `src/include/executor/executors/index_scan_executor.h`
- `src/include/executor/executors/values_executor.h`

此外，你还需要实现`src/include/executor/execute_engine.h`中的创建删除查询数据库、数据表、索引等函数。它们对应的语法树较为简单，因此不用通过Planner生成查询计划。它们被声明为`private`类型的成员，所有的执行过程对上层模块隐藏，上层模块只需要调用`ExecuteEngine::execute()`并传入语法树结点即可无感知地获取到执行结果。

- `ExecuteEngine::ExecuteCreateDatabase(*ast, *context)`
- `ExecuteEngine::ExecuteDropDatabase(*ast, *context)`
- `ExecuteEngine::ExecuteShowDatabases(*ast, *context)`
- `ExecuteEngine::ExecuteUseDatabase(*ast, *context)`
- `ExecuteEngine::ExecuteShowTables(*ast, *context)`
- `ExecuteEngine::ExecuteCreateTable(*ast, *context)`
- `ExecuteEngine::ExecuteDropTable(*ast, *context)`
- `ExecuteEngine::ExecuteShowIndexes(*ast, *context)`
- `ExecuteEngine::ExecuteCreateIndex(*ast, *context)`
- `ExecuteEngine::ExecuteDropIndex(*ast, *context)`
- `ExecuteEngine::ExecuteExecfile(*ast, *context)`
- `ExecuteEngine::ExecuteQuit(*ast, *context)`
- `ExecuteEngine::ExecuteTrxBegin(*ast, *context)`：事务相关，可不实现
- `ExecuteEngine::ExecuteTrxCommit(*ast, *context)`：事务相关，可不实现
- `ExecuteEngine::ExecuteTrxRollback(*ast, *context)`：事务相关，可不实现

## 5.4 模块相关代码

- `src/main.cpp`
- `src/include/executor/execute_engine.h`
- `src/executor/execute_engine.cpp`
- `src/include/executor/executors/abstract_executor.h`
- `src/include/executor/executors/delete_executor.h`
- `src/include/executor/executors/insert_executor.h`
- `src/include/executor/executors/seq_scan_executor.h`
- `src/include/executor/executors/index_scan_executor.h`
- `src/include/executor/executors/update_executor.h`

## 5.5 开发提示

1. 整个MiniSQL项目推荐在**夏学期第7周**前完成；
2. 框架中已经给出了语法树的`PrintTree()`方法，它能够打印语法树中的每一个结点（输出DOT格式），具体用法和之前的B+树打印类似，输出的结果放在可视化界面中可以用作调试。此外也可以使用GDB或IDE自带的调试工具在完成SQL语法分析后得到语法树的语句打上断点以进行调试。
3. 如果需要更改语法和文法以支持新的SQL命令，可以在学习LEX和YACC的相关知识后，修改`src/include/parser/minisql.l`和`src/include/parser/minisql.y`文件，然后执行`src/include/parser/compile.sh`脚本（它会自动生成对应的`lex`和`yacc`代码并移动到工程的指定目录下），最后需要重新执行`cmake ..`完成更新；
4. 在`test/execution/executor_test.cpp`中提供了一些执行器的测试样例。在executor实现之后，可以在`main.cpp`中手动输入SQL命令观察结果。

## 5.6 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为`0`；
2. 请勿将代码发布到公共Github存储库上。


  
# 6 RECOVERY MANAGER

## 6.1 实验概述

Recovery Manager 负责管理和维护数据恢复的过程，包括：

- 日志结构的定义
- 检查点CheckPoint的定义
- 执行Redo、Undo等操作，处理插入、删除、更新，事务的开始、提交、回滚等日志，将数据库恢复到宕机之前的状态

出于实现复杂度的考虑，同时为了避免各模块耦合太强，前面模块的问题导致后面模块完全无法完成，同组成员的工作之间影响过深，我们将Recovery Manager模块单独拆了出来。另外为了减少重复的内容，我们不重复实现日志的序列化和反序列化操作，实现一个纯内存的数据恢复模块即可。

## 6.2 数据恢复

数据恢复是一个很复杂的过程，需要涉及系统的多个模块。以InnoDB为例，在其恢复过程中需要redo log、binlog、undo log等参与，这里把InnoDB的恢复过程主要划分为两个阶段：第一阶段主要依赖于redo log的恢复，而第二阶段需要binlog和undo log的共同参与。

第一阶段，数据库启动后，InnoDB会通过redo log找到最近一次checkpoint的位置，然后根据checkpoint相对应的LSN开始，获取需要重做的日志，接着解析日志并且保存到一个哈希表中，最后通过遍历哈希表中的redo log信息，读取相关页进行恢复。

在该阶段中，所有被记录到redo log但是没有完成数据刷盘的记录都被重新落盘。然而，InnoDB单靠redo log的恢复是不够的，因为数据库在任何时候都可能发生宕机，需要保证重启数据库时都能恢复到一致性的状态。这个一致性的状态是指此时所有事务要么处于提交，要么处于未开始的状态，不应该有事务处于执行了一半的状态。所以我们可以通过undo log在数据库重启时把正在提交的事务完成提交，活跃的事务回滚，保证了事务的原子性。此外，只有redo log还不能解决主从数据不一致等问题。

第二阶段，根据undo中的信息构造所有未提交事务链表，最后通过上面两部分协调判断事务是否需要提交还是回滚。InnoDB使用了多版本并发控制(MVCC)以满足事务的隔离性，简单的说就是不同活跃事务的数据互相是不可见的，否则一个事务将会看到另一个事务正在修改的数据。InnoDB借助undo log记录的历史版本数据，来恢复出对于一个事务可见的数据，满足其读取数据的请求。

在我们的实验中，日志在内存中以`LogRec`的形式表现，定义于`src/include/recovery/log_rec.h`。出于实现复杂度的考虑，我们将Recovery Manager模块独立出来，不考虑日志的落盘，用一个unordered_map简易的模拟一个KV Database，并直接在内存中定义一个能够用于插入、删除、更新，事务的开始、提交、回滚的日志结构。`CheckPoint`检查点应包含当前数据库一个完整的状态，该结构已帮大家实现好了。`RecoveryManager`则包含`UndoPhase`和`RedoPhase`两个函数，代表Redo和Undo两个阶段。

在本节中，你需要实现以下结构和函数：

- `Init()`：`RecoveryManager`的初始化函数
- `RedoPhase()`:从`CheckPoint`开始，根据不同日志的类型对`KvDatabase`和活跃事务列表作出修改
- `UndoPhase()`：Undo阶段，对每个未完成的活跃事务进行回滚
- `struct LogRec`:内存日志结构。这里可以不考虑消耗内存空间的优化，实现一种能够用于所有类型日志的日志结构。
- `CreateInsertLog()`：创建一条插入日志
- `CreateDeleteLog()`：创建一条删除日志
- `CreateUpdateLog()`：创建一条更新日志
- `CreateBeginLog()`：创建一条事务开始日志
- `CreateCommitLog()`：创建一条事务提交日志
- `CreateAbortLog()`：创建一条事务回滚日志

实现完成后，你的代码需要通过`recovery_manager_test.cpp`中的测试用例。

## 6.3 模块相关代码

- `src/include/recovery/log_rec.h`
- `src/include/recovery/recovery_manager.h`
- `test/recovery/recovery_manager_test.cpp`

  

## 6.4 思考题

本模块中，为了简化实验难度，我们将Recovery Manager模块独立出来。如果不独立出来，真正做到数据库在任何时候断电都能恢复，同时支持事务的回滚，Recovery Manager应该怎样设计呢？此外，CheckPoint机制应该怎样设计呢？

注：如果完成了本模块，请在实验报告里完成思考题。思考题占本模块30%的分数，请尽量回答的详细些，比如具体到涉及哪些模块、哪些函数的改动，大致怎样改动。有能力、有时间的同学也可以挑战一下直接在代码上更改。

  

## 6.5 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为`0`；
2. 请勿将代码发布到公共Github存储库上。

   
# 7 LOCK MANAGER
## 7.1 实验概述

本次实验中，你需要实现Lock Manager模块，从而实现并发的查询，Lock Manager负责追踪发放给事务的锁，并依据隔离级别适当地授予和释放shared(共享)和exclusive(独占)锁。

**Bonus：**本模块涉及锁和条件变量，独立开发难度较大，属于Bonus。因本模块独立于验收流程外，完成的验收时会有专门针对本模块的提问环节考察理解。

## 7.2 事务管理器

数据库系统中，事务管理器（Transaction Manager）是负责处理所有与事务相关操作的组件。它是维护数据库ACID属性（原子性、一致性、隔离性、持久性）的关键组件，确保了数据库系统中的事务能够安全、一致且高效地执行。事务管理器主要负责以下几个方面：

1. **事务的边界控制**：事务管理器负责定义事务的开始（BEGIN TRANSACTION）和结束（COMMIT 或 ROLLBACK）。当事务开始时，事务管理器会为其分配所需的资源，并追踪其状态。当事务成功完成时，事务管理器会执行提交操作，将所有更改永久写入数据库。如果事务遇到错误或者需要撤销，事务管理器将执行回滚操作，撤销所有更改。
2. **并发控制**：在允许多个事务同时运行的系统中，事务管理器使用并发控制机制（如锁、时间戳、版本号lsn等）来确保事务不会相互干扰，导致数据不一致。并发控制也包括实现数据库的隔离级别，防止并发事务产生冲突。
3. **恢复管理**：事务管理器还负责实现恢复机制，以保证在系统故障（如崩溃、电源中断）后数据库的一致性和持久性。这通常通过使用日志记录（Logging）和检查点（Checkpointing）等技术来完成。事务日志存储了所有对数据库所做的更改的记录，可以用于恢复操作。
4. **故障处理**：在检测到错误或异常时，事务管理器负责采取适当的行动，例如触发回滚来撤销事务的操作，或者在某些情况下，尝试恢复事务执行。

在本次实验中，我们提供的`TxnManager`主要负责事务的边界控制、并发控制、故障处理。出于实现复杂度的考虑，同时为了避免各模块耦合太强，前面模块的问题导致后面模块完全无法完成，我们将`TxnManager`模块单独拆了出来。`TxnManager`的代码已经为大家实现好了，支持`Begin()`、`Commit()`、`Abort()`等方法。因为`TxnManager`模块独立，我们在`Commit()`、`Abort()`方法中不需要做其他事情（本来需要维护事务中的写、删除集合，结合Recovery模块回滚）。同时我们提供了`Txn`类,里面通过参数控制事务的隔离级别：

- READ_UNCOMMITED
- READ_COMMITTED
- REPEATABLE_READ

Lock Manager 负责检查事务的隔离级别，任何失败的锁操作都将导致事务中止，并同时抛出异常，此时`TxnManager`将捕获该异常并回滚。

## 7.3 锁管理器

Lock Manager的基本思想是它维护当前活动事务持有的锁。事务在访问数据项之前向 LM 发出锁请求，LM 来决定是否将锁授予该事务，或者是否阻塞该事务或中止事务。LM里定义了两个内部类：`LockRequest` and `LockRequestQueue`.

1. `LockRequest`：  
    此类代表由事务（`txn_id`）发出的锁请求。它包含以下成员：

- `txn_id_`：发出请求的事务的标识符。
- `lock_mode_`：请求的锁类型（例如，共享或排他）。
- `granted_`：已授予事务的锁类型。

构造函数使用给定的`txn_id`和`lock_mode`初始化这些成员，默认将`granted_`设置为`LockMode::kNone`。

2. `LockRequestQueue`：  
    此类管理一个锁请求队列，并提供操作它的方法。它使用一个列表（`req_list_`）存储请求，并使用一个unordered_map（`req_list_iter_map_`）跟踪列表中每个请求的迭代器。它还包括一个条件变量（`cv_`）用于同步目的，以及一些标志来管理并发访问：

- `is_writing_`：指示当前是否持有排他性写锁。
- `is_upgrading_`：指示是否正在进行锁升级。
- `sharing_cnt_`：持有共享锁的事务数量的整数计数。  
    该类提供以下方法：
- `EmplaceLockRequest()`：将新的锁请求添加到队列前端，并在map中存储其迭代器。
- `EraseLockRequest()`：根据`txn_id`从队列和map中移除锁请求。如果成功返回`true`，否则返回`false`。
- `GetLockRequestIter()`：根据`txn_id`检索队列中特定锁请求的迭代器。

在你的实现当中，整个数据库系统会存在一个全局的 LM 结构。每当一条事务需要去访问一条数据记录时，借助该全局的LM去获取数据记录上的锁。条件变量可用于阻塞等待直到它们的锁请求得到满足的事务。本次实验中，同学们实现的LM需要支持三种不同的隔离级别。

你需要修改的是`LockManager`类(`concurrency/lock_manager.cpp,concurrency/lock_manager.h`)中以下几个函数：

- `LockShared(Txn,RID)`：事务txn请求获取id为rid的数据记录上的共享锁。当请求需要等待时，该函数被阻塞（使用cv_.wait），请求通过后返回True
- `LockExclusive(Txn,RID)`: 事务txn请求获取id为rid的数据记录上的独占锁。当请求需要等待时，该函数被阻塞，请求通过后返回True
- `LockUpgrad(Txn,RID)`:事务txn请求更新id为rid的数据记录上的独占锁，当请求需要等待时，该函数被阻塞，请求通过后返回True
- `Unlock(Txn,RID)`：释放事物txn在rid数据记录上的锁。注意维护事务的状态，例如该操作中事务的状态可能会从 `GROWING` 阶段变为 `SHRINKING` 阶段（提示：查看 `transaction.h` 中的方法）。此外，当需要某种方式来通知那些等待中的事务，我们可以使用`notify_all()`方法
- `LockPrepare(Txn,RID)`：检测txn的state是否符合预期，并在`lock_table_`里创建rid和对应的队列
- `CheckAbort(Txn, LockRequestQueue)`：检查txn的state是否是abort，如果是，做出相应的操作

**Note：**

- 在锁管理器需要使用死锁检测时，我们建议首先实现一个不包含任何死锁处理的锁管理器，然后在确认其在没有死锁发生时能够正确地进行锁定和解锁后，再添加检测机制。
- 虽然通过确保严格两阶段锁（strict two phase lock）可以实现某些隔离级别，但本次实验的锁管理器实现只需确保两阶段锁的特性。严格两阶段锁的概念将通过执行器和事务管理器中的逻辑来实现。具体需要查看其中的 `Commit` 和 `Abort` 方法。
- 还需要跟踪事务所获取的共享/独占锁，使用 `shared_lock_set_` 和 `exclusive_lock_set_`，这样当 `TransactionManager` 想要提交/中止事务时，LM能够适当地释放它们。

## 7.4 死锁检测

本次实验实现的锁管理器应该在后台运行死锁检测，以中止阻塞事务。更准确地说，这意味着一个后台线程应该定期即时构建一个等待图，并打破任何循环。需要实现并用于循环检测以及测试的API如下：

- `AddEdge(txn_id_t t1, txn_id_t t2)`：在图中从t1到t2添加一条边。如果该边已存在，则无需进行任何操作。
- `RemoveEdge(txn_id_t t1, txn_id_t t2)`：从图中移除t1到t2的边。如果没有这样的边存在，则无需进行任何操作。
- `HasCycle(txn_id_t& txn_id)`：使用深度优先搜索(DFS)算法寻找循环。如果找到循环，`HasCycle`应该将循环中最早事务的id存储在`txn_id`中并返回true。该函数应该返回它找到的第一个循环。如果图中没有循环，`HasCycle`应该返回false。
- `GetEdgeList()`：返回一个元组列表，代表图中的边。一对(t1,t2)对应于从t1到t2的一条边。
- `RunCycleDetection()`：包含在后台运行循环检测的框架代码。需要在此实现循环检测逻辑。

实现完成后，你的代码需要通过`lock_manager_test.cpp`中的所有测试用例。

**Note:**

- 后台线程应该在每次唤醒时即时构建图表，而不是维护一个图表。等待图应该在每次线程唤醒时构建和销毁。
- 实验中的DFS循环检测算法**必须**是确定性的。为了做到这一点，必须始终选择首先探索最低的事务ID。这意味着在选择从哪个**未探索**的节点运行DFS时，始终选择具有最低事务ID的节点。这也意味着在探索邻居时，按从最低到最高的顺序探索它们。
- 当发现循环时，应该通过将该事务的状态设置为ABORTED来中止**最年轻**的事务以打破循环。
- 当检测线程唤醒时，它负责打破存在的**所有**循环。如果你遵循上述要求，你将总是以**确定性**的顺序找到循环。这也意味着当你构建图时，你**不应该**为已中止的事务添加节点或向已中止的事务绘制边。
- 等待图是一个**有向**图。当一个事务在等待另一个事务时，等待图会画出边。如果多个事务持有一个**共享**锁，一个单独的事务可能会等待多个事务。
- 当一个事务被中止时，确保将事务的状态设置为`ABORTED`并在您的锁管理器中抛出一个异常。事务管理器将负责明确的中止和回滚更改。一个等待锁的事务可能会被后台循环检测线程中止。您必须有一种方法通知等待的事务它们已被中止。

## 7.5 模块相关代码

- `src/include/concurrency/txn.h`
- `src/include/concurrency/txn_manager.h`
- `src/include/concurrency/lock_manager.h`
- `src/concurrency/lock_manager.cpp`
- `src/concurrency/txn_manager.cpp`
- `test/concurrency/lock_manager_test.cpp`

## 7.6 思考题

本模块中，为了简化实验难度，我们将Lock Manager模块独立出来。如果不独立出来，做到并发查询期间根据指定的隔离级别进行事务的边界控制，考虑模块3中B+树并发修改的情况，需要怎么设计？

注：如果完成了本模块，请在实验报告里完成思考题。思考题占本模块30%的分数，请尽量回答的详细些，比如具体到涉及哪些模块、哪些函数的改动，大致怎样改动。有能力、有时间的同学也可以挑战一下直接在代码上更改。

## 7.7 诚信守则

1. 请勿从其它组或在网络上找到的其它来源中复制源代码，一经发现抄袭，成绩为0；  
2. 请勿将代码发布到公共Github存储库上。  

若有收获，就点个赞吧


# 8 MiniSQL项目框架与环境配置

框架链接：[ZJU GitLab链接，请使用内网访问](https://git.zju.edu.cn/zjucsdb/minisql)

## #0 框架维护日志（更新中）

  

## #1 代码框架介绍

本实验基于CMU-15445 BusTub框架，并做了一些修改和扩展。注意：为了避免代码抄袭，请不要将自己的代码发布到任何公共平台中。

### #1.1 编译&开发环境

- `apple clang`: 11.0+ (MacOS)，使用`gcc --version`和`g++ --version`查看
- `gcc`&`g++` : 8.0+ (Linux)，使用`gcc --version`和`g++ --version`查看
- `cmake`: 3.16+ (Both)，使用`cmake --version`查看
- `gdb`: 7.0+ (Optional)，使用`gdb --version`查看
- `flex`& `bison`(暂时不需要安装，但如果需要对SQL编译器的语法进行修改，需要安装）

### #1.2 构建

#### #1.2.1 Windows

目前该代码暂不支持在Windows平台上的编译。但在Win10及以上的系统中，可以通过安装WSL（Windows的Linux子系统）来进行开发和构建。WSL请选择Ubuntu子系统（推荐Ubuntu20及以上）。如果你使用Clion作为IDE，可以在Clion中配置WSL从而进行调试，具体请参考链接[Clion with WSL](https://blog.jetbrains.com/clion/2018/01/clion-and-linux-toolchain-on-windows-are-now-friends/)。

#### #1.2.2 MacOS & Linux & WSL

基本构建命令

```
mkdir build
cd build
cmake ..
make -j
```

若不涉及到`CMakeLists`相关文件的变动且没有新增或删除`.cpp`代码（通俗来说，就是只是对现有代码做了修改）则无需重新执行`cmake..`命令，直接执行`make -j`编译即可。默认以`debug`模式进行编译，如果你需要使用`release`模式进行编译：

```
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### #1.3 测试

在构建后，默认会在`build/test`目录下生成`minisql_test`的可执行文件，通过`./minisql_test`即可运行所有测试。如果需要运行单个测试，例如，想要运行`lru_replacer_test.cpp`对应的测试文件，可以通过`make lru_replacer_test`命令进行构建。

### #1.4 工程目录

- `src`：与MiniSQL工程相关的头文件和源代码。`src/include`中为MiniSQL各个子模块的头文件，`src/buffer`、`src/record`、`src/index`、`src/catalog`等目录为MiniSQL各个子模块的源代码。
- `test`：与测试用例相关的源代码和头文件。
- `thirdparty`：第三方库，包括日志模块`glog`和测试模块`gtest`。

## #2 使用WSL-Ubuntu进行开发

**Note：Win10系统**下，参考[Win10系统安装WSL教程](https://www.cnblogs.com/jetttang/p/8186315.html)安装WSL，选择Ubuntu子系统即可，推荐选用Ubuntu 20.04以上的版本（示例使用的是20.04版本）。

### #2.1 配置编译环境

首次安装时，请使用`sudo apt-get update`更新软件源。然后使用命令`sudo apt install gcc g++ cmake gdb`安装编译和调试环境。

**Note:** 安装时一般都会提示是否需要安装，输入`y`回车即可：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649734231138-2ac9a4e8-352c-4fa1-8c7e-465f7be1c26b.png)

安装完成后，通过`--version`查看是否安装完成，如下图所示：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649606813086-3c0fb1cf-e1c7-4ce9-ab75-29a61c2126b0.png)

进入一个目录，从远程仓库中克隆代码到该目录下（建议先Fork到自己小组的私有仓库然后再进行克隆），在这里我选择将代码克隆到`/mnt/f`目录下（当然这个目录可以根据你的需要自由选择），`/mnt/f`目录实际上就是我们电脑本地磁盘的`F`盘：

```
cd /mnt/f
git clone https://git.zju.edu.cn/zjucsdb/minisql.git
```

**Note：**如果在克隆过程中提示`Permission Denied`，请在命令前面加上`sudo`以执行：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649607091757-ea7b8144-fd1d-42ea-a378-cebc3b3da1a4.png)

然后进入目录，进行构建或测试：

```
# 进入目录
cd /mnt/f/minisql
# 建立并进入build目录
mkdir build
cd build
# 生成Makefile
sudo cmake ..
# 多线程编译生成可执行文件, -j可以指定具体的线程数, 如-j4就是使用4线程编译
make -j
```

`cmake`构建成功后如下图所示：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649607637301-ab43428d-f6c0-409b-ab2e-9623a307752b.png)

`make`构建成功后如下图所示：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649607677478-f8b61ea0-d824-44f4-9298-238e5e4883ea.png)

编译生成的可执行文件位于`bin/`和`test/`（测试相关文件）下：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649607711368-2585b025-e20f-429f-ad3f-7a7457386d3b.png)

最终整个MiniSQL的主程序在这里：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649607810941-cee25b9d-3eae-4513-90df-3678dd4e1e1b.png?x-oss-process=image%2Fcrop%2Cx_0%2Cy_25%2Cw_553%2Ch_60)

一个运行测试用例的例子：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649607884595-e20b0de0-4b57-46d8-a360-6ab46123b461.png)

**Note：**此处运行测试遇到`Failed`是正常现象。

### #2.2 使用Clion连接WSL进行开发

**Note：**[Clion下载网址](https://www.jetbrains.com/zh-cn/clion/download/#section=windows)，开始时有30天的免费试用期。在使用ZJU的邮箱进行认证后可以一直免费使用。

打开Clion后，导入MiniSQL项目：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649688060956-bbd94934-ccef-4ecd-96bf-e9fec4886bce.png)

File-->Settings-->Build-->Toolchains 添加WSL相关设置：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649688297273-196057cb-7485-4a0b-aff5-6d152d0a6cc4.png)

File-->Settings-->Build-->CMake中添加CMake相关设置：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649688352706-e6f35b10-19d2-4509-93f7-795116f2257e.png)

保存后会自动运行CMake命令，或是通过下图左边刷新按钮运行。CMake构建成功后如下图所示：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649693620442-a32ec54a-7f9d-4f2f-a860-9a1379aff9db.png)

接下来就可以使用Clion进行开发和使用。

### #2.3 使用VSCode连接WSL进行开发

**Note：**VSCode需要事先在扩展`Ctrl+Shift+X`中安装以下插件：

- Remote-WSL（在本地安装）
- C/C++ （连接上WSL后再安装，安装在WSL）
- CMake Tools（连接上WSL后再安装，安装在WSL）

然后使用`Ctrl+Shift+P`打开选项卡输入`WSL`，选择`Remote-WSL:New Window`即可打开WSL。可以看到，左下角已连接的Linux子系统WSL:Ubuntu-20.04。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694193613-966cbf68-e467-463e-becb-b11bfd22740b.png)

然后打开源代码所在目录：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694237210-3c2b7d69-8d95-46a6-89e8-bb8cd9508d51.png)![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694265620-ff21f138-b6aa-4205-9da8-453f7cc008e3.png)

在WSL中安装C/C++和CMake插件：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694350521-d5cfde6a-2089-481c-90ba-51032f9cd4ec.png)

安装成功后可以看到下面的工具，对CMake进行Configure：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694385927-7243bae1-9e27-403a-8bb3-2f2cc6a91b2a.png)

选择WSL中的编译器：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694412540-2effa575-0a58-4b0c-be50-c0a37c58f79e.png)

构建完成后：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649694491890-20c9bb3a-8c4b-4180-a243-6b2b70d06a3c.png)

点击`Build`即可生成可执行文件。

## #3 使用MacOS进行开发

MacOS中一般自带Apple Clang，可以使用`g++ --version`和`gcc --version`查看，`cmake`和`gdb`可以通过`brew`命令安装（或是从官网上下载然后添加到环境变量中）。在MacOS中可以使用Clion和VS Code直接在本地进行开发调试，方法与#2中提到的类似。

## #4 远程连接服务器进行开发

同学们可以根据自己服务器选择合适的Linux镜像，推荐选用Ubuntu 20.04+或是CentOS 7.2+的镜像。若选用Ubuntu的镜像，请参考#2中的教程进行编译环境的配置，然后参考#4.2连接远程服务器进行开发调试。**在本节的示例中，服务器镜像选用的是CentOS 7.2**。

  

**Note:** 由于外网服务器正常情况下无法访问位于学校内网的`ZJU GitLab`，一个可行的解决办法是，先从`ZJU GitLab`上克隆源代码到本地，然后将代码推送到自己小组**私有**的远程`GitHub`仓库中，这样外网服务器就可以通过`GitHub`存储库访问到代码。另外一种可行的方法是，通过`scp`命令将源代码直接上传到远程服务器中，然后在远程服务器中新建`Git`仓库。

### #4.1 配置编译环境

本节以**CentOS 7.2镜像**进行示例。对于其它镜像，配置编译环境的方法类似，可以自行网上搜索在该类型的系统镜像中如何安装`GCC`、`G++`、`CMake`和`GDB`。

服务器镜像中如果自带`GCC`、`G++`、`CMake`和`GDB`，但版本较低的（如下图中`GCC`的版本是4.8.5），则需要对相应的软件进行升级（具体升级教程可上网查找）。

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649733795952-8009bff7-e347-4929-8f48-1566118df0db.png)

CentOS 7.2镜像中自带了`GCC`（但未自带`G++`），在这里简单叙述一下CentOS 7.2升级`GCC`、`G++`的方法，升级方法参考[引用链接](https://www.cnblogs.com/jixiaohua/p/11732225.html)，命令如下：

```
# 安装centos-release-scl
sudo yum install centos-release-scl
# 安装devtoolset
sudo yum install devtoolset-9-gcc*
sudo yum install devtoolset-9-g++*
# 替换旧的gcc和g++
mv /usr/bin/gcc /usr/bin/gcc-4.8.5
ln -s /opt/rh/devtoolset-9/root/bin/gcc /usr/bin/gcc
mv /usr/bin/g++ /usr/bin/g++-4.8.5	# Note: 如果CentOS中没有自带g++, 
                                    # 即g++ --version提示命令不存在，
                                    # 则不需要执行该步命令，只需要执行下面的ln即可。
ln -s /opt/rh/devtoolset-9/root/bin/g++ /usr/bin/g++
```

**Note:** 安装时一般都会提示是否需要安装，输入`y`回车即可：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649734231138-2ac9a4e8-352c-4fa1-8c7e-465f7be1c26b.png)

升级完成后，查看`GCC`和`G++`是否正确安装：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649734684334-99c7962a-4df4-4847-af06-ef64a5aee7db.png)

由于CentOS 7.2中通过`yum`源安装的`CMake`版本较老（2.X版本），因此需要从官网下载，下载链接：[CMake Download](https://cmake.org/download/)，根据不同的CPU架构，选择不同的链接下载，上面的是X86架构，下面的是ARM架构：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649735401854-b286c500-0b78-4de8-81f9-897038a849e0.png)

然后通过`wget`命令进行下载：

```
# X86架构
wget https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0-linux-x86_64.tar.gz
# ARM架构
wget https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0-linux-aarch64.tar.gz
```

下载完成后：

```
# 解压压缩包
tar -xzvf cmake-3.23.0-linux-aarch64.tar.gz	#ARM架构下使用该命令
tar -xzvf cmake-3.23.0-rc2-linux-x86_64.tar.gz #X86架构下使用该命令
# 重命名
mv cmake-3.23.0-linux-aarch64 cmake	#ARM架构下使用该命令
mv cmake-3.23.0-rc2-linux-x86_64 cmake #X86架构下使用该命令
# 移动 & 链接
mv cmake /usr/local/
ln -s /usr/local/cmake/bin/cmake /usr/bin/cmake
```

使用`cmake --version`即可查看`CMake`是否安装成功：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649741674198-4117a3ca-5468-482b-9618-f2a12d9ab362.png)

调试工具`GDB`使用`sudo yum install gdb`命令直接安装即可，然后使用`gdb --version`查看是否安装成功：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649741751349-e61be3de-1071-4fda-aebe-d117a1ea06d8.png)

### #4.2 使用VSCode连接进行开发

**Note：**VSCode需要事先在扩展`Ctrl+Shift+X`中安装以下插件：

- Remote-SSH（在本地安装）
- C/C++ （连接服务器后再安装，安装在服务器）
- CMake Tools（连接服务器后再安装，安装在服务器）

安装Remote-SSH扩展后，点击“+”号新建连接：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649741925166-ad7c054c-8f76-4457-9b77-e10c9101fe3e.png)

在弹框中输入`ssh <USERNAME>@<IP ADDRESS>`

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649741968901-8a6c483a-71a1-4fe4-808d-dbefc14ed690.png)

选择任意一个配置文件保存，通常是选上面那个，保存到用户下的配置文件：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649742048195-63c982ae-0288-47cb-885b-ba9fadba20b1.png)

在SSH TARGETS中选择刚刚添加的服务器，连接：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649742092198-03a9a532-f41c-467a-8d41-77f0c7a241fe.png)

输入密码后回车：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649742131772-4b50dfc1-52a3-4f69-989f-c0aada51815d.png)

打开你存放MiniSQL代码的文件夹：

![](https://cdn.nlark.com/yuque/0/2022/png/25540491/1649742162429-58f7744a-be8c-44bf-9f45-9106824adbb5.png)

后续操作方法与#2.3中类似，这里不做赘述。


  
# 9 MiniSQL项目验收流程
### 项目验收流程

1. 进行基本操作演示；
2. 简要介绍整个系统的设计思路，若实现了额外的功能或是性能优化可以一并介绍，介绍的时候最好能够体现出整体系统设计的亮点之处；
3. 模块提问抽查，针对一些实现细节做提问，考察项目是否是由小组独立完成的**（可能会问得非常深入，请做好充分准备）**

### 基本操作示例

1. 创建数据库`db0`、`db1`、`db2`，并列出所有的数据库
2. 在`db0`数据库上创建数据表`account`，表的定义如下：

```
create table account(
  id int, 
  name char(16) unique, 
  balance float, 
  primary key(id)
);

-- Note: 在实现中自动为UNIQUE列建立B+树索引的情况下，
--       这里的NAME列不加UNIQUE约束，UNIQUE约束将另行考察。
--			（NAME列创建索引的时候，不需要限制只有UNIQUE列才能建立索引）
```

3. 考察SQL执行以及数据插入操作：

4. 执行数据库文件`sql.txt`，向表中插入![](https://cdn.nlark.com/yuque/__latex/d3df89690bb9f1eb2c7e46882ad9383f.svg)条记录（分![](https://cdn.nlark.com/yuque/__latex/134c802fc5f0924cf1ea838feeca6c5e.svg)次插入，每次插入![](https://cdn.nlark.com/yuque/__latex/db65a9f55418ccd1c30293cc83355e67.svg)条，至少插入![](https://cdn.nlark.com/yuque/__latex/30ec88098f5570d8cb5dddda05629301.svg)条）

5. 参考SQL数据，由脚本自动生成：[📎验收数据.zip](https://www.yuque.com/attachments/yuque/0/2023/zip/29437275/1686492221764-b7ba2711-03b6-4a69-882e-de26d227ce9b.zip)
6. 批量执行时，所有sql执行完显示总的执行时间

7. 执行全表扫描`select * from account`，验证插入的数据是否正确（要求输出查询到![](https://cdn.nlark.com/yuque/__latex/d3df89690bb9f1eb2c7e46882ad9383f.svg)条记录）

8. 考察点查询操作：

9. `select * from account where id = ?`
10. `select * from account where balance = ?`
11. `select * from account where name = "name56789"`，此处记录执行时间![](https://cdn.nlark.com/yuque/__latex/a67f549adc92ae7ed58082ebbbc38d50.svg)
12. `select * from account where id <> ?`
13. `select * from account where balance <> ?`
14. `select * from account where name <> ?`

15. 考察多条件查询与投影操作：

16. `select id, name from account where balance >= ? and balance < ?`
17. `select name, balance from account where balance > ? and id <= ?`
18. `select * from account where id < 12515000 and name > "name14500"`
19. `select * from account where id < 12500200 and name < "name00100"`，此处记录执行时间![](https://cdn.nlark.com/yuque/__latex/fc76f3e57849e54734eb2c56803d9401.svg)

20. 考察唯一约束：

21. `insert into account values(?, ?, ?)`，提示PRIMARY KEY约束冲突或UNIQUE约束冲突

22. 考察索引的创建删除操作、记录的删除操作以及索引的效果：

23. `create index idx01 on account(name)`
24. `select * from account where name = "name56789"`，此处记录执行时间![](https://cdn.nlark.com/yuque/__latex/e8227c1365280f1b3384459eaa53daea.svg)，要求![](https://cdn.nlark.com/yuque/__latex/88a2f2d648d2bbbe372366c229580fe6.svg)
25. `select * from account where name = "name45678"`，此处记录执行时间![](https://cdn.nlark.com/yuque/__latex/72ac33d0d9858af251b540a40e4a071a.svg)
26. `select * from account where id < 12500200 and name < "name00100"`，此处记录执行时间![](https://cdn.nlark.com/yuque/__latex/c7f5ac7f2e1f9d307db65fab3b8bb664.svg)，比较![](https://cdn.nlark.com/yuque/__latex/fc76f3e57849e54734eb2c56803d9401.svg)和![](https://cdn.nlark.com/yuque/__latex/c7f5ac7f2e1f9d307db65fab3b8bb664.svg)
27. `delete from account where name = "name45678"`
28. `insert into account values(?, "name45678", ?)`
29. `drop index idx01`
30. 执行(c)的语句，此处记录执行时间![](https://cdn.nlark.com/yuque/__latex/6aea8b94ebb6edd43f2e9cd6705a1838.svg)，要求![](https://cdn.nlark.com/yuque/__latex/cc68fa880d9e06f3811b775bf6e4e6ce.svg)

31. 考察更新操作：

32. `update account set id = ?, balance = ? where name = "name56789";`

并通过`select`操作验证记录被更新

9. 考察删除操作：

10. `delete from account where balance = ?`，并通过`select`操作验证记录被删除
11. `delete from account`，并通过`select`操作验证全表被删除
12. `drop table account`，并通过`show tables`验证该表被删除

# 10 Q&A&BugFix
`RELEASE`模式下，`ASSERT`语句会被优化，从而不被执行。在小规模数据测试时，为了确保`ASSERT`能够被正常运行起到检测作用，请使用`DEBUG`模式进行。`RELEASE`模式下`DBStorageEngine`类的Bug修复详见：[BugFix](https://git.zju.edu.cn/zjucsdb/minisql/-/merge_requests/3)。该Bug会导致`DBStorageEngine`时预分配`CATALOG_META_PAGE`以及`INDEX_ROOTS_PAGE`这两个数据页的过程被跳过。

相较于去年，minisql做出了如下更新：

1. 增加了Recovery Manager和Lock Manager两个模块
2. 修复了去年实验过程中发现的若干bug

## #10.0 START

1. 在编译时，碰到了包含`'march=native'`相关的错误时，只需在`CMakeList.txt`文件中搜索`'march=native'`，将其去掉即可。
2. 运行单个测试时，出现`IMPORTANCE NOTICE: This test program did not call testing::InitGoogleTest() before calling RUN_ALL_TESTS(). This is INVALID.`类似的错误，重新cmake一下，再编译单个测试文件应该就能够解决。如果仍未解决，目前可以暂时使用运行所有测试来代替。运行所有测试时，可以通过指定`Filter`（在`main_test.cpp`中注释的那行）来选择需要运行的测试用例。
3. 很多同学在用GDB调试时发现调试信息很少，大概率是没有用`DEBUG`模式。在执行`cmake`命令时，指定`-DBUILD_TYPE=Debug`再重新编译即可。
4. 编译时遇到如下类似错误，是因为环境中已经安装了GTest，与ThirdPart中的GTest产生冲突。如果将报错的GTestTargets.cmake文件暂时移走或删除还不能解决问题，有以下解法：

5. 暂时移除anaconda的环境变量，或者删除anaconda里面的gtest
6. 寻找debug生成文件里面的CMakeCache.txt，检查里面和glog以及gflag相关的变量。如果如下图所示，有变量仍然链接到anaconda里面的gflags，则在cmake的时候增加编译选项`-Dgflags_DIR=/opt/homebrew/lib/cmake/gflags`，其中路径替换成自己安装的gflags路径。![](https://cdn.nlark.com/yuque/0/2024/png/29437275/1711010281092-51772079-8080-4a37-a566-cd5aca3ac669.png)![](https://cdn.nlark.com/yuque/0/2024/png/29437275/1716347436589-80e4d7a9-f0a5-469b-bcf8-2a0ebc1db6c4.png)

## #10.3 INDEX

1. `BPlusTree::FindLeafPage`函数中，参数`page_id`的含义是什么？

是以page_id的那一页为b+树的初始节点来找leafpage。如果page_id == INVALID_PAGE_ID，那么从根节点开始查找。

2. `InternalPage::MoveAllTo`函数中，这个函数的意思是什么？参数`middle_key`的含义是什么？

这个函数是在合并的时候使用，把右节点合并到左节点。因为在右节点里，最左边value的key值是存在parent里的，所以要从parent获取。这个middle_key即是最左边value的key值。

## #10.4 CATALOG

1. `IndexMetadata::Create`函数中，参数`key_map`和`CatalogManager::CreateIndex`函数中，参数`index_keys`的含义分别是？

PS：善用全局搜索，用全局搜索一下这些变量在哪里被用到可以更好地帮助理解这个变量的作用。

对于前者，通过全局搜索引用，可以发现它在`Schema::ShallowCopySchema`中有被用到，且该函数的注释中给出了一个例子。实际上`key_map`可以简单地理解成，索引键分别位于元组中的哪几列（即它们在元组中的下标）。对于后者，表示创建索引时，哪几列（列名，字符串类型）是需要作为索引键的。

2. `TableMetadata`类中，`root_page_id_`指的是`TableHeap`的`root_page_id_`
3. `CatalogTableTest`中，`schema`采用`make_shared<Schema>`方式建立，发生了重复析构的问题，这是怎么回事呢？

这是因为CreateTable函数中，没有采用正确的方式拷贝schema。建议看一下DeepCopySchema和ShallowCopySchema函数，并选择正确的拷贝方式。

## #10.5 EXECUTOR

1. 对于`PRIMARY KEY`的列如何进行区分？

对于PK列，可以在表创建的时候，将PK列的信息写入`TableMetaData`中，此外，在`TableInfo`对象中，也可以增加有关PK列的属性，以在内存中维护PK信息。

2. 语法树可视化时，生成的`DOT`文件在哪里？

正常情况下，是在`build`或`build/bin`或者`build/test`下面，可以通过`find`命令查找。

3. 事务`Transaction`模块需要实现吗？

今年不需要实现，因为没有确定好事务的框架。但是明年估计就要了（所以说好好学，争取不重修）。

4. `ExecuteContext`这个类有什么用呢？

用于向上层传递一些信息，保留一些上下文的状态。但如果你在执行层直接打印结果的话，那么这个类其实就不太用得到，忽略即可。

5. `drop index`语句目前只给出了`index name`而没有给出`table name`（设计上的问题），这里可以通过不考虑重复`index name`的问题或是对所有`table`的`index`进行搜索来解决。
6. `create index`语句使用`using`时，`using`关键字的语法树结点在打印时会出现`error type`（缺少`using`关键字的语法树结点定义）。如果没有选做另外的索引类型，可以直接忽略这个问题。如果选做了另外的索引类型，那么可以暂且忽略`using`关键字的语法树结点的类型，在其子结点中获取索引类型。