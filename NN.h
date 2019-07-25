#pragma once

//Autor - -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release [v0.1] on GitHub 27.09.2018
//Actual version 3.5
//The library include functions and classes to provide support the work with Neural Networks

#include <Windows.h>
//#include <bcrypt.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//#pragma comment(lib, "Bcrypt.lib")

#ifndef def_ERR_FL_
#define def_ERR_FL_
#define def_XTT_S(x) #x
#define def_XTT_S_(x) def_XTT_S(x)
#define def_XTT_S__LINE__ def_XTT_S_(__LINE__)
#define def__FILELINE (__FILE__  " line " def_XTT_S__LINE__)
#define ERROR_ ::std::cout << "Error - " << def__FILELINE << ::std::endl;
#endif

#ifndef _dTYPEFUNC
#define _dTYPEFUNC	unsigned short int
#endif

#define _fnSIGMOID	static_cast<_dTYPEFUNC>(0b0000)
#define _fnTANH		static_cast<_dTYPEFUNC>(0b0001)
#ifndef _fnDEFAULTFUNC // as default, able to change
#define _fnDEFAULTFUNC _fnSIGMOID
#define _fnSDRVDEFFUNC _m_fnSIGMOID_DRV
#define _fnSDEFAULTFUNC _m_fnSIGMOID
//#define _fnDEFAULTFUNC _fnTANH
//#define _fnSDRVDEFFUNC _m_fnTANH_DRV
//#define _fnSDEFAULTFUNC _m_fnTANH
#endif

namespace nndx
{
	int randT();
	int randB();

	struct dy_tpl
	{
	private:
		::std::vector<int> tempDATA;

	public:
		explicit dy_tpl(int, ...);
		size_t size() const noexcept;
		auto data() const noexcept;
		~dy_tpl();
	};

	class neuron
	{
		friend class neuronet;
	public:
		double data;
		double prevdata;
		double funcDRV;

	private:
		bool BIAS;

	protected:
		static void _m_fnSIGMOID(neuron&, const double&);
		static void _m_fnSIGMOID_DRV(neuron&);
		static void _m_fnTANH(neuron&, const double&);
		static void _m_fnTANH_DRV(neuron&);

	public:
		explicit neuron(const double&) noexcept;
		void setAsBias() noexcept;
		bool isBias() const noexcept;
	};

	class wWw
	{
	public:
		double wg;
		double dwg;
		double grad;

	public:
		explicit wWw(double) noexcept;
	};

	typedef ::std::vector<neuron> dataA;
	typedef ::std::vector<wWw> dataW;

	class neuronet
	{
	public:
		::std::vector<dataA> data;
		::std::vector<dataW> weight;
		::std::string nDataNet;
		::std::string nTrainNote;

	private:
		_dTYPEFUNC funcInstance;
		double moment; // 0.05
		double u; // 0.1
		bool isReady;
		::std::vector<int> topology_save;
		void (*RunFunc_T)(neuron&, const double&);
		void (*RunDRVFunc_T)(neuron&);
		double(*GenWeight)();

	public:
		explicit neuronet() noexcept; // ch
		explicit neuronet(_dTYPEFUNC) noexcept; // ch
		neuronet(neuronet&&); // ch
		neuronet(const dy_tpl&, double(), _dTYPEFUNC); // ch
		~neuronet(); // ch

		neuronet& operator=(const neuronet&);
		neuronet& operator=(neuronet&&);

		bool init(const dy_tpl&, _dTYPEFUNC); // ch
		bool init(const dy_tpl&); // ch
		bool init(const ::std::vector<int>&, _dTYPEFUNC); // ch
		bool init(const ::std::vector<int>&); // ch
		bool initFromKeyboard(); // ch
		bool initFromFile(); // ch
		bool setFunc(_dTYPEFUNC); // ch
		bool setGenWeightsFunc(double());
		bool setParams(double, double); // ch

		bool RunTraining(bool); // ch
		bool SPECmA(const ::std::vector<double>&); // ch
		bool fillInput(const ::std::vector<double>&); // ch
		bool saveF(const ::std::string&); // ch

		bool callActivationF();
		bool callBackProp(const ::std::vector<double>&);
		bool callFuncHebb();

		bool getState() const noexcept; // ch
		_dTYPEFUNC getSetFunc() const noexcept; // ch
		::std::pair<double, double> getParams() const noexcept; // ch
		::std::vector<double> getResults() const; // ch

	private:
		void activationF(); // work
		void backProp(const ::std::vector<double>&); // work
		void funcHebb();
	};
}
