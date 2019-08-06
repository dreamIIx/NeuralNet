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
					system("pause");
				return 0;
			}

	nndx::CNN test;
	bool res = false;

	res = test.initDir("images", "output", "data");
	if (!res)
	{
		ERROR_
			return 1;
	}

	res = test.initKrnl(0, 0, 3, 3,
		1., 1., 1.,
		0., 0., 0.,
		-1., -1., -1.);
	if (!res)
	{
		ERROR_
			return 1;
	}

	res = test.initKrnl(0, 1, 3, 3,
		-1., -1., -1.,
		-1., 8., -1.,
		-1., -1., -1.);
	if (!res)
	{
		ERROR_
			return 1;
	}

	/*res = test.initKrnl(0, 2, 5, 5,
		0., 0., 0.5, 1., 0.,
		0., 0.5, 1., 0., -1.,
		0.5, 1., 0., -1., -0.5,
		1., 0., -1., -0.5, 0.,
		0., -1., -0.5, 0., 0.);
	if (!res)
	{
		ERROR_
			return 1;
	}

	res = test.initKrnl(0, 3, 5, 5,
		0., 0., -0.5, -1., 0.,
		0., -0.5, -1., 0., 1.,
		-0.5, -1., 0., 1., 0.5,
		-1., 0., 1., 0.5, 0.,
		0., 1., 0.5, 0., 0.);
	if (!res)
	{
		ERROR_
			return 1;
	}*/

	res = test.initFuncEx(10, //100
		10,
		4,
		10,
		4,
		10,
		4,
		10,
		4,
		4,
		4);
	if (!res)
	{
		ERROR_
			return 1;
	}

	cv::Mat img;
	img = cv::imread("images/test11.jpg");
	res = test.init_image(img);
	if (!res)
	{
		ERROR_
			return 1;
	}

	/*res = test.init_image(100, 100, 0, 0, 0);
	if (!res)
	{
		ERROR_
			return 1;
	}*/
	res = test.autoInitVec(::std::vector<size_t>{0, 0, 0, 0});
	if (!res)
	{
		ERROR_
			return 1;
	}

	res = test.mA();
	if (!res)
	{
		ERROR_
			return 1;
	}
	res = test.init_neuronet(::std::vector<int>{15, 10}, []()->double { return static_cast<double>((nndx::randB(hProv) % 4) - 2.); },
		nndx::neuron::_func::_fnSIGMOID, 0.8, 1.2);
	if (!res)
	{
		ERROR_
			return 1;
	}

	/*res = test.init_neuronet("output/net.txt", nndx::neuron::_func::_fnSIGMOID, 0.8, 1.2);
	if (!res)
	{
		ERROR_
			return 1;
	}*/

	::std::vector<::std::vector<double>> resNet;
	resNet.reserve(20);
	resNet.emplace_back(::std::vector<double>{1., 0., 0., 0., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 1., 0., 0., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 1., 0., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 1., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 1., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 1., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 1., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 0., 1., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 0., 0., 1., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 0., 0., 0., 1.});
	resNet.emplace_back(::std::vector<double>{1., 0., 0., 0., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 1., 0., 0., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 1., 0., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 1., 0., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 1., 0., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 1., 0., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 1., 0., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 0., 1., 0., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 0., 0., 1., 0.});
	resNet.emplace_back(::std::vector<double>{0., 0., 0., 0., 0., 0., 0., 0., 0., 1.});

	//resNet, 5000, [](int& x)->int { return x % 20; }, "test", ".jpg"
	res = test.fullNet_mA(resNet, 2000, [](int& x)->int { return x % 20; }, "test", ".jpg");
	if (!res)
	{
		ERROR_
			return 1;
	}

	system("pause");
	return 0;
}