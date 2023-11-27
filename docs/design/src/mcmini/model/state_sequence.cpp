#include "mcmini/model/state/state_sequence.hpp"

#include "mcmini/misc/asserts.hpp"
#include "mcmini/misc/extensions/unique_ptr.hpp"
#include "mcmini/model/state/detached_state.hpp"

using namespace mcmini::model;

state_sequence::state_sequence(std::vector<visible_object> &&initial_objects)
    : visible_objects(std::move(initial_objects)) {
  this->states_in_sequence.push_back(state_sequence::element(*this));
}

bool state_sequence::contains_object_with_id(visible_object::objid_t id) const {
  return id < visible_objects.size();
}

visible_object::objid_t state_sequence::track_new_visible_object(
    std::unique_ptr<visible_object_state> initial_state) {
  visible_object obj = visible_object(std::move(initial_state));
  visible_objects.push_back(std::move(obj));
  return visible_objects.size() - 1;
}

void state_sequence::record_new_state_for_visible_object(
    visible_object::objid_t id,
    std::unique_ptr<visible_object_state> next_state) {
  asserts::assert_condition(
      contains_object_with_id(id),
      "The object must already tracked in order to add a new state");
  this->visible_objects.at(id).push_state(std::move(next_state));
}

const visible_object_state &state_sequence::get_state_of_object(
    visible_object::objid_t id) const {
  return *this->visible_objects.at(id).get_current_state();
}

std::unique_ptr<mutable_state> state_sequence::mutable_clone() const {
  return state::from_visible_objects<detached_state>(
      this->visible_objects.cbegin(), this->visible_objects.cend());
}

state_sequence state_sequence::consume_into_subsequence(size_t index) && {}

state_sequence::element::element(const state_sequence &owner) {
  for (const visible_object &obj : owner.visible_objects) {
    this->visible_object_states.push_back(obj.get_current_state());
  }
}

bool state_sequence::element::contains_object_with_id(
    visible_object::objid_t id) const {
  return id < this->visible_object_states.size();
}

const visible_object_state &state_sequence::element::get_state_of_object(
    visible_object::objid_t id) const {
  return *this->visible_object_states.at(id);
}

std::unique_ptr<mutable_state> state_sequence::element::mutable_clone() const {
  return state::from_visible_object_states<detached_state>(
      this->visible_object_states.cbegin(), this->visible_object_states.cend());
}