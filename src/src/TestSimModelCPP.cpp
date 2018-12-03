//#define _CRTDBG_MAP_ALLOC  
//#include <stdlib.h>  
//#include <crtdbg.h> 
//
//#ifdef _DEBUG
//	#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//	// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//	// allocations to be of _CLIENT_BLOCK type
//#else
//	#define DBG_NEW new
//#endif

#define _SECURE_SCL 0

#include "TestSimModel.h"
#include "FuncParser\ElemFunctions.h"
#include <conio.h>
//# include "rcm.h"

std::string _basisDir;

using namespace SimModelNative;
using namespace FuncParserNative;
//using namespace DCI;

void _tmain(int argc, _TCHAR* argv[])
{
	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	//_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	//_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	//_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	//_CrtMemState s1, s2, s3;

	try
	{

//		TestSolverCrash(); 

		_basisDir=BasisDir(argv[0]);
		
		CoInitialize(NULL);
		XMLCache * xmlCache = XMLCache::GetInstance();
		xmlCache->SetSchemaNamespace("http://www.systems-biology.com");
		xmlCache->LoadSchemaFromFile(XMLSchemaFile());

		//_CrtMemCheckpoint(&s1);

		std::string simFile;
//		cout<<"Enter File:"; cin>>simFile;

//		simFile = "PKModelCoreCaseStudy_01";
//		simFile = "ADC_MCMC_Prelim_Nonlin";
//		simFile = "GIM_Table_01";
//		simFile = "S1_GET_Div";
//		simFile = "GIM";
//		simFile = "GIM_WithConvergenceError";
//		simFile = "Neg_t_TimeSinceMeal";
//		simFile = "AdultPopulation";
//		simFile = "SimModel4_ExampleInput06";
		simFile = "PKSim_Input_01";
//		simFile = "FIM_sim";
//		simFile = "A 10mg IV labeled & 100 mg PO unlabeled";
//		simFile = "Rattus PO10.0mg with ehc_1";
//		simFile = "Rattus PO10.0mg with ehc_2";

		//LoadSimulation("Weibull_4_10days");
		//Test1("Weibull_4_10days");

		//"NoJac_SimInitAllButFormulas"
		//"SimModel4_ExampleInput06"
		//"SimInitAllButFormulas"
//		simFile="SimInitAllButFormulas";


//		TestMCMC_Crash(simFile, true);

//		TestMCMC_Crash_WithoutDCI(simFile, false);

//		TestBandSolver01(simFile,false);
//		TestBandSolver01(simFile,true);

//		TestModelCoreBandCrash();

		Test3(simFile);

		double * leak = new double[100];
		delete xmlCache;
	}
	catch(ErrorData & ED)
	{
		ShowErrorMessage(ED);
	}
	catch(const std::string & str)
	{
		ShowErrorMessage(str);
	}
	catch(SimModelSolverErrorData sED)
	{
		ShowErrorMessage(ErrorData(ErrorData::errNumber(sED.GetNumber()),sED.GetSource(),sED.GetDescription()));
	}
	catch(...)
	{
		ShowErrorMessage("Unknown Error");
	}

//	_CrtDumpMemoryLeaks(); 

	//_CrtMemCheckpoint(&s2);
	//if (_CrtMemDifference(&s3, &s1, &s2))
	//	_CrtMemDumpStatistics(&s3);

	ElemFunctions * elemFunctions = ElemFunctions::GetInstance();
	delete(elemFunctions);

	cout<<"Enter anything";
	_getch();

	//_CrtDumpMemoryLeaks();
}

