#pragma once

#include <functional>

namespace vars {
class VarsImpl;
class Directory;
class Vars;
class Resource;
class Caller;
enum class ResourceKind{BASIC,ENUM,CLASS,VECTOR,MAP};

using Destructor = std::function<void(void*)>;
using OnChange   = std::function<void()>;

template<typename T>
inline Destructor getDestructor();
}  // namespace vars
