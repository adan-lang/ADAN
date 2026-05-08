#include <stdexcept>
#include <string>

#include "slot_alloc.hpp"

int SlotAllocator::Alloc(const std::string &name) {
  if (Has(name)) {
    return _slots.at(name);
  }
  int slot = _next++;
  _slots[name] = slot;
  return slot;
}

int SlotAllocator::Get(const std::string &name) const {
  if (!Has(name)) {
    throw std::runtime_error("Cannot find entry " + name);
  }
  return _slots.at(name);
}

bool SlotAllocator::Has(const std::string &name) const {
  return SlotAllocator::_slots.find(name) != SlotAllocator::_slots.end();
}

int SlotAllocator::Count() const { return _next; }

void SlotAllocator::Reset() {
  _slots.clear();
  _next = 0;
}