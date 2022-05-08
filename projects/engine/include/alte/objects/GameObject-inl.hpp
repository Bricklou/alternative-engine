#pragma once

#include <alte/objects/GameObject.hpp>

namespace AltE {
  template <typename T> bool GameObject::has_component() {
    return _ecs->_registry.any_of<T>(_entity_handle);
  }

  template <typename T, typename... Targs>
  T &GameObject::add_component(Targs &&...args) {
    return _ecs->_registry.emplace<T>(_entity_handle,
                                      std::forward<Targs>(args)...);
  }

  template <typename T> T &GameObject::get_component() {
    return _ecs->_registry.get<T>(_entity_handle);
  }

  template <typename T> void GameObject::remove_component() {
    _ecs->_registry.remove<T>(_entity_handle);
  }
}; // namespace AltE