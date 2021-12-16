#include <Vars/Resource.h>
#include <Vars/VarsImpl.h>
#include <vector>
#include <sstream>

using namespace vars;

string VarsImpl::getFullCallerName()const{
  std::stringstream ss;

  bool first = true;
  for(auto const&x:callerNames){
    if(first)first = false;
    else ss << "::";
    ss << x;
  }

  return ss.str();
}

void VarsImpl::ifVarExistsThrow(string const& n) const
{
  if (!has(n)) return;
  std::stringstream ss;
  ss << "Vars - in function: \"" << getFullCallerName() << "\" ";
  ss << "variable: \"" << n << "\" already exists!";
  throw runtime_error(ss.str());
}

void VarsImpl::ifVarDoesNotExistThrow(string const& n) const
{
  if (has(n)) return;
  std::stringstream ss;
  ss << "Vars - in function: \"" << getFullCallerName() << "\" ";
  ss << "variable: \"" << n << "\" does not exist!";
  throw runtime_error(ss.str());
}

void* VarsImpl::add(string const&     n  ,
                    void*             d  ,
                    Destructor const& dst,
                    type_info const&  t  ,
                    ResourceKind      k  )
{
  ifVarExistsThrow(n);
  auto id      = resources.size();
  resources[n] = make_shared<Resource>(d, dst, t, k);
  idToName[id] = n;
  nameToId[n]  = id;
  root.add(n);
  return d;
}

void* VarsImpl::get(string const& n) const
{
  ifVarDoesNotExistThrow(n);
  return getNoCheck(n);
}

void* VarsImpl::getNoCheck(string const& n) const
{
  auto it = resources.find(n);
  if(it == resources.end())return nullptr;
  return it->second->getData();
}

void* VarsImpl::reCreate(string const&     n  ,
                         void*             d  ,
                         Destructor const& dst,
                         type_info const&  t  ,
                         ResourceKind      k  )
{
  if (!has(n)) return add(n, d, dst, t, k);
  return resources[n]->reCreate(d, dst, t);
}

void VarsImpl::updateTicks(string const& n)
{
  auto& d = resources.at(n);
  d->updateTicks();
}

size_t VarsImpl::getTicks(string const& n) const
{
  return resources.at(n)->getTicks();
}

ResourceKind VarsImpl::getKind(string const& n) const
{
  return resources.at(n)->getKind();
}

void VarsImpl::setChaneCallback(string const& n, OnChange const& clb)
{
  resources.at(n)->setChangeCallback(clb);
}

shared_ptr<Resource> VarsImpl::getResource(string const& n) const
{
  return resources.at(n);
}

size_t VarsImpl::getNofVars() const { return resources.size(); }


void VarsImpl::ifVarIdDoesNotExistThrow(size_t i) const{
  if (idToName.count(i) > 0) return;
  std::stringstream ss;
  ss << "Vars - in function: \"" << getFullCallerName() << "\" ";
  ss << "variable Id: \"" << i << "\" does not exist! ";
  ss << "nofVariables: " << getNofVars() << std::endl;
  throw runtime_error(ss.str());
}

string VarsImpl::getVarName(size_t i) const {
  ifVarIdDoesNotExistThrow(i);
  return idToName.at(i); 
}

void VarsImpl::pushCallerName(std::string const&n){
  callerNames.push_back(n);
}

void VarsImpl::popCallerName(){
  callerNames.pop_back();
}

void VarsImpl::erase(string const& n)
{
  eraseVar(n);
  eraseDir(n);
}

void VarsImpl::eraseDir(string const& n)
{
  if (!isDir(n)) return;
  auto forRemoval = root.getVars(n);
  for (auto const& r : forRemoval) eraseVar(r);
}

void VarsImpl::eraseVar(string const& nameToRemove)
{
  if (!isVar(nameToRemove)) return;


  auto idToRemove = nameToId.at(nameToRemove);
  auto lastId     = nameToId.size()-1;
  auto lastName   = idToName.at(lastId);

  if(lastId != idToRemove){
    nameToId.erase(nameToRemove);
    idToName.erase(idToRemove);
    nameToId.erase(lastName);
    idToName.erase(lastId);
    nameToId[lastName] = idToRemove;
    idToName[idToRemove] = lastName;
  }else{
    nameToId.erase(nameToRemove);
    idToName.erase(idToRemove);
  }

  resources.erase(nameToRemove);
  root.removeVar(nameToRemove);
}

bool VarsImpl::isDir(string const& n) const { return root.isDir(n); }

void VarsImpl::getDirFullNames(std::vector<std::string>&names,std::string const&n)const{
  names.clear();
  if(!isDir(n))return;
  auto vars = root.getVars(n);
  for(auto const&v:vars)
    names.push_back(v);
}

void VarsImpl::getDir(std::vector<std::string>&names,std::string const&n)const{
  names.clear();
  if(!isDir(n))return;
  auto vars = root.getVars(n);
  size_t const dirSeparatorSize = 1;
  for(auto const&v:vars)
    names.push_back(v.substr(n.size()+dirSeparatorSize));
}

bool VarsImpl::isVar(string const& n) const { return root.isVar(n); }

bool VarsImpl::has(string const& n) const { return resources.count(n) > 0; }

type_info const& VarsImpl::getType(string const& n) const
{
  ifVarDoesNotExistThrow(n);
  return resources.at(n)->getType();
}

void VarsImpl::checkTypes(string const& n, type_info const& t)
{
  if (getType(n) == t) return;
  stringstream ss;
  ss << "Vars - in function: \"" << getFullCallerName() << "\" ";
  ss << "variable: \"" << n << "\" has different type";
  throw runtime_error(ss.str());
}

VarsImpl::~VarsImpl()
{
  vector<string> names;
  for (auto const& x : nameToId) names.push_back(x.first);
  for (auto const& x : names) erase(x);
}
