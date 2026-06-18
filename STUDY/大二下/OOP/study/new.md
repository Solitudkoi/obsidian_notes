![[Pasted image 20260614152932.png]]
![[Pasted image 20260616151106.png]]
![[Pasted image 20260616235725.png]]



What happens if an exception is thrown in a C++ program but is never caught by any `catch` block?
A.The compiler will automatically generate a default catch block.
B.The program ignores the exception and executes the next line.
C.The program calls `std::terminate()` and aborts.✅
D.The program execution resets to the beginning of the `main()` function.

When a derived class inherits from a base class using `public` inheritance, the `protected` members of the base class become ________ members of the derived class.
A.`public`
B.`protected`✅
C.`private`
D.inaccessible

```C++
#include <iostream>
using namespace std;

class Base {
public:
    Base() { cout << "B"; }
    virtual ~Base() { cout << "~B"; }
};

class Derived : public Base {
public:
    Derived() { cout << "D"; }
    ~Derived() { cout << "~D"; }
};

int main() {
    Base* p = new Derived();
    delete p;
    return 0;
}
```
```C++
output:BD~D~B
virtual 函数 动态绑定
如果改为 ~Base() { cout << "~B"; }运行结果就是BD~B
```

```C++
#include <iostream>
using namespace std;

class Test {
public:
    static int count;
    Test() { count++; }
};

int Test::count = 0;

int main() {
    Test t1;
    Test t2;
    cout << Test::count;
    return 0;
}
```
```C++
ouput:2

Test() { count++; }就是这个类的构造函数，然后static int count;这个静态变量是全局的，所以每次构造函数内都会+1
```


```C++
#include <iostream>
using namespace std;

void test(int x) {
    try {
        if (x == 0) throw "Zero";
        if (x == 1) throw 1;
        cout << "OK";
    } catch (int e) {
        cout << "Int";
    } catch (...) {
        cout << "Other";
    }
}

int main() {
    test(1);
    test(0);
    return 0;
}
```
```C++
output:IntOther

throw 扔出一个值后不会直接跳回上一层的，而是将throw出的值和catch块做匹配
但是如果一直没有找到匹配的类型就会自动跳回上一层（调用者），如果一直到最外层都没有匹配到，就会调用 `std::terminate()` 终止
test(1);---> throw 1;---> catch (int e)符合int型---> cout << "Int";
test(0);---> throw "Zero";---> catch (...) ---> cout << "Other";
```


```C++
#include <iostream>
using namespace std;

class Base {
public:
    virtual void print() { cout << "Base"; }
    virtual ~Base() {}
};

class Derived : public Base {
public:
    void print() { cout << "Derived"; }
};

int main() {
    Base* p = new Derived();
    p->print();
    delete p;
    return 0;
}
```
```C++
output:Derived

p是多态变量，要构造的是Derived类，但是Derived是Base的子类，然后因为Base没有声明构造函数，所以new的时候先调用Base的默认构造函数然后调用Derived自己的构造函数，析构的时候（为了内存安全起见，基类的析构函数都应该是虚函数）先析构派生类再析构基类，然后在这个程序里派生类没有声明析构函数，所以会调用默认析构函数，然后再调用基类的析构函数
然后p->print();因为p是Derived类，而且Base的print()是虚函数，所以会直接调用Derived的print()函数；如果Base里的是void print() { cout << "Base"; }输出就会是Base而不会输出Derived了
```
- **有 `virtual`**：动态绑定，看实际对象类型（`Derived`）→ 输出 `Derived`。
- **没有 `virtual`**：静态绑定，看指针类型（`Base*`）→ 输出 `Base`。



```C++
#include <iostream>
using namespace std;

void process(int &a, int b) {
    a += 2; b += 2;
}

int main() {
    int x = 5, y = 5;
    process(x, y);
    cout << x << " " << y << endl;
    return 0;
}
```
```C++
output:7 5

被自己蠢笑了吧。。。。。。。
```


### 程序填空

```C++
#include <cstring>
#include <iostream>
#include <stdexcept>
using namespace std;

class MyString {
private:
    char* m_data;
    size_t m_size;
public:
    // 构造函数（默认参数 NULL）
    MyString(const char* str = NULL) {
        if (str == NULL) {                     // ① 判断空指针
            m_data = new char[1];
            *m_data = '\0';
            m_size = 0;
        } else {
            m_size = strlen(str);
            m_data = new char[m_size + 1];     // ② 分配空间（含 '\0'）
            strcpy(m_data, str);
        }
    }
    
    // 拷贝构造函数（深拷贝）
    MyString(const MyString& other) {
        m_size = other.m_size;
        m_data = new char[m_size + 1];
        strcpy(m_data, other.m_data);          // ③ 复制内容
    }
    
    // 赋值运算符（深拷贝 + 自赋值检查）
    MyString& operator=(const MyString& other) {
        if (this != &other) {                  // ④ 自赋值判断
            delete[] m_data;
            m_size = other.m_size;
            m_data = new char[m_size + 1];
            strcpy(m_data, other.m_data);
        }
        return *this;                          // ⑤ 返回自身引用
    }
    
    // 析构函数（释放内存）
    ~MyString() {
        delete[] m_data;                       // ⑥ 释放数组内存
    }

    // 在指定位置插入一个字符
    void insert(size_t pos, char c) {
        if (pos > m_size) {                    // ⑦ 非法位置（大于长度）
            throw std::out_of_range("Position out of bounds"); // ⑧ 抛出异常
        }
        char* newData = new char[m_size + 2];
        strncpy(newData, m_data, pos);
        newData[pos] = c;
        strcpy(newData + pos + 1, m_data + pos);
        delete[] m_data;
        m_data = newData;
        m_size++;
    }
    
    // 打印字符串（若非空）
    void print() const {
        if (m_data) cout << m_data << endl;
    }
};

int main() {
    try {
        MyString s1("OOP");
        MyString s2 = s1;
        MyString s3;
        s3 = s2;
        
        s3.insert(3, '!');   // 在末尾插入 '!'，变为 "OOP!"
        s3.print();          // 输出 OOP!

        s3 = s3;             // 自赋值，安全跳过
        
        s3.insert(10, '?');  // 位置 10 超出长度 4，抛出异常
    } catch (const std::exception& e) {
        cout << "Exception: " << e.what() << endl; // 捕获并打印异常信息
    }
    return 0;
}
```
```C++
output:
OOP!
Exception: Position out of bounds
```



```C++

```
```C++
output:


```