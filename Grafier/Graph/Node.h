#pragma once
#include <vector>
#include <utility>

class Node {
public:
	Node() {};
	Node(i32v2 center, int size = 0, f32v4 color = {1,1,1,1}, int id = 0, int weight = 0):
		center(center), color(color), size(size), id(id) {};
		
public:
	i32v2 center;
	f32v4 color;
	int size = 0;

	int id = 0;
	std::vector<std::pair<int, int>> weights; // Source id, weight
};