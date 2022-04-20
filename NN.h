#pragma once

//Autor - -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release [v0.1] on GitHub 27.09.2018
//Actual version 4.5
//The library include functions and classes to provide support the work with Neural Networks

#if defined(_WIN32)
	#include <Windows.h>
	typedef HCRYPTPROV dxCRYPT;
	typedef unsigned long int dxFastInt32;
	//#define	def_FILEROOT "./" is BAD
#elif defined(__unix__)
    #if defined(__linux__)
		#include <random>
		#include <chrono>
		typedef ::std::mt19937 dxCRYPT;
		typedef uint_fast32_t dxFastInt32;
		//#define	def_FILEROOT "./" is BAD
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>
#include <cstdarg>
#include <cmath>

#if !defined(defDX_S)
#define defDX_S(x)		#x
#endif
#if !defined(defDX_S_)
#define defDX_S_(x)		defDX_S(x)
#endif
#if !defined(defDX_S__LINE__)
#define defDX_S__LINE__	defDX_S_(__LINE__)
#endif
#if !defined(defDX__FILELINE)
#define defDX__FILELINE		(__FILE__  " line " defDX_S__LINE__)
#endif

#ifndef NOTATION_
#define NOTATION_ ::std::cout << "![NTTN]" << (const char*)defDX__FILELINE << ::std::endl;
#define NTTN_ NOTATION_
#endif

#ifndef ERROR_

#if defined(_WIN32)
#define ERROR_				{ NTTN_ 	\
							throw ::std::exception(); }
#elif defined(__unix__)
#if defined(__linux__)
#define ERROR_				try {																			\
                            	throw ::std::exception();													\
							} catch (::std::exception& x) {													\
								NTTN_	\
							}
#else
#error This UNIX operating system is not supported by dx::NN
#endif
#else
#error This operating system is not supported by dx::NN
#endif

#endif
#ifndef ER_IF
#define ER_IF(x, beforeExc, AfterExc) if ( (x) ) { beforeExc ERROR_ AfterExc }
#endif
#ifndef ER_IFN
#define ER_IFN(x, beforeExc, AfterExc) if ( !(x) ) { beforeExc ERROR_ AfterExc }
#endif

// header flag
#define _NNDX_NEURONET_DEF

namespace nndx
{
	using _dTYPEFUNC = unsigned short int;

	dxFastInt32 randT(dxCRYPT&);
	dxFastInt32 randB(dxCRYPT&);

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
#ifdef _NNDX_CONV_NEURONET_DEF
		friend class CNN;
#endif
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
		explicit neuronet(neuronet&&) noexcept(false); // ch
		neuronet(const dy_tpl&, double(void), neuron::_func); // ch
		~neuronet(); // ch

		neuronet& operator=(const neuronet&) noexcept(false);
		neuronet& operator=(neuronet&&) noexcept(false);

		bool init(const dy_tpl&, neuron::_func); // ch
		bool init(const dy_tpl&); // ch
		bool init(const ::std::vector<int>&, neuron::_func); // ch
		bool init(const ::std::vector<int>&); // ch
		bool initFromKeyboard(); // ch
		bool initFromFile(); // ch
		bool setFunc(neuron::_func); // ch
		bool setGenWeightsFunc(double(void));
		bool setParams(double amoment, double au); // ch

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
		void funcHebb(); // ch

#ifdef _NNDX_CONV_NEURONET_DEF
	protected:
		void backProp(const ::std::vector<double>&, ::std::vector<double>&); // ch
		void backPropByErr(const ::std::vector<double>&, ::std::vector<double>&); // ch
		bool callBackProp(const ::std::vector<double>&, ::std::vector<double>&); // ch
		bool callBackPropByErr(const ::std::vector<double>&, ::std::vector<double>&); // ch
#endif
	};
}
