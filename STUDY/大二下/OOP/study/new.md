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
		    m_data = new char[1];             //从这个 1 看出来的
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

构造函数（new）申请的资源，必须在析构函数（delete）中释放。
```



```C++
#include <iostream>
using namespace std;

template<typename T>
class SharedPtr {
private:
    T* ptr;
    int* ref_count;
public:
    SharedPtr(T* p = NULL) {
        if (p == NULL) {
            ref_count = NULL;           // ①
        } else {
            ref_count = new int(1);     // ② 构造函数，第一次指向，肯定是1
	        //new int(1)返回的是地址
        }
    }

    SharedPtr(const SharedPtr<T>& other) {
        ptr = other.ptr;
        ref_count = other.ref_count;
        if (ref_count) {
            (*ref_count)++;             // ③
        }
    }

    ~SharedPtr() {
        if (ref_count) {
            (*ref_count)--;             // ④
            if (*ref_count == 0) {
                delete ptr;             // ⑤
                delete ref_count;       // ⑥
            }
        }
    }

    T& operator*() {    //T* ptr;---> prt是指针，所以返回*prt才是T类型
        return *ptr;                    // ⑦
    }

    T* operator->() {   //T* ptr;---> prt是指针，所以返回prt就是T*类型了
        return ptr;                     // ⑧
    }
};

struct Node {
    int value;
    Node(int v) : value(v) {
        cout << "Node(" << value << ") created. ";
    }
    ~Node() {
        cout << "Node(" << value << ") destroyed. ";
    }
};

int main() {
    SharedPtr<Node> sp1(new Node(42)); 
    {
        SharedPtr<Node> sp2 = sp1;
        cout << "Value: " << sp2->value << ". ";
    }
    cout << "Still alive: " << (*sp1).value << ". ";
    return 0;
}
```
```C++
output:Node(42) created. Value: 42. Still alive: 42. Node(42) destroyed. 
```



```C++
// ======================== MazeGame 客户端 ========================
// ======================== Enchanted 主题 ========================
class EnchantedRoom : public Room {
public:
    // 直接调用基类构造函数，无需手动初始化 _sides
    EnchantedRoom(int n) : Room(n) {}

    // 重写 Enter（加上 override 是好习惯，但不必须）
    void Enter() override {
        cout << "Enter Enchanted Room " << _roomNumber 
             << " (Shining with blue magic aura)" << endl;
    }
};

class EnchantedDoor : public Door {
public:
    // 参数类型可以是 EnchantedRoom*，但基类构造函数接受 Room*，自动向上转型
    EnchantedDoor(EnchantedRoom* r1, EnchantedRoom* r2) 
        : Door(r1, r2) {}

    void Enter() override {
        cout << "Open a Magic Door (Whispering a glowing incantation)" << endl;
    }
};

class EnchantedMazeFactory : public MazeFactory {
public:
    // 返回基类指针，但实际创建派生类对象
    Room* MakeRoom(int n) const override {
        return new EnchantedRoom(n);
    }

    Door* MakeDoor(Room* r1, Room* r2) const override {
        // 注意：这里参数类型是 Room*，但我们知道它实际是 EnchantedRoom*
        // 为了安全，可以动态转换，但简单起见直接强制转换（因为工厂保证类型匹配）
        return new EnchantedDoor(
            static_cast<EnchantedRoom*>(r1),
            static_cast<EnchantedRoom*>(r2)
        );
    }
};

// ======================== Dungeon 主题 ========================
class DungeonRoom : public Room {
public:
    DungeonRoom(int n) : Room(n) {}

    void Enter() override {
        cout << "Enter Dungeon Room " << _roomNumber 
             << " (Cold, damp and dark)" << endl;
    }
};

class DungeonDoor : public Door {
public:
    DungeonDoor(DungeonRoom* r1, DungeonRoom* r2) 
        : Door(r1, r2) {}

    void Enter() override {
        cout << "Open a Heavy Iron Door (Creaking loudly with rust)" << endl;
    }
};

class DungeonMazeFactory : public MazeFactory {
public:
    Room* MakeRoom(int n) const override {
        return new DungeonRoom(n);
    }

    Door* MakeDoor(Room* r1, Room* r2) const override {
        return new DungeonDoor(
            static_cast<DungeonRoom*>(r1),
            static_cast<DungeonRoom*>(r2)
        );
    }
};

