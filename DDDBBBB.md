```
1.account(account_number, branch_name, balance)
找出余额在 1000 到 5000 之间的账户号和余额，并按余额从小到大排序

SELECT account_number,balance
FROM account
WHERE balance BETWEEN 1000 AND 5000
ORDER BY balance ASC;


2.找出平均余额大于2000的支行名称，并显示它们的平均余额

SELECT branch_name,avg(balance)
FROM account
GROUP BY branch_name
HAVING avg(balance)>2000;

3.找出账户数量大于3的支行名称

SELECT branch_name
FROM account
GROUP BY branch_name
HAVING count(account_number)>3;

SELECT
FROM
WHERE
```