
#include "DMLuaManager.h"

#include <sstream>
#include <iostream>

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"

#ifdef _MSC_VER
# pragma warning (disable: 4800)
#endif

#define api_incr_top(L)   {api_check(L, L->top < L->ci->top); L->top++;}

typedef unsigned char BYTE;

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//Методы класса DMLuaManager
//---------------------------------------------------------------------------
const char * DMLuaManager::ReadScalarString(const std::string& Path,lua_State * VM)
{
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarString]: Enter");

//Log::Instance().WriteInfo("DMLuaManager[ReadScalarString]: Path = [%s]",Path.c_str());

std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
const char * data=lua_tostring(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarString]: Exit");
return data;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
const char * data=lua_tostring(VM,-1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarString]: Exit");
return data;
}

//Log::Instance().WriteDebug("DMLuaManager[ReadScalarString]: Exit (0)");
return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::ReadScalarInt(const std::string& Path,lua_State * VM)
{
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarInt]: Enter");

//Log::Instance().WriteInfo("DMLuaManager[ReadScalarInt]: Path = [%s]",Path.c_str());

std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
int data=lua_tointeger(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarInt]: Exit");
return data;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
int data=lua_tointeger(VM,-1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarInt]: Exit");
return data;
}

//Log::Instance().WriteDebug("DMLuaManager[ReadScalarInt]: Exit (0)");
return 0;
}
//---------------------------------------------------------------------------
bool DMLuaManager::ReadScalarBool(const std::string& Path,lua_State * VM)
{
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarBool]: Enter");

//Log::Instance().WriteInfo("DMLuaManager[ReadScalarBool]: Path = [%s]",Path.c_str());

std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
bool data=static_cast<bool>(lua_toboolean(VM,-1));
lua_remove(VM, -1);
lua_remove(VM, -1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarBool]: Exit");
return data;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
bool data=static_cast<bool>(lua_toboolean(VM,-1));
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarBool]: Exit");
return data;
}

//Log::Instance().WriteDebug("DMLuaManager[ReadScalarBool]: Exit (0)");
return 0;
}
//---------------------------------------------------------------------------
/*int DMLuaManager::ReadScalarInt_Table(const void * LuaTable,std::string& Name,lua_State * VM)
{

lua_lock(VM);
sethvalue(VM,VM->top, LuaTable);
api_incr_top(VM);
lua_unlock(VM);
lua_getfield(VM,-1,Name.c_str());
int data=lua_tointeger(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
return data;
}*/
//---------------------------------------------------------------------------
float DMLuaManager::ReadScalarFloat(const std::string& Path,lua_State * VM)
{
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarFloat]: Enter");

//Log::Instance().WriteInfo("DMLuaManager[ReadScalarFloat]: Path = [%s]",Path.c_str());

std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
float data=static_cast<float>(lua_tonumber(VM,-1));
lua_remove(VM, -1);
lua_remove(VM, -1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarFloat]: Exit");
return data;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
float data=static_cast<float>(lua_tonumber(VM,-1));
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarFloat]: Exit");
return data;
}

//Log::Instance().WriteDebug("DMLuaManager[ReadScalarFloat]: Exit (0)");
return 0;
}
//---------------------------------------------------------------------------
double DMLuaManager::ReadScalarDouble(const std::string& Path,lua_State * VM)
{
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarDouble]: Enter");

//Log::Instance().WriteInfo("DMLuaManager[ReadScalarDouble]: Path = [%s]",Path.c_str());

std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
double data=lua_tonumber(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarDouble]: Exit");
return data;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
double data=lua_tonumber(VM,-1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarDouble]: Exit");
return data;
}

//Log::Instance().WriteDebug("DMLuaManager[ReadScalarDouble]: Exit (0)");
return 0;
}
//---------------------------------------------------------------------------
void * DMLuaManager::ReadScalarVoid(const std::string& Path,lua_State * VM)
{
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarVoid]: Enter");

//Log::Instance().WriteInfo("DMLuaManager[ReadScalarVoid]: Path = [%s]",Path.c_str());

