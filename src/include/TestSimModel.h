#ifndef __TESTSIMMODEL_H
#define __TESTSIMMODEL_H

#include "stdafx.h"
#include <string>
#include <vector>

//#include "SimModelComp/SimModelComp.h"
#include "SimModel/Simulation.h"
#include "XMLWrapper/XMLCache.h"
#include "FuncParser/ParsedFunction.h"
#include "FuncParser/FuncParserErrorData.h"
#include "SimModel/ExplicitFormula.h"
#include "SimModel/Simulation.h"
#include "SimModel/MatlabODEExporter.h"

#include <iostream>
//#include "DCI/Component.h"
//#include "DCI/Manager.h"

using namespace std;

void ShowErrorMessage(const string & msg);
void ShowErrorMessage(const ErrorData & ED);
string BasisDir(const string & exeName);
string TestFileFrom(const string & fileName);
string XMLSchemaFile();
string SimModelCompConfigFile();
void ShowTimeSpan(double tstart, double tend);

void Test1(const string & simFile);
void Test2();
void Test3(const string & simFile);
//void Test4(const string & simFile);
//void Test5(const string & simFile);
//void TestMCMC_Crash(const string & simFile, bool varyParams);
//void MCMC_SetVariableParameters(SimModelComp * hComp);

void TestMCMC_Crash_WithoutDCI(const string & simFile, bool varyParams);

using namespace SimModelNative;

//SimModelComp * CreateSimModelComp();
void FreeSim(Simulation * & sim);
Simulation * LoadSimulation(const string & fileName);
void FinalizeSimulation(Simulation * sim);
void RunSimulation(Simulation * sim, bool showInfo=true);
SimModelNative::Variable * GetVenousBloodPlasma(Simulation * sim, const string & venPlsId);

//void setAllValueParametersVariable(DCI::IComponentHandle hComp);

class ExplicitFormulaExtender : public SimModelNative::ExplicitFormula
{
public:
	void CreateFormulaFromEquation(const std::vector<std::string> & variableNames, 
		                           const std::vector<std::string> & parameterNames,
		                           const std::vector<double> & parameterValues,
		                           const std::vector<std::string> & parameterNotToSimplifyNames,
								   bool simplifyParameter)
	{
		SimModelNative::ExplicitFormula::CreateFormulaFromEquation(
			variableNames, parameterNames, parameterValues,
		    parameterNotToSimplifyNames, simplifyParameter);
	}

	void SetEquation(const std::string & equation)
	{
		_equation = equation;
	}
};

//namespace DCI
//{
//	Bool _LoadComponentFromXMLFile(IComponentHandle hComp,const String & FileName, 
//						 		  String *TypeName, String * TypeDescription);
//};

void TestSolverCrash(void);

void WriteRhsAndJacobianOutputs(Simulation * sim, const std::string & rhsFile, const std::string & jacobianFile);

void ShowFirstWarning(Simulation * sim);
// Testsystem with N variables:
//
//  y0' = y1
//  y1' = y2
//  y2'=0;
//  ...
//  Y_N'=0;
class TestSolverCaller : public ISolverCaller
{
public:
	TestSolverCaller();
	bool UseJacobian;
	double CrashTime;
	int NoOfVars;
	Rhs_Return_Value ODERhsFunction(double t, const double * y, const double * p, double * ydot, void * f_data);
	Jacobian_Return_Value ODEJacFunction(double t, const double * y, const double * p, const double * fy, double * * Jacobian, void * Jac_data);
	Rhs_Return_Value DDERhsFunction (double t, const double * y, const double * * yd, double * ydot, void * f_data);
	void DDEDelayFunction (double t, const double * y, double * delays, void * delays_data);
	bool IsSet_ODERhsFunction();
	bool IsSet_ODEJacFunction();
	bool IsSet_DDERhsFunction();
	bool IsSet_ODESensitivityRhsFunction();

	bool UseBandLinearSolver();
	int GetLowerHalfBandWidth();
	int GetUpperHalfBandWidth();

	Sensitivity_Rhs_Return_Value ODESensitivityRhsFunction(double t, const double * y, double * ydot, int iS, const double * yS, double * ySdot, void * f_data);
};

void TestBandSolver01(const std::string & simFile, bool useBandSolver);
void TestModelCoreBandCrash();

#endif