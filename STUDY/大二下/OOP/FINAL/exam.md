In computer science, abstraction is an effective way to hide implementation complexity. For instance, suppose we want to design a shape-drawing program for different operating systems, such as _Windows_, _Macintosh_, _X Windows_, and so on. However, the SDK functions for drawing shapes, such as rectangles and circles, are different on these systems:

1. _Windows_: `WRect(float lx, float ly, float w, float h)` for rectangles, `WCircle(float cx, float cy, float r)` for circles.
2. _Macintosh_: `MRect(float lx, float ly, float rx, float ry)` for rectangles, `MCircle(float cx, float cy, float r)` for circles.
3. _X Windows_: `XRect(float cx, float cy, float hw, float hh)` for rectangles, `XCircle(float cx, float cy, float r)` for circles.

The function parameters are specified as follows:

- `(cx, cy)`: the center of a rectangle or circle
- `(w, h)`: the width and height of a rectangle
- `(lx, ly)`: the bottom-left corner of a rectangle
- `(rx, ry)`: the top-right corner of a rectangle
- `(hw, hh)`: the half-width and half-height of a rectangle
- `(r)`: the radius of a circle

Thus, it is necessary to introduce interface abstraction to decouple the shape-drawing code from the operating-system-dependent SDK functions. We can then design the class hierarchy as follows:

![diagram.svg](https://images.ptausercontent.com/21734ad4-7dec-449f-ab16-4cfb94e7d53a.svg)

The following code shows the main function of the program, and the program's output is listed after the code. **You must implement five classes: `WindowsImpl`, `MacintoshImpl`, `XWindowsImpl`, `Rectangle`, and `Circle` in your submission. From the code and output, you can see that switching between different drawing functions does not affect the implementation of the shape classes. Please implement all the necessary functions required for these classes.**

```c++
#include <iostream>

using namespace  std;

void WRect(float lx, float ly, float w, float h)
{
    cout << "Windows WRect (lx,ly,w,h) " << lx << "," << ly << "," << w << "," << h << "," << endl;
}

void WCircle(float x, float y, float r)
{
    cout << "Windows WCircle" << endl;
}

void MRect(float lx, float ly, float rx, float ry)
{
    cout << "Macintosh MRect (lx,ly,rx,ry) " << lx << "," << ly  <<"," << rx << "," << ry << "," << endl;
}

void MCircle(float x, float y, float r)
{
    cout << "Macintosh MCircle" << endl;
}

void XRect(float cx, float cy, float hw, float hh)
{
    cout << "XWindows XRect (cx,cy,hw,hh) " << cx << "," << cy << "," << hw << "," << hh << ","  <<endl;
}

void XCircle(float x, float y, float r)
{
    cout << "XWindows XCircle" << endl;
}

class DrawContext
{
public:
    virtual void rect(float cx, float cy, float hw, float hh) = 0;
    virtual void circle(float cx, float cy, float r) = 0;
};

class WindowsImpl : public DrawContext
{
public:
    // todo ...
};

class MacintoshImpl : public DrawContext
{
    // todo ...
};

class XWindowsImpl : public DrawContext
{
    // todo ...
};


class Shape
{
public:
    Shape(float cx, float cy) { m_cx = cx, m_cy = cy; }
    virtual ~Shape() {}
    virtual void render(DrawContext* pDC) = 0;
    float getCX() { return m_cx; }
    float getCY() { return m_cy; }
private:
    float m_cx, m_cy;
};

class Rectangle : public Shape
{
public:
    // Ctor signature: Rectangle(float cx, float cy, float hw, float hh);
    // todo ...
};

class Circle : public Shape
{
public:
    // Ctor signature: Circle(float cx, float cy, float r);
    // todo ...
};

int main()
{
    Rectangle r1(10, 10, 4, 4);
    Circle c1(5, 5, 6);

    DrawContext* pDC = new WindowsImpl();
    r1.render(pDC);
    c1.render(pDC);

    pDC = new MacintoshImpl();
    r1.render(pDC);
    c1.render(pDC);

    pDC = new XWindowsImpl();
    r1.render(pDC);
    c1.render(pDC);
}
```
**Program output:**
```C++
Windows WRect (lx,ly,w,h) 6,6,8,8,
Windows WCircle
Macintosh MRect (lx,ly,rx,ry) 6,6,14,14,
Macintosh MCircle
XWindows XRect (cx,cy,hw,hh) 10,10,4,4,
XWindows XCircle
```
my answer:
```C++
class WindowsImpl : public DrawContext
{
public:
    void rect(float cx, float cy, float hw, float hh){
        WRect(cx,cy,hw,hh);
    }
    void circle(float cx, float cy, float r){
        WCircle(cx,cy,r);
    }
};

class MacintoshImpl : public DrawContext
{
public:
    void rect(float cx, float cy, float hw, float hh){
        MRect(cx,cy,hw,hh);
    }
    void circle(float cx, float cy, float r){
        MCircle(cx,cy,r);
    }
};

class XWindowsImpl : public DrawContext
{
public:
    void rect(float cx, float cy, float hw, float hh){
        XRect(cx,cy,hw,hh);
    }
    void circle(float cx, float cy, float r){
        XCircle(cx,cy,r);
    }
};

class Rectangle : public Shape
{
    float r_hw;
    float r_hh;
public:
    Rectangle(float cx, float cy, float hw, float hh):Shape(cx,cy),r_hw(hw),r_hh(hh) {}
    void render(DrawContext* pDC){
        pDC->rect(cx,cy,r_hw,r_hh);
    }
    ~Rectangle(){}
};

class Circle : public Shape
{
    float c_r;
public:
    Circle(float cx, float cy, float r):Shape(cx,cy),c_r(r) {}
    void render(DrawContext* pDC){
        pDC->circle(cx,cy,c_r);
    }
    ~Circle(){}
};
```
