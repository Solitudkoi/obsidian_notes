### Natural Join[¶](https://note.hobbitqia.cc/DB/db3/#natural-join "Permanent link")

**_e.g._** `select * from instructor natural join teaches;`

**_e.g._** `select name, course_id from instructor, teaches where instructor.ID = teaches.ID;`  
`select name, course_id from instructor natural join teaches;`  
上面两条语句是等价的。

**Unrelated attributes with same name** which get equated incorrectly!

Example

`course(course_id,title, dept_name,credits）`, `teaches(ID, course_id,sec_id,semester, year)`, `instructor(ID, name, dept_name,salary）`这里的 department 含义各有不同，不能直接自然连接。  
可以写成 `select name, title from (instructor natural join teaches）join course using(course_id);` 即规定连接的属性，对应于 

Find students who takes courses across his/her department.  
可写作

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-1-1)select distinct student.id [](https://note.hobbitqia.cc/DB/db3/#__codelineno-1-2)    from (student natural join takes)  [](https://note.hobbitqia.cc/DB/db3/#__codelineno-1-3)           join course using (course_id）  [](https://note.hobbitqia.cc/DB/db3/#__codelineno-1-4)    where student.dept_name <> course.dept_name`

### The Rename Operation[¶](https://note.hobbitqia.cc/DB/db3/#the-rename-operation "Permanent link")

The SQL allows renaming relations and attributes using the **`as`** clause.  
`old-name as new-name`

**_e.g._** `select distinct T. name from instructor as T, instructor as S where T.salary > S.salary and S.dept_name = ‘Comp. Sci.’`

- Keyword **`as`** is optional and may be omitted.

### String Operations[¶](https://note.hobbitqia.cc/DB/db3/#string-operations "Permanent link")

SQL includes a **string-matching operator** for comparisons on character strings. The operator **`like`** uses patterns that are described using two special characters.

注意单引号表示字符串。

- **percent (%)**. The % character matches any substring.  
    **_e.g._** `select name from instructor where name like '%dar%';` 找名字里面含有 `dar` 的字符串。
- **underscore (_)**. The _ character matches any character.

Match the string

匹配字符串 `'100 %'` 但是 `%` 符号被我们作为了通配符，这里我们需要用到转义符`\` . `\%`即将 `%` 作为正常字符匹配。  
`\` 也可以是一个基本符号，我们需要在后面写出 `escape` 表示其在这里作为转义符。类似地我们还可以将转义符定义为 `#`.

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-2-1)like ‘100 \%'  escape  '\'  [](https://note.hobbitqia.cc/DB/db3/#__codelineno-2-2)like ‘100 \%'   [](https://note.hobbitqia.cc/DB/db3/#__codelineno-2-3)like ‘100  #%'  escape  ‘#'` 

SQL supports a variety of string operations such as

- concatenation (using `||`)
- converting from upper to lower case (and vice versa)
- finding string length, extracting substrings, etc.

### Ordering the Display of Tuples[¶](https://note.hobbitqia.cc/DB/db3/#ordering-the-display-of-tuples "Permanent link")

关系是无序的，但我们可能规定显示出来的顺序。

- We may specify desc for _descending_ order or asc for _ascending_ order, for each attribute; ascending order is the default.  
    **_e.g._** `order by name desc`  
    可以排序的类型，如字符串、数字。
- Can sort on multiple attributes  
    **_e.g._** `order by dept_name, name` 先按第一个排，如果第一个元素相同再按第二个排。

### The `limit` Clause[¶](https://note.hobbitqia.cc/DB/db3/#the-limit-clause "Permanent link")

The **`limit`** clause can be used to constrain the number of rows returned by the select statement.  
limit clause takes one or two numeric arguments, which must both be nonnegative integer constants:

- `limit offset, row_count`
- `limit row_count == limit 0, row_count`

**_e.g._** `select name from instructor order by salary desc limit 3； // limit 0,3`

### Set Operations[¶](https://note.hobbitqia.cc/DB/db3/#set-operations "Permanent link")

- `union, intersect, except` 是严格的集合操作，会对结果去重 .
- `union all`, `intersect all`and `except all`. 保持多重集。

Example

![](http://cdn.hobbitqia.cc/202303131639119.png)

### Null Values[¶](https://note.hobbitqia.cc/DB/db3/#null-values "Permanent link")

**null** signifies _an unknown value_ or that _a value does not exist_.

- The result of any arithmetic expression involving null is null.  
    **_e.g._** `5 + null` returns null
- The predicate is null can be used to check for null values.  
    **_e.g._** Find all instructors whose salary is null.  
    `select name from instructor where salary is null`
- Comparisons with null values return the special truth value: **unknown**.

![](http://cdn.hobbitqia.cc/202303131118989.png)

- Result of select predicate is treated as false if it evaluates to unknown

### Aggregate Functions[¶](https://note.hobbitqia.cc/DB/db3/#aggregate-functions "Permanent link")

Example

![](http://cdn.hobbitqia.cc/202303131120904.png)

注意在 `select` 里出现的属性，除了统计函数以外，一定要是分组属性里出现过的。

#### Having Clause[¶](https://note.hobbitqia.cc/DB/db3/#having-clause "Permanent link")

对分组后的组进行筛选。

**_e.g._**

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-3-1)select dept_name, count (*) as cnt [](https://note.hobbitqia.cc/DB/db3/#__codelineno-3-2)from instructor [](https://note.hobbitqia.cc/DB/db3/#__codelineno-3-3)where  salary >=100000 [](https://note.hobbitqia.cc/DB/db3/#__codelineno-3-4)group by dept_name [](https://note.hobbitqia.cc/DB/db3/#__codelineno-3-5)having  count (*) > 10 [](https://note.hobbitqia.cc/DB/db3/#__codelineno-3-6)order by cnt;`

predicates in the **`having`** clause are applied _after the formation of groups_ whereas predicates in the **`where`** clause are applied _before forming groups_.

#### Null Values and Aggregates[¶](https://note.hobbitqia.cc/DB/db3/#null-values-and-aggregates "Permanent link")

`select sum (salary) from instructor`

- Above statement ignores null amounts
- Result is null if there is no non-null amount
- All aggregate operations except `count(*)` ignore tuples with null values on the aggregated attributes

Arithmetric expression with Aggregate functions

![](http://cdn.hobbitqia.cc/202303131130602.png)

第二个表示重名率小于千分之一的系。

### Nested Subqueries[¶](https://note.hobbitqia.cc/DB/db3/#nested-subqueries "Permanent link")

A **subquery** is a **select-from-where** expression that is nested within another query.

#### Set Membership[¶](https://note.hobbitqia.cc/DB/db3/#set-membership "Permanent link")

`in, not in`

Example

![](http://cdn.hobbitqia.cc/202303131147512.png)

除了单个元素外，元组也可以使用 `in, not in`

#### Set Comparison[¶](https://note.hobbitqia.cc/DB/db3/#set-comparison "Permanent link")

- `some` 某些成员
- `all` 所有成员

Example

工资大于生物系中的某些老师的老师 .  

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-4-1)select name [](https://note.hobbitqia.cc/DB/db3/#__codelineno-4-2)from instructor [](https://note.hobbitqia.cc/DB/db3/#__codelineno-4-3)where salary > some (select salary [](https://note.hobbitqia.cc/DB/db3/#__codelineno-4-4)                                    from instructor [](https://note.hobbitqia.cc/DB/db3/#__codelineno-4-5)                                    where dept_name = ’Biology’);`

#### Scalar Subquery[¶](https://note.hobbitqia.cc/DB/db3/#scalar-subquery "Permanent link")

**Scalar ( 标量 ) subquery** is one which is used where a single value is expected.

Example

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-5-1)select name [](https://note.hobbitqia.cc/DB/db3/#__codelineno-5-2)from instructor [](https://note.hobbitqia.cc/DB/db3/#__codelineno-5-3)where  salary * 10 >  [](https://note.hobbitqia.cc/DB/db3/#__codelineno-5-4)    (select budget  from department  [](https://note.hobbitqia.cc/DB/db3/#__codelineno-5-5)    where department.dept_name = instructor.dept_name)`

这里 `dept_name` 是这个表的主键，只返回一个元组，这种情况下是可以不用 `some, all` 的。

Runtime error if subquery returns more than one result tuple.

#### Test for Empty Relations[¶](https://note.hobbitqia.cc/DB/db3/#test-for-empty-relations "Permanent link")

The exists construct returns the value true if the argument subquery is nonempty.

- `exists r` 
- `not exists r` 

Example

09FALL 开的课，而且 10SPRING 也开了课。

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-6-1)select course_id [](https://note.hobbitqia.cc/DB/db3/#__codelineno-6-2)from section as S [](https://note.hobbitqia.cc/DB/db3/#__codelineno-6-3)where semester = ’Fall’ and year= 2009 and                exists (select *                             [](https://note.hobbitqia.cc/DB/db3/#__codelineno-6-4)from section as T                       [](https://note.hobbitqia.cc/DB/db3/#__codelineno-6-5)    where semester = ’Spring’ and year= 2010 and S.course_id= T.course_id);`

Example

Find all students who have taken all courses offered in the Biology department.  
SQL 语句往往需要逆向考虑，即找到这样的学生，不存在他没选过的生物系的课。  

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-1)select distinct S.ID, S.name [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-2)from student as S [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-3)where not exists ( (select course_id [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-4)                        from course [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-5)                        where dept_name = ’Biology’) [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-6)                except [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-7)                    (select T.course_id [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-8)                        from takes as T [](https://note.hobbitqia.cc/DB/db3/#__codelineno-7-9)                        where S.ID = T.ID));`

#### Test for Absence of Duplicate Tuples[¶](https://note.hobbitqia.cc/DB/db3/#test-for-absence-of-duplicate-tuples "Permanent link")

The **`unique`** construct tests whether a subquery has any duplicate tuples in its result.  
验证是否是一个集合，而非多重集。

- Evaluates to “true” on an empty set.  
    可以将 unique 理解为 at most once.

Example

![](http://cdn.hobbitqia.cc/202303131208597.png)

如果不加 exist, 可能有没开过的课也被算进去。我们这里求得是恰好只开过一次的。

Example

![](http://cdn.hobbitqia.cc/202303131210226.png)

也可以用 `group by count(*) > 1` 实现。

### With Clause[¶](https://note.hobbitqia.cc/DB/db3/#with-clause "Permanent link")

The **`with`** clause provides a way of defining a temporary view whose definition is available only to the query in which the with clause occurs.  
构造一个临时表

Example

Find all departments with the maximum budget.

`[](https://note.hobbitqia.cc/DB/db3/#__codelineno-8-1)with max_budget (value) as  [](https://note.hobbitqia.cc/DB/db3/#__codelineno-8-2)    (select max(budget) [](https://note.hobbitqia.cc/DB/db3/#__codelineno-8-3)    from department) [](https://note.hobbitqia.cc/DB/db3/#__codelineno-8-4)select dept_name [](https://note.hobbitqia.cc/DB/db3/#__codelineno-8-5)from department, max_budget [](https://note.hobbitqia.cc/DB/db3/#__codelineno-8-6)where department.budget = max_budget.value;`

Complex Queries using With Clause

![](http://cdn.hobbitqia.cc/202303131219550.png)

## Modification of the Database[¶](https://note.hobbitqia.cc/DB/db3/#modification-of-the-database "Permanent link")

### Deletion[¶](https://note.hobbitqia.cc/DB/db3/#deletion "Permanent link")

Example

![](http://cdn.hobbitqia.cc/202303131643578.png)

### Insertion[¶](https://note.hobbitqia.cc/DB/db3/#insertion "Permanent link")