void TestModelCoreBandCrash()
{
	//const string simFile = "PKModelCoreCaseStudy_01";
	//const string simFile = "ADC_MCMC_Prelim_Nonlin";
	const std::string simFile = "PKSim_Input_01";

	Simulation * sim = NULL;
	MatlabODEExporter odeExporter;

	const std::string DenseExportDir="C:\\VSS\\SimModel\\branches\\4.0\\Test\\02\\ML_Dense";
	const std::string BandExportDir="C:\\VSS\\SimModel\\branches\\4.0\\Test\\02\\ML_Band";

	try
	{
		sim = LoadSimulation(simFile);
		sim->SetUseBandLinearSolver(false);
		odeExporter.WriteMatlabCode(sim, DenseExportDir, true);
		FreeSim(sim); sim=NULL;

		sim = LoadSimulation(simFile);
		sim->SetUseBandLinearSolver(true);
		odeExporter.WriteMatlabCode(sim, BandExportDir, true);
		FreeSim(sim); sim=NULL;
	}
	catch(...)
	{
		try
		{
//			ShowFirstWarning(sim);
		}catch(...){}

		FreeSim(sim);
		throw;
	}
}

void TestBandSolver01(const std::string & simFile, bool useBandSolver)
{
	Simulation * sim = NULL;

	try
	{
		sim = LoadSimulation(simFile);
		
		sim->SetUseBandLinearSolver(useBandSolver);

		FinalizeSimulation(sim);
		
		bool isJacobianSet = sim->GetSolver().GetSolverProperties().GetUseJacobian();
		cout<<"Band solver: "<<useBandSolver<<" Jacobian: "<<isJacobianSet<<endl;

		RunSimulation(sim);
		RunSimulation(sim);
//		RunSimulation(sim);

		FreeSim(sim);
	}
	catch(...)
	{
		try
		{
//			ShowFirstWarning(sim);
		}catch(...){}

		FreeSim(sim);
		throw;
	}
}

void TestMCMC_Crash_WithoutDCI(const std::string & simFile, bool varyParams)
{
	Simulation * sim = NULL;
	std::string rhsFile, jacobianFile;
	
	try
	{
		std::string varInfo = varyParams ? "_Var" : "_NoVar";
		rhsFile = _basisDir+"\\"+simFile+varInfo+"_RHS.txt";
		jacobianFile = _basisDir+"\\"+simFile+varInfo+"_JACOBIAN.txt";

		sim = LoadSimulation(simFile);

		if(varyParams)
		{
			vector<ParameterInfo> allParams;
			sim->FillParameterProperties(allParams);

			ParameterInfo & paramInfo = allParams[7986];
			//for(int i=0, j=0; i<allParams.size(); i++)
			//{
			//	ParameterInfo & p = allParams[i];
			//	if (!p.CanBeVaried())
			//		continue;
			//	if(j==3397)
			//		cout<<p.GetFullName().c_str()<<endl;
			//	j++;

			//}
			cout<<"setting '"<<paramInfo.GetFullName().c_str()<<"' as variable"<<endl;

			vector<ParameterInfo> variableParams;
			variableParams.push_back(paramInfo);

			sim->SetVariableParameters(variableParams);
		}

		FinalizeSimulation(sim);

		RunSimulation(sim);

		WriteRhsAndJacobianOutputs(sim, rhsFile,jacobianFile);

		FreeSim(sim);
	}
	catch(...)
	{
		try
		{
			WriteRhsAndJacobianOutputs(sim, rhsFile,jacobianFile);
			ShowFirstWarning(sim);
		}catch(...){}

		FreeSim(sim);
		throw;
	}
}

void ShowFirstWarning(Simulation * sim)
{
	if( sim->SolverWarnings().size()==0)
		return;

	SolverWarning * warn = sim->SolverWarnings()[0];
	cout<<"1st solver warning: "<<warn->Message().c_str()<<endl;
}

