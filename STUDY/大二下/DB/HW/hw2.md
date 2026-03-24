
![[Pasted image 20260314130228.png]]
![[Pasted image 20260314130438.png]]
![[Pasted image 20260314130500.png]]

## 2.14
### a) Find the ID and name of each employee who works for “BigBank”.

There is no `ID` attribute in Figure 2.17, so only `person_name` can be returned from the given schema.

[  
\pi_{person_name}(\sigma_{company_name='BigBank'}(works))  
]

### b) Find the ID, name, and city of residence of each employee who works for “BigBank”.

Again, no `ID` exists in this schema, so from the given relations:

[  
\pi_{person_name,\ city}  
\big(  
\sigma_{company_name='BigBank'}(works)\ \bowtie\ employee  
\big)  
]

### c) Find the ID, name, street address, and city of residence of each employee who works for “BigBank” and earns more than $10000.

[  
\pi_{person_name,\ street,\ city}  
\big(  
\sigma_{company_name='BigBank' \wedge salary>10000}(works)\ \bowtie\ employee  
\big)  
]

### d) Find the ID and name of each employee in this database who lives in the same city as the company for which she or he works.

Join `employee` with `works`, then with `company`, and select tuples where employee city = company city.

[  
\pi_{person_name}  
\Big(  
\sigma_{employee.city = company.city}  
\big(  
(employee \bowtie works) \bowtie company  
\big)  
\Big)  
]

If you want this in fully disambiguated form with renaming:

[  
\pi_{person_name}  
\Big(  
\sigma_{E.city = C.city}  
\big(  
\rho_E(employee) \bowtie \rho_W(works) \bowtie \rho_C(company)  
\big)  
\Big)  
]

---

## 2.15

Bank database again:

- `branch(branch_name, branch_city, assets)`
    
- `customer(ID, customer_name, customer_street, customer_city)`
    
- `loan(loan_number, branch_name, amount)`
    
- `borrower(ID, loan_number)`
    
- `account(account_number, branch_name, balance)`
    
- `depositor(ID, account_number)`
    

### a) Find each loan number with a loan amount greater than $10000.

[  
\pi_{loan_number}(\sigma_{amount>10000}(loan))  
]

### b) Find the ID of each depositor who has an account with a balance greater than $6000.

[  
\pi_{ID}  
\big(  
depositor \bowtie \sigma_{balance>6000}(account)  
\big)  
]

### c) Find the ID of each depositor who has an account with a balance greater than $6000 at the “Uptown” branch.

[  
\pi_{ID}  
\big(  
depositor \bowtie \sigma_{balance>6000 \wedge branch_name='Uptown'}(account)  
\big)  
]

If you want, I can also turn 2.12 into a neat schema diagram with primary keys underlined and foreign keys marked.