//include ...opencv\build\include
//connect libs ...opencv\build\x64\vc15\lib
//and .dll ...opencv\build\x64\vc15\bin

#include <iostream>
#include <vector>
#include <string>

#define GETBIT(x,pos)	( ((x) & ( 0x1 << (pos) )) != 0 )

#include "ConvNN.h"

#if defined(_WIN32)
#include "Windows.h"
#elif defined(__unix__)
#if defined(__linux__)
#include <X11/Xlib.h>
#else
#error This UNIX operating system is not supported by dx::NN
#endif
#endif

dxCRYPT hProv;

int main(int argc, char** argv)
{
#if defined(_WIN32)
	if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		if (GetLastError() == NTE_BAD_KEYSET)
			if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				ERROR_
					system("pause");
				return 0;
			}
	// win defs
#elif defined(__unix__)
    #if defined(__linux__)
		hProv.seed(::std::chrono::system_clock::to_time_t(::std::chrono::system_clock::now()));
    	//XInitThreads();
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif

	nndx::CNN test;
	//nndx::CNN test("output/savedCNN.txt", []()->double { return static_cast<double>((nndx::randB(hProv) % 100) / 100.); });
	//nndx::__bool res;

	::std::vector<ptrdiff_t> vArg;
	cv::Mat img;

#if defined(_WIN32)
	ER_IFN(test.initDir("images/", "output/", "data/"), , return 1; )
#elif defined(__unix__)
#if defined(__linux__)
	ER_IFN(test.initDir("./images/", "./output/", "./data/"), , return 1; )
