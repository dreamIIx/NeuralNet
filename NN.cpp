#include "NN.h"

namespace nndx
{
	/*void somefunc(void somefunc2(bool, int, double), bool somefunc3(bool, bool) noexcept(false))
	{

	}*/
#if defined(_WIN32)
	dxFastInt32 randT(dxCRYPT& hProv)
	{
		BYTE Buf1 = 0b0;

		if (CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1))
		{
			dxFastInt32 i = static_cast<dxFastInt32>(Buf1);
			if (CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1))
			{
				i += static_cast<dxFastInt32>(Buf1);
				return i;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}

	dxFastInt32 randB(dxCRYPT& hProv)
	{
		BYTE Buf1 = 0b0;

		if (CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1))
		{
			dxFastInt32 i = static_cast<dxFastInt32>(Buf1);
			i <<= 0b1000;

			if (CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1))
			{
				i += static_cast<dxFastInt32>(Buf1);
				i <<= 0b1000;

				if (CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1))
				{
					i += static_cast<dxFastInt32>(Buf1);
					i <<= 0b1000; 

					if (CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1))
					{
						i += static_cast<dxFastInt32>(Buf1);
						return i;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
#elif defined(__unix__)
    #if defined(__linux__)
		dxFastInt32 randT(dxCRYPT& hProv)
		{
			/*dxFastInt32 temp = hProv() % 0b1'1111'1111;
			::std::cout << temp << ::std::endl;*/
			return hProv() % 0b1'1111'1111;
		}

		dxFastInt32 randB(dxCRYPT& hProv)
		{
			/*dxFastInt32 temp = hProv();
			::std::cout << temp << ::std::endl;*/
			return hProv();
		}
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif
	

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

	neuron::neuron(const double& num) noexcept : data(num), prevdata(0.), funcDRV(0.), BIAS(false) {}

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
		funcDRV = 1.;
	}

	bool neuron::isBias() const noexcept
	{
		return this->BIAS;
	}

	wWw::wWw(double num) noexcept : wg(num), dwg(0.), grad(0.) {}

	neuronet::neuronet() noexcept : funcInstance(neuron::_func::_fnDEFAULTFUNC), moment(0.), u(0.), isReady(false)
	{
			RunFunc_T		=	&neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T	=	&neuron::_fnSDRVDEFFUNC;
			GenWeight		=	nullptr;
	}

	neuronet::neuronet(neuron::_func fnIns) noexcept : funcInstance(fnIns), moment(0.), u(0.), isReady(false)
	{
		switch (fnIns)
		{
		case neuron::_func::_fnSIGMOID:
			RunFunc_T		=	&neuron::_m_fnSIGMOID;
			RunDRVFunc_T	=	&neuron::_m_fnSIGMOID_DRV;
			break;
		case neuron::_func::_fnTANH:
			RunFunc_T		=	&neuron::_m_fnTANH;
			RunDRVFunc_T	=	&neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance	=	neuron::_func::_fnDEFAULTFUNC;
			RunFunc_T		=	&neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T	=	&neuron::_fnSDRVDEFFUNC;
		}
		GenWeight = nullptr;
	}

