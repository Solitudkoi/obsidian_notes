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
output:
```