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

throw 仍出一个值后不会直接跳回上一层的，而是将throw出的值和catch块做匹配
test(1);--->
```
