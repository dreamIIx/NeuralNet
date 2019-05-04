#include "NN.h"

namespace nndx
{
	int randT()
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

	int randB()
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

	auto dy_tpl::data() const noexcept
	{
		return tempDATA.data();
	}

	dy_tpl::~dy_tpl()
	{
		tempDATA.clear();
	}

	neuron::neuron(const double& num) noexcept : data(num), prevdata(0.0), funcDRV(0.0), BIAS(false) {}

	void neuron::_m_fnSIGMOID(neuron& n, const double& value)
	{
		n.data = 1. / (1. + exp(-value));
	}

	void neuron::_m_fnSIGMOID_DRV(neuron& n)
	{
		n.funcDRV = n.data * (1. - n.data);
	}

	void neuron::_m_fnTANH(neuron& n, const double& value)
	{
		n.data = tanh(value);
	}

	void neuron::_m_fnTANH_DRV(neuron& n)
	{
		n.funcDRV = 1. - n.data * n.data;
	}

	void neuron::setAsBias() noexcept
	{
		BIAS = true;
	}

	bool neuron::isBias() const noexcept
	{
		return this->BIAS;
	}

	wWw::wWw(double num) noexcept : wg(num), dwg(0.0), grad(0.0) {}

	neuronet::neuronet() noexcept : funcInstance(_fnDEFAULTFUNC), moment(0.0), u(0.0), isReady(false)
	{
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
			GenWeight = nullptr;
	}

	neuronet::neuronet(_dTYPEFUNC fnIns) noexcept : funcInstance(fnIns), moment(0.0), u(0.0), isReady(false)
	{
		switch (fnIns)
		{
		case _fnSIGMOID:
			RunFunc_T = &neuron::_m_fnSIGMOID;
			RunDRVFunc_T = &neuron::_m_fnSIGMOID_DRV;
			break;
		case _fnTANH:
			RunFunc_T = &neuron::_m_fnTANH;
			RunDRVFunc_T = &neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance = _fnDEFAULTFUNC;
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
		}
		GenWeight = nullptr;
	}

	neuronet::neuronet(neuronet&& anet)
	{
		this->isReady = false;
		try
		{
			if (!anet.getState())
			{
				throw 12;
			}
			else
			{
				this->funcInstance = ::std::forward<decltype(anet.funcInstance)>(anet.funcInstance);
				this->moment = ::std::forward<decltype(anet.moment)>(anet.moment);
				this->u = ::std::forward<decltype(anet.u)>(anet.u);
				this->data = ::std::forward<decltype(anet.data)>(anet.data);
				this->weight = ::std::forward<decltype(anet.weight)>(anet.weight);
				this->topology_save = ::std::forward<decltype(anet.topology_save)>(anet.topology_save);
				this->nDataNet = ::std::forward<decltype(anet.nDataNet)>(anet.nDataNet);
				this->nTrainNote = ::std::forward<decltype(anet.nTrainNote)>(anet.nTrainNote);
				this->RunDRVFunc_T = ::std::forward<decltype(anet.RunDRVFunc_T)>(anet.RunDRVFunc_T);
				this->RunFunc_T = ::std::forward<decltype(anet.RunFunc_T)>(anet.RunFunc_T);
				this->GenWeight = ::std::forward<decltype(anet.GenWeight)>(anet.GenWeight);
				this->isReady = true;
				anet.isReady = false;
			}
		}
		catch (int x)
		{
			if (x == 12)
			{
				::std::cout << "Argument of function-constructor is rvalue/notReady!" << ::std::endl;
			}
		}
	}

