#include "StdInc.h"
#include "IniReader.h"

#pragma unmanaged

CIniReader::CIniReader(char* szFileName)
{
 memset(m_szFileName, 0x00, 255);
 memcpy(m_szFileName, szFileName, strlen(szFileName));
}
int CIniReader::ReadInteger(char* szSection, char* szKey, int iDefaultValue)
{
 int iResult = GetPrivateProfileIntA(szSection,  szKey, iDefaultValue, m_szFileName); 
 return iResult;
}
float CIniReader::ReadFloat(char* szSection, char* szKey, float fltDefaultValue)
{
 char szResult[255];
 char szDefault[255];
 float fltResult;
 sprintf(szDefault, "%f",fltDefaultValue);
 GetPrivateProfileStringA(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
 fltResult = (float)atof(szResult);
 return fltResult;
}
bool CIniReader::ReadBoolean(char* szSection, char* szKey, bool bolDefaultValue)
{
 char szResult[255];
 char szDefault[255];
 bool bolResult;
 sprintf(szDefault, "%s", bolDefaultValue? "True" : "False");
 GetPrivateProfileStringA(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
 bolResult =  (strcmp(szResult, "True") == 0 || 
		strcmp(szResult, "true") == 0) ? true : false;
 return bolResult;
}
void CIniReader::WriteBoolean(char* szSection, char* szKey, bool bolValue)
{
 char szValue[255];
 sprintf(szValue, "%s", bolValue ? "True" : "False");
 WritePrivateProfileStringA(szSection, szKey, szValue, m_szFileName); 
}
char* CIniReader::ReadString(char* szSection, char* szKey, const char* szDefaultValue)
{
 char* szResult = new char[255];
 memset(szResult, 0x00, 255);
 GetPrivateProfileStringA(szSection,  szKey, 
		szDefaultValue, szResult, 255, m_szFileName); 
 return szResult;
}