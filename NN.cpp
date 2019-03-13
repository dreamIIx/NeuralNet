#include "NN.h"

namespace nndx
{
	dy_tpl::dy_tpl(int size, ...)
	{
		va_list args;
		va_start(args, size);

		tempDATA.reserve(size);
		for (int i = 0; i < size; ++i)
		{
			int value = va_arg(args, int);
			tempDATA.emplace_back(value);
		}
		va_end(args);
	}

	size_t dy_tpl::size() const noexcept
	{
		return tempDATA.size();
	}

	const int* dy_tpl::data() const
	{
		return tempDATA.data();
	}

	dy_tpl::~dy_tpl()
	{
		tempDATA.clear();
	}

	neuron::neuron(const double& num, _dCRTYPEFUNC _afunc_) : data(num), prevdata(0.0), funcDRV(0.0), BIAS(false)
	{
		switch (_afunc_)
		{
		case _fnSIGMOID:
			RunDefaultFunc_T = &this->_m_fnSIGMOID_;
			break;
		case _fnTANH:
			RunDefaultFunc_T = &this->_m_fnTANH_;
			break;
		default:
#ifdef _fnDEFAULTFUNC // as default, able to change
			RunDefaultFunc_T = &this->_fnSDEFAULTFUNC;
#else
			RunDefaultFunc_T = nullptr;
#endif
		}
	}

	void neuron::_m_fnSIGMOID_(neuron& n)
	{
		n.funcDRV = n.data * (1 - n.data);
	}

	void neuron::_m_fnTANH_(neuron& n)
	{
		n.funcDRV = 1 - n.data * n.data;
	}

	void neuron::is_bias() noexcept
	{
		BIAS = true;
		RunDefaultFunc_T = nullptr;
	}

	wWw::wWw(const double& num) : wg(num), dwg(0.0), grad(0.0) {}

	neuronet::neuronet() noexcept : funcInstance(_fnDEFAULTFUNC) {}

	neuronet::neuronet(neuronet&& anet)
	{
		funcInstance = ::std::forward<decltype(anet.funcInstance)>(anet.funcInstance);
		moment = ::std::forward<decltype(anet.moment)>(anet.moment);
		u = ::std::forward<decltype(anet.u)>(anet.u);
		data = ::std::forward<decltype(anet.data)>(anet.data);
		weight = ::std::forward<decltype(anet.weight)>(anet.weight);
		topology_save = ::std::forward<decltype(anet.topology_save)>(anet.topology_save);
		nameF = ::std::forward<decltype(anet.nameF)>(anet.nameF);
		nameT = ::std::forward<decltype(anet.nameT)>(anet.nameT);
	}

	neuronet::neuronet(_dCRTYPEFUNC fnIns) noexcept : funcInstance(fnIns) {}

