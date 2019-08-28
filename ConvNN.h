#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <Windows.h>

#include "opencv2\opencv.hpp"

// main flag
#define _NNDX_CONV_NEURONET_DEF

#include "NN.h"

//#define _mainDEBUG
#define _CNN_COMMENTS

constexpr unsigned char _clr_ = static_cast<unsigned char>(0b1111'1111);
constexpr const char* _filend = "endOFfile";

namespace nndx
{
	double max_4(double, double, double, double);
	double min_4(double, double, double, double);
	double mid_4(double, double, double, double);
	double ReLU(double);
	template<typename _T>
	::std::string nts(const _T&);

	class __bool
	{
	private:
		volatile bool flag;
	public:
		explicit __bool() noexcept : flag(true) {}

		__bool& operator=(bool x)
		{
			flag = x;
			if (!flag)	throw ::std::exception("__bool is FALSE");
			return *this;
		}

		operator bool()
		{
			return flag;
		}

		~__bool() {}
	};

	class cneuron
	{
	public:
		double data;
		double mBIAS;
		double err;

		explicit cneuron() : data(0.), mBIAS(0.), err(0.) {}

		cneuron(double x, double bias) : data(x), mBIAS(bias), err(0.) {}

		bool operator==(cneuron& x)
		{
			return (*this == x);
		}

		/*cneuron& operator=(cneuron&& x)
		{
			if (this == &x) ERROR_
			else
			{
				this->data	=	::std::forward<decltype(x.data)>(x.data);
				this->mBIAS =	::std::forward<decltype(x.mBIAS)>(x.mBIAS);
				this->err	=	::std::forward<decltype(x.err)>(x.mBIAS);
				return *this;
			}
		}

		cneuron& operator=(const cneuron& x)
		{
			if (this == &x) ERROR_
			else
			{
				this->data = x.data;
				this->mBIAS = x.mBIAS;
				this->err = x.err;
				return *this;
			}
		}

		cneuron& operator=(cneuron x)
		{
			this->data = x.data;
			this->mBIAS = x.mBIAS;
			this->err = x.err;
			return *this;
		}

		cneuron& operator=(double x)
		{
			data = x;
			return *this;
		}*/

		static double mSIGMOID(double value, double x)
		{
			//return 1. / (1. + exp(-value));
			return value / x;
		}

		static double mSIGMOID_DRV(double value)
		{
			//return value * (1. - value);
			return 1.;
		}

		~cneuron() {}
	};

	class rgb_T
	{
	public:
		cneuron Rn;
		cneuron Gn;
		cneuron Bn;

		explicit rgb_T(double func(void)) : Rn(0., func()), Gn(0., func()), Bn(0., func()) {}

		rgb_T(double r, double g, double b) : Rn(r, 0.), Gn(g, 0.), Bn(b, 0.) {}

		rgb_T(double rbias, double gbias, double bbias, double initx) : Rn(initx, rbias), Gn(initx, gbias), Bn(initx, bbias) {}

		explicit rgb_T(const cv::Vec3b& v)
		{
			Bn.data = v[0] / _clr_;
			Gn.data = v[1] / _clr_;
			Rn.data = v[2] / _clr_;
		}

		void init(double r, double g, double b)
		{
			Rn.data = r;
			Rn.err = 0.;
			Gn.data = g;
			Gn.err = 0.;
			Bn.data = b;
			Bn.err = 0.;
		}

		double Grayn() noexcept
		{
			return 0.299 * Rn.data + 0.587 * Gn.data + 0.114 * Bn.data;
		}
	};

	class cnnKernel_c3
	{
	public:
		wWw R;
		wWw G;
		wWw B;

		explicit cnnKernel_c3() : R(0.), G(0.), B(0.) {}

		explicit cnnKernel_c3(double r, double g, double b) : R(r), G(g), B(b) {}

		~cnnKernel_c3() {}
	};

	typedef ::std::vector<::std::vector<rgb_T>> image;
	typedef ::std::vector<::std::vector<cnnKernel_c3>> mapge;

	class CNN
	{
	private:
		bool isReady;
		double u_net;
		double moment_net;

		::std::string inputF;
		::std::string outputF;
		::std::string dataF;

		image vinLayer;									// input image
		::std::vector<::std::vector<image>> vlayer;		// vector of layer's data
		::std::vector<::std::vector<mapge>> vkernel;	// vector of kernel's data
		::std::vector<int> vfunc;						// vector of functions(conv, decr)
		nndx::neuronet net;								// main forward neural network
		nndx::neuronet Inet;							// neuronet between CNN and NN neurons (3n -> 1n)

	public:
		explicit CNN() : isReady(false), u_net(0.), moment_net(0.) {}
		
		explicit CNN(const cv::Mat& img) : isReady(false), u_net(0.), moment_net(0.)
		{
			ER_IF((img.rows < 2) || (img.cols < 2), )
			else
			{
				vinLayer.reserve(img.rows);
				for (int y = 0; y < img.rows; ++y)
				{
					vinLayer.emplace_back(::std::vector<rgb_T>());
					vinLayer[y].reserve(img.cols);
					for (int x = 0; x < img.cols; ++x)
					{
						vinLayer[y].emplace_back(img.at<cv::Vec3b>(cv::Point(x, y)));
					}
				}
			}
		}

		CNN(const char* filename, double rand_func(void)) : isReady(false), u_net(0.), moment_net(0.)
		{
			::std::ifstream read(filename);
			ER_IF(!read.is_open(), )
			else
			{
				::std::string s1;
				int temp1, temp2;
				__bool res;

				read >> s1;
				read >> temp1;
				read >> temp2;
				res = init_image(temp1, temp2, 0., 0., 0.);
				read >> s1;
				::std::vector<int> vfuncIn;
				read >> temp1;
				vfuncIn.reserve(temp1);
				for (int i = 0; i < temp1; ++i)
				{
					read >> temp2;
					vfuncIn.emplace_back(temp2);
				}
				read >> s1;
				read >> inputF;
				read >> s1;
				read >> outputF;
				read >> s1;
				read >> dataF;
				read >> s1;
				read >> temp2;
				for(size_t i = 0; i < temp2; ++i)
				{
					read >> temp1;
					for (size_t j = 0; j < temp1; ++j)
					{
						size_t check1, check2;
						read >> s1;
						read >> check1;
						read >> check2;
						if (s1 == "rand")
						{
							res = initKrnl(i, j, check1, check2, rand_func);
						}
						else if (s1 == "save")
						{
							res = initKrnlFromFile(i, j);
							ER_IF(vkernel[i][j].size() != check1, )
							ER_IF(vkernel[i][j].back().size() != check2, )
						}
						else
						{
							ERROR_
						}
					}
				}
				read >> s1;
				read >> u_net;
				read >> s1;
				read >> moment_net;
				read >> s1;
				if (s1 == "BIASES")
				{
					double d1 = 0.;
					::std::vector<double> vd;
					read >> temp1;
					for (size_t idx = 0; idx < temp1; ++idx)
					{
						read >> temp2;
						for (size_t k = 0; k < temp2; ++k)
						{
							size_t ytemp;
							read >> ytemp;
							for (size_t y = 0; y < ytemp; ++y)
							{
								size_t xtemp;
								read >> xtemp;
								for (size_t x = 0; x < xtemp; ++x)
								{
									read >> d1;
									vd.reserve(vd.capacity() + 1);
									vd.emplace_back(d1);
								}
							}
						}
					}
					res = initFuncEx(vd, vfuncIn);
				}
				else
				{
					res = initFuncEx([]()->double { return 0.; }, vfuncIn);
				}
				
				read.close();
			}
		}

		CNN(CNN&& data)
		{
			this->isReady			=		::std::forward<decltype(data.isReady)>(data.isReady);
			this->u_net				=		::std::forward<decltype(data.u_net)>(data.u_net);
			this->moment_net		=		::std::forward<decltype(data.moment_net)>(data.moment_net);
			this->vinLayer			=		::std::forward<decltype(data.vinLayer)>(data.vinLayer);
			this->vkernel			=		::std::forward<decltype(data.vkernel)>(data.vkernel);
			this->vfunc				=		::std::forward<decltype(data.vfunc)>(data.vfunc);
			this->vlayer			=		::std::forward<decltype(data.vlayer)>(data.vlayer);
			this->inputF			=		::std::forward<decltype(data.inputF)>(data.inputF);
			this->outputF			=		::std::forward<decltype(data.outputF)>(data.outputF);
			this->dataF				=		::std::forward<decltype(data.dataF)>(data.dataF);
			this->net				=		::std::forward<decltype(data.net)>(data.net);
#ifdef _mainDEBUG
			::std::cout << "[CNN] call &&constructor!" << ::std::endl;
#endif
		}

		~CNN()
		{
			__bool res;
			res = FullSave();
		}
		
		bool initDir(const ::std::string& in, const ::std::string& out, const ::std::string& data)
		{
			ER_IF(in.empty() || out.empty() || data.empty(), return false; )
			inputF = in;
			if (inputF[inputF.length() - 1] != '/')
			{
				inputF += '/';
			}
			outputF = out;
			if (outputF[outputF.length() - 1] != '/')
			{
				outputF += '/';
			}
			dataF = data;
			if (dataF[dataF.length() - 1] != '/')
			{
				dataF += '/';
			}
			return true;
		}

		//count of rows and cols of image must be greater than 1, type - CV_8UC3
		bool init_image(const cv::Mat& img)
		{
			
			ER_IF((img.rows < 2) || (img.cols < 2), return false; )
			if (!vinLayer.empty())
			{
				if ((img.rows != vinLayer.size()) || (img.cols != vinLayer.back().size()))
					isReady = false;
				vinLayer.clear();
			}

			vinLayer.reserve(img.rows);
			for (int y = 0; y < img.rows; ++y)
			{
				vinLayer.emplace_back(::std::vector<rgb_T>());
				vinLayer[y].reserve(img.cols);
				for (int x = 0; x < img.cols; ++x)
				{
					vinLayer[y].emplace_back(img.at<cv::Vec3b>(cv::Point(x, y)));
				}
			}
			return true;
		}

		//for filled images (R, G, B: 0 - 1)
		bool init_image(size_t X, size_t Y, double R, double G, double B)
		{
			ER_IF((X < 2) || (Y < 2), return false; )
			if (!vinLayer.empty())
			{
				if ((Y < vinLayer.size()) || (X < vinLayer.back().size()))
					isReady = false;
				vinLayer.clear();
			}

			vinLayer.reserve(Y);
			for (size_t y = 0; y < Y; ++y)
			{
				vinLayer.emplace_back(::std::vector<rgb_T>());
				vinLayer[y].reserve(X);
				for (size_t x = 0; x < X; ++x)
				{
					vinLayer[y].emplace_back(R, G, B);
				}
			}
			return true;
		}

		bool initKrnl(size_t idx_, size_t idxKernel, size_t sizex, size_t sizey, ...)
		{
			ER_IF(idx_ > vkernel.size(), return false; )
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<mapge>());
			}

			ER_IF(idxKernel > vkernel[idx_].size(), return false; )
			else if (idxKernel < vkernel[idx_].size())
			{
				vkernel[idx_][idxKernel].clear();
			}
			else
			{
				vkernel[idx_].reserve(vkernel[idx_].capacity() + 1);
				vkernel[idx_].emplace_back(mapge());
			}

			va_list args;
			va_start(args, sizey);

			vkernel[idx_][idxKernel].reserve(sizey);
			for (size_t i = 0; i < sizey; ++i)
			{
				vkernel[idx_][idxKernel].emplace_back(::std::vector<cnnKernel_c3>());
				vkernel[idx_][idxKernel].back().reserve(sizex);
				for (size_t j = 0; j < sizex; ++j)
				{
					double temp1 = va_arg(args, double);
					double temp2 = va_arg(args, double);
					double temp3 = va_arg(args, double);
					vkernel[idx_][idxKernel].back().emplace_back(cnnKernel_c3(temp1, temp2, temp3));
				}
			}
			va_end(args);

			return true;
		}

		bool initKrnl(size_t idx_, size_t idxKernel, size_t sizex, size_t sizey, double rand_func(void))
		{
			ER_IF(idx_ > vkernel.size(), return false; )
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<mapge>());
			}

			ER_IF(idxKernel > vkernel[idx_].size(), return false; )
			else if (idxKernel < vkernel[idx_].size())
			{
				vkernel[idx_][idxKernel].clear();
			}
			else
			{
				vkernel[idx_].reserve(vkernel[idx_].capacity() + 1);
				vkernel[idx_].emplace_back(mapge());
			}

			vkernel[idx_][idxKernel].reserve(sizey);
			for (size_t i = 0; i < sizey; ++i)
			{
				vkernel[idx_][idxKernel].emplace_back(::std::vector<cnnKernel_c3>());
				vkernel[idx_][idxKernel].back().reserve(sizex);
				for (size_t j = 0; j < sizex; ++j)
				{
					vkernel[idx_][idxKernel].back().emplace_back(cnnKernel_c3(rand_func(), rand_func(), rand_func()));
				}
			}

			return true;
		}

		//files must be with .krnl extension
		bool initKrnlFromFile(size_t idx_, size_t idxKernel)
		{
			ER_IF(idx_ > vkernel.size(), return false; )
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<mapge>());
			}

			ER_IF(idxKernel > vkernel[idx_].size(), return false; )
			else if (idxKernel < vkernel[idx_].size())
			{
				vkernel[idx_][idxKernel].clear();
			}
			else
			{
				vkernel[idx_].reserve(vkernel[idx_].capacity() + 1);
				vkernel[idx_].emplace_back(mapge());
			}

			ER_IF(dataF.empty(), return false; )

			::std::string sfile = dataF + nts(idx_);
			sfile += '_';
			sfile += nts(idxKernel);
			::std::ifstream read(sfile + ".krnl");
			ER_IF(!read.is_open(), read.close(); return false; )

			size_t sizey, sizex;
			double kernelTempR = 0.;
			double kernelTempG = 0.;
			double kernelTempB = 0.;
			read >> sizex;
			read >> sizey;

			vkernel[idx_][idxKernel].reserve(sizey);
			for (size_t i = 0; i < sizey; ++i)
			{
				vkernel[idx_][idxKernel].emplace_back(::std::vector<cnnKernel_c3>());
				vkernel[idx_][idxKernel].back().reserve(sizex);
				for (size_t j = 0; j < sizex; ++j)
				{
					kernelTempR = 0.;
					kernelTempG = 0.;
					kernelTempB = 0.;
					read >> kernelTempR;
					read >> kernelTempG;
					read >> kernelTempB;
					vkernel[idx_][idxKernel].back().emplace_back(kernelTempR * 0.1, kernelTempG * 0.1, kernelTempB * 0.1);
				}
			}
			read >> sfile;
			ER_IF(sfile != _filend, read.close(); return false; )

			read.close();
			return true;
		}

		// first arg - size of vlayer and vfunc
		// vfunc:
		// x - convFunc_RGB (x - idx of kernel's layer)
		// -2 - decreaseX2_RGB(max)
		// -3 - decreaseX2_RGB(min)
		// -4 - decreaseX2_RGB(mid)
		// !!! Init			all need kernels		before !!!
		// !!! Init			input image				before !!!
		bool initFuncEx(const ::std::vector<double>& biases, const ::std::vector<int>& args)
		{
			ER_IF(vinLayer.empty(), return false; )
			ER_IF(vkernel.empty(), return false; )

			isReady = false;
			if (!vlayer.empty())
			{
				vlayer.clear();
			}
			if (!vfunc.empty())
			{
				vfunc.clear();
			}

			ER_IFN(args.size(), return false; )
			else
			{
				vlayer.reserve(args.size());
				vfunc.reserve(args.size());
				
				int functemp_ = args[0];

				// decl image()
				// for layer forward inputLayer
				if (functemp_ < 0)
				{
					vlayer.emplace_back(::std::vector<image>());
					vlayer.back().reserve(1);
					vlayer.back().emplace_back(image());
					vfunc.emplace_back(functemp_);
				}
				else
				{
					ER_IF(functemp_ >= vkernel.size(), return false; )

					vlayer.emplace_back(::std::vector<image>());
					size_t temp_ = vkernel[functemp_].size();
					vlayer.back().reserve(temp_);
					for (size_t n = 0; n < temp_; n++)
					{
						vlayer.back().emplace_back(image());
					}
					vfunc.emplace_back(functemp_);
				}

				// for other layers // WHAT IS I AND IDARGS???
				for (size_t i = 1; i < args.size(); ++i)
				{
					functemp_ = args[i];
					if (functemp_ < 0)
					{
						vlayer.emplace_back(::std::vector<image>());
						size_t temp_ = vlayer[i - 1].size();
						vlayer.back().reserve(temp_);
						for (size_t n = 0; n < temp_; n++)
						{
							vlayer.back().emplace_back(image());
						}
						vfunc.emplace_back(functemp_);
					}
					else
					{
						ER_IF(functemp_ >= vkernel.size(), return false; )

						vlayer.emplace_back(::std::vector<image>());
						size_t temp_ = vkernel[functemp_].size() * vlayer[i - 1].size();
						vlayer.back().reserve(temp_);
						for (size_t n = 0; n < temp_; n++)
						{
							vlayer.back().emplace_back(image());
						}
						vfunc.emplace_back(functemp_);
					}
				}

				ER_IFN(specInit(biases), return false; )
			}

			isReady = true;
			return isReady;
		}

		bool initFuncEx(double rand(), ::std::vector<int> args)
		{
			ER_IF(vinLayer.empty(), return false; )
			ER_IF(vkernel.empty(), return false; )

			isReady = false;
			if (!vlayer.empty())
			{
				vlayer.clear();
			}
			if (!vfunc.empty())
			{
				vfunc.clear();
			}

			ER_IFN(args.size(), return false; )
			else
			{
				vlayer.reserve(args.size());
				vfunc.reserve(args.size());

				int functemp_ = args[0];

				// decl image()
				// for layer forward inputLayer
				if (functemp_ < 0)
				{
					vlayer.emplace_back(::std::vector<image>());
					vlayer.back().reserve(1);
					vlayer.back().emplace_back(image());
					vfunc.emplace_back(functemp_);
				}
				else
				{
					ER_IF(functemp_ >= vkernel.size(), return false; )

					vlayer.emplace_back(::std::vector<image>());
					size_t temp_ = vkernel[functemp_].size();
					vlayer.back().reserve(temp_);
					for (size_t n = 0; n < temp_; n++)
					{
						vlayer.back().emplace_back(image());
					}
					vfunc.emplace_back(functemp_);
				}

				// for other layers
				for (size_t i = 1; i < args.size(); ++i)
				{
					functemp_ = args[i];
					if (functemp_ < 0)
					{
						vlayer.emplace_back(::std::vector<image>());
						size_t temp_ = vlayer[i - 1].size();
						vlayer.back().reserve(temp_);
						for (size_t n = 0; n < temp_; n++)
						{
							vlayer.back().emplace_back(image());
						}
						vfunc.emplace_back(functemp_);
					}
					else
					{
						ER_IF(functemp_ >= vkernel.size(), return false; )

						vlayer.emplace_back(::std::vector<image>());
						size_t temp_ = vkernel[functemp_].size() * vlayer[i - 1].size();
						vlayer.back().reserve(temp_);
						for (size_t n = 0; n < temp_; n++)
						{
							vlayer.back().emplace_back(image());
						}
						vfunc.emplace_back(functemp_);
					}
				}

				ER_IFN(specInit(rand), return false; )
			}

			isReady = true;
			return isReady;
		}

		void setParams(double moment, double u)
		{
			moment_net = moment;
			u_net = u;
		}

		//initFuncEx must be called earlier
		//mA() must be called earlier
		bool init_neuronet(::std::vector<int>&& tplNet, double funcWeights(), nndx::neuron::_func funcNet, double moment, double u)
		{
			ER_IF(!isReady, return false; )
			if (net.getState())
			{
				net.~neuronet();
				ER_IF(net.getState(), return false; )
			}

			__bool res;
			int num = 0;

			for (auto& x : vlayer.back())
			{
				num += static_cast<int>(x.size() * x[0].size());
			}
			//num *= 3; // CV_8UC3!
			tplNet.emplace(tplNet.begin(), num);

			res = net.setGenWeightsFunc(funcWeights);
			res = net.init(::std::forward<::std::vector<int>>(tplNet), funcNet);
			res = net.setParams(moment, u);

			// Inet init
			if (Inet.getState())
			{
				Inet.~neuronet();
				ER_IF(Inet.getState(), return false; )
			}
			else
			{
				res = Inet.setGenWeightsFunc(funcWeights);
				res = Inet.init(::std::vector<int>{3, 2, 1}, funcNet);
				res = Inet.setParams(moment, u);
			}

			return (net.getState() && Inet.getState());
		}

		bool init_neuronet(nndx::neuronet&& x, nndx::neuronet&& Ix)
		{
			net = ::std::forward<decltype(x)>(x);
			Inet = ::std::forward<decltype(Ix)>(Ix);

			return (net.getState() && Inet.getState());
		}

		bool init_neuronet(::std::string file, ::std::string Ifile, nndx::neuron::_func funcNet, double moment, double u)
		{
			ER_IF(!isReady, return false; )
			if (net.getState())
			{
				net.~neuronet();
				ER_IF(net.getState(), return false; )
			}

			__bool res;
			net.nDataNet = file;
			res = net.initFromFile();
			res = net.setFunc(funcNet);
			res = net.setParams(moment, u);

			// Inet init
			if (Inet.getState())
			{
				Inet.~neuronet();
				ER_IF(Inet.getState(), return false; )
			}
			else
			{
				Inet.nDataNet = Ifile;
				res = Inet.initFromFile();
				res = Inet.setFunc(funcNet);
				res = Inet.setParams(moment, u);
			}

			return (net.getState() && Inet.getState());
		}

		bool SaveCNN()
		{
			ER_IFN(isReady, return false; )
			ER_IF(outputF.empty(), return false; )

			::std::ofstream write(outputF + "savedCNN.txt");
			ER_IF(!write.is_open(), write.close();  return false; )

			write << "in " << vinLayer.size() << " " << vinLayer.back().size() << ::std::endl;
			write << "vfunc " << vfunc.size() << " ";
			for (auto& x : vfunc)
			{
				write << x << " ";
			}
			write << ::std::endl;
			write << "inF " << inputF << ::std::endl;
			write << "outF " << outputF << ::std::endl;
			write << "datF " << dataF << ::std::endl;
			write << "krnl_size: " << vkernel.size() << ::std::endl;
			for (size_t id = 0; id < vkernel.size(); ++id)
			{
				write << "	" << vkernel[id].size() << ::std::endl;
				for (size_t j = 0; j < vkernel[id].size(); ++j)
				{
					write << "save " << vkernel[id][j].size() << " " << vkernel[id][j].back().size() << ::std::endl;
				}
			}
			write << ::std::endl;
			write << "un " << u_net << ::std::endl;
			write << "mn " << moment_net << ::std::endl;
			write << "BIASES" << ::std::endl;
			write << vlayer.size() << ::std::endl;
			for (size_t idx = 0; idx < vlayer.size(); ++idx)
			{
				write << vlayer[idx].size() << ::std::endl;
				for (size_t k = 0; k < vlayer[idx].size(); ++k)
				{
					write << vlayer[idx][k].size() << ::std::endl;
					for (size_t y = 0; y < vlayer[idx][k].size(); ++y)
					{
						write << vlayer[idx][k].back().size() << ::std::endl;
						for (size_t x = 0; x < vlayer[idx][k].back().size(); ++x)
						{
							write << vlayer[idx][k][y][x].Rn.mBIAS << " " << vlayer[idx][k][y][x].Gn.mBIAS << " " << vlayer[idx][k][y][x].Bn.mBIAS << " ";
						}
						write << ::std::endl;
					}
				}
			}

			write.close();
			return true;
		}

		bool SaveKrnl()
		{
			ER_IF(vkernel.empty(), return false; )

			for (size_t id = 0; id < vkernel.size(); ++id)
			{
				for (size_t j = 0; j < vkernel[id].size(); ++j)
				{
					::std::string sstemp = "krnl/";
					sstemp += nts(id) + "_";
					sstemp += nts(j);
					sstemp += ".krnl";
					::std::ofstream write(outputF + sstemp);
					ER_IF(!write.is_open(), write.close(); return false; )

					write << vkernel[id][j].size() << " " << vkernel[id][j].back().size() << ::std::endl;
					for (size_t y = 0; y < vkernel[id][j].size(); ++y)
					{
						for (size_t x = 0; x < vkernel[id][j].back().size(); ++x)
						{
							write << vkernel[id][j][y][x].R.wg << " " << vkernel[id][j][y][x].G.wg << " " << vkernel[id][j][y][x].B.wg << ::std::endl;
						}
					}
					write << "endOFfile";

					write.close();
				}
			}

			return true;
		}

		inline bool FullSave()
		{
			__bool temp;
			temp = net.saveF(outputF + "net.txt");
			temp = Inet.saveF(outputF + "Inet.txt");
			temp = SaveCNN();
			temp = SaveKrnl();

			return temp;
		}

		bool mA_Iter(const ::std::vector<::std::vector<double>>& results, unsigned int iter, size_t func(unsigned int&), ::std::string subS, ::std::string extImg)
		{
			ER_IF(&results == nullptr, return false; )
			ER_IF(iter == 0, return false; )
			ER_IF(inputF.empty() || outputF.empty() || dataF.empty(), return false; )
			ER_IF(!net.getState(), return false; )
			ER_IF((u_net == 0.) || (moment_net == 0.), return false; )
			ER_IF(!isReady, return false; )

			::std::string sfile = inputF + subS;
			::std::vector<double> in;
			__bool resT;

			for (unsigned int i = 0; i < iter; ++i)
			{
				in.clear();
				resT = init_image(cv::imread(sfile + nndx::nts(func(i)) + extImg));

				resT = mA();

				//in.reserve(net.data[0].size() - 1); // <----------   important

				resT = fillInputCNN(in);

				resT = net.fillInput(in);
				resT = net.callActivationF();

#ifdef _CNN_COMMENTS
				::std::cout << " func(i) - " << func(i) << "\n";
				for (auto& x : net.getResults())
				{
					::std::cout << x << "\n";
				}
				::std::cout << "\n";
#endif

				resT = ConvBackProp(results[func(i)]);
			}
			::std::cout << ::std::endl;

			//actions after main work
			resT = FullSave();

			return true;
		}

		bool mA_ByValue(const ::std::vector<::std::vector<double>>& results, unsigned int Xnum, double CounterValue, size_t func(unsigned int&), ::std::string subS, ::std::string extImg)
		{
			ER_IF(&results == nullptr, return false; )
			else
			{
				ER_IF(results.size() != Xnum, return false; )
			}
			ER_IF(inputF.empty() || outputF.empty() || dataF.empty(), return false; )
			ER_IF(!net.getState(), return false; )
			ER_IF((u_net == 0.) || (moment_net == 0.), return false; )
			ER_IF(!isReady, return false; )

			::std::string sfile = inputF + subS;
			::std::vector<double> in;
			__bool resT;

			unsigned int i = 0;
			unsigned int numL = 0;
			while(true)
			{
				in.clear();
				resT = init_image(cv::imread(sfile + nndx::nts(func(i)) + extImg));

				resT = mA();

				//in.reserve(net.data[0].size() - 1); // <----------   important

				resT = fillInputCNN(in);

				resT = net.fillInput(in);
				resT = net.callActivationF();

#ifdef _CNN_COMMENTS
				::std::cout << " func(i) - " << func(i) << "\n";
#endif
				int tempid_ = 0;
				bool flagRes = true;
				for (auto& x : net.getResults())
				{
					if (abs(results[func(i)][tempid_++] - x) > CounterValue)
					{
						flagRes = false;
#ifndef _CNN_COMMENTS
						break;
#endif
					}
#ifdef _CNN_COMMENTS
					::std::cout << x << "\n";
#endif
				}
				if (flagRes) ++numL;
				else numL = 0;
#ifdef _CNN_COMMENTS
				::std::cout << "\n";
#endif

				resT = ConvBackProp(results[func(i)]);

				if (numL >= Xnum)
				{
					::std::cout << "Learned!" << ::std::endl;
					break;
				}

				++i;
			}
			::std::cout << ::std::endl;

			//actions after main work
			resT = FullSave();

			return true;
		}

		bool mA_Res()
		{
			ER_IF(!isReady, return false; )

			::std::vector<double> in;
			__bool resT;
			resT = mA();

			//in.reserve(net.data[0].size() - 1); // <----------   important

			resT = fillInputCNN(in);

			resT = net.fillInput(in);
			resT = net.callActivationF();

			for (auto& x : net.getResults())
			{
				::std::cout << x << "\n";
			}
			::std::cout << "\n";

			return true;
		}

	private:
		inline bool fillInputCNN(::std::vector<double>& input)
		{
			for (size_t s = 0; s < vlayer.back().size(); ++s)
			{
				for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
				{
					for (size_t j = 0; j < vlayer.back()[s].back().size(); ++j)
					{
						Inet.fillInput(::std::vector<double>{vlayer.back()[s][i][j].Rn.data, vlayer.back()[s][i][j].Gn.data, vlayer.back()[s][i][j].Bn.data});
						Inet.activationF();
						double resInet = Inet.getResults()[0];
						input.reserve(input.capacity() + 1);
						input.emplace_back(resInet);
					}
				}
			}

			return true;
		}

		bool specInit(double randForBias())
		{
			// init to zero-s
			// for layer forward inputLayer
			if (vfunc[0] < 0)
			{
#ifdef _DEBUG
				ER_IF(vlayer[0].size() != 1, return false; ) // there are only vlayer[0][0] for func < 0(-2, -3, -4...)
#endif
				size_t rows = (vinLayer.size() % 2 == 0) ? static_cast<size_t>(vinLayer.size() * 0.5) : static_cast<size_t>((vinLayer.size() - 1) * 0.5);
				size_t cols = (vinLayer[0].size() % 2 == 0) ? static_cast<size_t>(vinLayer[0].size() * 0.5) : static_cast<size_t>((vinLayer[0].size() - 1) * 0.5);

				vlayer[0].back().reserve(rows);
				for (size_t y = 0; y < rows; ++y)
				{
					vlayer[0].back().emplace_back(::std::vector<rgb_T>());
					vlayer[0].back().back().reserve(cols);
					for (size_t x = 0; x < cols; ++x)
					{
						vlayer[0].back().back().emplace_back(rgb_T(randForBias));
					}
				}
			}
			else
			{
				size_t szKrnl = vkernel[vfunc[0]].size();
#ifdef _DEBUG
				ER_IF(vlayer[0].size() != szKrnl, return false; )
#endif
				for (size_t i = 0; i < szKrnl; ++i)
				{
					ER_IF((vinLayer.size() < vkernel[vfunc[0]][i].size()) || (vinLayer.back().size() < vkernel[vfunc[0]][i].back().size()), return false; )

					size_t localy = vinLayer.size() - vkernel[vfunc[0]][i].size() + 1;
					size_t localx = vinLayer.back().size() - vkernel[vfunc[0]][i].back().size() + 1;
					vlayer[0][i].reserve(localy);
					for (size_t y = 0; y < localy; ++y)
					{
						vlayer[0][i].emplace_back(::std::vector<rgb_T>());
						vlayer[0][i].back().reserve(localx);
						for (size_t x = 0; x < localx; ++x)
						{
							vlayer[0][i].back().emplace_back(rgb_T(randForBias));
						}
					}
				}
			}

			// for other layers
			for (size_t i = 1; i < vlayer.size(); ++i)
			{
				if (vfunc[i] < 0)
				{
#ifdef _DEBUG
					ER_IF(vlayer[i].size() != vlayer[i - 1].size(), return false; )
#endif
					for (size_t prev = 0; prev < vlayer[i - 1].size(); ++prev)
					{
						ER_IF((vlayer[i - 1][prev].size() < 2) || (vlayer[i - 1][prev].back().size() < 2), return false; )

						size_t rows = (vlayer[i - 1][prev].size() % 2 == 0) ? static_cast<size_t>(vlayer[i - 1][prev].size() * 0.5) : static_cast<size_t>((vlayer[i - 1][prev].size() - 1) * 0.5);
						size_t cols = (vlayer[i - 1][prev].back().size() % 2 == 0) ? static_cast<size_t>(vlayer[i - 1][prev].back().size() * 0.5) : static_cast<size_t>((vlayer[i - 1][prev].back().size() - 1) * 0.5);

						vlayer[i][prev].reserve(rows);
						for (size_t y = 0; y < rows; ++y)
						{
							vlayer[i][prev].emplace_back(::std::vector<rgb_T>());
							vlayer[i][prev].back().reserve(cols);
							for (size_t x = 0; x < cols; ++x)
							{
								vlayer[i][prev].back().emplace_back(rgb_T(randForBias));
							}
						}
					}
				}
				else
				{
					size_t szKrnl = vkernel[vfunc[i]].size();
#ifdef _DEBUG
					ER_IF(vlayer[i].size() != szKrnl * vlayer[i - 1].size(), return false; )
#endif
					for (size_t prev = 0; prev < vlayer[i - 1].size(); ++prev)
					{
						for (size_t k = 0; k < szKrnl; ++k)
						{
							ER_IF((vlayer[i - 1][prev].size() < vkernel[vfunc[i]][k].size()) || (vlayer[i - 1][prev].back().size() < vkernel[vfunc[0]][k].back().size()), return false; )

							size_t localy = vlayer[i - 1][prev].size() - vkernel[vfunc[i]][k].size() + 1;
							size_t localx = vlayer[i - 1][prev].back().size() - vkernel[vfunc[i]][k].back().size() + 1;
							vlayer[i][szKrnl * prev + k].reserve(localy);
							for (size_t y = 0; y < localy; ++y)
							{
								vlayer[i][szKrnl * prev + k].emplace_back(::std::vector<rgb_T>());
								vlayer[i][szKrnl * prev + k].back().reserve(localx);
								for (size_t x = 0; x < localx; ++x)
								{
									vlayer[i][szKrnl * prev + k].back().emplace_back(rgb_T(randForBias));
								}
							}
						}
					}
				}
			}

			return true;
		}

		bool specInit(const ::std::vector<double>& bias)
		{
			size_t idxBias = 0;
			// init to zero-s
			// for layer forward inputLayer
			if (vfunc[0] < 0)
			{
#ifdef _DEBUG
				ER_IF(vlayer[0].size() != 1, return false; ) // there are only vlayer[0][0] for func < 0(-2, -3, -4...)
#endif
				size_t rows = (vinLayer.size() % 2 == 0) ? static_cast<size_t>(vinLayer.size() * 0.5) : static_cast<size_t>((vinLayer.size() - 1) * 0.5);
				size_t cols = (vinLayer[0].size() % 2 == 0) ? static_cast<size_t>(vinLayer[0].size() * 0.5) : static_cast<size_t>((vinLayer[0].size() - 1) * 0.5);

				vlayer[0].back().reserve(rows);
				for (size_t y = 0; y < rows; ++y)
				{
					vlayer[0].back().emplace_back(::std::vector<rgb_T>());
					vlayer[0].back().back().reserve(cols);
					for (size_t x = 0; x < cols; ++x)
					{
						vlayer[0].back().back().emplace_back(rgb_T(bias[idxBias++], bias[idxBias++], bias[idxBias++], 0.));
					}
				}
			}
			else
			{
				size_t szKrnl = vkernel[vfunc[0]].size();
#ifdef _DEBUG
				ER_IF(vlayer[0].size() != szKrnl, return false; )
#endif
				for (size_t i = 0; i < szKrnl; ++i)
				{
					ER_IF((vinLayer.size() < vkernel[vfunc[0]][i].size()) || (vinLayer.back().size() < vkernel[vfunc[0]][i].back().size()), return false; )

						size_t localy = vinLayer.size() - vkernel[vfunc[0]][i].size() + 1;
					size_t localx = vinLayer.back().size() - vkernel[vfunc[0]][i].back().size() + 1;
					vlayer[0][i].reserve(localy);
					for (size_t y = 0; y < localy; ++y)
					{
						vlayer[0][i].emplace_back(::std::vector<rgb_T>());
						vlayer[0][i].back().reserve(localx);
						for (size_t x = 0; x < localx; ++x)
						{
							vlayer[0][i].back().emplace_back(rgb_T(bias[idxBias++], bias[idxBias++], bias[idxBias++], 0.));
						}
					}
				}
		}

			// for other layers
			for (size_t i = 1; i < vlayer.size(); ++i)
			{
				if (vfunc[i] < 0)
				{
#ifdef _DEBUG
					ER_IF(vlayer[i].size() != vlayer[i - 1].size(), return false; )
#endif
					for (size_t prev = 0; prev < vlayer[i - 1].size(); ++prev)
					{
						ER_IF((vlayer[i - 1][prev].size() < 2) || (vlayer[i - 1][prev].back().size() < 2), return false; )

						size_t rows = (vlayer[i - 1][prev].size() % 2 == 0) ? static_cast<size_t>(vlayer[i - 1][prev].size() * 0.5) : static_cast<size_t>((vlayer[i - 1][prev].size() - 1) * 0.5);
						size_t cols = (vlayer[i - 1][prev].back().size() % 2 == 0) ? static_cast<size_t>(vlayer[i - 1][prev].back().size() * 0.5) : static_cast<size_t>((vlayer[i - 1][prev].back().size() - 1) * 0.5);

						vlayer[i][prev].reserve(rows);
						for (size_t y = 0; y < rows; ++y)
						{
							vlayer[i][prev].emplace_back(::std::vector<rgb_T>());
							vlayer[i][prev].back().reserve(cols);
							for (size_t x = 0; x < cols; ++x)
							{
								vlayer[i][prev].back().emplace_back(rgb_T(bias[idxBias++], bias[idxBias++], bias[idxBias++], 0.));
							}
						}
					}
				}
				else
				{
					size_t szKrnl = vkernel[vfunc[i]].size();
#ifdef _DEBUG
					ER_IF(vlayer[i].size() != szKrnl * vlayer[i - 1].size(), return false; )
#endif
					for (size_t prev = 0; prev < vlayer[i - 1].size(); ++prev)
					{
						for (size_t k = 0; k < szKrnl; ++k)
						{
							ER_IF((vlayer[i - 1][prev].size() < vkernel[vfunc[i]][k].size()) || (vlayer[i - 1][prev].back().size() < vkernel[vfunc[0]][k].back().size()), return false; )

							size_t localy = vlayer[i - 1][prev].size() - vkernel[vfunc[i]][k].size() + 1;
							size_t localx = vlayer[i - 1][prev].back().size() - vkernel[vfunc[i]][k].back().size() + 1;
							vlayer[i][szKrnl * prev + k].reserve(localy);
							for (size_t y = 0; y < localy; ++y)
							{
								vlayer[i][szKrnl * prev + k].emplace_back(::std::vector<rgb_T>());
								vlayer[i][szKrnl * prev + k].back().reserve(localx);
								for (size_t x = 0; x < localx; ++x)
								{
									vlayer[i][szKrnl * prev + k].back().emplace_back(rgb_T(bias[idxBias++], bias[idxBias++], bias[idxBias++], 0.));
								}
							}
						}
					}
				}
			}

			return true;
		}

		bool mA()
		{
			__bool res;
			ptrdiff_t current = -1;
			for (auto& x : vfunc)
			{
				if (x < 0)
				{
					switch (x)
					{
					case -2:
					{
						res = decreaseX2_RGB(current, max_4);
						break;
					}
					case -3:
					{
						res = decreaseX2_RGB(current, min_4);
						break;
					}
					case -4:
					{
						res = decreaseX2_RGB(current, mid_4);
						break;
					}
					default:
					{
						ERROR_
							return false;
					}
					}
				}
				else
				{
					res = convFunc_RGB(current, x);
				}
				++current;
				res = saveIm_RGB(static_cast<size_t>(current));
				res = saveIm_Gray(static_cast<size_t>(current));
				//res = saveDat_RGB(static_cast<size_t>(current));
				//res = saveDat_Gray(static_cast<size_t>(current));
			}
			return true;
		}

		//idx of this func - -2, -3, -4
		bool decreaseX2_RGB(ptrdiff_t idxSource, double pFunc(double, double, double, double))
		{
			if (idxSource == -1)
			{
				for (size_t y = 0; y < vlayer[0].back().size(); ++y)
				{
					for (size_t x = 0; x < vlayer[0].back().back().size(); ++x)
					{
						vlayer[0].back()[y][x].init(
							ReLU(pFunc(vinLayer[y * 2][x * 2].Rn.data, vinLayer[y * 2 + 1][x * 2].Rn.data, vinLayer[y * 2][x * 2 + 1].Rn.data, vinLayer[y * 2 + 1][x * 2 + 1].Rn.data)),
							ReLU(pFunc(vinLayer[y * 2][x * 2].Gn.data, vinLayer[y * 2 + 1][x * 2].Gn.data, vinLayer[y * 2][x * 2 + 1].Gn.data, vinLayer[y * 2 + 1][x * 2 + 1].Gn.data)),
							ReLU(pFunc(vinLayer[y * 2][x * 2].Bn.data, vinLayer[y * 2 + 1][x * 2].Bn.data, vinLayer[y * 2][x * 2 + 1].Bn.data, vinLayer[y * 2 + 1][x * 2 + 1].Bn.data)));
					}
				}
			}
			else
			{
				for (size_t i = 0; i < vlayer[idxSource].size(); ++i)
				{
					for (size_t y = 0; y < vlayer[idxSource + 1][i].size(); ++y)
					{
						for (size_t x = 0; x < vlayer[idxSource + 1][i].back().size(); ++x)
						{
							vlayer[idxSource + 1][i][y][x].init(
								ReLU(pFunc(vlayer[idxSource][i][y * 2][x * 2].Rn.data, vlayer[idxSource][i][y * 2 + 1][x * 2].Rn.data, vlayer[idxSource][i][y * 2][x * 2 + 1].Rn.data, vlayer[idxSource][i][y * 2 + 1][x * 2 + 1].Rn.data)),
								ReLU(pFunc(vlayer[idxSource][i][y * 2][x * 2].Gn.data, vlayer[idxSource][i][y * 2 + 1][x * 2].Gn.data, vlayer[idxSource][i][y * 2][x * 2 + 1].Gn.data, vlayer[idxSource][i][y * 2 + 1][x * 2 + 1].Gn.data)),
								ReLU(pFunc(vlayer[idxSource][i][y * 2][x * 2].Bn.data, vlayer[idxSource][i][y * 2 + 1][x * 2].Bn.data, vlayer[idxSource][i][y * 2][x * 2 + 1].Bn.data, vlayer[idxSource][i][y * 2 + 1][x * 2 + 1].Bn.data)));
						}
					}
				}
			}
			return true;
		}

		//convolution func
		bool convFunc_RGB(ptrdiff_t idxSource, size_t idxKernel)
		{
			size_t szKrnl = vkernel[idxKernel].size();
			if (idxSource == -1)
			{
				for (size_t i = 0; i < szKrnl; ++i)
				{
					for (size_t y = 0; y < vlayer[0][i].size(); ++y)
					{
						for (size_t x = 0; x < vlayer[0][i].back().size(); ++x)
						{
							double sumR = vlayer[0][i][y][x].Rn.mBIAS;
							double sumG = vlayer[0][i][y][x].Gn.mBIAS;
							double sumB = vlayer[0][i][y][x].Bn.mBIAS;
							double sumKernelR = 0.;
							double sumKernelG = 0.;
							double sumKernelB = 0.;
							for (size_t ky = 0; ky < vkernel[idxKernel][i].size(); ++ky)
							{
								for (size_t kx = 0; kx < vkernel[idxKernel][i].back().size(); ++kx)
								{
									sumR += vkernel[idxKernel][i][ky][kx].R.wg * vinLayer[y + ky][x + kx].Rn.data;
									sumKernelR += vkernel[idxKernel][i][ky][kx].R.wg;
									sumG += vkernel[idxKernel][i][ky][kx].G.wg * vinLayer[y + ky][x + kx].Gn.data;
									sumKernelG += vkernel[idxKernel][i][ky][kx].G.wg;
									sumB += vkernel[idxKernel][i][ky][kx].B.wg * vinLayer[y + ky][x + kx].Bn.data;
									sumKernelB += vkernel[idxKernel][i][ky][kx].B.wg;
								}
							}
							vlayer[0][i][y][x].init(nndx::cneuron::mSIGMOID(sumR, sumKernelR), nndx::cneuron::mSIGMOID(sumG, sumKernelG), nndx::cneuron::mSIGMOID(sumB, sumKernelB));
						}
					}
				}
			}
			else
			{
				for (size_t j = 0; j < vlayer[idxSource].size(); ++j)
				{
					for (size_t k = 0; k < szKrnl; ++k)
					{
						for (size_t y = 0; y < vlayer[idxSource + 1][szKrnl * j + k].size(); ++y)
						{
							for (size_t x = 0; x < vlayer[idxSource + 1][szKrnl * j + k].back().size(); ++x)
							{
								double sumR = vlayer[idxSource + 1][szKrnl * j + k][y][x].Rn.mBIAS;
								double sumG = vlayer[idxSource + 1][szKrnl * j + k][y][x].Gn.mBIAS;
								double sumB = vlayer[idxSource + 1][szKrnl * j + k][y][x].Bn.mBIAS;
								double sumKernelR = 0.;
								double sumKernelG = 0.;
								double sumKernelB = 0.;
								for (size_t ky = 0; ky < vkernel[idxKernel][k].size(); ++ky)
								{
									for (size_t kx = 0; kx < vkernel[idxKernel][k].back().size(); ++kx)
									{
										sumR += vkernel[idxKernel][k][ky][kx].R.wg * vlayer[idxSource][j][y + ky][x + kx].Rn.data;
										sumKernelR += vkernel[idxKernel][k][ky][kx].R.wg;
										sumG += vkernel[idxKernel][k][ky][kx].G.wg * vlayer[idxSource][j][y + ky][x + kx].Gn.data;
										sumKernelG += vkernel[idxKernel][k][ky][kx].G.wg;
										sumB += vkernel[idxKernel][k][ky][kx].B.wg * vlayer[idxSource][j][y + ky][x + kx].Bn.data;
										sumKernelB += vkernel[idxKernel][k][ky][kx].B.wg;
									}
								}
								vlayer[idxSource + 1][szKrnl * j + k][y][x].init(nndx::cneuron::mSIGMOID(sumR, sumKernelR), nndx::cneuron::mSIGMOID(sumG, sumKernelG), nndx::cneuron::mSIGMOID(sumB, sumKernelB));
							}
						}
					}
				}
			}
			return true;
		}

		bool ConvBackProp(const ::std::vector<double>& res)
		{
			//receive Error
			::std::vector<double> vErr, vErr2;
			net.callBackProp(res, vErr);
			for (auto& x : vErr)
			{
				Inet.callBackProp(::std::vector<double>{ x }, vErr2);
			}

			//init error into back layer
			size_t temp_ = 0;
			for (size_t s = 0; s < vlayer.back().size(); ++s)
			{
				for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
				{
					for (size_t j = 0; j < vlayer.back()[s].back().size(); ++j)
					{
						//* 0.299; // before [temp_++] 3 times
						//* 0.587;
						//* 0.114;

						vlayer.back()[s][i][j].Rn.err = vErr2[temp_++]; // before [temp_++] 3 times
						vlayer.back()[s][i][j].Gn.err = vErr2[temp_++];
						vlayer.back()[s][i][j].Bn.err = vErr2[temp_++];
					}
				}
			}

			//mA backward
			for (size_t i = vfunc.size() - 1; i > 0; --i)
			{
				if (vfunc[i] < 0)
				{
					if (vfunc[i] == -4)
					{
						for (size_t layer = 0; layer < vlayer[i].size(); ++layer)
						{
							for (size_t y = 0; y < vlayer[i][layer].size(); ++y)
							{
								for (size_t x = 0; x < vlayer[i][layer].back().size(); ++x)
								{
									auto& temp2 = vlayer[i][layer][y][x];
									if (temp2.Rn.err != 0.)
									{
										vlayer[i - 1][layer][y * 2][x * 2].Rn.err = temp2.Rn.err;
										vlayer[i - 1][layer][y * 2 + 1][x * 2].Rn.err = temp2.Rn.err;
										vlayer[i - 1][layer][y * 2][x * 2 + 1].Rn.err = temp2.Rn.err;
										vlayer[i - 1][layer][y * 2 + 1][x * 2 + 1].Rn.err = temp2.Rn.err;
									}
									if (temp2.Gn.err != 0.)
									{
										vlayer[i - 1][layer][y * 2][x * 2].Gn.err = temp2.Gn.err;
										vlayer[i - 1][layer][y * 2 + 1][x * 2].Gn.err = temp2.Gn.err;
										vlayer[i - 1][layer][y * 2][x * 2 + 1].Gn.err = temp2.Gn.err;
										vlayer[i - 1][layer][y * 2 + 1][x * 2 + 1].Gn.err = temp2.Gn.err;
									}
									if (temp2.Bn.err != 0.)
									{

										vlayer[i - 1][layer][y * 2][x * 2].Bn.err = temp2.Bn.err;
										vlayer[i - 1][layer][y * 2 + 1][x * 2].Bn.err = temp2.Bn.err;
										vlayer[i - 1][layer][y * 2][x * 2 + 1].Bn.err = temp2.Bn.err;
										vlayer[i - 1][layer][y * 2 + 1][x * 2 + 1].Bn.err = temp2.Bn.err;
									}
								}
							}
						}
					}
					else if ((vfunc[i] == -2) || (vfunc[i] == -3))
					{
						for (size_t layer = 0; layer < vlayer[i].size(); ++layer)
						{
							for (size_t y = 0; y < vlayer[i][layer].size(); ++y)
							{
								for (size_t x = 0; x < vlayer[i][layer].back().size(); ++x)
								{
									for (size_t ly = y * 2; ly < y * 2 + 1; ++ly)
									{
										for (size_t lx = x * 2; lx < x * 2 + 1; ++lx)
										{
											auto& tempnext = vlayer[i][layer][y][x];
											auto& temptarg = vlayer[i - 1][layer][ly][lx];
											if (tempnext.Rn.err != 0.)
											{
												if (temptarg.Rn.data == tempnext.Rn.data)	temptarg.Rn.err = tempnext.Rn.err;
											}
											if (tempnext.Gn.err != 0.)
											{
												if (temptarg.Gn.data == tempnext.Gn.data)	temptarg.Gn.err = tempnext.Gn.err;
											}
											if (tempnext.Bn.err != 0.)
											{
												if (temptarg.Bn.data == tempnext.Bn.data)	temptarg.Bn.err = tempnext.Bn.err;
											}
										}
									}
								}
							}
						}
					}
					else
					{
						ERROR_
							return false;
					}
				}
				else
				{
					auto& kern = vkernel[vfunc[i]]; //											current stack of kernels
					size_t szkern = kern.size(); //												kern size
					for (size_t k = 0; k < szkern; ++k)
					{
						auto& prevlayer = vlayer[i - 1]; //										-previous- layer
						auto& curkern__ = kern[k]; //											current kernel
						size_t szprevlayer = prevlayer.size(); //								prevlayer size
						size_t szcurkerny = curkern__.size(); //								current kernel size y
						size_t szcurkernx = curkern__.back().size(); //							current kernel size x
						for (size_t prev = 0; prev < szprevlayer; ++prev)
						{
							auto& next = vlayer[i][szkern * prev + k]; //						-current next- layer
							auto& curprevlayer__ = prevlayer[prev]; //							current previous layer
							size_t sznext = next.size(); //										next size y
							size_t szbknext = next.back().size(); //							next size x
							for (size_t y = 0; y < sznext; ++y)
							{
								for (size_t x = 0; x < szbknext; ++x)
								{
									for (size_t ky = 0; ky < szcurkerny; ++ky)
									{
										for (size_t kx = 0; kx < szcurkernx; ++kx)
										{
											auto& curprevlayer_ = curprevlayer__[y + ky][x + kx];
											auto& curnextpoint_ = next[y][x];
											auto& curkernpoint_ = curkern__[ky][kx];
											if (curnextpoint_.Rn.err != 0.)
											{
												curprevlayer_.Rn.err += curnextpoint_.Rn.err * curkernpoint_.R.wg;
												curkernpoint_.R.grad += curnextpoint_.Rn.err * curprevlayer_.Rn.data;
											}
											if (curnextpoint_.Gn.err != 0.)
											{
												curprevlayer_.Gn.err += curnextpoint_.Gn.err * curkernpoint_.G.wg;
												curkernpoint_.G.grad += curnextpoint_.Gn.err * curprevlayer_.Gn.data;
											}
											if (curnextpoint_.Bn.err != 0.)
											{
												curprevlayer_.Bn.err += curnextpoint_.Bn.err * curkernpoint_.B.wg;
												curkernpoint_.B.grad += curnextpoint_.Bn.err * curprevlayer_.Bn.data;
											}
										}
									}
								}
							}
							size_t szcurprevy = curprevlayer__.size(); //						size y of current previous layer
							size_t szcurprevx = curprevlayer__.back().size(); //				size x of current previous layer
							for (size_t y = 0; y < szcurprevy; ++y)
							{
								for (size_t x = 0; x < szcurprevx; ++x)
								{
									auto& curpoint_ = curprevlayer__[y][x];
									curpoint_.Rn.err *= nndx::cneuron::mSIGMOID_DRV(curpoint_.Rn.data);
									curpoint_.Gn.err *= nndx::cneuron::mSIGMOID_DRV(curpoint_.Gn.data);
									curpoint_.Bn.err *= nndx::cneuron::mSIGMOID_DRV(curpoint_.Bn.data);
								}
							}
						}

						for (size_t ky = 0; ky < szcurkerny; ++ky)
						{
							for (size_t kx = 0; kx < szcurkernx; ++kx)
							{
								auto& curkernpoint__ = curkern__[ky][kx];
								curkernpoint__.R.dwg = u_net * curkernpoint__.R.grad + moment_net * curkernpoint__.R.dwg;
								curkernpoint__.G.dwg = u_net * curkernpoint__.G.grad + moment_net * curkernpoint__.G.dwg;
								curkernpoint__.B.dwg = u_net * curkernpoint__.B.grad + moment_net * curkernpoint__.B.dwg;

								curkernpoint__.R.wg += curkernpoint__.R.dwg;
								curkernpoint__.G.wg += curkernpoint__.G.dwg;
								curkernpoint__.B.wg += curkernpoint__.B.dwg;
							}
						}
					}
				}
			}

			if (vfunc[0] >= 0) // if first vfunc < 0, then do nothing(wWw in decrease layers don't changing)
			{
				auto& kern = vkernel[vfunc[0]]; //											current stack of kernels
				size_t szkern = kern.size(); //												kern size
				for (size_t k = 0; k < szkern; ++k)
				{
					auto& curkern__ = kern[k]; //											current kernel
					size_t szcurkerny = curkern__.size(); //								current kernel size y
					size_t szcurkernx = curkern__.back().size(); //							current kernel size x
					auto& next = vlayer[0][k]; //											-current next- layer
					size_t sznext = next.size(); //											next size y
					size_t szbknext = next.back().size(); //								next size x
					for (size_t y = 0; y < sznext; ++y)
					{
						for (size_t x = 0; x < szbknext; ++x)
						{
							for (size_t ky = 0; ky < szcurkerny; ++ky)
							{
								for (size_t kx = 0; kx < szcurkernx; ++kx)
								{
									auto& curprevlayer_ = vinLayer[y + ky][x + kx];
									auto& curnextpoint_ = next[y][x];
									auto& curkernpoint_ = curkern__[ky][kx];
									if (curnextpoint_.Rn.err != 0.)
									{
										curkernpoint_.R.grad += curnextpoint_.Rn.err * curprevlayer_.Rn.data;
									}
									if (curnextpoint_.Gn.err != 0.)
									{
										curkernpoint_.G.grad += curnextpoint_.Gn.err * curprevlayer_.Gn.data;
									}
									if (curnextpoint_.Bn.err != 0.)
									{
										curkernpoint_.B.grad += curnextpoint_.Bn.err * curprevlayer_.Bn.data;
									}
								}
							}
						}
					}

					for (size_t ky = 0; ky < szcurkerny; ++ky)
					{
						for (size_t kx = 0; kx < szcurkernx; ++kx)
						{
							auto& curkernpoint__ = curkern__[ky][kx];
							curkernpoint__.R.dwg = u_net * curkernpoint__.R.grad + moment_net * curkernpoint__.R.dwg;
							curkernpoint__.G.dwg = u_net * curkernpoint__.G.grad + moment_net * curkernpoint__.G.dwg;
							curkernpoint__.B.dwg = u_net * curkernpoint__.B.grad + moment_net * curkernpoint__.B.dwg;

							curkernpoint__.R.wg += curkernpoint__.R.dwg;
							curkernpoint__.G.wg += curkernpoint__.G.dwg;
							curkernpoint__.B.wg += curkernpoint__.B.dwg;
						}
					}
				}
			}

			return true;
		}

		bool saveIm_RGB(size_t idx)
		{
			ER_IF(idx >= vlayer.size(), return false; )
			ER_IF(outputF.empty(), return false; )

			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				size_t locY = vlayer[idx][cur].size();
				size_t locX = vlayer[idx][cur].back().size();
				cv::Mat output(static_cast<int>(locY), static_cast<int>(locX), CV_8UC3, cv::Scalar(0, 0, 0));
				for (size_t y = 0; y < locY; ++y)
				{
					for (size_t x = 0; x < locX; ++x)
					{
						output.at<cv::Vec3b>(cv::Point(x, y))[2] = static_cast<unsigned char>(vlayer[idx][cur][y][x].Rn.data  * _clr_);
						output.at<cv::Vec3b>(cv::Point(x, y))[1] = static_cast<unsigned char>(vlayer[idx][cur][y][x].Gn.data  * _clr_);
						output.at<cv::Vec3b>(cv::Point(x, y))[0] = static_cast<unsigned char>(vlayer[idx][cur][y][x].Bn.data  * _clr_);
					}
				}
				::std::string s = outputF + "imRGB" + nts(idx);
				s += '_';
				s += nts(cur);
				cv::imwrite(s + ".jpg", output);
			}
			return true;
		}

		bool saveIm_Gray(size_t idx)
		{
			ER_IF(idx >= vlayer.size(), return false; )
			ER_IF(outputF.empty(), return false; )

			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				size_t locY = vlayer[idx][cur].size();
				size_t locX = vlayer[idx][cur].back().size();
				cv::Mat output(static_cast<int>(locY), static_cast<int>(locX), CV_8UC1, cv::Scalar(0));
				for (size_t y = 0; y < locY; ++y)
				{
					for (size_t x = 0; x < locX; ++x)
					{
						output.at<uint8_t>(cv::Point(x, y)) = static_cast<unsigned char>(vlayer[idx][cur][y][x].Grayn() * _clr_);
					}
				}
				::std::string s = outputF + "imGray" + nts(idx);
				s += '_';
				s += nts(cur);
				cv::imwrite(s + ".jpg", output);
			}
			return true;
		}

		bool saveDat_RGB(size_t idx)
		{
			ER_IF(idx >= vlayer.size(), return false; )
			ER_IF(outputF.empty(), return false; )

			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				::std::string s = outputF + "output_" + nts(idx);
				s += '_';
				s += nts(cur);
				::std::ofstream write(s + ".rgb");
				ER_IF(!write.is_open(), write.close(); return false; )

				size_t locY = vlayer[idx][cur].size();
				size_t locX = vlayer[idx][cur].back().size();
				write << locX << ' ' << locY << ::std::endl;
				for (size_t y = 0; y < locY; ++y)
				{
					for (size_t x = 0; x < locX; ++x)
					{
						write << vlayer[idx][cur][y][x].Rn.data * _clr_ << ' ';
						write << vlayer[idx][cur][y][x].Gn.data * _clr_ << ' ';
						write << vlayer[idx][cur][y][x].Bn.data * _clr_ << ::std::endl;
					}
				}
				write << _filend;
				write.close();
			}
			return true;
		}

		bool saveDat_Gray(size_t idx)
		{
			ER_IF(idx >= vlayer.size(), return false; )
			ER_IF(outputF.empty(), return false; )

			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				::std::string s = outputF + "output_" + nts(idx);
				s += '_';
				s += nts(cur);
				::std::ofstream write(s + ".bw");
				ER_IF(!write.is_open(), write.close(); return false; )

				size_t locY = vlayer[idx][cur].size();
				size_t locX = vlayer[idx][cur].back().size();
				write << locX << ' ' << locY << ::std::endl;
				for (size_t y = 0; y < locY; ++y)
				{
					for (size_t x = 0; x < locX; ++x)
					{
						write << vlayer[idx][cur][y][x].Grayn() << ' ';
					}
					write << ::std::endl;
				}
				write << _filend;
				write.close();
			}
			return true;
		}
	};

	double max_4(double f1, double f2, double f3, double f4)
	{
		double max = f1 > f2 ? f1 : f2;
		max = max > f3 ? max : f3;
		max = max > f4 ? max : f4;
		return max;
	}

	double min_4(double f1, double f2, double f3, double f4)
	{
		double min = f1 > f2 ? f2 : f1;
		min = min > f3 ? f3 : min;
		min = min > f4 ? f4 : min;
		return min;
	}

	double mid_4(double f1, double f2, double f3, double f4)
	{
		return static_cast<double>((f1 + f2 + f3 + f4) / 4.);
	}

	double ReLU(double x)
	{
		if (x > 1.)
		{
			//ERROR_
				x = 1.;
		}
		return (x > 0) ? x : 0;
	}

	template<typename _T>
	::std::string nts(const _T& example)
	{
		::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);

		ss << example;

		::std::string s = ss.str();

		return s;
	}

#ifdef _NNDX_CONV_NEURONET_DEF
	bool neuronet::callBackProp(const ::std::vector<double>& d, ::std::vector<double>& errback)
	{
		ER_IF(!this->isReady, return false; )
		ER_IF((this->moment == 0) || (this->u == 0), return false; )
		ER_IF(d.size() != data.back().size(), return false; )
		backProp(d, errback);
		return true;
	}

	void neuronet::backProp(const ::std::vector<double>& d, ::std::vector<double>& errback)
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
		
		for (size_t i = 0; i < errR[0].size() - 1; ++i) // -1 BIAS ERROR, since BIAS dont connected with previos layer
		{
			errback.reserve(errback.capacity() + 1);
			errback.emplace_back(errR[0][i]);
		}
	}

#endif
}