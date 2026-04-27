5.24

Consider the relation, r, shown in Figure 5.22. Give the result of the following
```
query:

select building, room number, time slot id, count(*)

from r

group by rollup (building, room number, time slot id)
```

![[Pasted image 20260421185947.png]]
6.1

Construct an E-R diagram for a car insurance company whose customers own

one or more cars each. Each car has associated with it zero to any number of

recorded accidents. Each insurance policy covers one or more cars and has one

or more premium payments associated with it. Each payment is for a particular

period of time, and has an associated due date, and the date when the payment

was received.


6.2

Consider a database that includes the entity sets student, course, and section

from the university schema and that additionally records the marks that students

receive in different exams of different sections.

a. Construct an E-R diagram that models exams as entities and uses a ternary

relationship as part of the design.

b. Construct an alternative E-R diagram that uses only a binary relationship

between student and section. Make sure that only one relationship exists

between a particular student and section pair, yet you can represent the

marks that a student gets in different exams.


6.21

Consider the E-R diagram in Figure 6.30, which models an online bookstore.

a. Suppose the bookstore adds Blu-ray discs and downloadable video to its

collection. The same item may be present in one or both formats, with dif

fering prices. Draw the part of the E-R diagram that models this addition,

showing just the parts related to video.

b. Now extend the full E-R diagram to model the case where a shopping bas

ket may contain any combination of books, Blu-ray discs, or downloadable

video.

![[Pasted image 20260421190153.png]]
6.22

Design a database for an automobile company to provide to its dealers to assist

them in maintaining customer records and dealer inventory and to assist sales

staff in ordering cars.

Each vehicle is identified by a vehicle identification number (VIN). Each indi

vidual vehicle is a particular model of a particular brand offered by the company

(e.g., the XF is a model of the car brand Jaguar of Tata Motors). Each model

can be offered with a variety of options, but an individual car may have only

some (or none) of the available options. The database needs to store informa

tion about models, brands, and options, as well as information about individual

dealers, customers, and cars.

Your design should include an E-R diagram, a set of relational schemas, and

a list of constraints, including primary-key and foreign-key constraints.



6.24

Design a database for an airline. The database must keep track of customers

and their reservations, flights and their status, seat assignments on individual

flights, and the schedule and routing of future flights.

Your design should include an E-R diagram, a set of relational schemas, and

a list of constraints, including primary-key and foreign-key constraints.