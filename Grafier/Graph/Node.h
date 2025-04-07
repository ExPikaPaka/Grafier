#pragma once
#include <vector>
#include <tuple>
#include "../Utility/DataTypes.h"

struct Node {
	Node() {};
	Node(i32v2 center, int size = 0, f32v4 color = {1,1,1,1}, i32 id = 0, i32 weight = 0, i32 totalWeight = -1):
		center(center), color(color), size(size), id(id), totalWeight(totalWeight) {};

	bool isTotalWeightSet();

	i32v2 center = {0, 0};
	f32v4 color = {0, 0, 0, 1};
	i32 size = 0;

	i32 id = 0;
	i32 totalWeight = -1;
	std::vector<std::tuple<int, int, bool, bool>> weights; // Destination id, weight, optimal, processed
};