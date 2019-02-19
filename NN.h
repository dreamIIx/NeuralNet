#pragma once

//Autor -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release on GitHub 27.01.2019

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifndef def_ERR_FL_
#define def_ERR_FL_
#define def_XTT_S(x) #x
#define def_XTT_S_(x) def_XTT_S(x)
#define def_XTT_S__LINE__ def_XTT_S_(__LINE__)
#define def__FILELINE (__FILE__  " line " def_XTT_S__LINE__)
#define ERROR_ ::std::cout << def__FILELINE << ::std::endl;
#endif

namespace nndx
{
	int inline randT();
	int inline randB();

	struct dy_tpl
	{
		::std::vector<int> tempDATA;

		dy_tpl(int, ...);

		~dy_tpl();
	};

	class neuron
	{
	public:
		double data;
		double prevdata;
		double funcBP;
		bool BIAS;

		neuron(const double&);
		void goF();
		void goF_BI();
		void is_bias();
	};

	class wWw
	{
	public:
		double wg;
		double dwg;
		//double grad;

		wWw();
		wWw(const double&);
	};

	typedef ::std::vector<neuron> dataA;
	typedef ::std::vector<wWw> dataW;

	class neuronet
	{
	public:
		bool BIfunc;
		//double moment = 0.05; // 0.05
		//double u = 15; // 0.1
		::std::vector<dataA> data;
		::std::vector<dataW> weight;
		::std::vector<int> topology_save;
		::std::string nameF;
		::std::string nameT;

		neuronet();
		neuronet(bool);
		neuronet(const dy_tpl&, bool);
		void saveF(::std::string&);
		//void mA();
		void SPECmA(::std::vector<double>&);
		//void init();
		void activationF();
		//void backProp(::std::vector<double>&);
		//void funcHebb();
	};
}
