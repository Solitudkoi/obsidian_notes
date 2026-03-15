```
#include<iostream>

#include <fstream>

#include <vector>

#include <iomanip>

#include <utility>

#include <algorithm>

#include "point_line.h"

using namespace std;

  

struct Point {

    double x;

    double y;

};

  

struct Line {

    int p1;

    int p2;

};

  

// 需要实现的函数

std::pair<std::vector<Point>, std::vector<Line>> read_data();

  

void print_table(const std::vector<Point> &points, const std::vector<Line> &lines);

  

void write_svg(const std::vector<Point> &points, const std::vector<Line> &lines);

  

int main()

{

    auto [points, lines] = read_data();

    print_table(points, lines);

    write_svg(points, lines);

    return 0;

}

  

std::pair<std::vector<Point>, std::vector<Line>> read_data()

{

    std::vector<Point> points;

    std::vector<Line> lines;

  

    std::ifstream infile("in.data.txt");

    if (!infile) {

        std::cerr << "无法打开文件 in.data.txt" << std::endl;

        return {points, lines};

    }

  

    int num_points, num_lines;

    infile >> num_points >> num_lines;

  

    for (int i = 0; i < num_points; ++i) {

        Point p;

        infile >> p.x >> p.y;

        points.push_back(p);

    }

  

    for (int i = 0; i < num_lines; ++i) {

        Line l;

        infile >> l.p1 >> l.p2;

        lines.push_back(l);

    }

  

    return {points, lines};

}

  

void print_table(const std::vector<Point> &points, const std::vector<Line> &lines)

{

    std::cout << "Points:" << std::endl;

    std::cout << std::setw(10) << "Index" << std::setw(10) << "X" << std::setw(10) << "Y" << std::endl;

    for (size_t i = 0; i < points.size(); ++i) {

        std::cout << std::setw(10) << i << std::setw(10) << points[i].x << std::setw(10) << points[i].y << std::endl;

    }

  

    std::cout << "\nLines:" << std::endl;

    std::cout << std::setw(10) << "Index" << std::setw(10) << "P1" << std::setw(10) << "P2" << std::endl;

    for (size_t i = 0; i < lines.size(); ++i) {

        std::cout << std::setw(10) << i << std::setw(10) << lines[i].p1 << std::setw(10) << lines[i].p2 << std::endl;

    }

}

  
  

void write_svg(const std::vector<Point> &points, const std::vector<Line> &lines)

{

    std::ofstream outfile("output.svg");

    if (!outfile) {

        std::cerr << "无法创建文件 output.svg" << std::endl;

        return;

    }

  

    // SVG头部

    outfile << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;

  

    // 绘制线段

    for (const auto &line : lines) {

        const Point &p1 = points[line.p1];

        const Point &p2 = points[line.p2];

        outfile << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y

                << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y

                << "\" style=\"stroke:rgb(0,0,0);stroke-width:2\" />" << std::endl;

    }

  

    // 绘制点

    for (const auto &point : points) {

        outfile << "<circle cx=\"" << point.x << "\" cy=\"" << point.y

                << "\" r=\"5\" fill=\"red\" />" << std::endl;

    }

  

    // SVG尾部

    outfile << "</svg>" << std::endl;

}
```



9-1 Points and Lines

### Objective

Welcome to your first assignment! The goal of this homework is to get you comfortable with basic C++ syntax, including:

- Defining and using `struct` (C-style data structures).
- Using the C++ Standard Library (`std::vector`, `std::pair`, `std::cin`/`std::cout`, `std::fstream`).
- Reading the geometric data from an input file (`.data`).
- Writing formatted data to standard output.
- Generating a web-viewable graphic (`.svg`) from raw data.

### The Skeleton Code

You will be provided with a C++ skeleton code. Your task is to implement three functions for reading and processing the geometric data: `read_data`, `print_table`, and `write_svg`.

struct Point;

struct Line;

// functions to implement

std::pair<std::vector<Point>, std::vector<Line>> read_data();

void print_table(const std::vector<Point> &points, const std::vector<Line> &lines);

void write_svg(const std::vector<Point> &points, const std::vector<Line> &lines);

### Input Data Format

Your program will read data from the input file `in.data`. The format is as follows:

1. **Header Line:** One line containing two integers:  
    `[num_points] [num_lines]`
2. **Point Data:** `num_points` lines, each containing two floating-point numbers:  
    `[x] [y]`
3. **Line Data:** `num_lines` lines, each containing two integers:  
    `[p1_index] [p2_index]` (These are 0-based indices into the points list).

### Example Data

5 5

165.0 711.0

387.0 27.0

609.0 711.0

27.0 288.0

747.0 288.0

0 1

1 2