std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
void * data=lua_touserdata(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarVoid]: Exit");
return data;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
void * data=lua_touserdata(VM,-1);
//Log::Instance().WriteDebug("DMLuaManager[ReadScalarVoid]: Exit");
return data;
}

//Log::Instance().WriteDebug("DMLuaManager[ReadScalarVoid]: Exit (0)");
return 0;
}
//---------------------------------------------------------------------------
const char * DMLuaManager::ReadVectorString(const std::string& Path,lua_State * VM,int index)
{

if(FindTable(Path,VM)==0)
{
lua_rawgeti(VM, -1, index);
const char * data=lua_tostring(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
return data;
}
return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::ReadVectorInt(const std::string& Path,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
lua_rawgeti(VM, -1, index);
int data=lua_tointeger(VM,-1);
lua_remove(VM, -1);
lua_remove(VM, -1);
return data;
}
return -1;
}
//---------------------------------------------------------------------------
float DMLuaManager::ReadVectorFloat(const std::string& Path,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
lua_rawgeti(VM, -1, index);
float data=static_cast<float>(lua_tonumber(VM,-1));
lua_remove(VM, -1);
lua_remove(VM, -1);
return data;
}
return -1;
}
//---------------------------------------------------------------------------
double DMLuaManager::ReadVectorDouble(const std::string& Path,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
lua_rawgeti(VM, -1, index);
float data=static_cast<float>(lua_tonumber(VM,-1));
lua_remove(VM, -1);
lua_remove(VM, -1);
return data;
}
return -1;
}
//---------------------------------------------------------------------------
void DMLuaManager::Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters, bool trimEmpty)
{
   std::string::size_type pos, lastPos = 0;

   while(true)
   {
      pos = str.find_first_of(delimiters, lastPos);
      if(pos == std::string::npos)
      {
         pos = str.length();

         if(pos != lastPos || !trimEmpty)
             tokens.push_back(std::string(str.data()+lastPos, pos-lastPos ));

         break;
      }
      else
      {
         if(pos != lastPos || !trimEmpty)
            tokens.push_back(std::string(str.data()+lastPos, pos-lastPos ));
      }

      lastPos = pos + 1;
   }
}
//---------------------------------------------------------------------------
const char * DMLuaManager::ReadVectorComplicatedString(const std::string& Path, const std::string& Path2,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
    lua_rawgeti(VM, -1, index);

    std::string NewPath=GetScalarPath(Path2);
    std::string Token=GetScalarName(Path2);
    
    std::vector<std::string> elems;
    Tokenize(NewPath, elems, ".");

    for(size_t q = 0; q < elems.size(); ++q)
    {
        lua_pushstring(VM,elems[q].c_str());
        lua_gettable(VM,-2);

        lua_remove(VM, -3);
        lua_remove(VM, -2);
    }

    

    lua_pushstring(VM,Token.c_str());
    lua_gettable(VM,-2);

    const char * data=lua_tostring(VM,-1);

    lua_remove(VM, -1);
    lua_remove(VM, -1);

    return data;
}
return 0;
}
//---------------------------------------------------------------------------
float DMLuaManager::ReadVectorComplicatedFloat(const std::string& Path, const std::string& Path2,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
    lua_rawgeti(VM, -1, index);

    std::string NewPath=GetScalarPath(Path2);
    std::string Token=GetScalarName(Path2);
    
    std::vector<std::string> elems;
    Tokenize(NewPath, elems, ".");

    for(size_t q = 0; q < elems.size(); ++q)
    {
        lua_pushstring(VM,elems[q].c_str());
        lua_gettable(VM,-2);

        lua_remove(VM, -3);
        lua_remove(VM, -2);
    }

    

    lua_pushstring(VM,Token.c_str());
    lua_gettable(VM,-2);

    float data=static_cast<float>(lua_tonumber(VM,-1));

    lua_remove(VM, -1);
    lua_remove(VM, -1);

    return data;
}
return -1;
}
//---------------------------------------------------------------------------
bool DMLuaManager::ReadVectorComplicatedBool(const std::string& Path, const std::string& Path2,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
    lua_rawgeti(VM, -1, index);

    std::string NewPath=GetScalarPath(Path2);
    std::string Token=GetScalarName(Path2);
    
    std::vector<std::string> elems;
    Tokenize(NewPath, elems, ".");

    for(size_t q = 0; q < elems.size(); ++q)
    {
        lua_pushstring(VM,elems[q].c_str());
        lua_gettable(VM,-2);

        lua_remove(VM, -3);
        lua_remove(VM, -2);
    }

    

    lua_pushstring(VM,Token.c_str());
    lua_gettable(VM,-2);

    bool data=static_cast<bool>(lua_toboolean(VM,-1));

    lua_remove(VM, -1);
    lua_remove(VM, -1);

    return data;
}
return 0;
}
//---------------------------------------------------------------------------
void DMLuaManager::CopyVectorString(std::string& Path,lua_State * VM,const char ** massive)
{
int Size=GetVectorSize(Path,VM);
if(FindTable(Path,VM)==0)
{
const char * msg;
                for(int z=1;z<=Size;z++)
                {
                lua_rawgeti(VM, -1, z);
                msg=lua_tostring(VM,-1);
                massive[z-1]=msg;
                lua_remove(VM, -1);
                }
lua_remove(VM, -1);
}
return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CopyVectorInt(std::string& Path,lua_State * VM,int * massive)
{
int Size=GetVectorSize(Path,VM);
if(FindTable(Path,VM)==0)
{
int data;
                for(int z=1;z<=Size;z++)
                {
                lua_rawgeti(VM, -1, z);
                data=lua_tointeger(VM,-1);
                massive[z-1]=data;
                lua_remove(VM, -1);
                }
lua_remove(VM, -1);
}
return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CopyVectorFloat(std::string& Path,lua_State * VM,float * massive)
{
int Size=GetVectorSize(Path,VM);
if(FindTable(Path,VM)==0)
{
float data;
                for(int z=1;z<=Size;z++)
                {
                lua_rawgeti(VM, -1, z);
                data=static_cast<float>(lua_tonumber(VM,-1));
                massive[z-1]=data;
                lua_remove(VM, -1);
                }
lua_remove(VM, -1);
}
return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CopyVectorDouble(std::string& Path,lua_State * VM,double * massive)
{
int Size=GetVectorSize(Path,VM);
if(FindTable(Path,VM)==0)
{
double data;
                for(int z=1;z<=Size;z++)
                {
                lua_rawgeti(VM, -1, z);
                data=lua_tonumber(VM,-1);
                massive[z-1]=data;
                lua_remove(VM, -1);
                }
lua_remove(VM, -1);
}
return;

}
//---------------------------------------------------------------------------
void DMLuaManager::CopyVector(std::string& Path,lua_State * VM, void * buffer, int blocksize,int type)
{

int Size=GetVectorSize(Path,VM);
if(FindTable(Path,VM)==0)
{
                BYTE * pbuffer;
                pbuffer=(BYTE *)buffer;
                        switch(type)
                        {
                        case DataInt:
                                for(int z=1;z<=Size;z++)
                                {
                                int data;
                                lua_rawgeti(VM, -1, z);
                                data=lua_tointeger(VM,-1);
                                memcpy(pbuffer,&data,sizeof(int));
                                pbuffer+=blocksize;
                                lua_pop(VM,1);
                                }
                        break;
                        case DataFloat:
                                for(int z=1;z<=Size;z++)
                                {
                                float data;
                                lua_rawgeti(VM, -1, z);
                                data=static_cast<float>(lua_tonumber(VM,-1));
                                memcpy(pbuffer,&data,sizeof(float));
                                pbuffer+=blocksize;
                                lua_pop(VM,1);
                                }
                        break;
                        case DataDouble:
                                for(int z=1;z<=Size;z++)
                                {
                                double data;
                                lua_rawgeti(VM, -1, z);
                                data=lua_tonumber(VM,-1);
                                memcpy(pbuffer,&data,sizeof(double));
                                pbuffer+=blocksize;
                                lua_pop(VM,1);
                                }
                        break;
                        }
lua_remove(VM, -1);
}
return;

}
//---------------------------------------------------------------------------
int DMLuaManager::WriteScalarString(const std::string& Path,lua_State * VM,const char * data)
{
std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_pushstring(VM,data);
lua_settable(VM,-3);
lua_remove(VM, -1);
return 1;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_pushstring(VM,data);
lua_settable(VM,LUA_GLOBALSINDEX);
return 1;
}

return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteScalarInt(const std::string& Path,lua_State * VM,int data)
{
std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_pushinteger(VM,data);
lua_settable(VM,-3);
lua_remove(VM, -1);
return 1;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_pushinteger(VM,data);
lua_settable(VM,LUA_GLOBALSINDEX);
return 1;
}

return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteScalarFloat(const std::string& Path,lua_State * VM,float data)
{
std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_pushnumber(VM,data);
lua_settable(VM,-3);
lua_remove(VM, -1);
return 1;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_pushnumber(VM,data);
lua_settable(VM,LUA_GLOBALSINDEX);
return 1;
}

return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteScalarDouble(const std::string& Path,lua_State * VM,double data)
{
std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_pushnumber(VM,data);
lua_settable(VM,-3);
lua_remove(VM, -1);
return 1;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_pushnumber(VM,data);
lua_settable(VM,LUA_GLOBALSINDEX);
return 1;
}


return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteScalarVoid(const std::string& Path,lua_State * VM,void * data)
{
std::string NewPath=GetScalarPath(Path);
std::string Token=GetScalarName(Path);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_pushlightuserdata(VM,data);
lua_settable(VM,-3);
lua_remove(VM, -1);
return 1;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_pushlightuserdata(VM,data);
lua_settable(VM,LUA_GLOBALSINDEX);
return 1;
}


return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteVectorString(std::string& Path,lua_State * VM,int index,const char * data)
{
if(FindTable(Path,VM)==0)
{
lua_pushstring(VM,data);
lua_rawseti(VM, -2, index);
lua_remove(VM, -1);
return 1;
}
return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteVectorInt(std::string& Path,lua_State * VM,int index,int data)
{
if(FindTable(Path,VM)==0)
{
lua_pushinteger(VM,data);
lua_rawseti(VM, -2, index);
lua_remove(VM, -1);
return 1;
}
return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteVectorFloat(std::string& Path,lua_State * VM,int index,float data)
{
if(FindTable(Path,VM)==0)
{
lua_pushnumber(VM,data);
lua_rawseti(VM, -2, index);
lua_remove(VM, -1);
return 1;
}
return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::WriteVectorDouble(std::string& Path,lua_State * VM,int index,double data)
{
if(FindTable(Path,VM)==0)
{
lua_pushnumber(VM,data);
lua_rawseti(VM, -2, index);
lua_remove(VM, -1);
return 1;
}
return 0;
}
//---------------------------------------------------------------------------
int DMLuaManager::PasteVector(std::string& Path,lua_State * VM, void * buffer, int blocksize,int massivesize,int type)
{
if(FindTable(Path,VM)==0)
{

                BYTE * pbuffer;
                pbuffer=(BYTE *)buffer;
                        switch(type)
                        {
                        case DataInt:
                                for(int z=1;z<=massivesize;z++)
                                {
                                int data;
                                int * pdata;
                                pdata=(int *)pbuffer;
                                data=*pdata;
                                lua_pushinteger(VM,data);
                                lua_rawseti(VM, -2, z);
                                pbuffer+=blocksize;
                                }
                        break;
                        case DataFloat:
                                for(int z=1;z<=massivesize;z++)
                                {
                                float data;
                                float *pdata;
                                pdata=(float *)pbuffer;
                                data=*pdata;
                                lua_pushnumber(VM,data);
                                lua_rawseti(VM, -2, z);
                                pbuffer+=blocksize;
                                }
                        break;
                        case DataDouble:
                                for(int z=1;z<=massivesize;z++)
                                {
                                double data;
                                double *pdata;
                                pdata=(double *)pbuffer;
                                data=*pdata;
                                lua_pushnumber(VM,data);                                
                                lua_rawseti(VM, -2, z);
                                pbuffer+=blocksize;
                                }
                        break;
                        }
lua_remove(VM, -1);
return 1;
}
return 0;

}
//---------------------------------------------------------------------------
void DMLuaManager::TableRemove(std::string& Path,lua_State * VM,int index)
{
if(FindTable(Path,VM)==0)
{
  int e = luaL_getn(VM, -1);
  int pos = index;

  if (e == 0) return;
  luaL_setn(VM, -1, e - 1);

  for ( ;pos<e; pos++)
  {
  lua_rawgeti(VM, -1, pos+1);
  lua_rawseti(VM, -2, pos);
  }
  lua_pushnil(VM);
  lua_rawseti(VM, -2, e);

lua_remove(VM, -1);
return;
}
return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_0(const std::string& FunctionName,lua_State * VM)
{
//без параметров и возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_call(VM,0,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_call(VM,0,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_1(std::string& FunctionName,lua_State * VM,const int P_1)
{
//-1 параметр и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushinteger(VM,P_1);
lua_call(VM,1,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushinteger(VM,P_1);
lua_call(VM,1,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_1_str(std::string& FunctionName,lua_State * VM,const std::string& P_1)
{
//-1 параметр и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushstring(VM,P_1.c_str());
lua_call(VM,1,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushstring(VM,P_1.c_str());
lua_call(VM,1,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_1_void(std::string& FunctionName,lua_State * VM,void * P_1)
{
//-1 параметр и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_call(VM,1,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_call(VM,1,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_2_void(std::string& FunctionName,lua_State * VM,void * P_1,void * P_2)
{
//-2 параметрa и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_call(VM,2,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_call(VM,2,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_2_string(std::string& FunctionName,lua_State * VM,const std::string&  P_1,const std::string& P_2)
{
//-2 параметрa и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushstring(VM,P_1.c_str());
lua_pushstring(VM,P_2.c_str());
lua_call(VM,2,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushstring(VM,P_1.c_str());
lua_pushstring(VM,P_2.c_str());
lua_call(VM,2,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_2_table(std::string& FunctionName,lua_State * VM,std::vector<int>& P_1,int P_2)
{
//2 параметра

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_newtable(VM);
	for(unsigned int q=0;q<P_1.size();++q){
	lua_pushinteger(VM,P_1[q]);
	lua_rawseti(VM, -2, q+1);
	}
lua_pushinteger(VM,P_2);
lua_call(VM,2,0);

return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_newtable(VM);
	for(unsigned int q=0;q<P_1.size();++q){
	lua_pushinteger(VM,P_1[q]);
	lua_rawseti(VM, -2, q+1);
	}
lua_pushinteger(VM,P_2);
lua_call(VM,2,0);

return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_3(std::string& FunctionName,lua_State * VM,
void * P_1,float P_2,float P_3)
{
//-3 параметра и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushnumber(VM,P_2);
lua_pushnumber(VM,P_3);
lua_call(VM,3,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushnumber(VM,P_2);
lua_pushnumber(VM,P_3);
lua_call(VM,3,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_4_void(std::string& FunctionName,lua_State * VM,
void * P_1,void * P_2,float P_3,float P_4)
{
//-4 параметра и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_call(VM,4,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_call(VM,4,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_4(std::string& FunctionName,lua_State * VM,
float P_1,void * P_2,float P_3,float P_4)
{
//-4 параметра и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushnumber(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_call(VM,4,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushnumber(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_call(VM,4,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_4_void_int_void_float(std::string& FunctionName,lua_State * VM,
void * P_1, const int P_2, void * P_3,float P_4)
{
//-4 параметра и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushinteger(VM,P_2);
lua_pushlightuserdata(VM,P_3);
lua_pushnumber(VM,P_4);
lua_call(VM,4,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushinteger(VM,P_2);
lua_pushlightuserdata(VM,P_3);
lua_pushnumber(VM,P_4);
lua_call(VM,4,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_4_DMLuaManagerParams(std::string& FunctionName,lua_State * VM,
DMLuaManagerParams& P_1,DMLuaManagerParams& P_2,DMLuaManagerParams& P_3,DMLuaManagerParams& P_4)
{
//-4 параметра и без возврата


std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);



if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
	if(lua_type(VM,-1)!=LUA_TFUNCTION){
	//Log::Instance().WriteErr("DMLuaManager[CallFunction_0_4_DMLuaManagerParams]: Wrong Function [%s]",FunctionName.c_str());
	return;
	}


ParseParam(VM,P_1);
ParseParam(VM,P_2);
ParseParam(VM,P_3);
ParseParam(VM,P_4);
lua_call(VM,4,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
	if(lua_type(VM,-1)!=LUA_TFUNCTION){
	//Log::Instance().WriteErr("DMLuaManager[CallFunction_0_4_DMLuaManagerParams]: Wrong Function [%s]",FunctionName.c_str());
	return;
	}
ParseParam(VM,P_1);
ParseParam(VM,P_2);
ParseParam(VM,P_3);
ParseParam(VM,P_4);
lua_call(VM,4,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_5(std::string& FunctionName,lua_State * VM,
void * P_1,float P_2,float P_3,float P_4,float P_5)
{
//-5 параметров и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushnumber(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushnumber(VM,P_5);
lua_call(VM,5,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushnumber(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushnumber(VM,P_5);
lua_call(VM,5,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_5_string(std::string& FunctionName,lua_State * VM,
float P_1,void * P_2,float P_3,float P_4,std::string& P_5)
{
//-5 параметров и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushnumber(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushnumber(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_5_void_string(std::string& FunctionName,lua_State * VM,
void * P_1,void * P_2,float P_3,float P_4,std::string& P_5)
{
//-5 параметров и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushlightuserdata(VM,P_2);
lua_pushnumber(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_5_void_string_void_float_string(std::string& FunctionName,lua_State * VM,
void * P_1, const std::string& P_2, void * P_3,float P_4, const std::string& P_5)
{
//-5 параметра и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushstring(VM,P_2.c_str());
lua_pushlightuserdata(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushstring(VM,P_2.c_str());
lua_pushlightuserdata(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_0_5_void_int_void_float_string(std::string& FunctionName,lua_State * VM,
void * P_1, const int P_2, void * P_3,float P_4, const std::string& P_5)
{
//-5 параметра и без возврата

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushlightuserdata(VM,P_1);
lua_pushinteger(VM,P_2);
lua_pushlightuserdata(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushlightuserdata(VM,P_1);
lua_pushinteger(VM,P_2);
lua_pushlightuserdata(VM,P_3);
lua_pushnumber(VM,P_4);
lua_pushstring(VM,P_5.c_str());
lua_call(VM,5,0);
return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_1_0(std::string& FunctionName,lua_State * VM,int& R_1)
{
//0 параметров и 1 возврат

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_call(VM,0,1);
R_1=lua_tointeger(VM, -1);
lua_pop(VM, 1);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_call(VM,0,1);
R_1=lua_tointeger(VM, -1);
lua_pop(VM, 1);
return;
}
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_1_1(std::string& FunctionName,lua_State * VM,void *& R_1,int P_1)
{
//1 параметр и 1 возврат
std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_pushnumber(VM,P_1);
lua_call(VM,1,1);
R_1=lua_touserdata(VM, -1);
lua_pop(VM, 1);
return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_pushnumber(VM,P_1);
lua_call(VM,1,1);
R_1=lua_touserdata(VM, -1);
lua_pop(VM, 1);
return;
}
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_1_2_table(std::string& FunctionName,lua_State * VM,int & R_1,std::vector<int>& P_1,int P_2)
{
//2 параметра и 1 возврат

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_newtable(VM);
	for(unsigned int q=0;q<P_1.size();++q){
	lua_pushinteger(VM,P_1[q]);
	lua_rawseti(VM, -2, q+1);
	}
lua_pushinteger(VM,P_2);
lua_call(VM,2,1);
R_1=lua_tointeger(VM, -1);
lua_pop(VM, 1);


return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_newtable(VM);
	for(unsigned int q=0;q<P_1.size();++q){
	lua_pushinteger(VM,P_1[q]);
	lua_rawseti(VM, -2, q+1);
	}
lua_pushinteger(VM,P_2);
lua_call(VM,2,1);
R_1=lua_tointeger(VM, -1);
lua_pop(VM, 1);

return;
}

return;
}
//---------------------------------------------------------------------------
void DMLuaManager::CallFunction_1_2_void_table(std::string& FunctionName,lua_State * VM,void *& R_1,std::vector<int>& P_1,int P_2)
{
//2 параметра и 1 возврат

std::string NewPath=GetScalarPath(FunctionName);
std::string Token=GetScalarName(FunctionName);

if(NewPath!="")
if(FindTable(NewPath,VM)==0)
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,-2);
lua_newtable(VM);
	for(unsigned int q=0;q<P_1.size();++q){
	lua_pushinteger(VM,P_1[q]);
	lua_rawseti(VM, -2, q+1);
	}
lua_pushinteger(VM,P_2);
lua_call(VM,2,1);
R_1=lua_touserdata(VM, -1);
lua_pop(VM, 1);


return;
}
if(NewPath=="")
{
lua_pushstring(VM,Token.c_str());
lua_gettable(VM,LUA_GLOBALSINDEX);
lua_newtable(VM);
	for(unsigned int q=0;q<P_1.size();++q){
	lua_pushinteger(VM,P_1[q]);
	lua_rawseti(VM, -2, q+1);
	}
lua_pushinteger(VM,P_2);
lua_call(VM,2,1);
R_1=lua_touserdata(VM, -1);
lua_pop(VM, 1);

return;
}

return;
}
//---------------------------------------------------------------------------
int DMLuaManager::GetVectorSize(const std::string& Path,lua_State * VM)
{
if(FindTable(Path,VM)==0)
{
int size=luaL_getn(VM,-1);
lua_remove(VM, -1);
return size;
}

return -1;
}
//---------------------------------------------------------------------------
int DMLuaManager::FindTable(const std::string& Path,lua_State * VM)
{
if (luaL_findtable(VM,LUA_GLOBALSINDEX,Path.c_str(),0)== NULL)
return 0;
else
return 1;
}
//---------------------------------------------------------------------------
std::string DMLuaManager::GetScalarName(const std::string& Path)
{
//Получение имени скаляра

int DotCount=0;
for (unsigned int q=0;q<Path.size();++q)
if(Path[q]=='.')
DotCount++;

int CurDot=0;
std::string Name="";
for(unsigned int q=0;q<Path.size();++q)
{
if(Path[q]=='.')
CurDot++;
if(CurDot==DotCount)
if(Path[q]!='.')
Name+=Path[q];
}

return Name;
}
//---------------------------------------------------------------------------
std::string DMLuaManager::GetScalarPath(const std::string& Path)
{
//Получение пути к таблице скаляра

int DotCount=0;
for (unsigned int q=0;q<Path.size();++q)
if(Path[q]=='.')
DotCount++;

int CurDot=0;
std::string NewPath="";
for(unsigned int q=0;q<Path.size();++q)
{
if(Path[q]=='.')
CurDot++;
if(CurDot!=DotCount)
NewPath+=Path[q];
}

return NewPath;
}
//---------------------------------------------------------------------------
void DMLuaManager::ParseParam(lua_State * VM,const DMLuaManagerParams& P)
{
int type;
int d_i;
double d_f;
std::string d_s;

type=P.GetType();
	switch(type){
	case DataII:
	P.GetParam(d_i);
	lua_pushinteger(VM,d_i);
	break;
	case DataFF:
	P.GetParam(d_f);
	lua_pushnumber(VM,d_f);
	break;
	case DataSS:
	P.GetParam(d_s);
	lua_pushstring(VM,d_s.c_str());
	break;
	}

}
//---------------------------------------------------------------------------
//Методы класса DMLuaManager END
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

