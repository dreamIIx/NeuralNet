#pragma once

//Autor - -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release [v0.1] on GitHub 27.09.2018
//Actual version 4.1.5
//The library include functions and classes to provide support the work with Neural Networks

#include <Windows.h>

#include <iostream>
#ifndef _DEBUG
#include <exception>
#endif
#include <fstream>
#include <string>
#include <vector>

#ifdef _DEBUG
#define def_XTT_S(x)		#x
#define def_XTT_S_(x)		def_XTT_S(x)
#define def_XTT_S__LINE__	def_XTT_S_(__LINE__)
#define def__FILELINE		(__FILE__  " line " def_XTT_S__LINE__)
#define ERROR_				::std::cout << "Error - " << def__FILELINE << ::std::endl;
#else
#define ERROR_				throw ::std::exception((const char*)__LINE__);
#endif

typedef unsigned short int _dTYPEFUNC;

namespace nndx
{
	int randT(HCRYPTPROV);
	int randB(HCRYPTPROV);

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

#define _fnSDRVDEFFUNC		_m_fnSIGMOID_DRV
#define _fnSDEFAULTFUNC		_m_fnSIGMOID
//#define _fnSDRVDEFFUNC _m_fnTANH_DRV
//#define _fnSDEFAULTFUNC _m_fnTANH

	public:
		enum class _func : _dTYPEFUNC
		{
			_fnSIGMOID		=	0b0000,
			_fnTANH			=	0b0001,
			_fnDEFAULTFUNC	=	_fnSIGMOID // able to change -> _fnTANH
		};

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
		neuron::_func funcInstance;
		double moment; // 0.05
		double u; // 0.1
		bool isReady;
		::std::vector<int> topology_save;
		void (*RunFunc_T)(neuron&, const double&);
		void (*RunDRVFunc_T)(neuron&);
		double(*GenWeight)();

	public:
		explicit neuronet() noexcept; // ch
		explicit neuronet(neuron::_func) noexcept; // ch
		neuronet(neuronet&&); // ch
		neuronet(const dy_tpl&, double(void), neuron::_func); // ch
		~neuronet(); // ch

		neuronet& operator=(const neuronet&);
		neuronet& operator=(neuronet&&);

		bool init(const dy_tpl&, neuron::_func); // ch
		bool init(const dy_tpl&); // ch
		bool init(const ::std::vector<int>&, neuron::_func); // ch
		bool init(const ::std::vector<int>&); // ch
		bool initFromKeyboard(); // ch
		bool initFromFile(); // ch
		bool setFunc(neuron::_func); // ch
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
		neuron::_func getSetFunc() const noexcept; // ch
		::std::pair<double, double> getParams() const noexcept; // ch
		::std::vector<double> getResults() const; // ch

	private:
		void activationF(); // work
		void backProp(const ::std::vector<double>&); // work
		void funcHebb();
	};
}