	neuronet::neuronet(const dy_tpl& temp, _dCRTYPEFUNC fnIns) : funcInstance(fnIns)
	{
		const int* pos = temp.data();
		for (int i = 0; i < temp.size(); i++)
		{
			int a = *pos++;
			if (a > 0)
			{
				data.push_back(dataA());

				for (int i = 0; i < a; i++)
				{
					data.back().push_back(neuron(0, funcInstance));
				}
				topology_save.push_back(a);
			}
		}

		for (size_t i = 0; i < data.size() - 1; i++)
		{
			data[i].push_back(neuron(1, funcInstance));
			data[i].back().is_bias();
		}

		for (size_t i = 0; i < data.size() - 2; i++)
		{
			weight.push_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); j++)
			{
				weight.back().push_back((nndx::randT() % 6) - 2);
			}
		}
		weight.push_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); j++)
		{
			weight.back().push_back((nndx::randT() % 6) - 2);
		}
	}

	void neuronet::saveF(const ::std::string& s)
	{
		::std::cout << "Outputing weights..." << ::std::endl;

		::std::ofstream f(s);
		for (size_t i = 0; i < topology_save.size(); i++)
		{
			f << topology_save[i] << " ";
		}
		f << "0" << ::std::endl;

		for (size_t i = 0; i < weight.size(); i++)
		{
			for (size_t j = 0; j < weight[i].size(); j++)
			{
				f << weight[i][j].wg << ::std::endl;
				f << weight[i][j].dwg << ::std::endl;
			}
			f << "-----" << ::std::endl;
		}
		f << s;
		f.close();
		::std::cout << "Weights is saved!(File - " << s << ")" << ::std::endl;
	}

	void neuronet::mA()
	{
		::std::ifstream read(nameT);
		if (!read.is_open())
		{
			::std::cout << "Error of opening file - " << nameT << ::std::endl;
			read.close();
			ERROR_
			system("pause"); // error
		}

		int num = 0;
		int nums;
		read >> nums;

		int k;
		read >> k;
		if (k != data[0].size() - 1)
		{
			::std::cout << "Error, wrong training data!" << ::std::endl;
			ERROR_
			system("pause"); // error
		}
		read >> k;
		if (k != data.back().size())
		{
			::std::cout << "Error, wrong number of output neurons!" << ::std::endl;
			ERROR_
			system("pause"); // error
		}

		system("cls");

		while (num < nums)
		{

			for (int i = 0; i < data[0].size() - 1; i++)
			{
				read >> data[0][i].data;
				if (!data[0][i].BIAS)	data[0][i].RunDefaultFunc_T(data[0][i]);
			}
			activationF();

			::std::vector<double> errDat;
			for (int i = 0; i < data.back().size(); i++)
			{
				double j;
				read >> j;
				errDat.push_back(j);
			}
			backProp(errDat);
			num++;
		}
		read.close();
	}

	void neuronet::SPECmA(::std::vector<double>& dataT)
	{
		if (dataT.size() == data[0].size() - 1)
		{
			for (size_t i = 0; i < data[0].size() - 1; i++)
			{
				data[0][i].data = dataT[i];
				if (!data[0][i].BIAS)
				{
					data[0][i].RunDefaultFunc_T(data[0][i]);
				}
				else
				{
					::std::cout << "checkout here!" << ::std::endl;
					ERROR_
					system("pause");
				}
			}
			activationF();
		}
		else
		{
			ERROR_
			system("pause");
		}
	}

	void neuronet::SPECmA(const dy_tpl& temp)
	{
		const int* pos = temp.data();
		if (temp.size() == data[0].size() - 1)
		{
			for (size_t i = 0; i < data[0].size() - 1; i++)
			{
				data[0][i].data = *pos++;
				if (!data[0][i].BIAS)
				{
					data[0][i].RunDefaultFunc_T(data[0][i]);
				}
				else
				{
					::std::cout << "checkout here!" << ::std::endl;
					ERROR_
						system("pause");
				}
			}
			activationF();
		}
		else
		{
			ERROR_
				system("pause");
		}
	}

	void neuronet::init()
	{
		int a = -1;

		::std::cout << "Write -1000 for new topology, or any number for init topology from file: ";
		::std::cin >> a;
		if (a == -1000)
		{
			while (a != 0)
			{
				system("cls");
				::std::cout << "Please write topology(size of every layer)" << ::std::endl;
				::std::cin >> a;
				if (a > 0)
				{
					data.push_back(dataA());

					for (int i = 0; i < a; i++)
					{
						data.back().push_back(neuron(0, funcInstance));
					}
					topology_save.push_back(a);
				}
			}

			for (size_t i = 0; i < data.size() - 1; i++)
			{
				data[i].push_back(neuron(1, funcInstance));
				data[i].back().is_bias();
			}

			for (size_t i = 0; i < data.size() - 2; i++)
			{
				weight.push_back(dataW());
				for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); j++)
				{
					weight.back().push_back((nndx::randT() % 6) - 2);
				}
			}
			weight.push_back(dataW());
			for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); j++)
			{
				weight.back().push_back((nndx::randT() % 6) - 2);
			}
			::std::cout << "Training file(is defined as): " << nameT << ::std::endl;
			::std::cout << "SUCCESS! Press anykey to start..." << ::std::endl;
		}
		else
		{
			::std::ifstream read(nameF);
			if (!read.is_open())
			{
				::std::cout << "Wrong filename! ERROR" << ::std::endl;
				read.close();
				ERROR_
				system("pause"); // error
			}
			read >> a;
			while (a != 0)
			{
				::std::cout << a << ::std::endl;
				data.push_back(dataA());

				for (int i = 0; i < a; i++)
				{
					data.back().push_back(neuron(0, funcInstance));
				}
				topology_save.push_back(a);

				read >> a;
			}
			double w;

			for (size_t i = 0; i < data.size() - 1; i++)
			{
				data[i].push_back(neuron(1, funcInstance));
				data[i].back().is_bias();
			}
			::std::cout << "Init weights from file..." << ::std::endl;
			for (size_t i = 0; i < data.size() - 2; i++)
			{
				weight.push_back(dataW());
				for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); j++)
				{
					read >> w;
					weight.back().push_back(w);
					::std::cout << w << ::std::endl;
					read >> w;
					weight.back().back().dwg = w;
					::std::cout << w << ::std::endl;
				}
				::std::string check;
				read >> check;
				if (check != "-----")
				{
					::std::cout << "check = " << check << ::std::endl;
					::std::cout << "ERROR! CHECKOUT!" << ::std::endl;
					ERROR_
					system("pause"); // error
				}
			}
			weight.push_back(dataW());
			for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); j++)
			{
				read >> w;
				weight.back().push_back(w);
				::std::cout << w << ::std::endl;
				read >> w;
				weight.back().back().dwg = w;
				::std::cout << w << ::std::endl;
			}
			::std::string check;
			read >> check;
			if (check != "-----")
			{
				::std::cout << "check = " << check << ::std::endl;
				::std::cout << "ERROR! CHECKOUT!" << ::std::endl;
				ERROR_
				system("pause"); // error
			}
			read >> nameT;
			read.close();
			::std::cout << "Name of training file: " << nameT << " (check)" << ::std::endl;
			::std::cout << "SUCCESS! The neuronet already...(press anykey to start)" << ::std::endl;
			system("pause");
		}
	}

	void neuronet::activationF()
	{
		double local_sum = 0;

		for (size_t i = 1; i < data.size() - 1; i++)
		{
			//::std::cout << "----------------  data.size - " << data.size() << "   now - " << i << ::std::endl;
			for (size_t j = 0; j < data[i].size(); j++)
			{
				//::std::cout << "----------------  data[" << i <<  "].size - " << data[i].size() << "  now - " << j << "   ";
				if (!data[i][j].BIAS)
				{
					//::std::cout << "- is not BIAS | ";
					local_sum = 0;
					for (size_t prev = 0; prev < data[i - 1].size(); prev++)
					{
						//::std::cout << prev * (data[i].size() - 1) + j << " ";
						local_sum += data[i - 1][prev].data * weight[i - 1][prev * (data[i].size() - 1) + j].wg;
					}
					//::std::cout << ::std::endl;
					data[i][j].prevdata = data[i][j].data;
					switch(funcInstance)
					{
					case _fnSIGMOID:
						data[i][j].data = 1 / (1 + exp(-local_sum));
						data[i][j].RunDefaultFunc_T(data[i][j]);
						break;
					case _fnTANH:
						data[i][j].data = tanh(local_sum);
						data[i][j].RunDefaultFunc_T(data[i][j]);
						break;
					}
				}
				//else ::std::cout << "- is BIAS | " << ::std::endl;
			}
		}
		//::std::cout << "----------------  data.size - " << data.size() << "   now - last" << ::std::endl;
		for (size_t j = 0; j < data.back().size(); j++)
		{
			//::std::cout << "----------------  data[last].size - " << data[data.size() - 1].size() << "  now - " << j << "   ";
			if (!data.back()[j].BIAS)
			{
				//::std::cout << "- is not BIAS | ";
				local_sum = 0;
				for (size_t prev = 0; prev < data[data.size() - 2].size(); prev++)
				{
					//::std::cout << prev * data.back().size() + j << " ";
					local_sum += data[data.size() - 2][prev].data * weight[data.size() - 2][prev * data.back().size() + j].wg;
				}
				//::std::cout << ::std::endl;
				data.back()[j].prevdata = data.back()[j].data;
				switch (funcInstance)
				{
				case _fnSIGMOID:
					data.back()[j].data = 1 / (1 + exp(-local_sum));
					data.back()[j].RunDefaultFunc_T(data.back()[j]);
					break;
				case _fnTANH:
					data.back()[j].data = tanh(local_sum);
					data.back()[j].RunDefaultFunc_T(data.back()[j]);
					break;
				}
			}
			//else ::std::cout << "- is BIAS | " << ::std::endl;
		}
	}

	void neuronet::backProp(::std::vector<double>& d)
	{
		typedef ::std::vector<double> dw;
		::std::vector<dw> errR;

		for (size_t i = 0; i < data.size(); i++)
		{
			errR.push_back(dw());
		}

		for (size_t i = 0; i < data.back().size(); i++)
		{
			errR.back().push_back((d[i] - data.back()[i].data) * data.back()[i].funcDRV);
		}

		double local_sum = 0;
		for (size_t j = 0; j < data[data.size() - 2].size(); j++)
		{
			local_sum = 0;
			for (size_t next = 0; next < data[data.size() - 1].size(); next++)
			{
				local_sum += errR[data.size() - 1][next] * weight[data.size() - 2][data[data.size() - 1].size() * j + next].wg;
				weight[data.size() - 2][data[data.size() - 1].size() * j + next].grad = errR[data.size() - 1][next] * data[data.size() - 2][j].data;
			}
			errR[data.size() - 2].push_back(local_sum * data[data.size() - 2][j].funcDRV);
		}
		for (int i = data.size() - 3; i >= 0; i--) // обязательно  -> signed <-, 'cause число может быть отрицательным(для выхода из цикла)
		{
			for (size_t j = 0; j < data[i].size(); j++)
			{
				local_sum = 0;
				for (size_t next = 0; next < data[i + 1].size() - 1; next++)
				{
					local_sum += errR[i + 1][next] * weight[i][(data[i + 1].size() - 1) * j + next].wg;
					weight[i][(data[i + 1].size() - 1) * j + next].grad = errR[i + 1][next] * data[i][j].data;
				}
				errR[i].push_back(local_sum * data[i][j].funcDRV);
			}
		}

		for (size_t i = 1; i < data.size() - 1; i++)
		{
			for (size_t j = 0; j < data[i].size() - 1; j++)
			{
				for (size_t prev = 0; prev < data[i - 1].size(); prev++)
				{
					weight[i - 1][prev * (data[i].size() - 1) + j].dwg = u * weight[i - 1][prev * (data[i].size() - 1) + j].grad + (moment * weight[i - 1][prev * (data[i].size() - 1) + j].dwg);
					weight[i - 1][prev * (data[i].size() - 1) + j].wg += weight[i - 1][prev * (data[i].size() - 1) + j].dwg;
				}
			}
		}
		for (size_t j = 0; j < data[data.size() - 1].size(); j++)
		{
			for (size_t prev = 0; prev < data[data.size() - 2].size(); prev++)
			{
				weight[data.size() - 2][prev * data[data.size() - 1].size() + j].dwg = u * weight[data.size() - 2][prev * data[data.size() - 1].size() + j].grad + (moment * weight[data.size() - 2][prev * data[data.size() - 1].size() + j].dwg);
				weight[data.size() - 2][prev * data[data.size() - 1].size() + j].wg += weight[data.size() - 2][prev * data[data.size() - 1].size() + j].dwg;
			}
		}
	}

	void neuronet::funcHebb()
	{
		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			for (size_t j = 0; j < data[i].size(); ++j)
			{
				for (size_t next = 0; next < data[i + 1].size() - 1; ++next)
				{
					weight[i][j * (data[i + 1].size() - 1) + next].wg = weight[i][j * (data[i + 1].size() - 1) + next].wg + u * (data[i][j].prevdata - data[i][j].data) * (data[i + 1][next].prevdata - data[i + 1][next].data);
				}
			}
		}
		for (size_t j = 0; j < data[data.size() - 2].size(); ++j)
		{
			for (size_t next = 0; next < data.back().size(); ++next)
			{
				weight[data.size() - 2][j * data.back().size() + next].wg = weight[data.size() - 2][j * data.back().size() + next].wg + u * (data[data.size() - 2][j].prevdata - data[data.size() - 2][j].data) * (data.back()[next].prevdata - data.back()[next].data);
			}
		}
	}

	int inline randT()
	{
		HCRYPTPROV hProv;

		BYTE Buf1;
		BYTE Buf2;

		if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		{
			if (GetLastError() == NTE_BAD_KEYSET)
			{
				if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				{
					::std::cout << "Error ... [crypt]" << ::std::endl;
					ERROR_
						system("pause");
					return 0;
				}
				else
				{
					CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1);
					CryptReleaseContext(hProv, 0);
				}
			}
		}
		else
		{
			CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1);
			CryptReleaseContext(hProv, 0);
		}

		if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		{
			if (GetLastError() == NTE_BAD_KEYSET)
			{
				if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				{
					::std::cout << "Error ... [crypt]" << ::std::endl;
					ERROR_
						system("pause");
					return 0;
				}
				else
				{
					CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf2);
					CryptReleaseContext(hProv, 0);
				}
			}
		}
		else
		{
			CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf2);
			CryptReleaseContext(hProv, 0);
		}

		int i = (int)Buf1;
		i += (int)Buf2;

		return i;
	}

	int inline randB()
	{
		HCRYPTPROV hProv;

		BYTE Buf1;
		BYTE Buf2;
		int i;

		if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		{
			if (GetLastError() == NTE_BAD_KEYSET)
			{
				if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				{
					//Error
					return 0;
				}
				else
				{
					CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1);
					CryptReleaseContext(hProv, 0);
					i = static_cast<int>(Buf1);
				}
			}
		}
		else
		{
			CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1);
			CryptReleaseContext(hProv, 0);
			i = static_cast<int>(Buf1);
		}

		if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		{
			if (GetLastError() == NTE_BAD_KEYSET)
			{
				if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				{
					//Error
					return 0;
				}
				else
				{
					CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf2);
					CryptReleaseContext(hProv, 0);
					i <<= static_cast<int>(Buf2 % 24);
				}
			}
		}
		else
		{
			CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf2);
			CryptReleaseContext(hProv, 0);
			i <<= static_cast<int>(Buf2 % 24);
		}

		return i;
	}
}