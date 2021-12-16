#pragma once

#include <Vars/Fwd.h>
#include <Vars/vars_export.h>

class vars::Resource {
 public:
  VARS_EXPORT Resource(void* d, Destructor const& dst, std::type_info const&t,ResourceKind kind);
  VARS_EXPORT ~Resource();
  VARS_EXPORT void                  updateTicks();
  VARS_EXPORT size_t                getTicks() const;
  VARS_EXPORT std::type_info const& getType() const;
  VARS_EXPORT void                  setChangeCallback(OnChange const& clb);
  VARS_EXPORT void*                 getData() const;
  VARS_EXPORT void*                 reCreate(void*d,Destructor const&dst,std::type_info const&t);
  VARS_EXPORT ResourceKind          getKind()const;

 private:
  void*                 data;
  size_t                ticks          = 1;
  OnChange              changeCallback = nullptr;
  Destructor            destructor;
  std::type_info const& type;
  ResourceKind          kind;
};
