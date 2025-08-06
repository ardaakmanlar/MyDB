#include "Schema.h"


Column::Column(const std::string& _name, const std::string& _type):name(_name), type(_type) {}



Row::Row(const std::vector<std::string>& _values): values(_values) {}