void WriteRhsAndJacobianOutputs(Simulation * sim, const std::string & rhsFile, const std::string & jacobianFile)
{
	double t1,t2;
	cout<<"writing RHS and JACOBIAN outputs ... ";
	t1 = GetTickCount();

	ofstream outfile;
	size_t i;

	vector<TimeValueTriple> rhs_outputs = sim->GetSolver().Rhs_outputs();	
	outfile.open(rhsFile.c_str());

	for(i=0; i<rhs_outputs.size(); i++)
	{
		TimeValueTriple rhsValue = rhs_outputs[i];

		outfile<<rhsValue.Time<<";"<<rhsValue.Y<<";"<<rhsValue.YDot<<endl;
	}

	outfile.close();

	vector<TimeValueTriple> jacobian_outputs = sim->GetSolver().Jacobian_outputs();
	outfile.open(jacobianFile.c_str());

	for(i=0; i<jacobian_outputs.size(); i++)
	{
		TimeValueTriple jacobianValue = jacobian_outputs[i];

		outfile<<jacobianValue.Time<<";"<<jacobianValue.Y<<";"<<jacobianValue.YDot<<endl;
	}	

	t2 = GetTickCount();
	ShowTimeSpan(t1,t2);
}

//void TestMCMC_Crash(const std::string & simFile, bool varyParams)
//{
//	SimModelComp * hComp = NULL;
//
//	try
//	{
//		double t1,t2;
//		hComp = CreateSimModelComp();
//
//		ITableHandle configTab=hComp->GetParameterPorts()->Item(1)->GetTable();
//
//		configTab->SetValue(1,1,XMLSchemaFile().c_str());
//		configTab->SetValue(1,2,TestFileFrom(simFile).c_str());
//
//		t1 = GetTickCount();
//		cout<<"configuring "<<simFile.c_str()<<" ... ";
//		cout.flush();
//		if (!hComp->Configure())
//			throw std::string((const char *)DCI::Error::GetDescription());
//		t2 = GetTickCount();
//		ShowTimeSpan(t1,t2);
//
//		if (varyParams)
//			MCMC_SetVariableParameters(hComp);
//
//		t1 = GetTickCount();
//		cout<<"Performing ProcessMetaData ... ";
//		cout.flush();
//		if (!hComp->ProcessMetaData())
//			throw std::string((const char *)DCI::Error::GetDescription());
//		t2 = GetTickCount();
//		ShowTimeSpan(t1,t2);
//
//		cout<<"Performing ProcessData ... ";
//		if (!hComp->ProcessData())
//			throw std::string((const char *)DCI::Error::GetDescription());
//		t2 = GetTickCount();
//		ShowTimeSpan(t1,t2);
//
//		hComp->Release();
//	}
//	catch(...)
//	{
//		if (hComp!=NULL)
//			hComp->Release();
//
//		throw;
//	}
//}

//void MCMC_SetVariableParameters(SimModelComp * hComp)
//{
//	ITableHandle hTab = hComp->GetInputPorts()->Item(2)->GetTable();
//	int idx = 3398; //index of Sim_ApplicationMax|FGFR2|Reference concentration
//
//	DCI::String path = hTab->GetColumn("Path")->GetValue(idx);
//	cout<<"setting '"<<(const char *)path<<"' as variable"<<endl;
//
//	if (!hTab->GetColumn("IsVariable")->SetValue(idx, (DCI::Byte)1))
//		throw "DCI Set value failed";
//}

//void Test5(const std::string & simFile)
//{
//	vector <SimModelComp *> compVec;
//
//	for(int i=0; i<4; i++)
//	{
//		SimModelComp * hComp = CreateSimModelComp();
//		//IComponentHandle hComp = DCI::Manager::LoadComponentFromXMLFile(SimModelCompConfigFile().c_str());
//		//if (!hComp)
//		//{
//		//	throw (std::string)DCI::Error::GetDescription();
//		//}
//		//hComp.GetPtr()->AddRef();
//
//		ITableHandle configTab=hComp->GetParameterPorts()->Item(1)->GetTable();
//
//		configTab->SetValue(1,1,XMLSchemaFile().c_str());
//		configTab->SetValue(1,2,TestFileFrom(simFile).c_str());
//
//		double t1,t2;
//
//		t1 = GetTickCount();
//		cout<<"configuring "<<simFile.c_str()<<" ... ";
//		cout.flush();
//		if (!hComp->Configure())
//			throw std::string((const char *)DCI::Error::GetDescription());
//		t2 = GetTickCount();
//		ShowTimeSpan(t1,t2);
//
//		compVec.push_back(hComp);
//	}
//	
//	for(size_t idx=0; idx<compVec.size(); idx++)
//	{
//		SimModelComp * hComp = compVec[idx];
//		delete hComp;
//		//hComp.GetPtr()->Release();
//	}
//
//	compVec.clear();
//}

