#pragma once

#include "Component.h"
#include "Entity.h"

#include <memory>
#include <vector>
#include <unordered_map>

// Features:
// Add entities [X]
// Add entity components [X]
// Access specific components of an entity [X]
// Iterate an entities components [X]
// Iterate components of a particular type [X]
// Remove components from entities [X]
// Remove entities [X]

// NOTE:
// When looping through components, the pointers must be validated,
// and the vector must be cleaned up if any empty pointers are found to keep it packed.
// This is because when a component is removed, it only resets the unique_ptr and does not touch the sAllComponents vector storing them

namespace ECS {
	class family {
		static std::size_t identifier() noexcept {
			static std::size_t value = 0;
			return value++;
		}

	public:
		template<typename>
		static std::size_t type() noexcept {
			static const std::size_t value = identifier();
			return value;
		}
	};

	using EntityId = std::size_t;
	using ComponentId = std::size_t;

	using Components = std::vector<ComponentId>;

	using UniqueEntity = std::unique_ptr<Entity>;
	using UniqueComponent = std::unique_ptr<Component>;
	using UniqueComponents = std::vector<UniqueComponent>;
	using UniqueComponentPtrs = std::vector<UniqueComponent*>;

	std::vector<UniqueEntity> sAllEntities;						// A vector to store all the unique entities
	std::vector<UniqueComponents> sAllComponents;				// A vector to hold all unique components, grouped by component id.
	std::vector<UniqueComponentPtrs> sEntityComponents;			// A vector which tracks an entities component by storing a pointer to it's unique component. Used for direct access.

	Entity* CreateEntity() {
		UniqueEntity ent = std::make_unique<Entity>();
		EntityId eId = ent.get()->GetUniqueId();

		if(eId >= sAllEntities.size()) {
			sAllEntities.resize(eId + 1);
		}

		sAllEntities[eId] = std::move(ent);

		return sAllEntities[eId].get();
	}

	bool DestroyEntity(EntityId entityUID) {

		if(entityUID < sAllEntities.size()) {
			if(sAllEntities[entityUID].get() != nullptr) {

				for(auto& component : sEntityComponents[entityUID]) {
					component->reset();
				}
				sEntityComponents[entityUID].resize(0);

				sAllEntities[entityUID].reset();
				return true;
			}
		}
		return false;
	}

	template <typename C>
	bool HasComponent(EntityId entityUID) {
		if(entityUID < sEntityComponents.size()) {
			auto& comps = sEntityComponents[entityUID];
			ComponentId cId = family::type<C>();
			if(cId < comps.size()) {
				return sEntityComponents[entityUID][cId]->get() != nullptr;
			}
		}
		return false;
	}

	template <typename C>
	C* GetComponent(EntityId entityUID) {
		if(HasComponent<C>(entityUID)) {
			return static_cast<C*>(sEntityComponents[entityUID][family::type<C>()]->get());
		}
		return nullptr;
	}

	// Currently possible to add components to non-existing entities. Feature or bug? TBD.
	template <typename C, typename... CArgs>
	C* AddComponent(EntityId entityUID, CArgs... args) {
		if(entityUID >= sEntityComponents.size()) {
			sEntityComponents.resize(entityUID + 1);
		}

		auto& comps = sEntityComponents[entityUID];
		ComponentId cId = family::type<C>();

		if(cId >= comps.size()) {
			comps.resize(cId + 1);
		}

		if(cId >= sAllComponents.size()) {
			sAllComponents.resize(cId + 1);
		}

		if(sEntityComponents[entityUID][cId] == nullptr) {
			auto& result = sAllComponents[cId].emplace_back(std::make_unique<C>(std::forward(args)...));
			sEntityComponents[entityUID][cId] = &result;
			return static_cast<C*>(result.get());
		}

		return nullptr;
	}

	template <typename C>
	bool RemoveComponent(EntityId entityUID) {
		if(HasComponent<C>(entityUID)) {
			ComponentId cId = family::type<C>();
			sEntityComponents[entityUID][cId]->reset(); // Removes the actual component from the entity.
			return true;
		}
		return false;
	}
}