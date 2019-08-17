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

	nndx::CNN test;
	nndx::__bool res;

	res = test.initDir("images", "output", "data");

	res = test.initKrnl(0, 0, 3, 3, []()->double { return (nndx::randT(hProv) % 10 - 5.) / 10.; });
	res = test.initKrnl(0, 1, 3, 3, []()->double { return (nndx::randT(hProv) % 10 - 5.) / 10.; });
	res = test.initKrnl(1, 0, 3, 3, []()->double { return (nndx::randT(hProv) % 10 - 5.) / 10.; });
	res = test.initKrnl(1, 1, 3, 3, []()->double { return (nndx::randT(hProv) % 10 - 5.) / 10.; });
	res = test.initKrnlFromFile(2, 0);

	cv::Mat img;
	img = cv::imread("images/test0.jpg");
	res = test.init_image(img);

	res = test.initFuncEx([]()->double { return (nndx::randT(hProv) % 10) / 10.; }, 8, //num
		0,
		-2,
		1,
		-2,
		1,
		-2,
		2,
		-2);

	res = test.init_neuronet(::std::vector<int>{50, 25, 1}, []()->double { return static_cast<double>(((nndx::randB(hProv) % 10) - 5.) / 10.); },
		nndx::neuron::_func::_fnSIGMOID, 0.3, 0.5, 0.4);

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

	/*resNet.emplace_back(::std::vector<double>{0.0});
	resNet.emplace_back(::std::vector<double>{0.1});
	resNet.emplace_back(::std::vector<double>{0.2});
	resNet.emplace_back(::std::vector<double>{0.3});
	resNet.emplace_back(::std::vector<double>{0.4});
	resNet.emplace_back(::std::vector<double>{0.5});
	resNet.emplace_back(::std::vector<double>{0.6});
	resNet.emplace_back(::std::vector<double>{0.7});
	resNet.emplace_back(::std::vector<double>{0.8});
	resNet.emplace_back(::std::vector<double>{0.9});*/

	//resNet, 5000, [](int& x)->int { return x % 20; }, "test", ".jpg"
	res = test.fullNet_mA(resNet, 2, [](int& x)->int { return x % 10; }, "test", ".jpg");

	system("pause");
	return 0;
}