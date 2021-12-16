#pragma once

#include <Vars/Fwd.h>
#include <Vars/vars_export.h>
#include <string>

class vars::Caller{
  public:
    VARS_EXPORT Caller(Vars&v,std::string const&n);
    VARS_EXPORT ~Caller();
  protected:
    vars::Vars&vars;
};
