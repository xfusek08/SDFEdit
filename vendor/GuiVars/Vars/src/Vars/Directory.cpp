#include <Vars/Directory.h>
#include <tuple>

using namespace vars;

tuple<string,string> splitPathToFirstDirAndRest(string const&fullPath){
  auto pos = fullPath.find(".");
  if(pos == string::npos)return tuple<string,string>("",fullPath);
  return tuple<string,string>(fullPath.substr(0,pos),fullPath.substr(pos+1));
}


void Directory::add(string const&fullPath){
  auto split = splitPathToFirstDirAndRest(fullPath);
  auto const&dir  = std::get<0>(split);
  auto const&rest = std::get<1>(split);
  if(dir == ""){
    vars.insert(rest);
    return;
  }
  if(dirs.count(dir) == 0)
    dirs[dir] = {};
  dirs.at(dir).add(rest);
}

bool Directory::hasDir(string const&d)const{
  return dirs.count(d) > 0;
}

bool Directory::hasVar(string const&v)const{
  return vars.count(v) > 0;
}

Directory&Directory::getDir(string const&d){
  return dirs.at(d);
}

Directory const&Directory::getDir(string const&d)const{
  return dirs.at(d);
}

set<string>decorate(string const&dir,set<string>const&vars){
  set<string>result;
  for(auto const&v:vars)
    result.insert(dir+"."+v);
  return result;
}

void append(set<string>&target,set<string>const&vars){
  for(auto const&v:vars)
    target.insert(v);
}

set<string>Directory::getVars(string const&fullPath)const{
  if(fullPath == ""){
    set<string>result = vars;
    for(auto const&d:dirs)
      append(result,decorate(d.first,d.second.getVars()));
    return result;
  }

  auto const split = splitPathToFirstDirAndRest(fullPath);
  auto const&dir   = std::get<0>(split);
  auto const&rest  = std::get<1>(split);

  if(dir == ""){
    if(!isDir(rest))return {};
    return decorate(rest,getDir(rest).getVars());
  }

  if(!isDir(dir))return {};
  return decorate(dir,getDir(dir).getVars(rest));
}

bool Directory::isDir(string const&fullPath)const{
  auto const split = splitPathToFirstDirAndRest(fullPath);
  auto const&dir  = std::get<0>(split);
  auto const&rest = std::get<1>(split);

  if(dir == "")
    return dirs.count(rest) > 0;

  if(dirs.count(dir) == 0)return false;

  return dirs.at(dir).isDir(rest);
}

bool Directory::isVar(string const&fullPath)const{
  auto const split = splitPathToFirstDirAndRest(fullPath);
  auto const&dir  = std::get<0>(split);
  auto const&rest = std::get<1>(split);

  if(dir == "")
    return vars.count(rest) > 0;

  if(dirs.count(dir) == 0)return false;

  return dirs.at(dir).isVar(rest);
}

void Directory::remove(string const&fullPath){
  removeVar(fullPath);
  removeDir(fullPath);
}

void Directory::removeDir(string const&fullPath){
  auto const split = splitPathToFirstDirAndRest(fullPath);
  auto const&dir  = std::get<0>(split);
  auto const&rest = std::get<1>(split);

  if(dir == ""){
    dirs.erase(rest);
    return;
  }

  if(dirs.count(dir) == 0)return;

  dirs.at(dir).removeDir(rest);
}

void Directory::removeVar(string const&fullPath){
  auto const split = splitPathToFirstDirAndRest(fullPath);
  auto const&dir  = std::get<0>(split);
  auto const&rest = std::get<1>(split);

  if(dir == ""){
    vars.erase(rest);
    return;
  }

  if(dirs.count(dir) == 0)return;

  dirs.at(dir).removeVar(rest);

  if(dirs.at(dir).vars.empty() && dirs.at(dir).dirs.empty())
    dirs.erase(dir);
}

