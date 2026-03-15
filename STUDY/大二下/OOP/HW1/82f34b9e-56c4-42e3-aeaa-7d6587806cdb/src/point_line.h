#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>


// C-style struct for a 2D point
struct Point {
  double x;
  double y;
};

// C-style struct for a line segment defined by two point indices
struct Line {
  int p1_index;
  int p2_index;
};

/**
 * @brief Reads points and lines data from the input file "in.data".
 * @return A pair of vectors: the first vector contains Point structs, and the
 * second vector contains Line structs.
 */
std::pair<std::vector<Point>, std::vector<Line>>
read_data() {
  std::ifstream infile("in.data");
  if (!infile.is_open()) {
    std::cerr << "Error: Could not open the input file." << std::endl;
    return {{}, {}};
  }

  int num_points, num_lines;
  infile >> num_points >> num_lines;

  std::vector<Point> points(num_points);
  std::vector<Line> lines(num_lines);

  // [todo] Implement reading point data
  for (int i = 0; i < num_points; ++i) {
        Point p;
        infile >> p.x >> p.y;
        points.push_back(p);
    }

    // [todo] Implement reading line data
    for (int i = 0; i < num_lines; ++i) {
        Line l;
        infile >> l.p1_index >> l.p2_index;
        lines.push_back(l);
    }


  return {points, lines};
}

/**
 * @brief Prints the points and lines in a formatted table to the console.
 * @param points A vector of Point structs.
 * @param lines A vector of Line structs.
 */
void print_table(const std::vector<Point> &points, const std::vector<Line> &lines) {
  // [todo] Implement this function
  std::cout << "Points:" << std::endl;
    std::cout << std::setw(10) << "Index" << std::setw(10) << "X" << std::setw(10) << "Y" << std::endl;
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << std::setw(10) << i << std::setw(10) << points[i].x << std::setw(10) << points[i].y << std::endl;
    }

    std::cout << "\nLines:" << std::endl;
    std::cout << std::setw(10) << "Index" << std::setw(10) << "P1" << std::setw(10) << "P2" << std::endl;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::cout << std::setw(10) << i << std::setw(10) << lines[i].p1_index << std::setw(10) << lines[i].p2_index << std::endl;
    }
}

/**
 * @brief Writes the points and lines to an SVG file named "out.svg".
 * @param points A vector of Point structs.
 * @param lines A vector of Line structs.
 */
void write_svg(const std::vector<Point> &points, const std::vector<Line> &lines) {
  std::ofstream svgfile("out.svg");

  // [todo] Implement this function

  std::ofstream outfile("output.svg");
    if (!outfile) {
        std::cerr << "无法创建文件 output.svg" << std::endl;
        return;
    }

    // SVG头部
    outfile << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << std::endl;

    // 绘制线段
    for (const auto &line : lines) {
        const Point &p1 = points[line.p1_index];
        const Point &p2 = points[line.p2_index];
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
