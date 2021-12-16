#pragma once

#include <Vars/Fwd.h>
#include <map>
#include <set>
#include <string>

using namespace std;

class vars::Directory{
  public:
    void add(string const&fullPath);
    void remove(string const&fullPath);
    void removeDir(string const&fullPath);
    void removeVar(string const&fullPath);
    bool hasDir(string const&d)const;
    bool hasVar(string const&v)const;
    Directory&getDir(string const&d);
    Directory const&getDir(string const&d)const;
    set<string>getVars(string const&d = "")const;
    bool isDir(string const&fullPath)const;
    bool isVar(string const&fullPath)const;
    map<string,Directory>dirs;
    set<string>vars;
};

