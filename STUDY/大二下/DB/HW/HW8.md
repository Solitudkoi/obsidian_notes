DB-HW8  12.13 and 13.11

**12.13** Suppose you have data that should not be lost on disk failure, and the application is write-intensive. How would you store the data?

**13.11** List two advantages and two disadvantages of each of the following strategies for storing a relational database:

a. Store each relation in one ﬁle.

b. Store multiple relations (perhaps even the entire database) in one ﬁle.



## 12.13

If the data must survive disk failure and the application is write-intensive, the best choice is **RAID 1 (mirroring)** or **RAID 10 (striped mirrors)**.

The reasoning is as follows. RAID 5 and RAID 6 provide fault tolerance, but every write requires computing and updating parity blocks, which creates a significant write overhead (known as the "write penalty"). For a write-intensive workload, this penalty is unacceptable.

RAID 1 simply writes the same data to two disks simultaneously. There is no parity computation, so write performance is much better. The tradeoff is higher storage cost since you need twice the disk space, but for write-intensive applications that cannot afford data loss, this is the appropriate choice.

RAID 10 combines mirroring with striping, providing both fault tolerance and high write throughput by distributing writes across multiple mirrored pairs. This is ideal when both performance and reliability are critical.

---

## 13.11

### (a) Store each relation in one file

**Advantages:**

1. Simple to manage and understand. Each relation maps directly to a file, making it easy to locate, back up, or restore individual relations independently.
2. Operations on one relation (such as sequential scans or reorganization) do not affect other relations. Concurrent access to different relations is naturally separated since they reside in different files.

**Disadvantages:**

1. Joins or queries involving multiple relations require cross-file I/O, which can be slower since the operating system must manage many open file descriptors and buffers separately.
2. If there are many relations, the large number of files can create overhead for the file system and waste space due to partially filled blocks in each file.

### (b) Store multiple relations in one file

**Advantages:**

1. Related tuples from different relations can be stored physically close together (clustering), which can greatly speed up joins and multi-relation queries by reducing I/O.
2. Fewer files to manage, reducing file system overhead. Space utilization can be better since multiple small relations can share blocks.

**Disadvantages:**

1. More complex storage management is required. The system needs internal metadata to track where each relation's data resides within the shared file.
2. Operations on one relation (such as reorganization, backup, or recovery) can affect or be complicated by the presence of other relations in the same file. A full scan of one relation may require reading through data belonging to other relations.