//void Test4(const std::string & simFile)
//{
//	SimModelComp * hComp = CreateSimModelComp();
//	//IComponentHandle hComp = DCI::Manager::LoadComponentFromXMLFile(SimModelCompConfigFile().c_str());
//	//if (!hComp)
//	//{
//	//	throw (std::string)DCI::Error::GetDescription();
//	//}
// //   hComp.GetPtr()->AddRef();
//
////    _simModelComp=dynamic_cast<SimModelComp *> (hComp.GetPtr());
//    ITableHandle configTab=hComp->GetParameterPorts()->Item(1)->GetTable();
//
//    configTab->SetValue(1,1,XMLSchemaFile().c_str());
//    configTab->SetValue(1,2,TestFileFrom(simFile).c_str());
//
//	int i, iterationsCount=1;
//	double t1,t2;
//
//	for(i=1;i<=iterationsCount; i++)
//	{
//		t1 = GetTickCount();
//		cout<<"configuring "<<simFile.c_str()<<" ... ";
//		cout.flush();
//		if (!hComp->Configure())
//			throw std::string((const char *)DCI::Error::GetDescription());
//		t2 = GetTickCount();
//		ShowTimeSpan(t1,t2);
////		cout<<"done"<<endl;
//	}
//	
//	//set all parameters variable
//	setAllValueParametersVariable(hComp); 
//
//	t1 = GetTickCount();
//	cout<<"Performing ProcessMetaData ... ";
//	if (!hComp->ProcessMetaData())
//		throw std::string((const char *)DCI::Error::GetDescription());
//	t2 = GetTickCount();
//	ShowTimeSpan(t1,t2);
////	cout<<"done"<<endl;
//
//	for(i=1;i<=iterationsCount; i++)
//	{
//		t1 = GetTickCount();
//		cout<<"Performing ProcessData ... ";
//		if (!hComp->ProcessData())
//			throw std::string((const char *)DCI::Error::GetDescription());
//		t2 = GetTickCount();
//		ShowTimeSpan(t1,t2);
////		cout<<"done"<<endl;
//	}
//	
//	cout.flush();
//	
//	std::string xmlFile = "C:\\temp\\Sim01.xml";
//	cout<<"saving simulation to "<<xmlFile.c_str()<<" ...";
//	hComp->Invoke("SaveSimulationToXml", xmlFile.c_str());
//	cout<<"done"<<endl;
//
//	t1 = GetTickCount();
//	cout<<"reloading from xml file ...";
//	configTab->SetValue(1,2,xmlFile.c_str());
//	if (!hComp->Configure())
//			throw std::string((const char *)DCI::Error::GetDescription());
//	t2 = GetTickCount();
//	ShowTimeSpan(t1,t2);
//
//	//hComp.GetPtr()->Release();
//	delete hComp;
//}

