![[Pasted image 20260614152932.png]]
![[Pasted image 20260616151106.png]]
![[Pasted image 20260616235725.png]]



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

p是多态变量，要构造的是Derived类，但是Derived是Base的子类，然后因为Base没有声明构造函数，所以new的时候先调用Base的默认构造函数然后调用Derived自己的构造函数，析构的时候会先调用Derived自己的析构函数（但是derived类内没有声明析构函数，程序就会调用自动默认析构函数）然后因为Base的析构函数是虚函数，所以不会调用。
然后p->print();因为p是Derived类，而且Base的print()是虚函数，所以会直接调用Derived的print()函数；如果Base里的是void print() { cout << "Base"; }输出就会是Base而不会输出Derived了
```
- **有 `virtual`**：动态绑定，看实际对象类型（`Derived`）→ 输出 `Derived`。
- **没有 `virtual`**：静态绑定，看指针类型（`Base*`）→ 输出 `Base`。



```C++

```
```C++
output:


```