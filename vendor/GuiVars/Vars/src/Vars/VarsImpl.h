#pragma once

#include <Vars/Directory.h>
#include <Vars/Vars.h>
#include <map>

using namespace std;

class vars::VarsImpl {
 public:
  void* add(string const&     n  ,
            void*             d  ,
            Destructor const& dst,
            type_info const&  t  ,
            ResourceKind      k  );

  void             erase(string const& n);
  void             eraseDir(string const& n);
  void             eraseVar(string const& n);
  bool             isDir(string const& n) const;
  void             getDirFullNames(std::vector<std::string>&names,std::string const&n)const;
  void             getDir(std::vector<std::string>&names,std::string const&n)const;
  bool             isVar(string const& n) const;
  bool             has(string const& n) const;
  type_info const& getType(string const& n) const;
  void             checkTypes(string const& n, type_info const& t);
  void             ifVarExistsThrow(string const& n) const;
  void             ifVarDoesNotExistThrow(string const& n) const;
  void             ifVarIdDoesNotExistThrow(size_t i) const;
  ~VarsImpl();
  void*                get(string const& n) const;
  void*                getNoCheck(string const& n)const;
  void*                reCreate(string const&     n  ,
                                void*             d  ,
                                Destructor const& dst,
                                type_info const&  t  ,
                                ResourceKind      k  );
  void                 updateTicks(string const& n);
  size_t               getTicks(string const& n) const;
  ResourceKind         getKind(string const& n) const;
  void                 setChaneCallback(string const& n, OnChange const& clb);
  shared_ptr<Resource> getResource(string const& n) const;
  size_t               getNofVars() const;
  string               getVarName(size_t i) const;
  void                 pushCallerName(std::string const&n = "");
  void                 popCallerName();
  string               getFullCallerName()const;

  template <typename T>
  T& add(string const& n, T const& v, ResourceKind k);
  template <typename T>
  T& get(string const& n) const;
  template <typename T>
  T& addOrGet(string const& n, T const& v, ResourceKind k);


 protected:
  std::vector<string>callerNames;
  map<string, shared_ptr<Resource>> resources;
  Directory                         root;
  map<size_t, string>               idToName;
  map<string, size_t>               nameToId;
};

template <typename T>
T& vars::VarsImpl::add(string const& n, T const& v, ResourceKind k)
{
  auto d   = new T(v);
  auto dst = getDestructor<T>();
  return reinterpret_cast<T&>(*reinterpret_cast<T*>(add(n, d, dst, typeid(T), k)));
}

template <typename T>
T& vars::VarsImpl::get(string const& n) const
{
  return reinterpret_cast<T&>(*reinterpret_cast<T*>(get(n)));
}

template <typename T>
T& vars::VarsImpl::addOrGet(string const& n, T const& v, ResourceKind k){
  if(has(n))return get<T>(n);
  return add<T>(n,v,k);
}
