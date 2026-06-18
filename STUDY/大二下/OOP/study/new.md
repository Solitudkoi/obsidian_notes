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

```