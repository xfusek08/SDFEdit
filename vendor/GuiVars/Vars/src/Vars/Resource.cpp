#include <Vars/Resource.h>

using namespace vars;
using namespace std;

vars::Resource::Resource(void* d, Destructor const& dst, type_info const&t,ResourceKind k)
    : data(d), destructor(dst), type(t), kind(k) {}

vars::Resource::~Resource() { destructor(data); }

void vars::Resource::updateTicks() {
  ticks++;
  if (changeCallback) changeCallback();
}

size_t vars::Resource::getTicks() const { return ticks; }

type_info const& vars::Resource::getType() const { return type; }

void vars::Resource::setChangeCallback(OnChange const& clb) {
  changeCallback = clb;
}

void* vars::Resource::getData() const { return data; }

void* vars::Resource::reCreate(void*d,Destructor const&dst,std::type_info const&t){
  destructor(data);
  auto const oldTicks = ticks;
  new(this)Resource(d,dst,t,kind);
  ticks = oldTicks;
  updateTicks();
  return d;
}

vars::ResourceKind vars::Resource::getKind()const { return kind; }
