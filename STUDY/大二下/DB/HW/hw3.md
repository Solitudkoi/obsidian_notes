![[Pasted image 20260324132914.png]]

3.8 Consider the bank database of Figure 3.18, where the primary keys are underlined. Construct the following SQL queries for this relational database.
	a. Find the ID of each customer of the bank who has an account but not a
		loan.
	b. Find the ID of each customer who lives on the same street and in the same
		city as customer '12345'.
	c. Find the name of each branch that has at least one customer who has an
		account in the bank and who lives in “Harrison”.



3.9 Consider the relational database of Figure 3.19, where the primary keys are underlined. Give an expression in SQL for each of the following queries.
	a. Find the ID, name, and city of residence of each employee who works for “First Bank Corporation”.
	b. Find the ID, name, and city of residence of each employee who works for “First Bank Corporation” and earns more than $10000.
	c. Find the ID of each employee who does not work for “First Bank Corporation”.
	d. Find the ID of each employee who earns more than every employee of
		“Small Bank Corporation”.
	e. Assume that companies may be located in several cities. Find the name of each company that is located in every city in which “Small Bank Corporation” is located.
	f. Find the name of the company that has the most employees (or companies, in the case where there is a tie for the most).
	g. Find the name of each company whose employees earn a higher salary,on average, than the average salary at “First Bank Corporation”.

![[Pasted image 20260324132852.png]]

3.10 Consider the relational database of Figure 3.19. Give an expression in SQL foreach of the following:
	a. Modify the database so that the employee whose ID is '12345' now lives in “Newtown”.
	b. Give each manager of “First Bank Corporation” a 10 percent raise unless the salary becomes greater than $100000; in such cases, give only a 3 percent raise.


3.15 Consider the bank database of Figure 3.18, where the primary keys are underlined. Construct the following SQL queries for this relational database.
	a. Find each customer who has an account at every branch located in “Brooklyn”.
	b. Find the total sum of all loan amounts in the bank.
	c. Find the names of all branches that have assets greater than those of at least one branch located in “Brooklyn”.
![[Pasted image 20260324132914.png]]
## 3.15 a

**找出在位于 “Brooklyn” 的每一个支行都有账户的客户**

又是典型“除法”问题。

select c.ID, c.customer_name  
from customer c  
where not exists (  
    select b.branch_name  
    from branch b  
    where b.branch_city = 'Brooklyn'  
      and not exists (  
          select *  
          from depositor d  
          join account a on d.account_number = a.account_number  
          where d.ID = c.ID  
            and a.branch_name = b.branch_name  
      )  
);

意思是：

> 不存在某个 Brooklyn 的支行，使得该客户在这个支行没有账户。

如果只要客户 ID，也可以写成只选 `c.ID`。

---

## 3.15 b

**求银行所有贷款金额总和**

select sum(amount) as total_loan_amount  
from loan;

---

## 3.15 c

**找出资产大于至少一个 Brooklyn 支行资产的所有支行名**

“greater than those of at least one”  
就是 **大于 Brooklyn 中某一个支行的资产**，用 `> ANY`。

select branch_name  
from branch  
where assets > any (  
    select assets  
    from branch  
    where branch_city = 'Brooklyn'  
);

有些数据库不支持 `ANY`，可以改写成：

select branch_name  
from branch  
where assets > (  
    select min(assets)  
    from branch  
    where branch_city = 'Brooklyn'  
);

因为：

> 大于 Brooklyn 中至少一个支行的资产  
> 等价于  
> 大于 Brooklyn 支行资产中的最小值





```
SELECT customer_name 
FROM costomer c ,branch b ,depositor d,account a 
WHERE not exist (
	SELECT branch_name
	FROM branch
	WHERE branch_city='Brooklyn')
	EXCEPT
	( SELECT branch.name
	  WHERE c.ID=d.ID
			and  d.account_name=a.account_name)
```