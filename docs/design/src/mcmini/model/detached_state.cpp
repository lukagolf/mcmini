#include "mcmini/model/state/detached_state.hpp"

#include "mcmini/misc/asserts.hpp"
#include "mcmini/misc/extensions/unique_ptr.hpp"

using namespace mcmini::model;

/* `state` overrrides */
bool detached_state::contains_object_with_id(state::objid_t id) const {
  return id < this->visible_objects.size();
}

state::objid_t detached_state::track_new_visible_object(
    std::unique_ptr<visible_object_state> initial_state) {
  auto obj =
      mcmini::extensions::make_unique < visible(std::move(initial_state));
  visible_objects.push_back(std::move(obj));
  return visible_objects.size() - 1;
}

void detached_state::record_new_state_for_visible_object(
    state::objid_t id, std::unique_ptr<visible_object_state> next_state) {
  asserts::assert_condition(
      contains_object_with_id(id),
      "The object must already tracked in order to add a new state");
  this->visible_objects.at(id).push_state(std::move(next_state));
}

const visible_object_state &detached_state::get_state_of_object(
    state::objid_t id) const {
  return *this->visible_objects.at(id).get_current_state();
}

std::unique_ptr<mutable_state> detached_state::mutable_clone() const {
  return mcmini::extensions::make_unique<detached_state>(*this);
}