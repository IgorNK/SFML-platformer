#pragma once
#include <vector>
#include <map>
#include <memory>
#include "component.h"
#include "tag.h"
#include <tuple>

typedef std::tuple<
	CTransform,
	CBoundingBox
	> Components;

class Entity {
	friend class EntityManager;
	const size_t m_id = 0;
	const Tag m_tag;
	Components m_components;
	bool m_is_alive = true;
public:
	
	Entity(const Tag tag, const size_t id) : m_tag(tag), m_id(id) {}
	
	const size_t id() {
		return m_id;
	}

	Tag tag() {
		return m_tag;
	}

	template <typename T>
	T & getComponent() {
		return std::get<T>(m_components);
	}

	template <typename T>
	const T & getComponent() const {
		return std::get<T>(m_components);
	}

	template <typename T>
	bool hasComponent() const {
		return getComponent<T>().has;
	}

	template <typename T, typename... TArgs>
	T & addComponent(TArgs&&... mArgs) {
		auto & component = getComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	void destroy() {
		m_is_alive = false;
	}

	bool is_alive() {
		return m_is_alive;
	}
};