2 3

3 4

4 0

### Table Output

Your program will print the points and lines in a formatted table to the standard output. Make sure to use the same width, precision, and alignment as the following example.

+------------------------------------+

| Points |

+--------+-------------+-------------+

| Index | X | Y |

+--------+-------------+-------------+

| 0 | 165.0 | 711.0 |

| 1 | 387.0 | 27.0 |

| 2 | 609.0 | 711.0 |

| 3 | 27.0 | 288.0 |

| 4 | 747.0 | 288.0 |

+--------+-------------+-------------+

  

+------------------------------------+

| Lines |

+--------+-------------+-------------+

| Index | Point 1 | Point 2 |

+--------+-------------+-------------+

| 0 | 0 | 1 |

| 1 | 1 | 2 |

| 2 | 2 | 3 |

| 3 | 3 | 4 |

| 4 | 4 | 0 |

+--------+-------------+-------------+

  

### SVG Output

Your program will write the points and lines to an SVG file named `out.svg`, which uses an XML-based vector graphics format to define two-dimensional graphics. Ensure your output format matches the following example.

▾

<svg width="740.0" height="704.0" viewBox="17.0 17.0 740.0 704.0" xmlns="http://www.w3.org/2000/svg">

<line x1="165.0" y1="711.0" x2="387.0" y2="27.0" stroke="black" stroke-width="2.0"/>

<line x1="387.0" y1="27.0" x2="609.0" y2="711.0" stroke="black" stroke-width="2.0"/>

<line x1="609.0" y1="711.0" x2="27.0" y2="288.0" stroke="black" stroke-width="2.0"/>

<line x1="27.0" y1="288.0" x2="747.0" y2="288.0" stroke="black" stroke-width="2.0"/>

<line x1="747.0" y1="288.0" x2="165.0" y2="711.0" stroke="black" stroke-width="2.0"/>

<circle cx="165.0" cy="711.0" r="4.0" fill="black"/>

<circle cx="387.0" cy="27.0" r="4.0" fill="black"/>

<circle cx="609.0" cy="711.0" r="4.0" fill="black"/>

<circle cx="27.0" cy="288.0" r="4.0" fill="black"/>

<circle cx="747.0" cy="288.0" r="4.0" fill="black"/>

</svg>

  

Specifically, your program should generate the following content:

- A `<line>` element for each line, with a fixed width of 2.
- A `<circle>` element for each point, with a fixed radius of 4.
- The `width` and `height` of the canvas, as well as the corresponding `viewBox`. To determine these values, first calculate the bounding box of the geometric data and then add a fixed padding of 10 units around it.

Once the SVG file is produced, you can open it in any major desktop browsers, e.g., Google Chrome, and you will see something like this:

![drawing](https://images.ptausercontent.com/c4a44df3-39cb-4273-b769-031087a38934.svg)

---

### Optional Task (not included in the score)

For those with extra energy, we have a little puzzle for you. Your program has to process a different data file in a more complex format, as follows, and reconstruct colored polygons (loops) from it.

1. **Header Line:** One line containing three integers:  
    `[num_points] [num_lines] [num_loops]`
2. **Point Data (same as before):** `num_points` lines of  
    `[x] [y]`.
3. **Line Data (same as before):** `num_lines` lines of  
    `[p1_index] [p2_index]` .
4. **Loop Data:** `num_loops` lines, each containing:  
    `[start_point_index] [r] [g] [b]` (This gives the starting point index of a loop and its RGB fill color).

You will be provided with two such data files in the `optional` folder. Make a new program to:

- Read the new format data.
- Besides points and lines, trace each loop (starting from the given point) to collect all its point indices. To achieve this, it would be helpful first to build the adjacency information.
- After finding all the loops, modify your `write_svg` function to draw `<polygon>` elements, one polygon per loop. The `points` attribute of a polygon is a string that consists of point coordinates sequentially: `"x1,y1 x2,y2 x3,y3 ..."`, and the `fill` attribute should use the given RGB color: `fill="rgb(r,g,b)"`. See the following snippet.
- Do not add any padding to the canvas this time.

▾

<svg width="900.0" height="600.0" viewBox="0.0 0.0 900.0 600.0" xmlns="http://www.w3.org/2000/svg">

<polygon points="0.0,0.0 900.0,0.0 900.0,600.0 0.0,600.0" fill="rgb(238,28,37)"/>

...

</svg>

Again, open the generated SVG files in any browser to check the results. Good luck!

时间限制

10000 ms

内存限制

64 MB

CPU核数限制

1

题目文件夹

optional/

src/

(提交文件夹)

main.sh

(入口文件)

请在提交文件夹src中完成作答后将此文件夹压缩为zip格式上传。