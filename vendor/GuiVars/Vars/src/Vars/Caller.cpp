#include <Vars/Caller.h>
#include <Vars/Vars.h>

vars::Caller::Caller(vars::Vars&v,std::string const&n):vars(v){
  vars.pushCallerName(n);
}

vars::Caller::~Caller(){
  vars.popCallerName();
}