void Test3(const std::string & simFile)
{
	Simulation * sim = NULL;
	
	try
	{
		sim = LoadSimulation(simFile);

		vector<ParameterInfo> parameterProperties, variableParams;
		sim->FillParameterProperties(parameterProperties);

		for(size_t i=0; i<parameterProperties.size(); i++)
		{
			ParameterInfo & info = parameterProperties[i];
			if(!info.CanBeVaried())
				continue;

			if(info.IsFormula() || info.IsTable())
				continue;

			variableParams.push_back(info);
		}

		cout << "Setting " << variableParams.size() << " variable parameters" << endl;
		sim->SetVariableParameters(variableParams);

		FinalizeSimulation(sim);

		int simCount = 10;
		for (int i = 1; i <= simCount; i++)
		{
			cout << "Sim #" << i << "/" << simCount<<" ";
			sim->SetParametersValues(variableParams);
			RunSimulation(sim);
		}

		delete sim;
		sim=NULL;
	}
	catch(...)
	{
		FreeSim(sim);
		throw;
	}
}

void Test2()
{
	double t1,t2;

	vector<std::string> variableNames, parameterNames, parameterNotToSimplifyNames;
    vector<double> parameterValues;
	
	parameterNames.push_back("K_cell_pls");
	parameterValues.push_back(6.88365168539326);

	parameterNames.push_back("AeffFactor");
	parameterValues.push_back(447.9877);

	parameterNames.push_back("Ageom");
	parameterValues.push_back(475.219342291257);

	parameterNames.push_back("DrugMucosa");
	parameterValues.push_back(0.0231858232104688);

	parameterNames.push_back("fu");
	parameterValues.push_back(0.11);

	parameterNames.push_back("MW");
	parameterValues.push_back(400);

	parameterNames.push_back("P_int_trans");
	parameterValues.push_back(3.1171875E-05);

	parameterNames.push_back("P_int_trans_cell_lum_factor");
	parameterValues.push_back(1);

	parameterNames.push_back("P_int_trans_lum_cell_factor");
	parameterValues.push_back(1);

	parameterNames.push_back("Solubility");
	parameterValues.push_back(12000);

	parameterNames.push_back("k_Liquid_trans");
	parameterValues.push_back(1);

	parameterNames.push_back("OralApplicationsEnabled");
	parameterValues.push_back(1);
	
	variableNames.push_back("DrugLiquid");
	variableNames.push_back("Liquid");

	cout<<"start parsing ... ";
	t1 = GetTickCount();

	for(int i=0; i<500; i++)
	{
		ExplicitFormulaExtender ef1, ef2;
		
		ef1.SetEquation("OralApplicationsEnabled ? P_int_trans*Ageom*AeffFactor*(P_int_trans_lum_cell_factor*min(Solubility/ MW ;DrugLiquid/(Liquid))-(k_Liquid_trans ? P_int_trans_cell_lum_factor*fu/K_cell_pls*DrugMucosa : 0))*1e-3 : 0");
		ef1.CreateFormulaFromEquation(variableNames, parameterNames, 
			                          parameterValues,parameterNotToSimplifyNames,
									  true);
	}

	t2 = GetTickCount();
	ShowTimeSpan(t1,t2);
}

void Test1(const std::string & simFile)
{
	Simulation * sim = NULL;
	
	try
	{
		sim = LoadSimulation(simFile);
//		sim->SetUseBandLinearSolver(false);
		FinalizeSimulation(sim);

		const int noOfRuns=1;
		for(int i=1;i<= noOfRuns; i++)
			RunSimulation(sim);

		delete sim;
		sim=NULL;
	}
	catch(...)
	{
		FreeSim(sim);
		throw;
	}
}

void FreeSim(Simulation * & sim)
{
	if (sim == NULL)
		return;

	delete sim;
	sim  = NULL;
}

Simulation * LoadSimulation(const std::string & fileName)
{
	Simulation * sim = new Simulation();
	sim->Options().SetAutoReduceTolerances(false);
	double t1,t2;
	cout<<"loading "<<fileName.c_str()<<" ... ";
	t1 = GetTickCount();
	sim->LoadFromXMLFile(TestFileFrom(fileName));
	t2 = GetTickCount();
	ShowTimeSpan(t1,t2);
	return sim;
}