	neuronet::neuronet(neuronet&& anet) noexcept(false)
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
				this->funcInstance		=	/*::std::forward<decltype(anet.funcInstance)>*/ ::std::move(anet.funcInstance);
				this->moment			=	/*::std::forward<decltype(anet.moment)>*/ ::std::move(anet.moment);
				this->u					=	/*::std::forward<decltype(anet.u)>*/ ::std::move(anet.u);
				this->data				=	/*::std::forward<decltype(anet.data)>*/ ::std::move(anet.data);
				this->weight			=	/*::std::forward<decltype(anet.weight)>*/ ::std::move(anet.weight);
				this->topology_save		=	/*::std::forward<decltype(anet.topology_save)>*/ ::std::move(anet.topology_save);
				this->nDataNet			=	/*::std::forward<decltype(anet.nDataNet)>*/ ::std::move(anet.nDataNet);
				this->nTrainNote		=	/*::std::forward<decltype(anet.nTrainNote)>*/ ::std::move(anet.nTrainNote);
				this->RunDRVFunc_T		=	/*::std::forward<decltype(anet.RunDRVFunc_T)>*/ ::std::move(anet.RunDRVFunc_T);
				this->RunFunc_T			=	/*::std::forward<decltype(anet.RunFunc_T)>*/ ::std::move(anet.RunFunc_T);
				this->GenWeight			=	/*::std::forward<decltype(anet.GenWeight)>*/ ::std::move(anet.GenWeight);
				this->isReady			=	true;
				anet.isReady			=	false;
			}
		}
		catch (int x)
		{
			ER_IF(x == 12,, )
		}
	}

	neuronet::neuronet(const dy_tpl& temp, double func(void), neuron::_func fnIns) : funcInstance(fnIns), moment(0.), u(0.)
	{
		this->GenWeight = func;
		ER_IF(this->GenWeight == nullptr,, )
		this->isReady = false;

		switch (fnIns)
		{
		case neuron::_func::_fnSIGMOID:
			RunFunc_T		=	&neuron::_m_fnSIGMOID;
			RunDRVFunc_T	=	&neuron::_m_fnSIGMOID_DRV;
			break;
		case neuron::_func::_fnTANH:
			RunFunc_T		=	&neuron::_m_fnTANH;
			RunDRVFunc_T	=	&neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance	=	neuron::_func::_fnDEFAULTFUNC;
			RunFunc_T		=	&neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T	=	&neuron::_fnSDRVDEFFUNC;
		}

		auto pos = temp.data();
		for (int i = 0; i < temp.size(); i++)
		{
			int a = *pos++;
			ER_IFN(a > 0,, )
			else
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int j = 0; j < a; j++)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
		}

		ER_IF(data.empty(),, )

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
		isReady			=	false;
		RunDRVFunc_T	=	nullptr;
		RunFunc_T		=	nullptr;
		GenWeight		=	nullptr;
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

				this->funcInstance			=	anet.funcInstance;
				this->moment				=	anet.moment;
				this->u						=	anet.u;
				this->data					=	anet.data;
				this->weight				=	anet.weight;
				this->topology_save			=	anet.topology_save;
				this->nDataNet				=	anet.nDataNet;
				this->nTrainNote			=	anet.nTrainNote;
				this->RunDRVFunc_T			=	anet.RunDRVFunc_T;
				this->RunFunc_T				=	anet.RunFunc_T;
				this->GenWeight				=	anet.GenWeight;
				this->isReady				=	true;
			}
		}
		catch (int x)
		{
			ER_IF(x == 12,, )
		}

		return *this;
	}

	neuronet& neuronet::operator=(neuronet&& anet) noexcept(false)
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
					this->funcInstance			=	/*::std::forward<decltype(anet.funcInstance)>*/ ::std::move(anet.funcInstance);
					this->moment				=	/*::std::forward<decltype(anet.moment)>*/ ::std::move(anet.moment);
					this->u						=	/*::std::forward<decltype(anet.u)>*/ ::std::move(anet.u);
					this->data					=	/*::std::forward<decltype(anet.data)>*/ ::std::move(anet.data);
					this->weight				=	/*::std::forward<decltype(anet.weight)>*/ ::std::move(anet.weight);
					this->topology_save			=	/*::std::forward<decltype(anet.topology_save)>*/ ::std::move(anet.topology_save);
					this->nDataNet				=	/*::std::forward<decltype(anet.nDataNet)>*/ ::std::move(anet.nDataNet);
					this->nTrainNote			=	/*::std::forward<decltype(anet.nTrainNote)>*/ ::std::move(anet.nTrainNote);
					this->RunDRVFunc_T			=	/*::std::forward<decltype(anet.RunDRVFunc_T)>*/ ::std::move(anet.RunDRVFunc_T);
					this->RunFunc_T				=	/*::std::forward<decltype(anet.RunFunc_T)>*/ ::std::move(anet.RunFunc_T);
					this->GenWeight				=	/*::std::forward<decltype(anet.GenWeight)>*/ ::std::move(anet.GenWeight);
					this->isReady				=	true;
					anet.isReady				=	false;
				}
			}
		}
		catch (int x)
		{
			ER_IF(x == 12,, )
		}

		return *this;
	}

	bool neuronet::init(const dy_tpl& temp, neuron::_func fnIns)
	{
		ER_IF(this->GenWeight == nullptr,, return false; )

		funcInstance = fnIns;
		switch (funcInstance)
		{
		case neuron::_func::_fnSIGMOID:
			RunFunc_T		=	&neuron::_m_fnSIGMOID;
			RunDRVFunc_T	=	&neuron::_m_fnSIGMOID_DRV;
			break;
		case neuron::_func::_fnTANH:
			RunFunc_T		=	&neuron::_m_fnTANH;
			RunDRVFunc_T	=	&neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance	=	neuron::_func::_fnDEFAULTFUNC;
			RunFunc_T		=	&neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T	=&neuron::_fnSDRVDEFFUNC;
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
			ER_IFN(a > 0,, return false; )
			else
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int j = 0; j < a; ++j)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
		}

		ER_IF(data.empty(),, return false; )

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
		ER_IF(this->GenWeight == nullptr,, return false; )
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
			ER_IFN(a > 0,, return false; )
			else
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int j = 0; j < a; ++j)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
		}

		ER_IF(data.empty(),, return false; )

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

	bool neuronet::init(const ::std::vector<int>& temp, neuron::_func fnIns)
	{
		ER_IF(temp.empty(),, return false; )
		ER_IF(this->GenWeight == nullptr,, return false; )

		funcInstance = fnIns;
		switch (funcInstance)
		{
		case neuron::_func::_fnSIGMOID:
			RunFunc_T		=	&neuron::_m_fnSIGMOID;
			RunDRVFunc_T	=	&neuron::_m_fnSIGMOID_DRV;
			break;
		case neuron::_func::_fnTANH:
			RunFunc_T		=	&neuron::_m_fnTANH;
			RunDRVFunc_T	=	&neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance	=	neuron::_func::_fnDEFAULTFUNC;
			RunFunc_T		=	&neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T	=	&neuron::_fnSDRVDEFFUNC;
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
			ER_IFN(a > 0,, return false; )
			else
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int j = 0; j < a; ++j)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
		}

		ER_IF(data.empty(),, return false; )

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
		ER_IF(temp.empty(),, return false; )
		ER_IF(this->GenWeight == nullptr,, return false; )

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
			ER_IFN(a > 0,, return false; )
			else
			{
				data.reserve(data.capacity() + 1);
				data.emplace_back(dataA());

				for (int j = 0; j < a; ++j)
				{
					data.back().reserve(data.back().capacity() + 1);
					data.back().emplace_back(neuron(0));
				}
				topology_save.reserve(topology_save.capacity() + 1);
				topology_save.emplace_back(a);
			}
		}

		ER_IF(data.empty(),, return false; )

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
		ER_IF(this->GenWeight == nullptr,, return false; )
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
			ER_IFN(a > 0,, return false; )
			else
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

		ER_IF(data.empty(),, return false; )

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
		ER_IF(!read.is_open(), read.close();, return false; )

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

		ER_IF(data.empty(),, return false; )

		for (size_t i = 0; i < data.size() - 1; ++i)
		{
			data[i].reserve(data[i].capacity() + 1);
			data[i].emplace_back(neuron(1));
			data[i].back().setAsBias();
		}

		auto w = 0.;
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
			ER_IF(check != "-----", read.close();, return false; )
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
		ER_IF(check != "-----", read.close();, return false; )

		read >> nTrainNote;
		read.close();
		this->isReady = true;
		return true;
	}

	bool neuronet::setFunc(neuron::_func afn)
	{
		funcInstance = afn;
		switch (funcInstance)
		{
		case neuron::_func::_fnSIGMOID:
			RunFunc_T = &neuron::_m_fnSIGMOID;
			RunDRVFunc_T = &neuron::_m_fnSIGMOID_DRV;
			break;
		case neuron::_func::_fnTANH:
			RunFunc_T = &neuron::_m_fnTANH;
			RunDRVFunc_T = &neuron::_m_fnTANH_DRV;
			break;
		default:
			funcInstance = neuron::_func::_fnDEFAULTFUNC;
			RunFunc_T = &neuron::_fnSDEFAULTFUNC;
			RunDRVFunc_T = &neuron::_fnSDRVDEFFUNC;
		}
		return true;
	}

	bool neuronet::setGenWeightsFunc(double func(void))
	{
		this->GenWeight = func;
		return true;
	}

	bool neuronet::setParams(double amoment, double au)
	{
		ER_IF((amoment == 0) || (au == 0),, return false; )
		this->moment = amoment;
		this->u = au;
		return true;
	}

	bool neuronet::RunTraining(bool acomments)
	{
		ER_IF(!this->isReady,, return false; )
		ER_IF((this->moment == 0) || (this->u == 0),, return false; )

		::std::ifstream read(nTrainNote);
		ER_IF(!read.is_open(), read.close();, return false; )

		int numT = 0;
		int nums = 0;
		read >> nums;

		read >> numT;
		ER_IF(numT != data[0].size() - 1,, return false; )
		read >> numT;
		ER_IF(numT != data.back().size(),, return false; )

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
				if (acomments) ::std::cout << "j - data[" << i << "] = " << abs(j - data.back()[i].data) << ::std::endl;
			}

			backProp(errDat);
			++numT;
		}
		read.close();

		return true;
	}

	bool neuronet::SPECmA(const ::std::vector<double>& dataT)
	{
		ER_IF(!this->isReady,, return false; )
		ER_IFN(dataT.size() == data[0].size() - 1,, return false; )
		else
		{
			for (size_t i = 0; i < data[0].size() - 1; ++i)
			{
				data[0][i].data = dataT[i];
				ER_IF(data[0][i].isBias(),, return false; )
				else
				{
					RunDRVFunc_T(data[0][i]);
				}
			}
			activationF();
			return true;
		}
	}

	bool neuronet::fillInput(const ::std::vector<double>& dataT)
	{
		ER_IFN(dataT.size() == data[0].size() - 1,, return false; )
		else
		{
			for (size_t i = 0; i < data[0].size() - 1; ++i)
			{
				data[0][i].data = dataT[i];
				ER_IF(data[0][i].isBias(),, return false; )
				else
				{
					RunDRVFunc_T(data[0][i]);
				}
			}
			return true;
		}
	}

	bool neuronet::saveF(const ::std::string& s)
	{
		ER_IF(!this->isReady,, return false; )
		::std::ofstream f(s);
		ER_IF(!f.is_open(), f.close();, return false; )

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
		ER_IF(!this->isReady,, return false; )
		activationF();
		return true;
	}

	bool neuronet::callBackProp(const ::std::vector<double>& d)
	{
		ER_IF(!this->isReady,, return false; )
		ER_IF((this->moment == 0) || (this->u == 0),, return false; )
		ER_IF(d.size() != data.back().size(),, return false; )
		backProp(d);
		return true;
	}

	bool neuronet::callFuncHebb()
	{
		ER_IF(!this->isReady,, return false; )
		ER_IF(this->u == 0,, return false; )
		funcHebb();
		return true;
	}

	void neuronet::activationF()
	{
		double local_sum = 0.;

		for (size_t i = 1; i < data.size() - 1; ++i)
		{
			for (size_t j = 0; j < data[i].size(); ++j)
			{
				if (!data[i][j].isBias())
				{
					local_sum = 0.;
					for (size_t prev = 0; prev < data[i - 1].size(); ++prev)
					{
						local_sum += data[i - 1][prev].data * weight[i - 1][prev * (data[i].size() - 1) + j].wg;
					}
					data[i][j].prevdata = data[i][j].data;
					RunFunc_T(data[i][j], local_sum);
					RunDRVFunc_T(data[i][j]);
				}
			}
		}
		for (size_t j = 0; j < data.back().size(); ++j)
		{
			if (!data.back()[j].isBias())
			{
				local_sum = 0.;
				for (size_t prev = 0; prev < data[data.size() - 2].size(); ++prev)
				{
					local_sum += data[data.size() - 2][prev].data * weight[data.size() - 2][prev * data.back().size() + j].wg;
				}
				data.back()[j].prevdata = data.back()[j].data;
				RunFunc_T(data.back()[j], local_sum);
				RunDRVFunc_T(data.back()[j]);
			}
		}
	}

	void neuronet::backProp(const ::std::vector<double>& d)
	{
		using dw = ::std::vector<double>;
		::std::vector<dw> errR;

		errR.reserve(data.size());
		for (size_t i = 0; i < data.size(); ++i)
		{
			errR.emplace_back(dw());
		}

		//default error
		for (size_t i = 0; i < data.back().size(); ++i)
		{
			errR.back().emplace_back((d[i] - data.back()[i].data) * data.back()[i].funcDRV);
		}

		double local_sum = 0.;
		errR[data.size() - 2].reserve(data[data.size() - 2].size());
		for (size_t j = 0; j < data[data.size() - 2].size(); ++j)
		{
			local_sum = 0.;
			for (size_t next = 0; next < data.back().size(); ++next)
			{
				local_sum += errR[data.size() - 1][next] * weight[data.size() - 2][data.back().size() * j + next].wg;
				weight[data.size() - 2][data.back().size() * j + next].grad = errR[data.size() - 1][next] * data[data.size() - 2][j].data;
			}
			errR[data.size() - 2].emplace_back(local_sum * data[data.size() - 2][j].funcDRV);
		}
		for (ptrdiff_t i = static_cast<ptrdiff_t>(data.size() - 3); i >= 0; --i)
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
		for (size_t j = 0; j < data.back().size(); ++j)
		{
			for (size_t prev = 0; prev < data[data.size() - 2].size(); ++prev)
			{
				weight[data.size() - 2][prev * data.back().size() + j].dwg = u * weight[data.size() - 2][prev * data.back().size() + j].grad + (moment * weight[data.size() - 2][prev * data.back().size() + j].dwg);
				weight[data.size() - 2][prev * data.back().size() + j].wg += weight[data.size() - 2][prev * data.back().size() + j].dwg;
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

	neuron::_func neuronet::getSetFunc() const noexcept
	{
		return this->funcInstance;
	}

	::std::pair<double, double> neuronet::getParams() const noexcept
	{
		return ::std::make_pair(this->moment, this->u);
	}

	::std::vector<double> neuronet::getResults() const
	{
		ER_IF(!this->isReady,, return {}; )
		::std::vector<double> temp;
		temp.reserve(data.back().size());
		for (size_t i = 0; i < data.back().size(); ++i)
		{
			temp.emplace_back(data.back()[i].data);
		}
		return temp;
	}
}