#else
#error This UNIX operating system is not supported by dx::NN
#endif
#else
#error This operating system is not supported by dx::NN
#endif

	int mA_var = -1;
	bool isloadedCNN = false;
	bool isloadedNN = false;

	// <NUMOFARGV (ARGC)> --<option1> <numofargv of option1> <argv of option1> --<option2> ...
	// example: 9 --loadkrnl 3 1 kr11 0 kr12 1 kr21 --initfunc 3 prll 0 -2
	// Default dirs(pathes) should be ./image/, ./output/, ./data/, or specify them by
	// --initdirs: --initdirs <path/to/images> <path/to/output> <path/to/data>
	//		instead of following flags
	// --loadCNN: <path/to/saved/CNN.txt> <rand function accuracy ([int])>
	// --loadNN: <path/to/saved/NN.txt> <path/to/saved/INN.txt> <func: 0 or 1 (SIGMOID, TANH)> <moment of learning> <speed of learning>
	// --initimg: <path/filename.extension (of input image)>
	// --loadkrnl: --loadkrnl <numofargs> <bool flag1: create new vkernel element> <filename1 of kernel1 (without ext - .krnl)> <flag2> <filename2> ...
	// --loadkrnl: <flag> <filename> - is ONE arg
	// --loadkrnl: mostly first argv should be "1 <name>", since vkernel.size() == 0 (for emplace first vkernel element)
	// --loadkinf: <path/filename.kinf (of kernel info file)>
	// .kinf files: syntax same as --loadkrnl init (without first arg - numofargs)
	// --initfunc: --initfunc <numofargs> <arg1: vfunc> <arg2: vfunc> ...
	// --initfunc: --initfunc <numofargs> ... "prll" <parallel arg> ...
	// --initfunc: "prll" <parallel arg> - is ONE arg
	// 				vfunc:
	// 				x >= 0 - convFunc_RGB (x - idx of kernel's layer)
	// 				-2 - decreaseX2_RGB(max)
	// 				-3 - decreaseX2_RGB(min)
	// 				-4 - decreaseX2_RGB(mid)
	// --loadfunc: <path/filename.finf (of functions info file)>
	// .finf files: syntax same as --initfunc init (without first arg - numofargs)
	// --maoption: option to learn by
	// 			1: result on one picture
	//			2: learn CNN on result pack by defined iterations (<file with desire result> <count of iterations> <subString of images> <num: func to get id of each image (x % num)> <extension of images>)
	ER_IF(argc <= 0, ::std::cout << "Nothing to do" << ::std::endl;, return 1; )

	int i = 1;
	while(i < argc)
	{
		::std::string option = argv[i++];
		
		if (option == "--initdirs")
		{
			option = argv[i++];
			::std::string str1 = argv[i++];
			::std::string str2 = argv[i++];
			ER_IFN(test.initDir(option, str1, str2),, return 1; )
		}
		else if (option == "--loadCNN")
		{
			option = argv[i++];
			int accur = ::std::atoi(argv[i++]);
			ER_IFN(test.initCNN(option.c_str(), [accur]() -> double { return static_cast<double>((nndx::randB(hProv) % (accur + 1)) / static_cast<double>(accur)); }),
				::std::cout << "ERROR while loading an CNN [defined path : " << option << ']' << ::std::endl;, return 1; )
			isloadedCNN = true;
		}
		else if (option == "--loadNN")
		{
			::std::string str1 = argv[i++];
			::std::string str2 = argv[i++];
			int funcopt = ::std::atoi(argv[i++]);
			double mnt = ::std::atof(argv[i++]);
			double u = ::std::atof(argv[i++]);
			if (funcopt == 0)
			{
				test.init_neuronet(str1, str2, nndx::neuron::_func::_fnSIGMOID, mnt, u);
			}
			else if (funcopt == 1)
			{
				test.init_neuronet(str1, str2, nndx::neuron::_func::_fnTANH, mnt, u);
			}
			isloadedNN = true;
		}
		else if (option == "--loadkrnl")
		{
			int num = ::std::atoi(argv[i++]);
			ER_IFN(num > 0,, return 1; )
			for(int j = 0; j < num; ++j)
			{
				int flag = ::std::atoi(argv[i++]);
				option = argv[i++];
				ER_IFN(test.addKrnlFromFile((flag == 0) ? false : true, option.c_str()),, return 1; )
			}
		}
		else if (option == "--loadkinf")
		{
			option = argv[i++];
			::std::ifstream read(option);
			ER_IFN(read.is_open(),
				::std::cout << "read.is_open() returns - false" << ::std::endl;
				::std::cout << "option = " << option << ::std::endl;, return 1; )
			else
			{
				while (!read.eof())
				{
					read >> option;
					if (option == "###") break;
					int flag1 = ::std::atoi(option.c_str());
					read >> option;
					int flag2 = ::std::atoi(option.c_str());
					read >> option;
					if (!::std::strcmp(option.c_str(), "none")) option = "";
					ER_IFN(test.defKrnlFromFile(static_cast<size_t>(flag1), static_cast<size_t>(flag2), option.c_str()),, return 1; )
				}
			}
		}
		else if (option == "--initfunc")
		{
			int num = ::std::atoi(argv[i++]);
			ER_IFN(num > 0,, return 1; )
			vArg.reserve(num);
			for(int j = 0; j < num; ++j)
			{
				int arg;
				option = argv[i++];
				if (option != "prll") arg = ::std::atoi(option.c_str());
				else arg = nndx::operator ""_sprll(::std::atoi(argv[i++]));
				vArg.emplace_back(arg);
			}
		}
		else if (option == "--loadfunc")
		{
			option = argv[i++];
			::std::ifstream read(option);
			ER_IFN(read.is_open(),
				::std::cout << "read.is_open() returns - false" << ::std::endl;
				::std::cout << "option = " << option << ::std::endl; , return 1; )
			else
			{
				while (!read.eof())
				{
					int arg;
					read >> option;
					if (option == "###") break;
					if (option != "prll")
					{
						arg = ::std::atoi(option.c_str());
					}
					else
					{
						read >> arg;
						arg = nndx::operator ""_sprll(arg);
					}
					vArg.reserve(vArg.capacity() + 1);
					vArg.emplace_back(arg);
				}
			}
		}
		else if (option == "--initimg")
		{
			option = argv[i++];
			img = cv::imread(option);
			ER_IFN(test.init_image(img), ::std::cout << "option = " << option << ::std::endl; , return 1; )
		}
		else if (option == "--ma")
		{
			mA_var = ::std::atoi(argv[i++]);
		}
		else if (option == "--help")
		{
			::std::cout << R"(
<NUMOFARGV (ARGC)> <option1> <numofargv of option1> <argv of option1> <option2> ...
example: 9 --loadkrnl 3 1 kr11 0 kr12 1 kr21 --initfunc 2 prll 0 -2
Default dirs(pathes) should be ./image/, ./output/, ./data/, or specify them by
--initdirs: --initdirs <path/to/images> <path/to/output> <path/to/data>
	instead of following flags
--loadCNN: <path/to/saved/CNN.txt> <rand function accuracy ([int])>
--loadNN: <path/to/saved/NN.txt> <path/to/saved/INN.txt> <func: 0 or 1 (SIGMOID, TANH)> <moment of learning> <speed of learning>
--initimg: <path/filename.extension (of input image)>
--loadkrnl: --loadkrnl <numofargs> <bool flag1: create new vkernel element> <filename1 of kernel1 (without ext - .krnl)> <flag2> <filename2> ...
--loadkrnl: <flag> <filename> - is ONE arg
--loadkrnl: mostly first argv should be "1 <name>", since vkernel.size() == 0 (for emplace first vkernel element)
--loadkinf: <path/filename.kinf (of kernel info file)>
.kinf files: syntax same as --loadkrnl init (without first arg - numofargs)
--initfunc: --initfunc <numofargs> <arg1: vfunc> <arg2: vfunc> ...
--initfunc: --initfunc <numofargs> ... "prll" <parallel arg> ...
--initfunc: "prll" <parallel arg> - is ONE arg
		vfunc:
		x >= 0 - convFunc_RGB (x - idx of kernel's layer)
		-2 - decreaseX2_RGB(max)
		-3 - decreaseX2_RGB(min)
		-4 - decreaseX2_RGB(mid)
--loadfunc: <path/filename.finf (of functions info file)>
.finf files: syntax same as --initfunc init (without first arg - numofargs)
--ma: option to learn by
		1: result on one picture
		2: learn CNN on result pack by defined iterations (<file with desire result> <count of iterations> <subString of images> <num: func to get id of each image (x % num)> <extension of images>)
				)" << ::std::endl;
			return 0;
		}
		else
		{
			::std::cout << "unknown option - " << option << ::std::endl;
			ERROR_
			return 1;
		}
	}

	if (!isloadedCNN)
	{
		// as default
		ER_IFN(test.initFuncEx([]()->double { return 0.; }, vArg),, return 1; )
	}
	if (!isloadedNN)
	{
		// as default
		test.setParams(0.2, 0.4);
		ER_IFN(test.init_neuronet(::std::vector<int>{10, 5, 4}, []()->double { return static_cast<double>(((nndx::randB(hProv) % 100) - 50.) / 100.); },
			nndx::neuron::_func::_fnSIGMOID, 0.1, 1),, return 1; )
	}

	ER_IF(mA_var == -1, ::std::cout << "Chose no option to learn by, using default (1)" << ::std::endl;, mA_var = 1; )

	if (mA_var == 1)
	{
		::std::cout << "---Single result---" << ::std::endl;

		ER_IFN(test.mA_Res(), ::std::cout << "test.mA_Res() returns - false" << ::std::endl;, return 1; )
	}
	else if (mA_var == 2)
	{
		::std::cout << "---Learn by iterations---" << ::std::endl;

		::std::string opt;
		::std::string subString;
		::std::string extOfImage;
		unsigned int nIter;
		unsigned int num4Func;
		::std::cout << "Specify file of desire results:" << ::std::endl;
		::std::getline(::std::cin, opt);

		::std::ifstream read(opt);
		ER_IFN(read.is_open(), ::std::cout << "Can not read from " << opt << ::std::endl;, return 1; )

		::std::getline(read, opt);
		int szResults = ::std::atoi(opt.c_str());
		::std::getline(read, opt);
		int szResultLine = ::std::atoi(opt.c_str());
		::std::vector<::std::vector<double>> resNet;
		resNet.reserve(szResults);

		for(size_t i {0}; i < szResults; ++i)
		{
			::std::vector<double> tempV;
			tempV.reserve(szResultLine);
			for(size_t j {0}; j < szResultLine; ++j)
			{
				double tempD;
				read >> tempD;
				tempV.emplace_back(tempD);
			}
			resNet.emplace_back(tempV);
		}
		ER_IFN(resNet.size() == szResults, ::std::cout << "Error: resNet.size() not equal szResults" << ::std::endl;, return 1; )
		::std::cout << "Specify count of iterations:" << ::std::endl;
		::std::cin >> nIter;
		::std::cout << "Specify num for func (x % <num):" << ::std::endl;
		::std::cin >> num4Func;
		::std::getline(::std::cin, subString);
		::std::getline(::std::cin, extOfImage);
		ER_IFN(test.mA_Iter(resNet, nIter, subString, [](unsigned int x, unsigned int y) -> size_t { return x % y; }, num4Func, extOfImage), ::std::cout << "test.mA_Iter() returns - false" << ::std::endl;, return 1; )
	}
	ER_IFN(test.FullSave(), ::std::cout << "test.FullSave() returns - false" << ::std::endl;, return 1; )

