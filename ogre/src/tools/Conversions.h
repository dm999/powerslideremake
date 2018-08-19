#ifndef ConversionsH
#define ConversionsH

#include <iomanip>
#include <iostream>
#include <sstream>

//using namespace std;
				/*
template<class T>
string DMToString(const T& t){
ostringstream o;
o<<t;
return o.str();
}      */
		   /*
template<class T>
string DMToString(const T& t){
std::ostringstream o;
o<<t;
return o.str();
}

template<class T>
string DMToString(const T& t,int width){
std::ostringstream o;
o<<std::setw(width);
o<<t;
return o.str();
}

template<class T>
string DMToString(const T& t,int width,int precision){
std::ostringstream o;
o<<std::setw(width);
o<<std::setprecision(precision);
o<<t;
return o.str();
}        */

#include <ctime>
#include <locale>   // isdigit
//---------------------------------------------------------------------------
namespace Conversions{
//---------------------------------------------------------------------------
template<class T>
static std::string DMToString(const T& t){
std::ostringstream o;
o<<t;
return o.str();
}

template<class T>
static std::string DMToString(const T& t,int width){
std::ostringstream o;
o<<std::setw(width);
o<<t;
return o.str();
}

template<class T>
static std::string DMToString(const T& t,int width,int precision){
std::ostringstream o;
o << std::setw(width);
o << std::setprecision(precision);
o << std::fixed;
o << t;
return o.str();
}

template<class T>
static bool DMFromString(const std::string& str, T& res)
{
    bool isCorrect = true;

    if(true == isCorrect)
    {
        std::locale loc;
        for (std::string::const_iterator it=str.begin(); it!=str.end(); ++it)
        {
            if (!std::isdigit (*it, loc) && (*it) != '+' && (*it) != '-' && (*it) != '.')
            {
                isCorrect = false;
                break;
            }
        }
    }

    if(false == str.empty())
    {
        std::istringstream ss(str);
        ss >> res;
        if (ss.fail())
        {
            isCorrect = false;
        }
    }
    else
    {
        isCorrect = false;
    }
    return isCorrect;
}


//---------------------------------------------------------------------------
}//END Conversions namespace
//---------------------------------------------------------------------------

#endif
