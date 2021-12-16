cmake_minimum_required (VERSION 3.18)

set(SOURCES
  Vars/src/Vars/Vars.cpp
  Vars/src/Vars/VarsImpl.cpp
  Vars/src/Vars/Resource.cpp
  Vars/src/Vars/Directory.cpp
  Vars/src/Vars/Caller.cpp
  
  imguiVars/src/imguiVars/imguiVars.cpp
  imguiVars/src/imguiVars/addVarsLimits.cpp
  )
set(PRIVATE_INCLUDES
  Vars/src/Vars/vars_export.h
  Vars/src/Vars/VarsImpl.h
  Vars/src/Vars/Directory.h
  )
set(PUBLIC_INCLUDES
  Vars/src/Vars/Fwd.h
  Vars/src/Vars/Vars.h
  Vars/src/Vars/Resource.h
  Vars/src/Vars/Caller.h
  
  imguiVars/src/imguiVars/imguivars_export.h
  imguiVars/src/imguiVars/imguiVars.h
  imguiVars/src/imguiVars/addVarsLimits.h
  )

add_library(GuiVars ${SOURCES} ${PRIVATE_INCLUDES} ${PUBLIC_INCLUDES})
target_include_directories(GuiVars
    PUBLIC
    Vars/src/
    imguiVars/src/
)

target_link_libraries(GuiVars
    ImGui
    glm
)
