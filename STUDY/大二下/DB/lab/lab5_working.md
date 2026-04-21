```
# 1. 先跑借书证（最简单）
mvn -Dtest=LibraryTest#registerAndShowAndRemoveCardTest clean test

# 2. 图书入库
mvn -Dtest=LibraryTest#bookRegisterTest clean test

# 3. 增加库存
mvn -Dtest=LibraryTest#incBookStockTest clean test

# 4. 批量入库
mvn -Dtest=LibraryTest#bulkRegisterBookTest clean test

# 5. 删除图书
mvn -Dtest=LibraryTest#removeBookTest clean test

# 6. 修改图书
mvn -Dtest=LibraryTest#modifyBookTest clean test

# 7. 查询图书
mvn -Dtest=LibraryTest#queryBookTest clean test

# 8. 借还书
mvn -Dtest=LibraryTest#borrowAndReturnBookTest clean test

# 9. 并发借书
mvn -Dtest=LibraryTest#parallelBorrowBookTest clean test
```

