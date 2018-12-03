#include "TestSimModel.h"

void ShowErrorMessage(const string & msg)
{
	cout<<msg.c_str()<<endl;
}

void ShowErrorMessage(const ErrorData & ED)
{
	cout<<ED.GetDescription()<<endl<<"    in "<<ED.GetSource()<<endl;
}

void ShowTimeSpan(double tstart, double tend)
{
	cout<<(tend-tstart)/1000.0<<"s"<<endl;
}

string BasisDir(const string & exeName)
{
//	cout<<exeName.c_str()<<endl;
	const string shortExeName="SimModelProfiler.exe";
	
	string basisDir = exeName;

	try
	{
		//will crash if using with rational purify or quantify
		basisDir.replace(basisDir.find(shortExeName),shortExeName.length(),"");
	}
	catch(...)
	{
		basisDir="C:\\VSS\\SimModel\\SimModelProfiling\\src\\Build\\Debug\\x64\\";
	}
	
	return basisDir;
}