// ======================== MazeGame 客户端 ========================
class MazeGame {
public:
    Maze* CreateMaze(MazeFactory& factory) {
        // 1. 创建迷宫
        Maze* maze = factory.MakeMaze();

        // 2. 创建两个房间（编号 1 和 2）
        Room* room1 = factory.MakeRoom(1);
        Room* room2 = factory.MakeRoom(2);

        // 3. 创建连接这两个房间的门
        Door* door = factory.MakeDoor(room1, room2);

        // 4. 设置门的方向：房间1的东侧是门，房间2的西侧是门
        //    （Direction 枚举通常定义 East = 2, West = 3，这里直接用符号）
        room1->SetSide(East, door);
        room2->SetSide(West, door);

        // 5. 将房间和门加入迷宫
        maze->AddRoom(room1);
        maze->AddRoom(room2);
        maze->AddDoor(door);

        // 6. 返回迷宫指针
        return maze;
    }
};
```


```C++
class EnchantedRoom:public Room{
public:
    EnchantedRoom(int n) : _roomNumber(n) {
        for (int i = 0; i < 4; ++i)
            _sides[i] = NULL;
    }
    void Enter(){    cout<<"Enter Enchanted Room <"<<_roomNumber<<"> (Shining with blue magic aura)"<<endl;}
};
class EnchantedDoor:public Door{
public:
    EnchantedDoor(EnchantedRoom* r1, EnchantedRoom* r2) : _room1(r1), _room2(r2) {}
    void Enter(){    cout<<"Open a Magic Door (Whispering a glowing incantation)"<<endl;}
};
class EnchantedMazeFactory:public MazeFactory{
public:
    Room* MakeRoom(int n) const { return new EnchantedRoom(n); }
    Door* MakeDoor(EnchantedRoom* r1, EnchantedRoom* r2) const { return new EnchantedDoor(r1, r2); }
    ~EnchantedMazeFactory() {}
};


class DungeonRoom:public Room{
public:
    DungeonRoom(int n) : _roomNumber(n) {
        for (int i = 0; i < 4; ++i)
            _sides[i] = NULL;
    }
    void Enter(){    cout<<"Enter Dungeon Room <"<<_roomNumber<<"> (Cold, damp and dark)"<<endl;}
};
class DungeonDoor:public Door{
public:
    DungeonDoor(DungeonRoom* r1, DungeonRoom* r2) : _room1(r1), _room2(r2) {}
    void Enter(){    cout<<"Open a Heavy Iron Door (Creaking loudly with rust)"<<endl;}
};
class DungeonMazeFactory:public MazeFactory{
public:
    Room* MakeRoom(int n) const { return new DungeonRoom(n); }
    Door* MakeDoor(DungeonRoom* r1, DungeonRoom* r2) const { return new DungeonDoor(r1, r2); }
    ~DungeonMazeFactory() {}
};