void FinalizeSimulation(Simulation * sim)
{
	double t1,t2;
	cout<<"finalizing ... ";
	t1 = GetTickCount();
	sim->Finalize();
	t2 = GetTickCount();
	ShowTimeSpan(t1,t2);
}

void RunSimulation(Simulation * sim, bool showInfo)
{
	double t1,t2;
	if(showInfo)
		cout<<"running ... ";
	t1 = GetTickCount();
	bool toleranceWasReduced=false;
	double newAbsTol, newRelTol;
	sim->RunSimulation(toleranceWasReduced, newAbsTol, newRelTol);
	t2 = GetTickCount();
	if(showInfo)
		ShowTimeSpan(t1,t2);
	ShowFirstWarning(sim);
}


std::string XMLSchemaFile()
{
	return _basisDir+"OSPSuite.SimModel.xsd";
}

std::string SimModelCompConfigFile()
{
	return _basisDir+"..\\..\\..\\..\\Dist\\Config\\SimModelComp4_1.xml";
}

std::string TestFileFrom(const std::string & fileName)
{
	return _basisDir+"..\\..\\..\\..\\OSPSuite.SimModel\\tests\\TestData\\"+fileName+".xml";
}

//void setAllValueParametersVariable(IComponentHandle hComp)
//{
//	cout<<"setting all value parameters to variable"<<endl;
//	ITableHandle hTab = hComp->GetInputPorts()->Item(2)->GetTable();
//
//	DCI::ByteVector isVariableVec = hTab->GetColumn("IsVariable")->GetValues();
//	DCI::StringVector paramTypeVec = hTab->GetColumn("ParameterType")->GetValues();
//
//	for(size_t i=0;i<isVariableVec.Len();i++)
//	{
//		std::string paramType = paramTypeVec[i];
//
//		if (paramType != "Value")
//			continue;
//
//		isVariableVec[i] = 1;
//	}
//	hTab->GetColumn("IsVariable")->SetValues(isVariableVec);
//
///*
//	for(size_t rowIdx=1; rowIdx<=hTab->GetRecords()->GetCount(); rowIdx++)
//	{
//		hTab->SetValue(rowIdx, "IsVariable", 1);
//	}
//*/
//}

SimModelNative::Variable * GetVenousBloodPlasma(Simulation * sim, const std::string & venPlsId)
{
	SimModelNative::Variable * ven_pls = 
		sim->SpeciesList().GetObjectByEntityId(venPlsId);
	if (ven_pls == NULL)
		ven_pls = sim->Observers().GetObjectByEntityId(venPlsId);
	
	if (ven_pls == NULL)
		throw "No variable with entity ID " + venPlsId + " found";

	return ven_pls;
}
//
//SimModelComp * CreateSimModelComp()
//{
//	IComponentHandle hComp;
//	SimModelComp * simModelComp = new SimModelComp();
//	hComp.BindTo(simModelComp);
//
//	String TypeName = "C++.SimModelComp4_1";
//	String TypeDescription="";
//
//	//TODO
//	//if (!_LoadComponentFromXMLFile(hComp, SimModelCompConfigFile().c_str(),&TypeName, &TypeDescription))
//	//{
//	//	delete simModelComp;
//	//	throw (std::string)DCI::Error::GetDescription();
//	//}
//
//	hComp->AddRef();
//	return simModelComp;
//}

TestSolverCaller::TestSolverCaller()
{
	UseJacobian = false;
	CrashTime = 1e300;
	NoOfVars = 2;
}

