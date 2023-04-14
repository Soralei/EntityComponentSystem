#pragma once

#include <iostream>
#include <set>

using EntityId = std::size_t;

class Entity {
	static inline std::set<EntityId> sPriorityId {};
	static inline EntityId sUniqueId {};
	EntityId mUniqueId;

public:
	Entity() {
		if(!sPriorityId.empty()) {
			auto it = sPriorityId.begin();
			mUniqueId = *it;
			sPriorityId.erase(it);
		}
		else {
			mUniqueId = sUniqueId++;
		}
	}

	~Entity() {
		sPriorityId.insert(mUniqueId);
	}

	EntityId GetUniqueId() {
		return mUniqueId;
	}
};