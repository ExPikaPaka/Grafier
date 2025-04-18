#pragma once
#include <vector>
#include "../Utility/DataTypes.h"

struct Weight {
	Weight(int dest, int weight, bool optimal, bool processed):
		dest(dest), weight(weight), optimal(optimal), processed(processed) {};

	int dest = -1;
	int weight = -1;
	bool optimal = false;
	bool processed = false;
	bool targetPath = false;
};

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
	bool hasPath = false;
	std::vector<Weight> weights; // Destination id, weight, optimal, processed
};