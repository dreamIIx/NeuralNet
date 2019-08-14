//include ...opencv\build\include
//connect libs ...opencv\build\x64\vc15\lib
//and .dll ...opencv\build\x64\vc15\bin

#include <iostream>
#include <vector>
#include <string>

#include <Windows.h>

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

	nndx::CNN test;
	nndx::__bool res;

	res = test.initDir("images", "output", "data");

	res = test.initKrnlFromFile(0, 0);
	res = test.initKrnlFromFile(0, 1);
	//res = test.initKrnlFromFile(0, 2);
	//res = test.initKrnlFromFile(0, 3);
	//res = test.initKrnlFromFile(0, 4);
	res = test.initKrnlFromFile(1, 0);
	//res = test.initKrnlFromFile(1, 1);
	res = test.initKrnlFromFile(2, 0);

	res = test.initFuncEx(8, //num
		10,
		4,
		10,
		4,
		10,
		4,
		10,
		4);

	cv::Mat img;
	img = cv::imread("images/test11.jpg");
	res = test.init_image(img);

	/*res = test.init_image(100, 100, 0, 0, 0);
	if (!res)
	{
		ERROR_
			return 1;
	}*/
	res = test.autoInitVec(::std::vector<size_t>{0, 1, 1, 2});

	res = test.mA();

	res = test.init_neuronet(::std::vector<int>{50, 25, 1}, []()->double { return static_cast<double>(((nndx::randB(hProv) % 100) - 50.) / 100.); },
		nndx::neuron::_func::_fnSIGMOID, 0.3, 0.5);

	/*res = test.init_neuronet("output/net.txt", nndx::neuron::_func::_fnSIGMOID, 0.8, 1.2);*/

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

	//resNet, 5000, [](int& x)->int { return x % 20; }, "test", ".jpg"
	res = test.fullNet_mA(resNet, 2000, [](int& x)->int { return x % 10; }, "test", ".jpg");

	system("pause");
	return 0;
}