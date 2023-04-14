#pragma once

#include <iostream>
#include <vector>

namespace WorldGeneration {
	int DEFAULT_WIDTH = 25;
	int DEFAULT_HEIGHT = 25;
}

struct TileData {
	int posX {};
	int posY {};
	bool blocked {};
};

class WorldGenerator {
	std::vector<TileData> mTileData {};
	int mWidth {};
	int mHeight {};
public:
	WorldGenerator(int width = WorldGeneration::DEFAULT_WIDTH, int height = WorldGeneration::DEFAULT_HEIGHT);

	int GetWidth();
	int GetHeight();
	TileData* GetTile(int index);

	void DebugPrintMap() {
		for(int i = 0; i < mTileData.size(); i++) {
			if(i % mWidth == 0 && i != 0) {
				std::cout << '\n';
			}
			std::cout << (mTileData[i].blocked ? "#" : " . ");
		}
	}
};

WorldGenerator::WorldGenerator(int width, int height) {
	int totalSize = width*height;

	mWidth = width;
	mHeight = height;

	for(int i = 0; i < totalSize; i++) {
		int x = i % width;
		int y = i / height;
		mTileData.push_back( {x, y} );
	}
}

int WorldGenerator::GetWidth() {
	return mWidth;
}

int WorldGenerator::GetHeight() {
	return mHeight;
}

TileData* WorldGenerator::GetTile(int index) {
	if(index >= mTileData.size()) {
		return nullptr;
	}
	return &mTileData[index];
}