	neuronet::neuronet(const dy_tpl& temp, double func(), _dTYPEFUNC fnIns) : funcInstance(fnIns)
	{
		this->GenWeight = func;
		if (this->GenWeight == nullptr)
		{
			ERROR_
				system("pause"); // ERROR <---
		}
		this->isReady = false;

		switch (fnIns)
		{
		case _fnSIGMOID:
			RunFunc_T = &neuron::_m_fnSIGMOID;
			RunDRVFunc_T = &neuron::_m_fnSIGMOID_DRV;
			break;
		case _fnTANH:
			RunFunc_T = &neuron::_m_fnTANH;
			RunDRVFunc_T = &neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance = _fnDEFAULTFUNC;
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
		}

		auto pos = temp.data();
		for (int i = 0; i < temp.size(); i++)
		{
			int a = *pos++;
			if (a > 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; i++)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
			else
			{
				ERROR_
					system("pause"); // ERROR <---
			}
		}

		if (data.empty())
		{
			ERROR_
				system("pause"); // ERROR <---

		}

		for (size_t i = 0; i < data.size() - 1; i++)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		for (size_t i = 0; i < data.size() - 2; i++)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); j++)
			{
				weight.back().reserve(weight.back().capacity() + 1);
				weight.back().emplace_back(this->GenWeight());
			}
		}
		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); j++)
		{
			weight.back().reserve(weight.back().capacity() + 1);
			weight.back().emplace_back(this->GenWeight());
		}

		this->isReady = true;
	}

	neuronet::~neuronet()
	{
		isReady = false;
		RunDRVFunc_T = nullptr;
		RunFunc_T = nullptr;
		GenWeight = nullptr;
		data.clear();
		weight.clear();
		topology_save.clear();
	}

	neuronet& neuronet::operator=(const neuronet& anet)
	{
		try
		{
			if (!anet.getState())
			{
				throw 12;
			}
			else
			{
				if (this == &anet)
				{
					return *this;
				}

				this->funcInstance = anet.funcInstance;
				this->moment = anet.moment;
				this->u = anet.u;
				this->data = anet.data;
				this->weight = anet.weight;
				this->topology_save = anet.topology_save;
				this->nDataNet = anet.nDataNet;
				this->nTrainNote = anet.nTrainNote;
				this->RunDRVFunc_T = anet.RunDRVFunc_T;
				this->RunFunc_T = anet.RunFunc_T;
				this->GenWeight = anet.GenWeight;
				this->isReady = true;
				return *this;
			}
		}
		catch (int x)
		{
			if (x == 12)
			{
				::std::cout << "Argument of function-constructor is rvalue/notReady!" << ::std::endl;
			}
		}
	}

	neuronet& neuronet::operator=(neuronet&& anet)
	{
		try
		{
			if (!anet.getState())
			{
				throw 12;
			}
			else
			{
				if (this == &anet)
				{
					return *this;
				}
				else
				{
					this->funcInstance = ::std::forward<decltype(anet.funcInstance)>(anet.funcInstance);
					this->moment = ::std::forward<decltype(anet.moment)>(anet.moment);
					this->u = ::std::forward<decltype(anet.u)>(anet.u);
					this->data = ::std::forward<decltype(anet.data)>(anet.data);
					this->weight = ::std::forward<decltype(anet.weight)>(anet.weight);
					this->topology_save = ::std::forward<decltype(anet.topology_save)>(anet.topology_save);
					this->nDataNet = ::std::forward<decltype(anet.nDataNet)>(anet.nDataNet);
					this->nTrainNote = ::std::forward<decltype(anet.nTrainNote)>(anet.nTrainNote);
					this->RunDRVFunc_T = ::std::forward<decltype(anet.RunDRVFunc_T)>(anet.RunDRVFunc_T);
					this->RunFunc_T = ::std::forward<decltype(anet.RunFunc_T)>(anet.RunFunc_T);
					this->GenWeight = ::std::forward<decltype(anet.GenWeight)>(anet.GenWeight);
					this->isReady = true;
					anet.isReady = false;

					return *this;
				}
			}
		}
		catch (int x)
		{
			if (x == 12)
			{
				::std::cout << "Argument of function-constructor is rvalue/notReady!" << ::std::endl;
			}
		}
	}

	bool neuronet::init(const dy_tpl& temp, _dTYPEFUNC fnIns)
	{
		if (this->GenWeight == nullptr)
		{
			ERROR_
				return false;
		}

		funcInstance = fnIns;
		switch (funcInstance)
		{
		case _fnSIGMOID:
			RunFunc_T = &neuron::_m_fnSIGMOID;
			RunDRVFunc_T = &neuron::_m_fnSIGMOID_DRV;
			break;
		case _fnTANH:
			RunFunc_T = &neuron::_m_fnTANH;
			RunDRVFunc_T = &neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance = _fnDEFAULTFUNC;
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
		}

		if (this->isReady)
		{
			data.clear();
			weight.clear();
			topology_save.clear();
			this->isReady = false;
		}

		auto pos = temp.data();
		for (int i = 0; i < temp.size(); ++i)
		{
			int a = *pos++;
			if (a > 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; ++i)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
			else
			{
				ERROR_
				return false;
			}
		}

		if (data.empty())
		{
			ERROR_
				return false;
		}

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); ++j)
			{
				weight.back().reserve(weight.back().capacity() + 1);
				weight.back().emplace_back(this->GenWeight());
			}
		}
		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); ++j)
		{
			weight.back().reserve(weight.back().capacity() + 1);
			weight.back().emplace_back(this->GenWeight());
		}

		this->isReady = true;
		return true;
	}
	
	bool neuronet::init(const dy_tpl& temp)
	{
		if (this->GenWeight == nullptr)
		{
			ERROR_
				return false;
		}
		if (this->isReady)
		{
			data.clear();
			weight.clear();
			topology_save.clear();
			this->isReady = false;
		}

		auto pos = temp.data();
		for (int i = 0; i < temp.size(); ++i)
		{
			int a = *pos++;
			if (a > 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; ++i)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
			else
			{
				ERROR_
					return false;
			}
		}

		if (data.empty())
		{
			ERROR_
				return false;
		}

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); ++j)
			{
				weight.back().reserve(weight.back().capacity() + 1);
				weight.back().emplace_back(this->GenWeight());
			}
		}
		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); ++j)
		{
			weight.back().reserve(weight.back().capacity() + 1);
			weight.back().emplace_back(this->GenWeight());
		}

		this->isReady = true;
		return true;
	}

	bool neuronet::init(const ::std::vector<int>& temp, _dTYPEFUNC fnIns)
	{
		if (temp.empty())
		{
			ERROR_
				return false;
		}
		if (this->GenWeight == nullptr)
		{
			ERROR_
				return false;
		}

		funcInstance = fnIns;
		switch (funcInstance)
		{
		case _fnSIGMOID:
			RunFunc_T = &neuron::_m_fnSIGMOID;
			RunDRVFunc_T = &neuron::_m_fnSIGMOID_DRV;
			break;
		case _fnTANH:
			RunFunc_T = &neuron::_m_fnTANH;
			RunDRVFunc_T = &neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance = _fnDEFAULTFUNC;
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
		}

		if (this->isReady)
		{
			data.clear();
			weight.clear();
			topology_save.clear();
			this->isReady = false;
		}

		auto pos = temp.data();
		for (int i = 0; i < temp.size(); ++i)
		{
			int a = *pos++;
			if (a > 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; ++i)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
			else
			{
				ERROR_
					return false;
			}
		}

		if (data.empty())
		{
			ERROR_
				return false;
		}

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); ++j)
			{
				weight.back().reserve(weight.back().capacity() + 1);
				weight.back().emplace_back(this->GenWeight());
			}
		}
		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); ++j)
		{
			weight.back().reserve(weight.back().capacity() + 1);
			weight.back().emplace_back(this->GenWeight());
		}

		this->isReady = true;
		return true;
	}

	bool neuronet::init(const ::std::vector<int>& temp)
	{
		if (temp.empty())
		{
			ERROR_
				return false;
		}
		if (this->GenWeight == nullptr)
		{
			ERROR_
				return false;
		}

		if (this->isReady)
		{
			data.clear();
			weight.clear();
			topology_save.clear();
			this->isReady = false;
		}

		auto pos = temp.data();
		for (int i = 0; i < temp.size(); ++i)
		{
			int a = *pos++;
			if (a > 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; ++i)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
			else
			{
				ERROR_
					return false;
			}
		}

		if (data.empty())
		{
			ERROR_
				return false;
		}

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); ++j)
			{
				weight.back().reserve(weight.back().capacity() + 1);
				weight.back().emplace_back(this->GenWeight());
			}
		}
		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); ++j)
		{
			weight.back().reserve(weight.back().capacity() + 1);
			weight.back().emplace_back(this->GenWeight());
		}

		this->isReady = true;
		return true;
	}

	bool neuronet::initFromKeyboard()
	{
		if (this->GenWeight == nullptr)
		{
			ERROR_
				return false;
		}
		if (this->isReady)
		{
			data.clear();
			weight.clear();
			topology_save.clear();
			this->isReady = false;
		}
		::std::cout << "Please write topology(size of every layer, !PRESS '0' to stop!)" << ::std::endl;

		auto a = -1;
		do
		{
			::std::cin >> a;
			if (a == 0) continue;
			if (a > 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; ++i)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
			else
			{
				ERROR_
				return false;
			}
		} while (a);

		if (data.empty())
		{
			ERROR_
				return false;
		}

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); ++j)
			{
				weight.back().reserve(weight.back().capacity() + 1);
				weight.back().emplace_back(this->GenWeight());
			}
		}
		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); ++j)
		{
			weight.back().reserve(weight.back().capacity() + 1);
			weight.back().emplace_back(this->GenWeight());
		}

		::std::cout << "SUCCESS! Press anykey to start..." << ::std::endl;
		this->isReady = true;
		return true;
	}

	bool neuronet::initFromFile()
	{
		if (this->isReady)
		{
			data.clear();
			weight.clear();
			topology_save.clear();
			this->isReady = false;
		}
		::std::ifstream read(nDataNet);
		if (!read.is_open())
		{
			read.close();
			ERROR_
				return false;
		}

		auto a = 0;
		do
		{
			read >> a;
			if (a != 0)
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int i = 0; i < a; ++i)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
		} while (a);

		if (data.empty())
		{
			ERROR_
				return false;
		}

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		auto w = 0.0;
		for (size_t i = 0; i < data.size() - 2; ++i)
		{
			weight.reserve(weight.capacity() + 1);
			weight.emplace_back(dataW());
			for (size_t j = 0; j < (data[i].size() * data[i + 1].size()) - data[i].size(); ++j)
			{
				weight.back().reserve(weight.back().capacity() + 1);

				read >> w;
				weight.back().emplace_back(w);

				read >> w;
				weight.back().back().dwg = w;
			}

			::std::string check;
			read >> check;
			if (check != "-----")
			{
				read.close();
				ERROR_
					return false;
			}
		}

		weight.reserve(weight.capacity() + 1);
		weight.emplace_back(dataW());
		for (size_t j = 0; j < data[data.size() - 2].size() * data.back().size(); ++j)
		{
			weight.back().reserve(weight.back().capacity() + 1);

			read >> w;
			weight.back().emplace_back(w);

			read >> w;
			weight.back().back().dwg = w;
		}

		::std::string check;
		read >> check;
		if (check != "-----")
		{
			read.close();
			ERROR_
				return false;
		}

		read >> nTrainNote;
		read.close();
		this->isReady = true;
		return true;
	}

	bool neuronet::setFunc(_dTYPEFUNC afn)
	{
		funcInstance = afn;
		switch (funcInstance)
		{
		case _fnSIGMOID:
			RunFunc_T = &neuron::_m_fnSIGMOID;
			RunDRVFunc_T = &neuron::_m_fnSIGMOID_DRV;
			break;
		case _fnTANH:
			RunFunc_T = &neuron::_m_fnTANH;
			RunDRVFunc_T = &neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance = _fnDEFAULTFUNC;
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
		}
		return true;
	}

	bool neuronet::setGenWeightsFunc(double func())
	{
		this->GenWeight = func;
		return true;
	}

	bool neuronet::setParams(double amoment, double au)
	{
		if ((amoment == 0) || (au == 0))
		{
			ERROR_
				return false;
		}
		this->moment = amoment;
		this->u = au;
		return true;
	}

	bool neuronet::RunTraining(bool acomments)
	{
		if (!this->isReady)
		{
			ERROR_
				return false;
		}
		if ((this->moment == 0) || (this->u == 0))
		{
			ERROR_
				return false;
		}
		::std::ifstream read(nTrainNote);
		if (!read.is_open())
		{
			read.close();
			ERROR_
				return false;
		}

		int numT = 0;
		int nums = 0;
		read >> nums;

		read >> numT;
		if (numT != data[0].size() - 1)
		{
			ERROR_
				return false;
		}
		read >> numT;
		if (numT != data.back().size())
		{
			ERROR_
				return false;
		}

		numT = 0;
		while (numT < nums)
		{
			for (size_t i = 0; i < data[0].size() - 1; ++i)
			{
				read >> data[0][i].data;
				if (!data[0][i].isBias())	RunDRVFunc_T(data[0][i]);
			}
			activationF();

			int j = 0;
			::std::vector<double> errDat;
			errDat.reserve(data.back().size());
			for (size_t i = 0; i < data.back().size(); ++i)
			{
				read >> j;
				if (acomments) ::std::cout << "j - " << j << ::std::endl;
				errDat.emplace_back(static_cast<double>(j));
				if (acomments) ::std::cout << abs(j - data.back()[i].data) << ::std::endl;
			}

			backProp(errDat);
			++numT;
		}
		read.close();

		return true;
	}

	bool neuronet::SPECmA(const ::std::vector<double>& dataT)
	{
		if (!this->isReady)
		{
			ERROR_
				return false;
		}
		if (dataT.size() == data[0].size() - 1)
		{
			for (size_t i = 0; i < data[0].size() - 1; ++i)
			{
				data[0][i].data = dataT[i];
				if (!data[0][i].isBias())
				{
					RunDRVFunc_T(data[0][i]);
				}
				else
				{
					ERROR_
						return false;
				}
			}
			activationF();
			return true;
		}
		else
		{
			ERROR_
				return false;
		}
	}

	bool neuronet::fillInput(const ::std::vector<double>& dataT)
	{
		if (dataT.size() == data[0].size() - 1)
		{
			for (size_t i = 0; i < data[0].size() - 1; ++i)
			{
				data[0][i].data = dataT[i];
				if (!data[0][i].isBias())
				{
					RunDRVFunc_T(data[0][i]);
				}
				else
				{
					ERROR_
						return false;
				}
			}
			return true;
		}
		else
		{
			ERROR_
				return false;
		}
	}

	bool neuronet::saveF(const ::std::string& s)
	{
		if (!this->isReady)
		{
			ERROR_
				return false;
		}
		::std::ofstream f(s);
		if (!f.is_open())
		{
			f.close();
			ERROR_
				return false;
		}

		for (size_t i = 0; i < topology_save.size(); ++i)
		{
			f << topology_save[i] << " ";
		}
		f << "0" << ::std::endl;

		for (size_t i = 0; i < weight.size(); ++i)
		{
			for (size_t j = 0; j < weight[i].size(); ++j)
			{
				f << weight[i][j].wg << ::std::endl;
				f << weight[i][j].dwg << ::std::endl;
			}
			f << "-----" << ::std::endl;
		}
		f << this->nTrainNote;
		f.close();

		return true;
	}

	bool neuronet::callActivationF()
	{
		if (!this->isReady)
		{
			ERROR_
				return false;
		}
		activationF();
		return true;
	}

	bool neuronet::callBackProp(const ::std::vector<double>& d)
	{
		if (!this->isReady)
		{
			ERROR_
				return false;
		}
		if ((this->moment == 0) || (this->u == 0))
		{
			ERROR_
				return false;
		}
		backProp(d);
		return true;
	}

	bool neuronet::callFuncHebb()
	{
		if (!this->isReady)
		{
			ERROR_
				return false;
		}
		if (this->u == 0)
		{
			ERROR_
				return false;
		}
		funcHebb();
		return true;
	}

	void neuronet::activationF()
	{
		double local_sum = 0.;

		for (size_t i = 1; i < data.size() - 1; ++i)
		{
			//::std::cout << "----------------  data.size - " << data.size() << "   now - " << i << ::std::endl;
			for (size_t j = 0; j < data[i].size(); ++j)
			{
				//::std::cout << "----------------  data[" << i <<  "].size - " << data[i].size() << "  now - " << j << "   ";
				if (!data[i][j].isBias())
				{
					//::std::cout << "- is not BIAS | ";
					local_sum = 0.;
					for (size_t prev = 0; prev < data[i - 1].size(); ++prev)
					{
						//::std::cout << prev * (data[i].size() - 1) + j << " ";
						local_sum += data[i - 1][prev].data * weight[i - 1][prev * (data[i].size() - 1) + j].wg;
					}
					//::std::cout << ::std::endl;
					data[i][j].prevdata = data[i][j].data;
					RunFunc_T(data[i][j], local_sum);
					RunDRVFunc_T(data[i][j]);
				}
				//else ::std::cout << "- is BIAS | " << ::std::endl;
			}
		}
		//::std::cout << "----------------  data.size - " << data.size() << "   now - last" << ::std::endl;
		for (size_t j = 0; j < data.back().size(); ++j)
		{
			//::std::cout << "----------------  data[last].size - " << data[data.size() - 1].size() << "  now - " << j << "   ";
			if (!data.back()[j].isBias())
			{
				//::std::cout << "- is not BIAS | ";
				local_sum = 0.;
				for (size_t prev = 0; prev < data[data.size() - 2].size(); ++prev)
				{
					//::std::cout << prev * data.back().size() + j << " ";
					local_sum += data[data.size() - 2][prev].data * weight[data.size() - 2][prev * data.back().size() + j].wg;
				}
				//::std::cout << ::std::endl;
				data.back()[j].prevdata = data.back()[j].data;
				RunFunc_T(data.back()[j], local_sum);
				RunDRVFunc_T(data.back()[j]);
			}
			//else ::std::cout << "- is BIAS | " << ::std::endl;
		}
	}

	void neuronet::backProp(const ::std::vector<double>& d)
	{
		typedef ::std::vector<double> dw;
		::std::vector<dw> errR;

		errR.reserve(data.size());
		for (size_t i = 0; i < data.size(); ++i)
		{
			errR.emplace_back(dw());
		}

		errR.back().reserve(data.back().size());
		for (size_t i = 0; i < data.back().size(); ++i)
		{
			errR.back().emplace_back((d[i] - data.back()[i].data) * data.back()[i].funcDRV);
		}

		double local_sum = 0.;
		errR[data.size() - 2].reserve(data[data.size() - 2].size());
		for (size_t j = 0; j < data[data.size() - 2].size(); ++j)
		{
			local_sum = 0.;
			for (size_t next = 0; next < data[data.size() - 1].size(); ++next)
			{
				local_sum += errR[data.size() - 1][next] * weight[data.size() - 2][data[data.size() - 1].size() * j + next].wg;
				weight[data.size() - 2][data[data.size() - 1].size() * j + next].grad = errR[data.size() - 1][next] * data[data.size() - 2][j].data;
			}
			errR[data.size() - 2].emplace_back(local_sum * data[data.size() - 2][j].funcDRV);
		}
		for (int i = data.size() - 3; i >= 0; --i) // �����������  -> signed <-, 'cause ����� ����� ���� �������������(��� ������ �� �����)
		{
			errR[i].reserve(data[i].size());
			for (size_t j = 0; j < data[i].size(); ++j)
			{
				local_sum = 0.;
				for (size_t next = 0; next < data[i + 1].size() - 1; ++next)
				{
					local_sum += errR[i + 1][next] * weight[i][(data[i + 1].size() - 1) * j + next].wg;
					weight[i][(data[i + 1].size() - 1) * j + next].grad = errR[i + 1][next] * data[i][j].data;
				}
				errR[i].emplace_back(local_sum * data[i][j].funcDRV);
			}
		}

		for (size_t i = 1; i < data.size() - 1; ++i)
		{
			for (size_t j = 0; j < data[i].size() - 1; ++j)
			{
				for (size_t prev = 0; prev < data[i - 1].size(); ++prev)
				{
					weight[i - 1][prev * (data[i].size() - 1) + j].dwg = u * weight[i - 1][prev * (data[i].size() - 1) + j].grad + (moment * weight[i - 1][prev * (data[i].size() - 1) + j].dwg);
					weight[i - 1][prev * (data[i].size() - 1) + j].wg += weight[i - 1][prev * (data[i].size() - 1) + j].dwg;
				}
			}
		}
		for (size_t j = 0; j < data[data.size() - 1].size(); ++j)
		{
			for (size_t prev = 0; prev < data[data.size() - 2].size(); ++prev)
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

	bool neuronet::getState() const noexcept
	{
		return this->isReady;
	}

	_dTYPEFUNC neuronet::getSetFunc() const noexcept
	{
		return this->funcInstance;
	}

	::std::pair<double, double> neuronet::getParams() const noexcept
	{
		return ::std::make_pair(this->moment, this->u);
	}

	::std::vector<double> neuronet::getResults() const
	{
		if (!this->isReady)
		{
			ERROR_
				return {};
		}
		::std::vector<double> temp;
		temp.reserve(data.back().size());
		for (size_t i = 0; i < data.back().size(); ++i)
		{
			temp.emplace_back(data.back()[i].data);
		}
		return temp;
	}
}