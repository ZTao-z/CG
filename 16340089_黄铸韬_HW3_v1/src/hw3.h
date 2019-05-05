#ifndef bresenham_h
#define bresenham_h

// ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// GLEW/GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// OTHER
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

class Bresenham {
private:
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
public:
	Bresenham(int w, int h) :WINDOW_WIDTH(w), WINDOW_HEIGHT(h) {};

	// 保存直线上的点
	void saveLinePoint(int temp_X, int temp_Y, std::vector<GLfloat>& out) {
		GLfloat color = 1.0f;
		out.push_back(GLfloat(float(temp_X) / float(WINDOW_WIDTH / 2)) - 1.0f);
		out.push_back(1.0f - GLfloat(float(temp_Y) / float(WINDOW_HEIGHT / 2)));
		out.push_back(0.0f);
		for (int i = 0; i < 3; i++) {
			out.push_back(color);
		}
	}
	// 保存圆上的点
	void saveCirclePoint(int temp_X, int temp_Y, std::vector<GLfloat>& out) {
		GLfloat color = 1.0f;
		out.push_back(GLfloat(float(temp_X) / float(WINDOW_WIDTH / 2)));
		out.push_back(GLfloat(float(temp_Y) / float(WINDOW_HEIGHT / 2)));
		out.push_back(0.0f);
		for (int i = 0; i < 3; i++) {
			out.push_back(color);
		}
	}
	// Bresenham画直线
	void drawLineWithBresenham(GLint x0, GLint y0,
		GLint x1, GLint y1,
		std::vector<GLfloat>& out) {
		int dx = abs(x1 - x0);
		int dy = abs(y1 - y0);
		int direct_x = x1 > x0 ? 1 : -1;
		int direct_y = y1 > y0 ? 1 : -1;
		int p = 0;

		if (dx > dy) {
			p = 2 * dy - dx;
			while (x0 != x1) {
				this->saveLinePoint(x0, y0, out);
				if (p > 0) {
					y0 += direct_y;
					p = p + 2 * (dy - dx);
				}
				else {
					p = p + 2 * dy;
				}
				x0 += direct_x;
			}
		}
		else {
			p = 2 * dx - dy;
			while (y0 != y1) {
				this->saveLinePoint(x0, y0, out);
				if (p > 0) {
					x0 += direct_x;
					p = p + 2 * (dx - dy);
				}
				else {
					p = p + 2 * dx;
				}
				y0 += direct_y;
			}
		}
	}
	// Bresenham画圆
	void drawCircleWithBresenham(int center_x, int center_y, int radius, std::vector<GLfloat>& out) {
		int x = 0, y = radius, d = 3 - 2 * radius;
		while (x <= y) {
			// 八点法画圆
			this->saveCirclePoint( x + center_x,  y + center_y, out);
			this->saveCirclePoint( x + center_x, -y + center_y, out);
			this->saveCirclePoint(-y + center_x,  x + center_y, out);
			this->saveCirclePoint(-y + center_x, -x + center_y, out);
			this->saveCirclePoint(-x + center_x, -y + center_y, out);
			this->saveCirclePoint(-x + center_x,  y + center_y, out);
			this->saveCirclePoint( y + center_x, -x + center_y, out);
			this->saveCirclePoint( y + center_x,  x + center_y, out);
			if (d < 0) {
				d = d + 4 * x + 6;
			}
			else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
	// 计算直线一般式：Ax+By+C=0
	std::vector<int> lineEquation(GLint X1, GLint Y1, GLint X2, GLint Y2) {
		std::vector<int> res;
		int A = Y2 - Y1,
			B = X1 - X2,
			C = X2 * Y1 - X1 * Y2;
		res.push_back(A);
		res.push_back(B);
		res.push_back(C);
		return res;
	}
	// 使用Edge Equation方法填充颜色
	void fillTriangleUsingEdgeEquation(GLint Xa, GLint Ya,
		GLint Xb, GLint Yb,
		GLint Xc, GLint Yc,
		std::vector<GLfloat>& out) {
		// 获取外接矩形
		int max_x = std::max(Xa, std::max(Xb, Xc)),
			max_y = std::max(Ya, std::max(Yb, Yc)),
			min_x = std::min(Xa, std::min(Xb, Xc)),
			min_y = std::min(Ya, std::min(Yb, Yc));
		// 获取三角形三条边
		std::vector<std::vector<int>> lines;
		lines.push_back(this->lineEquation(Xa, Ya, Xb, Yb));
		lines.push_back(this->lineEquation(Xb, Yb, Xc, Yc));
		lines.push_back(this->lineEquation(Xc, Yc, Xa, Ya));
		// 确定直线方程的方向
		for (int i = 0; i < 3; i++) {
			GLint x_t, y_t;
			if (i == 0) {
				x_t = Xc; y_t = Yc;
			}
			else if (i == 1) {
				x_t = Xa; y_t = Ya;
			}
			else {
				x_t = Xb; y_t = Yb;
			}
			if (lines[i][0] * x_t + lines[i][1] * y_t + lines[i][2] < 0) {
				for (int k = 0; k < 3; k++) {
					lines[i][k] *= -1;
				}
			}
		}
		// 填充颜色
		for (GLint x = min_x; x <= max_x; x++) {
			for (GLint y = min_y; y <= max_y; y++) {
				bool inside = true;
				for (int i = 0; i < lines.size(); i++) {
					if (lines[i][0] * x + lines[i][1] * y + lines[i][2] < 0) {
						inside = false;
						break;
					}
				}
				if (inside) {
					this->saveLinePoint(x, y, out);
				}
			}
		}
	}
	// 画三角形
	void drawTriangle(GLfloat Xa, GLfloat Ya,
					  GLfloat Xb, GLfloat Yb,
					  GLfloat Xc, GLfloat Yc,
					  std::vector<GLfloat>& res,
					  bool fullfill = false) {
		GLint Xa_t = (GLint)std::floor((Xa + 1)*WINDOW_WIDTH) / 2,
			Ya_t = (GLint)std::floor((1 - Ya)*WINDOW_HEIGHT) / 2,
			Xb_t = (GLint)std::floor((Xb + 1)*WINDOW_WIDTH) / 2,
			Yb_t = (GLint)std::floor((1 - Yb)*WINDOW_HEIGHT) / 2,
			Xc_t = (GLint)std::floor((Xc + 1)*WINDOW_WIDTH) / 2,
			Yc_t = (GLint)std::floor((1 - Yc)*WINDOW_HEIGHT) / 2;
		drawLineWithBresenham(Xa_t, Ya_t, Xb_t, Yb_t, res);
		drawLineWithBresenham(Xb_t, Yb_t, Xc_t, Yc_t, res);
		drawLineWithBresenham(Xc_t, Yc_t, Xa_t, Ya_t, res);

		if (fullfill) {
			fillTriangleUsingEdgeEquation(Xa_t, Ya_t, Xb_t, Yb_t, Xc_t, Yc_t, res);
		}
	}
	// 画圆
	void drawCircle(GLfloat x, GLfloat y, int radius, std::vector<GLfloat>& res) {
		int center_x = std::floor(x * WINDOW_WIDTH / 2),
			center_y = std::floor(y * WINDOW_HEIGHT / 2);
		drawCircleWithBresenham(center_x, center_y, radius, res);
	}
};

#endif // !bresenham_h