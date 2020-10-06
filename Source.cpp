//#include <iostream>
//
//#include "NN.h"
//
//HCRYPTPROV hProv1;
//
//int main()
//{
//	if (!CryptAcquireContext(&hProv1, 0, NULL, PROV_RSA_FULL, 0))
//		if (GetLastError() == NTE_BAD_KEYSET)
//			if (!CryptAcquireContext(&hProv1, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
//			{
//				ERROR_
//					return 0;
//			}
//
//	nndx::neuronet net;
//	net.setGenWeightsFunc([]()->double { return static_cast<double>( ((nndx::randB(hProv1) % 10) - 5.) / 10.); });
//	net.init(nndx::dy_tpl( 3, 10, 15, 1 ), nndx::neuron::_func::_fnSIGMOID);
//	net.setParams(0.35, 0.5);
//	if (!net.getState())
//	{
//		ERROR_
//	}
//
//	int val = 0;
//
//	::std::vector<double> input;
//	input.reserve(10);
//
//	/*::std::vector<::std::vector<double>> resNet;
//	resNet.reserve(10);
//	resNet.emplace_back(::std::vector<double>{1., -1., -1., -1., -1., -1., -1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., 1., -1., -1., -1., -1., -1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., 1., -1., -1., -1., -1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., 1., -1., -1., -1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., -1., 1., -1., -1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., -1., -1., 1., -1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., -1., -1., -1., 1., -1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., -1., -1., -1., -1., 1., -1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., -1., -1., -1., -1., -1., 1., -1.});
//	resNet.emplace_back(::std::vector<double>{-1., -1., -1., -1., -1., -1., -1., -1., -1., 1.});*/
//
//	while (val < 10000)
//	{
//		input.clear();
//		for (int i = 0; i < 10; ++i)
//		{
//			input.emplace_back((val % 10) / 10.);
//		}
//		net.fillInput(input);
//
//		if (!net.callActivationF()) { ERROR_ }
//
//		::std::cout << val % 10 << "\n";
//		int tempval_ = 0;
//		for (auto& x : net.getResults())
//		{
//			::std::cout << tempval_++ << ": " << x << "| " << "\n";
//		}
//		::std::cout << "\n";
//
//		if (!net.callBackProp(::std::vector<double>{ (val % 10) / 10. })) { ERROR_ }
//		
//		++val;
//	}
//	::std::cout << ::std::endl;
//
//	system("pause");
//	return 0;
//}