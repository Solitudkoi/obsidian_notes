### 长度与复制

|函数|作用|
|---|---|
|`strlen(s)`|返回字符串长度（不含 `\0`）|
|`strcpy(dest, src)`|复制 src 到 dest|
|`strncpy(dest, src, n)`|最多复制 n 个字符|
|`strcat(dest, src)`|将 src 追加到 dest 末尾|
|`strncat(dest, src, n)`|最多追加 n 个字符|

### 比较

| 函数                 | 作用                      |
| ------------------ | ----------------------- |
| `strcmp(a, b)`     | 比较，返回 `<0` / `0` / `>0` |
| `strncmp(a, b, n)` | 最多比较 n 个字符              |
| `strcasecmp(a, b)` | 忽略大小写比较（非标准，POSIX）      |

### 查找

|函数|作用|
|---|---|
|`strchr(s, c)`|查找字符 c 第一次出现的位置|
|`strrchr(s, c)`|查找字符 c 最后一次出现的位置|
|`strstr(s, sub)`|查找子串 sub 第一次出现的位置|
|`strpbrk(s, chars)`|查找 chars 中任意字符首次出现位置|

### 分割与转换

|函数|作用|
|---|---|
|`strtok(s, delim)`|按 delim 分割字符串（会修改原串）|
|`strtok_r(s, delim, &saveptr)`|线程安全版本（POSIX）|

### 其他常用（`<stdlib.h>` + `<ctype.h>`）

| 函数                          | 作用             |     |
| --------------------------- | -------------- | --- |
| `atoi(s)`                   | 字符串转 int       |     |
| `atof(s)`                   | 字符串转 double    |     |
| `strtol(s, &end, base)`     | 字符串转 long（更安全） |     |
| `sprintf(buf, fmt, ...)`    | 格式化输出到字符串      |     |
| `sscanf(s, fmt, ...)`       | 从字符串格式化读取      |     |
| `tolower(c)` / `toupper(c)` | 大小写转换（单个字符）    | ）   |
