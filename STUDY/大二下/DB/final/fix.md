好问题——这正是 ER 建模里一个容易纠结的点。先回答 email,再看你第二大题。

**为什么 author 已有 email,write 还要 email**

你说得没错:author 是 user,profile 里确实有 email。关键在题目这句的主语:

> "Every author **of a paper submission** must contain rank, affiliation, and email."

主语是"某篇投稿里的某个作者",也就是 **write 这个联系(author×paper 的配对)**,不是 user 本身。再看同一句里并列的另外两个属性:

- **rank**:作者在**这篇论文**上的排序/位置(第一作者、通讯作者),显然是 per-论文的。
- **affiliation**:作者**在这篇论文上署名的单位**,可能这篇署 A 校、下篇署 B 校(换工作了),也是 per-论文的。
- **email** 和它俩并列写在一起,所以读作"作者**在这篇投稿上留的联系邮箱**",同样 per-论文。

所以 `user.email`(账号登录邮箱)和 `write.email`(这篇稿子上这位作者留的联系邮箱)是**两个不同的东西**,只是都叫 email,不算冗余。类比:你往会议投稿时,要给每位共同作者各填一遍 姓名/单位/邮箱,那个邮箱是跟着这篇稿子走的。

**不过**,这确实是个判断题。用规范化的话说:如果你认为"同一作者在任何论文上邮箱都一样"(email 只由 ID 决定,跟 paper 无关),那它就该留在 user、不该放进 write(否则是部分依赖、冗余)——这时你的直觉就对了。如果认为同一作者不同论文可以留不同邮箱(email 由 (ID, paper) 共同决定),才放进 write。题目的措辞偏向后者,所以我建议**跟题面把 email 放在 write**,但你写成只放 user.email、write 里不放,也能自圆其说。看你们老师讲义里更强调哪种。

---

**你第二大题点评**

**(1) ✓ 正确。** `>=` 配 `max` 没问题(只有等于最大值的行能满足),用 `=` 读起来更直接而已。可以顺手把 title 也 select 出来。

**(2) ✓ 正确,而且用 VIEW 这招很漂亮。** 你划掉的那版 `HAVING sum(credits) >= (SELECT max(sum(credit)))` 之所以不行,是因为标准 SQL 不允许 `max(sum(...))` 这样嵌套聚集——所以改成视图是对的判断。

**(3) ✗ 思路对,但把两种写法混在一起了,而且子句顺序错。** 两个问题:① `GROUP BY` 必须放在 `WHERE` **之后**(你放到 FROM 和 WHERE 中间了);② 一旦 `GROUP BY title`,就不能再 `SELECT A.course-id`(它既没在 GROUP BY 里也没被聚集)。自连接和分组,二选一就行:

自连接版(最接近你写的):

```sql
SELECT A.course-id, B.course-id, A.title
FROM   course A, course B
WHERE  A.course-id < B.course-id AND A.title = B.title;
-- 用 < 而不是 <>，否则同一对会出现两次(A,B 和 B,A)
```

或者分组版(更简洁):

```sql
SELECT title
FROM   course
GROUP BY title
HAVING COUNT(*) > 1;
```

**(4) ✗ 方向反了。** `course natural join prereq` 是按 **course-id** 连的,所以 `GROUP BY course-id, count(prereq-id)` 数的是"**每门课有几门先修课**",而题目要的是"**每门课被多少门课当作先修课**"。"被当作先修课"的那门,在 `prereq` 表里是 **prereq-id** 这一列,所以要按它分组;另外只 `SELECT count(...)` 没把分组键带出来,看不出是哪门课:

```sql
SELECT prereq-id, COUNT(*) AS times
FROM   prereq
GROUP BY prereq-id;
```

如果想把"从没被当过先修课"的课程(0 次)也列进来:

```sql
SELECT c.course-id, COUNT(p.prereq-id) AS times
FROM   course c LEFT OUTER JOIN prereq p ON c.course-id = p.prereq-id
GROUP BY c.course-id;
```

小结:(1)(2) 没问题,(3) 把多余的 `GROUP BY` 去掉就好,(4) 把分组键从 `course-id` 改成 `prereq-id`。这两处都是很典型的考点,值得记一下。