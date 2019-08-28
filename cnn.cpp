//include ...opencv\build\include
//connect libs ...opencv\build\x64\vc15\lib
//and .dll ...opencv\build\x64\vc15\bin

#include <iostream>
#include <vector>
#include <string>

#define GETBIT(x,pos)	( ((x) & ( 0x1 << (pos) )) != 0 )

#include "ConvNN.h"

HCRYPTPROV hProv;

int main()
{
	if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		if (GetLastError() == NTE_BAD_KEYSET)
			if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				ERROR_
					return 0;
			}

	//nndx::CNN test;
	nndx::CNN test("output/savedCNN.txt", []()->double { return (nndx::randT(hProv) % 100) / 100.; });
	nndx::__bool res;

	/*res = test.initDir("images", "output", "data");

	res = test.initKrnlFromFile(0, 0);
	//res = test.initKrnlFromFile(1, 0);
	//res = test.initKrnlFromFile(2, 0);
	res = test.initKrnl(1, 0, 4, 4, []()->double { return (nndx::randT(hProv) % 100) / 100.; });
	res = test.initKrnl(2, 0, 5, 5, []()->double { return (nndx::randT(hProv) % 100) / 100.; });
	//res = test.initKrnl(3, 0, 3, 3, []()->double { return (nndx::randT(hProv) % 100) / 100.; });
	res = test.initKrnlFromFile(3, 0);

	cv::Mat img;
	img = cv::imread("images/test0.jpg");
	res = test.init_image(img);

	res = test.initFuncEx([]()->double { return 0.; }, ::std::vector<int>{
		0,
		-2,
		1,
		-2,
		2,
		-2,
		3,
		-2	});
		
	test.setParams(0.2, 0.4);*/

	res = test.init_neuronet(::std::vector<int>{10, 1}, []()->double { return static_cast<double>(((nndx::randB(hProv) % 100) - 50.) / 100.); },
		nndx::neuron::_func::_fnSIGMOID, 0.3, 0.5);

	::std::vector<::std::vector<double>> resNet;
	resNet.reserve(10);
	resNet.emplace_back(::std::vector<double>{0.0});
	resNet.emplace_back(::std::vector<double>{0.1});
	resNet.emplace_back(::std::vector<double>{0.2});
	resNet.emplace_back(::std::vector<double>{0.3});
	resNet.emplace_back(::std::vector<double>{0.4});
	resNet.emplace_back(::std::vector<double>{0.5});
	resNet.emplace_back(::std::vector<double>{0.6});
	resNet.emplace_back(::std::vector<double>{0.7});
	resNet.emplace_back(::std::vector<double>{0.8});
	resNet.emplace_back(::std::vector<double>{0.9});

	int temp1;
	double temp2;

	::std::cout << "0 - iter, else by value" << ::std::endl;
	::std::cin >> temp1;

	if (temp1 == 0)
	{
		::std::cout << "Num iter: ";
		::std::cin >> temp1;

		res = test.mA_Iter(resNet, temp1, [](unsigned int& x)->size_t { return x % 10; }, "test", ".jpg");
	}
	else
	{
		::std::cout << "Instances: ";
		::std::cin >> temp1;
		::std::cout << "value: ";
		::std::cin >> temp2;

		res = test.mA_ByValue(resNet, temp1, temp2, [](unsigned int& x)->size_t { return x % 10; }, "test", ".jpg");
	}

	system("pause");
	return 0;
}