Rhs_Return_Value TestSolverCaller::ODERhsFunction(double t, const double * y, const double * p, double * ydot, void * f_data)
{
	ydot[0] = y[1];
	ydot[1] = y[0];
	for(int ii=3;ii<=NoOfVars;ii++)
		ydot[ii-1] = 0.0;

	if (t>CrashTime)
		throw std::string("Blabla");

	return RHS_OK;
}
Jacobian_Return_Value TestSolverCaller::ODEJacFunction(double t, const double * y, const double * p, const double * fy, double * * Jacobian, void * Jac_data)
{
	Jacobian[0][0] = 0;
	Jacobian[0][1] = 1;
	Jacobian[1][0] = 1;
	Jacobian[1][1] = 0;

	return JACOBIAN_OK;
}
Rhs_Return_Value TestSolverCaller::DDERhsFunction (double t, const double * y, const double * * yd, double * ydot, void * f_data)
{return RHS_OK;}

void TestSolverCaller::DDEDelayFunction (double t, const double * y, double * delays, void * delays_data)
{}

bool TestSolverCaller::IsSet_ODERhsFunction(){return true;}
bool TestSolverCaller::IsSet_ODEJacFunction(){return UseJacobian;}
bool TestSolverCaller::IsSet_DDERhsFunction(){return false;}
bool TestSolverCaller::IsSet_ODESensitivityRhsFunction() { return false; }

bool TestSolverCaller::UseBandLinearSolver(){return false;}
int TestSolverCaller::GetLowerHalfBandWidth(){return 0;}
int TestSolverCaller::GetUpperHalfBandWidth(){return 0;}

Sensitivity_Rhs_Return_Value TestSolverCaller::ODESensitivityRhsFunction(double t, const double * y, double * ydot,
	int iS, const double * yS, double * ySdot, void * f_data)
{return SENSITIVITY_RHS_OK;}

void TestSolverCrash(void)
{

	const int noOfVars = 10000;
	const unsigned int numberOfTimesteps = 20000;
	const double   dt = 0.001;
	double* time;
	double* y0;
	double* y1;

	time = new double[numberOfTimesteps];
	y0   = new double[numberOfTimesteps];
	y1   = new double[numberOfTimesteps];

	HINSTANCE hLib = NULL;
	typedef SimModelSolverBase * (* GetSolverInterfaceFnType)(ISolverCaller *, int);
	GetSolverInterfaceFnType pGetSolverInterface = NULL;

	SimModelSolverBase * pCVODES282 = NULL;

	try
	{
		TestSolverCaller SC;
		SC.CrashTime=(numberOfTimesteps-2)*dt;
		SC.UseJacobian = false;
		SC.NoOfVars = noOfVars;

		std::string LibName = "OSPSuite.SimModelSolver_CVODES282.dll";
		hLib = LoadLibrary(LibName.c_str());
		if(!hLib)
			throw "Cannot load library " + LibName;

		pGetSolverInterface = (GetSolverInterfaceFnType)GetProcAddress(hLib, "GetSolverInterface");
		if(!pGetSolverInterface)
			throw LibName+" is not valid SimModel Solver";

		pCVODES282 = (pGetSolverInterface)(&SC, noOfVars);

		pCVODES282->SetAbsTol(1e-12);
		pCVODES282->SetInitialTime(0.0);

		vector<double> startValues;
		startValues.push_back(2.0);
		startValues.push_back(0.0);

		for(int ii=3;ii<=noOfVars;ii++)
			startValues.push_back(0.0);

		pCVODES282->SetInitialValues(startValues);

		pCVODES282->Init();

		double Solution[noOfVars];

		for(int i=1; i<=numberOfTimesteps; i++)
		{
			double tout = dt*i;
			double tret;
			pCVODES282->PerformSolverStep(tout, Solution, NULL, tret);

			time[i-1] = tret;
			y0[i-1]   = Solution[0];
			y1[i-1]   = Solution[1];
		}

		delete pCVODES282;
		//pCVODES282->Terminate();
		pCVODES282=NULL;

		FreeLibrary(hLib);
		hLib = NULL;
	}
	catch(...)
	{
		if(pCVODES282)
			delete pCVODES282;
		if(hLib)
			FreeLibrary(hLib);
		throw;
	}

}

