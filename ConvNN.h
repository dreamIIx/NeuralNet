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

		~__bool() {}
	};

	class cneuron
	{
	public:
		double data;
		double mBIAS;
		double err;

		explicit cneuron() : data(0.), mBIAS(0.), err(0.) {}

		cneuron(double x, double bias) : data(x), mBIAS(bias), err(0.)
		{
			::std::cout << "bias = " << bias << ::std::endl;
		}

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
		}*/

		cneuron& operator=(double x)
		{
			data = x;
			return *this;
		}

		static double mSIGMOID(double value)
		{
			return 1. / (1. + exp(-value));
		}

		static double mSIGMOID_DRV(cneuron& cn)
		{
			return cn.data * (1. - cn.data);
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

		explicit rgb_T(const cv::Vec3b& v)
		{
			Bn = v[0] / _clr_;
			Gn = v[1] / _clr_;
			Rn = v[2] / _clr_;
		}

		void init(double r, double g, double b)
		{
			Rn = r;
			Gn = g;
			Bn = b;
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

		explicit cnnKernel_c3(double r, double g, double b) : R(r), G(g), B(b)
		{
			::std::cout << "r = " << r << ::std::endl;
			::std::cout << "g = " << g << ::std::endl;
			::std::cout << "b = " << b << ::std::endl;
		}

		~cnnKernel_c3() {}
	};

	typedef ::std::vector<::std::vector<rgb_T>> image;
	typedef ::std::vector<::std::vector<cnnKernel_c3>> mapge;

	class CNN
	{
	private:
		bool isReady;
		double u_net;

		::std::string inputF;
		::std::string outputF;
		::std::string dataF;

		image vinLayer;									// input image
		::std::vector<::std::vector<image>> vlayer;		// vector of layer's data
		::std::vector<::std::vector<mapge>> vkernel;	// vector of kernel's data
		::std::vector<int> vfunc;						// vector of functions(conv, decr)
		nndx::neuronet net;								// main forward neural network

	public:
		explicit CNN() : isReady(false), u_net(0.) {}

		CNN(CNN&& data)
		{
			this->isReady	=		::std::forward<decltype(data.isReady)>(data.isReady);
			this->u_net		=		::std::forward<decltype(data.u_net)>(data.u_net);
			this->vinLayer	=		::std::forward<decltype(data.vinLayer)>(data.vinLayer);
			this->vkernel	=		::std::forward<decltype(data.vkernel)>(data.vkernel);
			this->vfunc		=		::std::forward<decltype(data.vfunc)>(data.vfunc);
			this->vlayer	=		::std::forward<decltype(data.vlayer)>(data.vlayer);
			this->inputF	=		::std::forward<decltype(data.inputF)>(data.inputF);
			this->outputF	=		::std::forward<decltype(data.outputF)>(data.outputF);
			this->dataF		=		::std::forward<decltype(data.dataF)>(data.dataF);
			this->net		=		::std::forward<decltype(data.net)>(data.net);
#ifdef _mainDEBUG
			::std::cout << "[CNN] call &&constructor!" << ::std::endl;
#endif
		}

		CNN(const cv::Mat& img) : isReady(false), u_net(0.)
		{
			if ((img.rows < 2) || (img.cols < 2))
			{
				ERROR_
			}
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

		bool initDir(const ::std::string& in, const ::std::string& out, const ::std::string& data)
		{
			if (in.empty() || out.empty() || data.empty())
			{
				ERROR_
					return false;
			}
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
			if (!vinLayer.empty())
				if ((img.rows < vinLayer.size()) || (img.cols < vinLayer.back().size()))
					isReady = false;
			if ((img.rows < 2) || (img.cols < 2))
			{
				ERROR_
					return false;
			}
			if (!vinLayer.empty())
			{
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
			if (!vinLayer.empty())
				if ((Y < vinLayer.size()) || (X < vinLayer.back().size()))
					isReady = false;
			if (!vinLayer.empty())
			{
				vinLayer.clear();
			}
			if ((X < 2) || (Y < 2))
			{
				ERROR_
					return false;
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
			if (idx_ > vkernel.size())
			{
				ERROR_
					return false;
			}
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<mapge>());
			}
			if (idxKernel > vkernel[idx_].size())
			{
				ERROR_
					return false;
			}
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

		bool initKrnl(size_t idx_, size_t idxKernel, size_t sizex, size_t sizey, double func(void))
		{
			if (idx_ > vkernel.size())
			{
				ERROR_
					return false;
			}
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<mapge>());
			}
			if (idxKernel > vkernel[idx_].size())
			{
				ERROR_
					return false;
			}
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
					vkernel[idx_][idxKernel].back().emplace_back(cnnKernel_c3(func(), func(), func()));
				}
			}

			return true;
		}

		//files must be with .krnl extension
		bool initKrnlFromFile(size_t idx_, size_t idxKernel)
		{
			if (idx_ > vkernel.size())
			{
				ERROR_
					return false;
			}
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<mapge>());
			}
			if (idxKernel > vkernel[idx_].size())
			{
				ERROR_
					return false;
			}
			else if (idxKernel < vkernel[idx_].size())
			{
				vkernel[idx_][idxKernel].clear();
			}
			else
			{
				vkernel[idx_].reserve(vkernel[idx_].capacity() + 1);
				vkernel[idx_].emplace_back(mapge());
			}
			if (dataF.empty())
			{
				ERROR_
					return false;
			}

			::std::string sfile = dataF + nts(idx_);
			sfile += '_';
			sfile += nts(idxKernel);
			::std::ifstream read(sfile + ".krnl");
			if (!read.is_open())
			{
				read.close();
				ERROR_
					return false;
			}

			size_t sizey, sizex;
			double kernelTemp1 = 0.;
			double kernelTemp2 = 0.;
			double kernelTemp3 = 0.;
			read >> sizex;
			read >> sizey;

			vkernel[idx_][idxKernel].reserve(sizey);
			for (size_t i = 0; i < sizey; ++i)
			{
				vkernel[idx_][idxKernel].emplace_back(::std::vector<cnnKernel_c3>());
				vkernel[idx_][idxKernel].back().reserve(sizex);
				for (size_t j = 0; j < sizex; ++j)
				{
					read >> kernelTemp1;
					read >> kernelTemp2;
					read >> kernelTemp3;
					vkernel[idx_][idxKernel].back().emplace_back(kernelTemp1, kernelTemp2, kernelTemp3);
				}
			}
			read >> sfile;
			if (sfile != _filend)
			{
				ERROR_
					return false;
			}
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
		bool initFuncEx(double randF(), size_t size, ...)
		{
			isReady = false;
			if (vinLayer.empty())
			{
				ERROR_
					return false;
			}
			if (vkernel.empty())
			{
				ERROR_
					return false;
			}
			if (!vlayer.empty())
			{
				vlayer.clear();
			}
			if (!vfunc.empty())
			{
				vfunc.clear();
			}

			if (size)
			{
				va_list args;
				va_start(args, size);

				vlayer.reserve(size);
				vfunc.reserve(size);
				int functemp_ = va_arg(args, int);

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
					if (functemp_ >= vkernel.size())
					{
						ERROR_
							return false;
					}

					vlayer.emplace_back(::std::vector<image>());
					auto temp_ = vkernel[functemp_].size();
					vlayer.back().reserve(temp_);
					for (size_t n = 0; n < temp_; n++)
					{
						vlayer.back().emplace_back(image());
					}
					vfunc.emplace_back(functemp_);
				}

				// for other layers
				for (size_t i = 1; i < size; ++i)
				{
					functemp_ = va_arg(args, int);
					if (functemp_ < 0)
					{
						vlayer.emplace_back(::std::vector<image>());
						auto temp_ = vlayer[i - 1].size();
						vlayer.back().reserve(temp_);
						for (size_t n = 0; n < temp_; n++)
						{
							vlayer.back().emplace_back(image());
						}
						vfunc.emplace_back(functemp_);
					}
					else
					{
						if (functemp_ >= vkernel.size())
						{
							ERROR_
								return false;
						}

						vlayer.emplace_back(::std::vector<image>());
						auto temp_ = vkernel[functemp_].size() * vlayer[i - 1].size();
						vlayer.back().reserve(temp_);
						for (size_t n = 0; n < temp_; n++)
						{
							vlayer.back().emplace_back(image());
						}
						vfunc.emplace_back(functemp_);
					}
				}
				va_end(args);

				// init to zero-s
				// for layer forward inputLayer
				if (vfunc[0] < 0)
				{
#ifdef _DEBUG
					if (vlayer[0].size() != 1) // there are only vlayer[0][0] for func < 0(-2, -3, -4...)
					{
						ERROR_
							return false;
					}
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
							vlayer[0].back().back().emplace_back(rgb_T(randF));
						}
					}
				}
				else
				{
					auto szKrnl = vkernel[vfunc[0]].size();
#ifdef _DEBUG
					if (vlayer[0].size() != szKrnl)
					{
						ERROR_
							return false;
					}
#endif

					for (size_t i = 0; i < szKrnl; ++i)
					{
						if ((vinLayer.size() < vkernel[vfunc[0]][i].size()) || (vinLayer.back().size() < vkernel[vfunc[0]][i].back().size()))
						{
							ERROR_
								return false;
						}

						size_t localy = vinLayer.size() - vkernel[vfunc[0]][i].size() + 1;
						size_t localx = vinLayer.back().size() - vkernel[vfunc[0]][i].back().size() + 1;
						vlayer[0][i].reserve(localy);
						for (size_t y = 0; y < localy; ++y)
						{
							vlayer[0][i].emplace_back(::std::vector<rgb_T>());
							vlayer[0][i].back().reserve(localx);
							for (size_t x = 0; x < localx; ++x)
							{
								vlayer[0][i].back().emplace_back(rgb_T(randF));
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
						if (vlayer[i].size() != vlayer[i - 1].size())
						{
							ERROR_
								return false;
						}
#endif

						for (size_t prev = 0; prev < vlayer[i - 1].size(); ++prev)
						{
							if ((vlayer[i - 1][prev].size() < 2) || (vlayer[i - 1][prev].back().size() < 2))
							{
								ERROR_
									return false;
							}

							size_t rows = (vlayer[i - 1][prev].size() % 2 == 0) ? static_cast<size_t>(vlayer[i - 1][prev].size() * 0.5) : static_cast<size_t>((vlayer[i - 1][prev].size() - 1) * 0.5);
							size_t cols = (vlayer[i - 1][prev].back().size() % 2 == 0) ? static_cast<size_t>(vlayer[i - 1][prev].back().size() * 0.5) : static_cast<size_t>((vlayer[i - 1][prev].back().size() - 1) * 0.5);

							vlayer[i][prev].reserve(rows);
							for (size_t y = 0; y < rows; ++y)
							{
								vlayer[i][prev].emplace_back(::std::vector<rgb_T>());
								vlayer[i][prev].back().reserve(cols);
								for (size_t x = 0; x < cols; ++x)
								{
									vlayer[i][prev].back().emplace_back(rgb_T(randF));
								}
							}
						}
					}
					else
					{
						auto szKrnl = vkernel[vfunc[i]].size();
#ifdef _DEBUG
						if (vlayer[i].size() != szKrnl * vlayer[i - 1].size())
						{
							ERROR_
								return false;
						}
#endif

						for (size_t prev = 0; prev < vlayer[i - 1].size(); ++prev)
						{
							for (size_t k = 0; k < szKrnl; ++k)
							{
								if ((vlayer[i - 1][prev].size() < vkernel[vfunc[i]][k].size()) || (vlayer[i - 1][prev].back().size() < vkernel[vfunc[0]][k].back().size()))
								{
									ERROR_
										return false;
								}

								size_t localy = vlayer[i - 1][prev].size() - vkernel[vfunc[i]][k].size() + 1;
								size_t localx = vlayer[i - 1][prev].back().size() - vkernel[vfunc[i]][k].back().size() + 1;
								vlayer[i][szKrnl * prev + k].reserve(localy);
								for (size_t y = 0; y < localy; ++y)
								{
									vlayer[i][szKrnl * prev + k].emplace_back(::std::vector<rgb_T>());
									vlayer[i][szKrnl * prev + k].back().reserve(localx);
									for (size_t x = 0; x < localx; ++x)
									{
										vlayer[i][szKrnl * prev + k].back().emplace_back(rgb_T(randF));
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

			isReady = true;
			return isReady;
		}

		//initFuncEx must be called earlier
		//mA() must be called earlier
		bool init_neuronet(::std::vector<int>&& tplNet, double funcWeights(), nndx::neuron::_func funcNet, double moment, double u, double mainu)
		{
			if (!isReady)
			{
				ERROR_
					return false;
			}
			if (net.getState())
			{
				net.~neuronet();
				if (net.getState())
				{
					ERROR_
						return false;
				}
			}

			__bool res;
			int num = 0;

			for (auto& x : vlayer.back())
			{
				num += static_cast<int>(x.size() * x[0].size());
			}
			num *= 3; // CV_8UC3!
			tplNet.emplace(tplNet.begin(), num);

			res = net.setGenWeightsFunc(funcWeights);
			res = net.init(tplNet, funcNet);
			res = net.setParams(moment, u);
			u_net = mainu;

			return net.getState();
		}

		bool init_neuronet(nndx::neuronet&& x, double mainu)
		{
			net = ::std::forward<decltype(x)>(x);
			u_net = mainu;

			return net.getState();
		}

		bool init_neuronet(::std::string file, nndx::neuron::_func funcNet, double moment, double u, double mainu)
		{
			if (!isReady)
			{
				ERROR_
					return false;
			}
			if (net.getState())
			{
				net.~neuronet();
				if (net.getState())
				{
					ERROR_
						return false;
				}
			}

			__bool res;
			net.nDataNet = file;
			res = net.initFromFile();
			res = net.setFunc(funcNet);
			res = net.setParams(moment, u);
			u_net = mainu;

			return net.getState();
		}

		bool fullNet_mA(const ::std::vector<::std::vector<double>>& results, const int& ittr, int func(int&), ::std::string subS, ::std::string extImg)
		{
			if (!net.getState())
			{
				ERROR_
					return false;
			}
			if (u_net == 0.)
			{
				ERROR_
					return false;
			}
			if (!isReady)
			{
				ERROR_
					return false;
			}

			::std::string sfile = inputF + subS;
			__bool resT;

			for (int i = 0; i < ittr; ++i)
			{
				resT = init_image(cv::imread(sfile + nndx::nts(func(i)) + extImg));

				resT = mA();

				::std::vector<double> in;
				in.reserve(net.data[0].size() - 1); // <----------   important

				for (size_t s = 0; s < vlayer.back().size(); ++s)
				{
					for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
					{
						for (size_t j = 0; j < vlayer.back()[s][0].size(); ++j)
						{
							in.emplace_back(vlayer.back()[s][i][j].Rn.data);
							in.emplace_back(vlayer.back()[s][i][j].Gn.data);
							in.emplace_back(vlayer.back()[s][i][j].Bn.data);
						}
					}
				}

				resT = net.fillInput(in);
				resT = net.callActivationF();

				::std::cout << " func(i) - " << func(i) << "\n";
				for (auto& x : net.getResults())
				{
					::std::cout << x << "\n";
				}
				::std::cout << "\n";

				resT = ConvBackProp(results[func(i)]);
			}
			::std::cout << ::std::endl;
			resT = net.saveF(outputF + "net.txt");

			return true;
		}

		bool fullNet_mA()
		{
			if (!isReady)
			{
				ERROR_
					return false;
			}

			__bool resT;
			resT = mA();

			::std::vector<double> in;
			in.reserve(net.data[0].size() - 1); // <----------   important

			for (size_t s = 0; s < vlayer.back().size(); ++s)
			{
				for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
				{
					for (size_t j = 0; j < vlayer.back()[s][0].size(); ++j)
					{
						in.emplace_back(vlayer.back()[s][i][j].Rn.data);
						in.emplace_back(vlayer.back()[s][i][j].Gn.data);
						in.emplace_back(vlayer.back()[s][i][j].Bn.data);
					}
				}
			}

			resT = net.fillInput(in);
			resT = net.callActivationF();

			for (auto& x : net.getResults())
			{
				::std::cout << x << "\n";
			}
			::std::cout << ::std::endl;

			return true;
		}

	private:
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
				//res = saveIm_RGB(static_cast<size_t>(current));
				res = saveIm_Gray(static_cast<size_t>(current));
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
			auto szKrnl = vkernel[idxKernel].size();
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
							for (size_t ky = 0; ky < vkernel[idxKernel][i].size(); ++ky)
							{
								for (size_t kx = 0; kx < vkernel[idxKernel][i].back().size(); ++kx)
								{
									sumR += vkernel[idxKernel][i][ky][kx].R.wg * vinLayer[y + ky][x + kx].Rn.data;
									sumG += vkernel[idxKernel][i][ky][kx].G.wg * vinLayer[y + ky][x + kx].Gn.data;
									sumB += vkernel[idxKernel][i][ky][kx].B.wg * vinLayer[y + ky][x + kx].Bn.data;
								}
							}
							vlayer[0][i][y][x].init(nndx::cneuron::mSIGMOID(sumR), nndx::cneuron::mSIGMOID(sumG), nndx::cneuron::mSIGMOID(sumB));
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
								for (size_t ky = 0; ky < vkernel[idxKernel][k].size(); ++ky)
								{
									for (size_t kx = 0; kx < vkernel[idxKernel][k].back().size(); ++kx)
									{
										sumR += vkernel[idxKernel][k][ky][kx].R.wg * vlayer[idxSource][j][y + ky][x + kx].Rn.data;
										sumG += vkernel[idxKernel][k][ky][kx].G.wg * vlayer[idxSource][j][y + ky][x + kx].Gn.data;
										sumB += vkernel[idxKernel][k][ky][kx].B.wg * vlayer[idxSource][j][y + ky][x + kx].Bn.data;
									}
								}
								vlayer[idxSource + 1][szKrnl * j + k][y][x].init(nndx::cneuron::mSIGMOID(sumR), nndx::cneuron::mSIGMOID(sumG), nndx::cneuron::mSIGMOID(sumB));
							}
						}
					}
				}
			}
			return true;
		}

		bool ConvBackProp(const ::std::vector<double>& res)
		{
			//receive errors
			::std::vector<double> vErr;
			net.callBackProp(res, vErr);

			/*//init error into back layer
			size_t temp_ = 0;
			for (size_t s = 0; s < vlayer.back().size(); ++s)
			{
				for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
				{
					for (size_t j = 0; j < vlayer.back()[s].back().size(); ++j)
					{
						vlayer.back()[s][i][j].Error = vErr[temp_++];
					}
				}
			}

			//mA backward
			for (size_t i = vfunc.size() - 1; i > 0; --i)
			{
				if (vfunc[i] < 0)
				{
					if (vfunc[i] == 4)
					{
						for (size_t layer = 0; layer < vlayer[i].size(); ++layer)
						{
							for (size_t y = 0; y < vlayer[i][layer].size(); ++y)
							{
								for (size_t x = 0; x < vlayer[i][layer].back().size(); ++x)
								{
									if (vlayer[i][layer][y][x].Error == 0.)	continue;
									auto& temp2 = vlayer[i][layer][y][x];
									vlayer[i - 1][layer][y * 2][x * 2].Error = temp2.Error;
									vlayer[i - 1][layer][y * 2 + 1][x * 2].Error = temp2.Error;
									vlayer[i - 1][layer][y * 2][x * 2 + 1].Error = temp2.Error;
									vlayer[i - 1][layer][y * 2 + 1][x * 2 + 1].Error = temp2.Error;
								}
							}
						}
					}
					else
					{
						for (size_t layer = 0; layer < vlayer[i].size(); ++layer)
						{
							for (size_t y = 0; y < vlayer[i][layer].size(); ++y)
							{
								for (size_t x = 0; x < vlayer[i][layer].back().size(); ++x)
								{
									if (vlayer[i][layer][y][x].Error == 0.)	continue;
									for (size_t ly = y * 2; ly < y * 2 + 1; ++ly)
									{
										for (size_t lx = x * 2; lx < x * 2 + 1; ++lx)
										{
											auto& temp = vlayer[i - 1][layer][ly][lx];
											auto& temp2 = vlayer[i][layer][y][x];
											if		(temp.Rn == temp2.Rn)	temp.Error = temp2.Error;
											else if (temp.Gn == temp2.Gn)	temp.Error = temp2.Error;
											else if (temp.Bn == temp2.Bn)	temp.Error = temp2.Error;
										}
									}
								}
							}
						}
					}
				}
				else
				{
					auto& kern = vkernel[vfunc[i]];
					size_t szkern = kern.size();
					for (size_t k = 0; k < szkern; ++k)
					{
						auto& prevlayer = vlayer[i - 1];
						size_t szprevlayer = prevlayer.size();
						size_t sztemp = kern[k].size();
						size_t szbktemp = kern[k].back().size();
						for (size_t prev = 0; prev < szprevlayer; ++prev)
						{
							auto& next = vlayer[i][szkern * prev + k];
							size_t sznext = vlayer[i][szkern * prev + k].size();
							size_t szbknext = vlayer[i][szkern * prev + k].back().size();
							for (size_t y = 0; y < sznext; ++y)
							{
								for (size_t x = 0; x < szbknext; ++x)
								{
									if (next[y][x].Error == 0.) continue;
									for (size_t ky = 0; ky < sztemp; ++ky)
									{
										for (size_t kx = 0; kx < szbktemp; ++kx)
										{
											prevlayer[prev][y + ky][x + kx].Error = next[y][x].Error * kern[k][ky][kx];
										}
									}
								}
							}
						}

						for (size_t prev = 0; prev < szprevlayer; ++prev)
						{
							auto& next = vlayer[i][szkern * prev + k];
							size_t sznext = vlayer[i][szkern * prev + k].size();
							size_t szbknext = vlayer[i][szkern * prev + k].back().size();
							for (size_t ky = 0; ky < sztemp; ++ky)
							{
								for (size_t kx = 0; kx < szbktemp; ++kx)
								{
									for (size_t y = 0; y < (sznext - sztemp + 1); ++y)
									{
										for (size_t x = 0; x < (szbknext - szbktemp + 1); ++x)
										{
											if (next[y][x].Error == 0.) continue;
											kern[k][ky][kx] += u_net * next[y][x].Error * prevlayer[prev][y + ky][x + kx].Grayn;
										}
									}
								}
							}
						}

						/*for (size_t prev = 0; prev < szprevlayer; ++prev)
						{
							size_t szprevsizey = prevlayer[prev].size() - sztemp + 1;
							size_t szbkprevsizex = prevlayer[prev].back().size() - szbktemp + 1;
							for (size_t y = 0; y < szprevsizey; ++y)
							{
								for (size_t x = 0; x < szbkprevsizex; ++x)
								{
									for (size_t ky = 0; ky < sztemp; ++ky)
									{
										for (size_t kx = 0; kx < szbktemp; ++kx)
										{
											kern[k][ky][kx] += u_net * prevlayer[prev][y + ky][x + kx].Error * prevlayer[prev][y + ky][x + kx].Grayn;
										}
									}
								}
							}
						}
					}
				}
			}*/

			return true;
		}

		bool saveIm_RGB(size_t idx)
		{
			if (idx >= vlayer.size())
			{
				ERROR_
					return false;
			}
			if (outputF.empty())
			{
				ERROR_
					return false;
			}
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
			if (idx >= vlayer.size())
			{
				ERROR_
					return false;
			}
			if (outputF.empty())
			{
				ERROR_
					return false;
			}
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
			if(idx >= vlayer.size())
			{
				ERROR_
					return false;
			}
			if (outputF.empty())
			{
				ERROR_
					return false;
			}
			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				::std::string s = outputF + "output_" + nts(idx);
				s += '_';
				s += nts(cur);
				::std::ofstream write(s + ".rgb");
				if (!write.is_open())
				{
					write.close();
					ERROR_
						return false;
				}

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
			if (idx >= vlayer.size())
			{
				ERROR_
					return false;
			}
			if (outputF.empty())
			{
				ERROR_
					return false;
			}
			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				::std::string s = outputF + "output_" + nts(idx);
				s += '_';
				s += nts(cur);
				::std::ofstream write(s + ".bw");
				if (!write.is_open())
				{
					write.close();
					ERROR_
						return false;
				}

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
			ERROR_
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
		if (d.size() != data.back().size())
		{
			ERROR_
				return false;
		}
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
		for (size_t j = 0; j < data[data.size() - 1].size(); ++j)
		{
			for (size_t prev = 0; prev < data[data.size() - 2].size(); ++prev)
			{
				weight[data.size() - 2][prev * data[data.size() - 1].size() + j].dwg = u * weight[data.size() - 2][prev * data[data.size() - 1].size() + j].grad + (moment * weight[data.size() - 2][prev * data[data.size() - 1].size() + j].dwg);
				weight[data.size() - 2][prev * data[data.size() - 1].size() + j].wg += weight[data.size() - 2][prev * data[data.size() - 1].size() + j].dwg;
			}
		}

		errback = errR[0];
	}

#endif
}