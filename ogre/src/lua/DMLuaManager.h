//---------------------------------------------------------------------------

#ifndef DMLuaManagerH
#define DMLuaManagerH
//---------------------------------------------------------------------------
extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <string>
#include <cstdlib>
#include <vector>
#include <cstring>

#include "../tools/Conversions.h"
//---------------------------------------------------------------------------
enum DMLuaManagerParamsType{
DataII=0,
DataFF=1,
DataSS=2
};

class DMLuaManagerParams{
private:
	union{
	double Data_F;
	int Data_I;
	char* Data_S;
	};
int DataType;

void ClearStr(){if(DataType==DataSS&&Data_S){delete[]Data_S;Data_S=NULL;}}

void UpdStr(const std::string & d_s){
ClearStr();
DataType=DataSS;
Data_S = new char[d_s.size()+1];
memcpy(Data_S,d_s.c_str(),d_s.size());
Data_S[d_s.size()]=0;
}

void SetParam(int d_i){ClearStr();Data_I=d_i;DataType=DataII;}
void SetParam(double d_f){ClearStr();Data_F=d_f;DataType=DataFF;}
void SetParam(const std::string& d_s){UpdStr(d_s);}

public:
DMLuaManagerParams(){DataType=DataII;Data_S=NULL;Data_I=0;}
DMLuaManagerParams(int d_i){Data_S=NULL;SetParam(d_i);}
DMLuaManagerParams(double d_f){Data_S=NULL;SetParam(d_f);}
DMLuaManagerParams(const std::string& d_s){Data_S=NULL;SetParam(d_s);}
DMLuaManagerParams(const DMLuaManagerParams&);					//��� �����������
~DMLuaManagerParams(){ClearStr();}

DMLuaManagerParams* operator = (const DMLuaManagerParams&);		//��� �����������
DMLuaManagerParams* operator = (int d_i){SetParam(d_i);return this;}
DMLuaManagerParams* operator = (double d_f){SetParam(d_f);return this;}
DMLuaManagerParams* operator = (const std::string& d_s){SetParam(d_s);return this;}

int GetType()const{return DataType;}

void GetParam(int& d_i)const{
	switch(DataType){
	case DataII:
	d_i=Data_I;
	break;
	case DataFF:
	d_i=(int)Data_F;
	break;
	case DataSS:
	d_i=atoi(Data_S);
	break;
	}
}

void GetParam(double& d_f)const{
	switch(DataType){
	case DataII:
	d_f=(double)Data_I;
	break;
	case DataFF:
	d_f=Data_F;
	break;
	case DataSS:
	d_f=atof(Data_S);
	break;
	}
}

void GetParam(std::string& d_s)const{
	switch(DataType){
	case DataII:
	d_s=Conversions::DMToString(Data_I);
	break;
	case DataFF:
	d_s=Conversions::DMToString(Data_F,0,50);
	break;
	case DataSS:
	d_s=Data_S;
	break;
	}
}
};
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//����� DMLuaManager
class DMLuaManager
{
//~~~~~~~~~~~~~~~~~~~~~~~~������
private:
public:
//~~~~~~~~~~~~~~~~~~~~~~~~������ END
/////////////////////////////////////////////////////////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~~������
private:

std::string GetScalarName(const std::string& Path);      //-��������� ����� �������
std::string GetScalarPath(const std::string& Path);      //-��������� ���� � ������� �������

void ParseParam(lua_State *,const DMLuaManagerParams& P);

public:
DMLuaManager(){}
~DMLuaManager(){}

int FindTable(const std::string&,lua_State *);
void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ", bool trimEmpty = true);

//READ-----------------------------

//������ ��������� ���������� �� ���� � ��� (Elk.Data.Data1)
const char *    ReadScalarString (const std::string&,lua_State *);
int             ReadScalarInt    (const std::string&,lua_State *);
float           ReadScalarFloat  (const std::string&,lua_State *);
double          ReadScalarDouble (const std::string&,lua_State *);
bool            ReadScalarBool   (const std::string&,lua_State *);
void *          ReadScalarVoid   (const std::string&,lua_State *);
//------------------------------------------------
//������ ��������� ���������� �� ������� ������� � �������� ����������
//int             ReadScalarInt_Table	(const void*,std::string&,lua_State *);
//------------------------------------------------

//������ �������� ������� �� ������� � ���� � ����
const char *    ReadVectorString (const std::string&,lua_State *,int index);
int             ReadVectorInt    (const std::string&,lua_State *,int index);
float           ReadVectorFloat  (const std::string&,lua_State *,int index);
double          ReadVectorDouble (const std::string&,lua_State *,int index);
//------------------------------------------------

const char *    ReadVectorComplicatedString  (const std::string&, const std::string&,lua_State *,int index);
float           ReadVectorComplicatedFloat  (const std::string&, const std::string&,lua_State *,int index);
bool            ReadVectorComplicatedBool  (const std::string&, const std::string&,lua_State *,int index);

//����������� ���� ��������� ������� �� ���� � ����
void CopyVectorString (std::string&,lua_State *,const char **);
void CopyVectorInt    (std::string&,lua_State *,int *);
void CopyVectorFloat  (std::string&,lua_State *,float *);
void CopyVectorDouble (std::string&,lua_State *,double *);

//����������� � ������������ ��������� ������
//blocksize - ������ ��������� ������
//type - ��� �������� �� DataType
//�� - ���������� ������ �������� � ������� ��������
void CopyVector(std::string&,lua_State *, void *, int blocksize,int type);
enum {DataInt,DataFloat,DataDouble} DataType;
//------------------------------------------------

//WRITE-----------------------------

//������ ��������� ���������� �� ���� � ��� (Elk.Data.Data1)
int WriteScalarString (const std::string&,lua_State *,const char *);
int WriteScalarInt    (const std::string&,lua_State *,int);
int WriteScalarFloat  (const std::string&,lua_State *,float);
int WriteScalarDouble (const std::string&,lua_State *,double);
int WriteScalarVoid   (const std::string&,lua_State *,void *);
//------------------------------------------------

//������ �������� ������� �� ������� � ���� � ����
int WriteVectorString (std::string&,lua_State *,int index,const char *);
int WriteVectorInt    (std::string&,lua_State *,int index,int);
int WriteVectorFloat  (std::string&,lua_State *,int index,float);
int WriteVectorDouble (std::string&,lua_State *,int index,double);
//------------------------------------------------

//����������� �� ������������ ��������� ������
//blocksize - ������ ��������� ������
//type - ��� �������� �� DataType
int PasteVector(std::string&,lua_State *, void *, int blocksize,int massivesize,int type);
//------------------------------------------------

//TABLE-----------------------------
void TableRemove      (std::string&,lua_State *,int index);
//------------------------------------------------

//LUA FUNCTIONS-----------------------------
//����� �������
void CallFunction_0_0(			//-��� ���������� � ��������
const std::string&,lua_State *);

void CallFunction_0_1(			//-1 �������� � ��� ��������
std::string&,lua_State *,
const int P_1);

void CallFunction_0_1_str(		//-1 �������� � ��� ��������
std::string&,lua_State *,
const std::string& P_1);

void CallFunction_0_1_void(		//-1 �������� � ��� ��������
std::string&,lua_State *,
void * P_1);

void CallFunction_0_2_void(		//-2 ��������� � ��� ��������
std::string&,lua_State *,
void * P_1,void * P_2);

void CallFunction_0_2_string(		//-2 ��������� � ��� ��������
std::string&,lua_State *,
const std::string&  P_1,const std::string& P_2);

void CallFunction_0_2_table(	//-2 ���������
std::string&,lua_State *,
std::vector<int>& P_1,int P_2);

void CallFunction_0_3(			//-3 ��������� � ��� ��������
std::string&,lua_State *,
void * P_1, float P_2, float P_3);


void CallFunction_0_4(			//-4 ��������� � ��� ��������
std::string&,lua_State *,
float P_1,void * P_2, float P_3, float P_4);

void CallFunction_0_4_void(			//-4 ��������� � ��� ��������
std::string&,lua_State *,
void * P_1,void * P_2, float P_3, float P_4);

void CallFunction_0_4_void_int_void_float(	//-4 ��������� � ��� ��������
std::string&,lua_State *,
void * P_1, const int P_2, void * P_3,float P_4);

void CallFunction_0_4_DMLuaManagerParams(			//-4 ��������� � ��� ��������
std::string&,lua_State *,
DMLuaManagerParams& P_1,DMLuaManagerParams& P_2,DMLuaManagerParams& P_3,DMLuaManagerParams& P_4);

void CallFunction_0_5(			//-5 ���������� � ��� ��������
std::string&,lua_State *,
void * P_1, float P_2, float P_3, float P_4, float P_5);

void CallFunction_0_5_string(			//-5 ���������� � ��� ��������
std::string&,lua_State *,
float P_1,void * P_2,  float P_3, float P_4, std::string& P_5);

void CallFunction_0_5_void_string(			//-5 ���������� � ��� ��������
std::string&,lua_State *,
void * P_1, void * P_2, float P_3, float P_4, std::string& P_5);

void CallFunction_0_5_void_string_void_float_string(	//-5 ��������� � ��� ��������
std::string&,lua_State *,
void * P_1, const std::string& P_2, void * P_3,float P_4, const std::string& P_5);

void CallFunction_0_5_void_int_void_float_string(	//-5 ��������� � ��� ��������
std::string&,lua_State *,
void * P_1, const int P_2, void * P_3,float P_4, const std::string& P_5);


void CallFunction_1_0(			//-0 ���������� � 1 �������
std::string&,lua_State *,
int& R_1);

void CallFunction_1_1(			//-1 �������� � 1 �������
std::string&,lua_State *,
void *& R_1,int P_1);

void CallFunction_1_2_table(	//-2 ��������� � 1 �������
std::string&,lua_State *,
int& R_1,std::vector<int>& P_1,int P_2);

void CallFunction_1_2_void_table(	//-2 ��������� � 1 �������
std::string&,lua_State *,
void*& R_1,std::vector<int>& P_1,int P_2);
//------------------------------------------------

//��������� ������� ������� �� ����
//������ ���� ������ ��� ��� ������ �������
//�� �������� � �������� ������!!!!!!!!!! ��������
int GetVectorSize(const std::string&,lua_State *);

//~~~~~~~~~~~~~~~~~~~~~~~~������ END
};
//����� DMLuaManager END
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
#endif