template<class T>
class MazeGame{
public:
    Maze* CreateMaze(MazeFactory& factory){
        T m=factory.MakeMaze();
        Room Room1=m.MakeRoom(1);
        Room Room2=m.MakeRoom(2);
        Door Door=m.MakeDoor(Room1,Room2);
        Room1.SetSide(2,*Room2._sides);
        Room2.SetSide(4,*Room1._sides);
        m.AddRoom(Room1);
        m.AddRoom(Room2);
        m.AddDoor(Door);
        return *m;
    }
};
```



**有，而且问题很严重！** 
除了 `MazeGame` 类写得完全错误之外，你写的 `EnchantedDoor`、`DungeonDoor` 以及对应的两个工厂类，**都存在致命的编译错误和多态逻辑错误**。

我帮你逐一排查（按严重程度排序）：

---

### 🚨 错误 1（致命编译错误）：派生类 Door 没有正确调用基类构造函数

**你的写法：**
```cpp
EnchantedDoor(EnchantedRoom* r1, EnchantedRoom* r2) : _room1(r1), _room2(r2) {}
```
**问题所在：**
基类 `Door` 的构造函数是 `Door(Room* r1, Room* r2)`，并且**没有**提供默认构造函数（`Door()`）。
在 C++ 中，派生类构造时**必须先构造基类**。你直接写 `: _room1(r1), _room2(r2)` 会试图先调用 `Door()`（不存在），导致**编译失败**。

✅ **正确写法：**
```cpp
EnchantedDoor(EnchantedRoom* r1, EnchantedRoom* r2) : Door(r1, r2) {}
```
（`DungeonDoor` 同理，必须改成 `: Door(r1, r2)`）

---

### 🚨 错误 2（致命逻辑错误）：工厂重写函数的参数类型写错了

**你的写法：**
```cpp
class EnchantedMazeFactory : public MazeFactory {
public:
    Door* MakeDoor(EnchantedRoom* r1, EnchantedRoom* r2) const { 
        return new EnchantedDoor(r1, r2); 
    }
};
```
**问题所在：**
基类 `MazeFactory` 中的虚函数签名是 `virtual Door* MakeDoor(Room* r1, Room* r2)`。你在派生类中将参数改成了 `EnchantedRoom*`，这在 C++ 中**不是重写（Override）**，而是**隐藏（Hide）**或**重载（Overload）**。

这会导致什么后果？
在主函数 `game.CreateMaze(enchantedFactory)` 中，因为参数是 `MazeFactory&` 引用，编译器只会查找基类的虚函数表。由于参数类型不匹配，你的 `EnchantedMazeFactory::MakeDoor` **不会被调用**，而是直接调用了基类的 `MazeFactory::MakeDoor`，最后创建出来的是一扇普通的 `Door`，而不是 `EnchantedDoor`，游戏输出就会完全错误！

✅ **正确写法（保持参数类型为基类指针，内部强转）：**
```cpp
class EnchantedMazeFactory : public MazeFactory {
public:
    Room* MakeRoom(int n) const override {
        return new EnchantedRoom(n);
    }
    Door* MakeDoor(Room* r1, Room* r2) const override {
        // 因为工厂保证传入的 r1, r2 一定是 EnchantedRoom，所以可以安全强转
        return new EnchantedDoor(
            static_cast<EnchantedRoom*>(r1),
            static_cast<EnchantedRoom*>(r2)
        );
    }
};
```
（`DungeonMazeFactory` 同理修正）

---

### ⚠️ 错误 3（代码冗余）：Room 派生类重复初始化 `_sides`

**你的写法：**
```cpp
EnchantedRoom(int n) : _roomNumber(n) {
    for (int i = 0; i < 4; ++i)
        _sides[i] = NULL;
}
```
**问题所在：**
虽然编译能通过，但这属于**冗余代码**。基类 `Room` 的构造函数已经帮你把 `_sides` 全部置为 `NULL` 了，派生类再重复一遍完全没必要，而且如果基类逻辑改变，这里容易遗漏。

✅ **正确写法（直接调用基类构造）：**
```cpp
EnchantedRoom(int n) : Room(n) {}   // 基类 Room(n) 已经做了所有初始化工作
```
（`DungeonRoom` 同理）

---

### ✅ 只有 `Enter()` 函数你写对了

平心而论，你写的四个 `Enter()` 函数（`EnchantedRoom::Enter`、`EnchantedDoor::Enter`、`DungeonRoom::Enter`、`DungeonDoor::Enter`）的**打印内容完全正确**，只要把上述问题改掉，程序就能完美运行。

---

### 📝 给你的完整修正清单（直接抄作业）

| 类名 | 你的错误写法 | 正确修正 |
| :--- | :--- | :--- |
| `EnchantedRoom` | 构造函数手动初始化 `_sides` | `EnchantedRoom(int n) : Room(n) {}` |
| `DungeonRoom` | 构造函数手动初始化 `_sides` | `DungeonRoom(int n) : Room(n) {}` |
| `EnchantedDoor` | `: _room1(r1), _room2(r2)` | `: Door(r1, r2)` |
| `DungeonDoor` | `: _room1(r1), _room2(r2)` | `: Door(r1, r2)` |
| `EnchantedMazeFactory` | `MakeDoor(EnchantedRoom* ...)` | `MakeDoor(Room* ..., Room* ...)` + 内部强转 |
| `DungeonMazeFactory` | `MakeDoor(DungeonRoom* ...)` | `MakeDoor(Room* ..., Room* ...)` + 内部强转 |

把这些地方改掉，再加上我之前给你修正的 `MazeGame` 类，你的代码就能完美通过测试了！😊