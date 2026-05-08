#include <string>
#include <unordered_map>

class SlotAllocator {
public:
  int Alloc(const std::string &name);
  int Get(const std::string &name) const;
  bool Has(const std::string &name) const;
  int Count() const;
  void Reset();

private:
  std::unordered_map<std::string, int> _slots;
  int _next = 0;
};