//	run --initdirs ./_TEST_/images/ ./_TEST_/output/ ./_TEST_/data/ --initimg ./_TEST_/images/111.jpg --loadkinf ./_TEST_/data/loadkernel.kinf --loadfunc ./_TEST_/data/loadfunc.finf

/*
	ER_IFN(test.defKrnlFromFile(0, 0),, return 1; )
	ER_IFN(test.defKrnlFromFile(1, 0),, return 1; )
	//ER_IFN(test.initKrnlFromFile(2, 0),, return 1; )
	//ER_IFN(test.initKrnl(1, 0, 4, 4, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
	//ER_IFN(test.initKrnl(1, 1, 4, 4, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
	ER_IFN(test.initKrnl(2, 0, 2, 2, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
	ER_IFN(test.initKrnl(2, 1, 2, 2, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
	//ER_IFN(test.initKrnl(3, 0, 3, 3, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
	ER_IFN(test.defKrnlFromFile(3, 0),, return 1; )
	ER_IFN(test.initKrnl(4, 0, 2, 2, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
	ER_IFN(test.initKrnl(4, 1, 2, 2, []()->double { return (nndx::randT(hProv) % 100) / 100.; }),, return 1; )
*/

/*
	{
		using nndx::operator ""_sprll;
		ER_IFN(test.initFuncEx([]()->double { return 0.; }, ::std::vector<ptrdiff_t>{
			1,
			-2,
			3,
			-2,
			3,
			-2,
			3,
			-2,
			3,
			-2	}),, return 1; )
	}
*/

/*
	ER_IFN(test.init_neuronet("output/net.txt", "output/Inet.txt", nndx::neuron::_func::_fnSIGMOID, 0.3, 0.6),, return 1; )
*/
/*
	::std::vector<::std::vector<double>> resNet;
	resNet.reserve(20);
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

		ER_IFN(test.mA_Iter(resNet, temp1, "test", [](unsigned int& x)->size_t { return x % 10; }, ".jpg"),, return 1; )
	}
	else if (temp1 == 1)
	{
		::std::cout << "Instances: ";
		::std::cin >> temp1;
		::std::cout << "value: ";
		::std::cin >> temp2;

		ER_IFN(test.mA_ByValue(resNet, temp1, temp2, "test", [](unsigned int& x)->size_t { return x % 20; }, ".jpg"),, return 1; )
	}
	else if (temp1 == 2)
	{
		::std::cout << "Num: ";
		::std::cin >> temp1;

		cv::Mat img;
		img = cv::imread("images/test" + nndx::nts(temp1) + ".jpg");
		ER_IFN(test.init_image(img),, return 1; )

		ER_IFN(test.mA_Res(),, return 1; )
	}
	else
	{
		ERROR_
	}
*/
	return 0;
}