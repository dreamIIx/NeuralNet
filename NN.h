#pragma once

//Autor -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release on GitHub 27.01.2019

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

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
#ifndef _dCRTYPEFUNC
#define _dCRTYPEFUNC const unsigned short int&
#endif
#endif

#define _fnSIGMOID	0b0000
#define _fnTANH		0b0001
#ifndef _fnDEFAULTFUNC // as default, able to change
//#define _fnDEFAULTFUNC _fnSIGMOID
//#define _fnSDEFAULTFUNC _m_fnSIGMOID_
#define _fnDEFAULTFUNC _fnTANH
#define _fnSDEFAULTFUNC _m_fnTANH_
#endif

namespace nndx
{
	int inline randT();
	int inline randB();

	struct dy_tpl
	{
	private:
		::std::vector<int> tempDATA;

	public:
		dy_tpl(int, ...);
		size_t size() const noexcept;
		const int* data() const;
		~dy_tpl();
	};

	class neuron
	{
	public:
		double data;
		double prevdata;
		double funcDRV;
		bool BIAS;
		void(*RunDefaultFunc_T)(neuron&);

		static void _m_fnSIGMOID_(neuron&);
		static void _m_fnTANH_(neuron&);

	public:
		neuron(const double&, _dCRTYPEFUNC);
		void setAsBias() noexcept;
	};

	class wWw
	{
	public:
		double wg;
		double dwg;
		double grad;

	public:
		wWw(const double&);
	};

	typedef ::std::vector<neuron> dataA;
	typedef ::std::vector<wWw> dataW;

	class neuronet
	{
	public:
		_dTYPEFUNC funcInstance;
		double moment; // 0.05
		double u; // 0.1
		::std::vector<dataA> data;
		::std::vector<dataW> weight;
		::std::string nDataNet;
		::std::string nTrainNote;

	private:
		bool isReady;
		::std::vector<int> topology_save;

	public:
		neuronet() noexcept;
		neuronet(neuronet&&);
		neuronet(_dCRTYPEFUNC) noexcept;
		neuronet(dy_tpl&&, _dCRTYPEFUNC);
		~neuronet();

		void operator=(neuronet&&);

		bool init(dy_tpl&&, _dCRTYPEFUNC);
		bool initFromKeyboard();
		bool initFromFile();

		bool RunTraining();
		bool SPECmA(::std::vector<double>&);
		//bool SPECmA(dy_tpl&&); // args type is INT!!!

		bool saveF(::std::string&&);

		void activationF();
		void backProp(::std::vector<double>&);
		void funcHebb();

		bool getState() const noexcept;
		::std::vector<double> getResults() const;
	};
}
