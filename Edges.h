#pragma once

class Edges {
	int edges[4];
public:
	Edges() {
		for (int x = 0; x < 4; x++) {
			edges[x] = -1;
		}
	}
	Edges(int up, int right, int down, int left) {
		edges[0] = up;
		edges[1] = right;
		edges[2] = down;
		edges[3] = left;
	}
	Edges(int* _edges) {
		for (int x = 0; x < 4; x++) {
			edges[x] = _edges[x];
		}
	}
	void changeEdges(int up, int right, int down, int left) {
		edges[0] = up;
		edges[1] = right;
		edges[2] = down;
		edges[3] = left;
	}
	int operator[](int pos) {
		return edges[pos];
	}
	int getUp() {
		return edges[0];
	}
	int getRight() {
		return edges[1];
	}
	int getDown() {
		return  edges[2];
	}
	int getLeft() {
		return edges[3];
	}
	~Edges() {

	}
};
