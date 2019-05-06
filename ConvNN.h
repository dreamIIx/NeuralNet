#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "opencv2\opencv.hpp"

#include "NN.h"

#ifndef def_ERR_FL_
#define def_ERR_FL_
#define def_XTT_S(x) #x
#define def_XTT_S_(x) def_XTT_S(x)
#define def_XTT_S__LINE__ def_XTT_S_(__LINE__)
#define def__FILELINE (__FILE__  " line " def_XTT_S__LINE__)
#define ERROR_ ::std::cout << "Error - " << def__FILELINE << ::std::endl;
#endif

//#define _mainDEBUG

#ifndef _clr
#define _clr uint8_t
#define _clr_ static_cast<_clr>(0b1111'1111)
#endif

#ifndef _filend
#define _filend "endOFfile"
#endif

namespace nndx
{
	_clr max_4(_clr, _clr, _clr, _clr);
	_clr min_4(_clr, _clr, _clr, _clr);
	_clr mid_4(_clr, _clr, _clr, _clr);
	inline double ReLU(const double&);
	::std::string nts(const int&);
	::std::string nts(const size_t&);

#pragma pack(push, 1)
	struct rgb_T
	{
		_clr R;
		_clr G;
		_clr B;
		_clr Gray;
		double Rn;
		double Gn;
		double Bn;
		double Grayn;

		rgb_T() : R(0), G(0), B(0), Gray(0), Rn(0.), Gn(0.), Bn(0.), Grayn(0.) {}

		rgb_T(_clr r, _clr g, _clr b) : R(r), G(g), B(b), Rn(r / _clr_), Gn(g / _clr_), Bn(b / _clr_)
		{
			Gray = static_cast<_clr>(0.299 * R + 0.587 * G + 0.114 * B);
			Grayn = 0.299 * Rn + 0.587 * Gn + 0.114 * Bn;
		}

		rgb_T(double r, double g, double b) : Rn(r), Gn(g), Bn(b)
		{
			R = static_cast<_clr>(r * _clr_);
#ifdef _mainDEBUG
			::std::wcout << r << " to " << R << ::std::endl;
#endif

			G = static_cast<_clr>(g * _clr_);
#ifdef _mainDEBUG
			::std::wcout << g << " to " << G << ::std::endl;
#endif

			B = static_cast<_clr>(b * _clr_);
#ifdef _mainDEBUG
			::std::wcout << b << " to " << B << ::std::endl;
#endif
			Gray = static_cast<_clr>(0.299 * R + 0.587 * G + 0.114 * B);
			Grayn = 0.299 * r + 0.587 * g + 0.114 * b;
		}

		rgb_T(const cv::Vec3b& v)
		{
			B = v[0];
			G = v[1];
			R = v[2];
			Bn = v[0] / _clr_;
			Gn = v[1] / _clr_;
			Rn = v[2] / _clr_;
			Gray = static_cast<_clr>(0.299 * R + 0.587 * G + 0.114 * B);
			Grayn = 0.299 * Rn + 0.587 * Gn + 0.114 * Bn;
		}
	};
#pragma pack(pop)

	class CNN
	{
	private:
		using image = ::std::vector<::std::vector<rgb_T>>;
		unsigned int nConv;
		::std::string inputF;
		::std::string outputF;
		::std::string dataF;
		image vinLayer;
		::std::vector<::std::vector<::std::vector<::std::vector<double>>>> vkernel;
		::std::vector<int> vfunc;
		::std::vector<size_t> krnl;
		nndx::neuronet net;

	public:
		::std::vector<::std::vector<image>> vlayer;

		CNN() : nConv(0) {}

		CNN(CNN&& data)
		{
			this->nConv = ::std::forward<decltype(data.nConv)>(data.nConv);
			this->vinLayer = ::std::forward<decltype(data.vinLayer)>(data.vinLayer);
			this->vkernel = ::std::forward<decltype(data.vkernel)>(data.vkernel);
			this->vfunc = ::std::forward<decltype(data.vfunc)>(data.vfunc);
			this->krnl = ::std::forward<decltype(data.krnl)>(data.krnl);
			this->vlayer = ::std::forward<decltype(data.vlayer)>(data.vlayer);
			this->inputF = ::std::forward<decltype(data.inputF)>(data.inputF);
			this->outputF = ::std::forward<decltype(data.outputF)>(data.outputF);
			this->dataF = ::std::forward<decltype(data.dataF)>(data.dataF);
			this->net = ::std::forward<decltype(data.net)>(data.net);
#ifdef _mainDEBUG
			::std::cout << "[CNN] call &&constructor!" << ::std::endl;
#endif
		}

		CNN(const cv::Mat& img) : nConv(0)
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

		// first arg - size of vlayer and vfunc
		// vfunc:
		// 10 - convFunc_RGB
		// 2 - decreaseX2_RGB(max)
		// 3 - decreaseX2_RGB(min)
		// 4 - decreaseX2_RGB(mid)
		CNN(unsigned int size, ...) : nConv(0)
		{
			if (!!size)
			{
				va_list args;
				va_start(args, size);
				vlayer.reserve(size);
				vfunc.reserve(size);
				for (unsigned int i = 0; i < size; ++i)
				{
					vlayer.emplace_back(::std::vector<image>());
					vfunc.emplace_back(va_arg(args, int));
					if (vfunc.back() == 10)	++nConv;
				}
				va_end(args);
			}
			else
			{
				ERROR_
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

		//count of rows and cols of image must be greater than 1
		bool init_image(const cv::Mat& img)
		{
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

		//for filled images (R, G, B: 0 - 255)
		bool init_image(unsigned int X, unsigned int Y, _clr R, _clr G, _clr B)
		{
			if (!vinLayer.empty())
			{
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

		bool initKrnl(size_t idx_, size_t idxKernel, unsigned int sizex, unsigned int sizey, ...)
		{
			if (idx_ > vkernel.size())
			{
				ERROR_
					return false;
			}
			else if (idx_ == vkernel.size())
			{
				vkernel.reserve(vkernel.capacity() + 1);
				vkernel.emplace_back(::std::vector<::std::vector<::std::vector<double>>>());
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
				vkernel[idx_].emplace_back(::std::vector<::std::vector<double>>());
			}
			va_list args;
			va_start(args, sizey);

			vkernel[idx_][idxKernel].reserve(sizey);
			for (unsigned int i = 0; i < sizey; ++i)
			{
				vkernel[idx_][idxKernel].emplace_back(::std::vector<double>());
				vkernel[idx_][idxKernel].back().reserve(sizex);
				for (unsigned int j = 0; j < sizex; ++j)
				{
					vkernel[idx_][idxKernel].back().emplace_back(va_arg(args, double));
				}
			}

			va_end(args);
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
				vkernel.emplace_back(::std::vector<::std::vector<::std::vector<double>>>());
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
				vkernel[idx_].emplace_back(::std::vector<::std::vector<double>>());
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

			unsigned int sizey, sizex;
			double kernelTemp = 0.;
			read >> sizex;
			read >> sizey;

			vkernel[idx_][idxKernel].reserve(sizey);
			for (unsigned int i = 0; i < sizey; ++i)
			{
				vkernel[idx_][idxKernel].emplace_back(::std::vector<double>());
				vkernel[idx_][idxKernel].back().reserve(sizex);
				for (unsigned int j = 0; j < sizex; ++j)
				{
					read >> kernelTemp;
					vkernel[idx_][idxKernel].back().emplace_back(kernelTemp);
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
		// 10 - convFunc_RGB
		// 2 - decreaseX2_RGB(max)
		// 3 - decreaseX2_RGB(min)
		// 4 - decreaseX2_RGB(mid)
		bool initFuncEx(unsigned int size, ...)
		{
			if (!vlayer.empty())
			{
				vlayer.clear();
			}
			if (!vfunc.empty())
			{
				vfunc.clear();
				nConv = 0;
			}
			if (!!size)
			{
				va_list args;
				va_start(args, size);
				vlayer.reserve(size);
				vfunc.reserve(size);
				for (unsigned int i = 0; i < size; ++i)
				{
					vlayer.emplace_back(::std::vector<image>());
					vfunc.emplace_back(va_arg(args, int));
					if (vfunc.back() == 10)	++nConv;
				}
				va_end(args);
			}
			else
			{
				ERROR_
					return false;
			}
			return true;
		}

		//initFuncEx must be called earlier
		bool autoInitVec(const ::std::vector<size_t>& data)
		{
			if (data.empty())
			{
				ERROR_
					return false;
			}
			if (data.size() != nConv)
			{
				ERROR_
					return false;
			}
			for (auto& x : data)
			{
				if (x >= vkernel.size())
				{
					ERROR_
						return false;
				}
			}
			if (vfunc.empty())
			{
				ERROR_
					return false;
			}
			if (vlayer.empty())
			{
				ERROR_
					return false;
			}
			krnl = data;
			vlayer[0].clear();
			if (vfunc[0] == 10)
			{
				size_t temp = vkernel[data[0]].size();
				vlayer[0].reserve(temp);
				for (int i = 0; i < temp; ++i)
				{
					vlayer[0].emplace_back(image());
				}
			}
			else if ((vfunc[0] >= 2) && (vfunc[0] <= 4))
			{
				vlayer[0].reserve(1);
				vlayer[0].emplace_back(image());
			}
			else
			{
				ERROR_
					return false;
			}

			size_t krnl = 1;
			for (size_t i = 1; i < vfunc.size(); ++i)
			{
				vlayer[i].clear();
				if (vfunc[i] == 10)
				{
					size_t temp = vkernel[data[krnl++]].size() * vlayer[i - 1].size();
					vlayer[i].reserve(temp);
					for (int j = 0; j < temp; ++j)
					{
						vlayer[i].emplace_back(image());
					}
				}
				else if ((vfunc[i] >= 2) && (vfunc[i] <= 4))
				{
					size_t temp = vlayer[i - 1].size();
					vlayer[i].reserve(temp);
					for (int j = 0; j < temp; ++j)
					{
						vlayer[i].emplace_back(image());
					}
				}
				else
				{
					ERROR_
						return false;
				}
			}
			return true;
		}

		//autoInitVec must be called earlier
		bool init_neuronet(::std::vector<int>&& tplNet, double funcWeights(), _dTYPEFUNC funcNet, double moment, double u)
		{
			if (krnl.empty())
			{
				ERROR_
					return false;
			}
			bool res = false;
			int num = 0;
			for (auto& x : vlayer.back())
			{
				num += x.size() * x[0].size();
			}
			tplNet.emplace(tplNet.begin(), num);
			res = net.setGenWeightsFunc(funcWeights);
			if (!res)
			{
				ERROR_
					return false;
			}
			res = net.init(tplNet, funcNet);
			if (!res)
			{
				ERROR_
					return false;
			}
			res = net.setParams(moment, u);
			if (!res)
			{
				ERROR_
					return false;
			}
			return net.getState();
		}

		//autoInitVec must be called earlier
		bool init_neuronet(::std::string file, _dTYPEFUNC funcNet, double moment, double u)
		{
			if (krnl.empty())
			{
				ERROR_
					return false;
			}
			bool res = false;
			net.nDataNet = file;
			res = net.initFromFile();
			if (!res)
			{
				ERROR_
					return false;
			}
			res = net.setFunc(funcNet);
			if (!res)
			{
				ERROR_
					return false;
			}
			res = net.setParams(moment, u);
			if (!res)
			{
				ERROR_
					return false;
			}
			return net.getState();
		}

		//takes vec of vkernel's indexes
		bool mA()
		{
			if (vinLayer.empty())
			{
				ERROR_
					return false;
			}
			if (krnl.empty())
			{
				ERROR_
					return false;
			}
			bool res = false;
			int current = -1;
			size_t idxK = 0;
			for (auto& x : vfunc)
			{
				switch (x)
				{
				case 10:
					if (krnl[idxK] >= vkernel.size()) ERROR_
					else
					{
						res = convFunc_RGB(current, krnl[idxK++]);
					}
					break;
				case 2:
					res = decreaseX2_RGB(current, max_4);
					break;
				case 3:
					res = decreaseX2_RGB(current, min_4);
					break;
				case 4:
					res = decreaseX2_RGB(current, mid_4);
					break;
				default:
					ERROR_
						break;
				}
				++current;
				//res = saveIm_RGB(static_cast<size_t>(current));
				//res = saveIm_Gray(static_cast<size_t>(current));
			}
			return true;
		}

		bool fullNet_mA(const ::std::vector<::std::vector<double>>& results, const int& ittr, int func(int&), ::std::string subS, ::std::string extImg)
		{
			if (krnl.empty())
			{
				ERROR_
					return false;
			}
			if (!net.getState())
			{
				ERROR_
					return false;
			}

			bool resT = false;
			for (int i = 0; i < ittr; ++i)
			{
				cv::Mat imgT;
				::std::string sfile = inputF + subS + nndx::nts(func(i));
				imgT = cv::imread(sfile + extImg);
				resT = init_image(imgT);
				if (!resT)
				{
					ERROR_
						return false;
				}

				resT = mA();
				if (!resT)
				{
					ERROR_
						return false;
				}

				::std::vector<double> in;
				in.reserve(net.data[0].size() - 1);
				//double localsum = 0;
				for (size_t s = 0; s < vlayer.back().size(); ++s)
				{
					//localsum = 0;
					for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
					{
						for (size_t j = 0; j < vlayer.back()[s][0].size(); ++j)
						{
							in.emplace_back(vlayer.back()[s][i][j].Grayn);
							//localsum += vlayer.back()[s][i][j].Grayn;
							//if (vlayer.back()[s][i][j].Grayn > 1.) ERROR_
						}
					}
					//in.emplace_back(localsum / (vlayer.back()[s].size() * vlayer.back()[s][0].size()));
					//if (localsum / (vlayer.back()[s].size() * vlayer.back()[s][0].size()) > 1.)
						//::std::cout << "localsum = " << localsum / (vlayer.back()[s].size() * vlayer.back()[s][0].size()) << ::std::endl;
				}
				resT = net.fillInput(in);
				if (!resT)
				{
					ERROR_
						return false;
				}
				resT = net.callActivationF();
				if (!resT)
				{
					ERROR_
						return false;
				}

				int temp1 = 0;
				int temp2 = 0;
				double maxTemp = -2.;
				for (auto& x : net.getResults())
				{
					if (x > maxTemp)
					{
						maxTemp = x;
						temp2 = temp1;
					}
					++temp1;
				}
				::std::cout << temp2 << ::std::endl;

				resT = net.callBackProp(results[func(i)]);
				if (!resT)
				{
					ERROR_
						return false;
				}
			}
			resT = net.saveF(outputF + "net.txt");
			if (!resT)
			{
				ERROR_
					return false;
			}
			return true;
		}

		bool fullNet_mA()
		{
			if (vinLayer.empty())
			{
				ERROR_
					return false;
			}
			if (krnl.empty())
			{
				ERROR_
					return false;
			}
			if (!net.getState())
			{
				ERROR_
					return false;
			}

			bool resT = false;
			resT = mA();
			if (!resT)
			{
				ERROR_
					return false;
			}

			::std::vector<double> in;
			in.reserve(net.data[0].size() - 1);
			for (size_t s = 0; s < vlayer.back().size(); ++s)
			{
				for (size_t i = 0; i < vlayer.back()[s].size(); ++i)
				{
					for (size_t j = 0; j < vlayer.back()[s][0].size(); ++j)
					{
						in.emplace_back(vlayer.back()[s][i][j].Grayn);
					}
				}
			}
			resT = net.fillInput(in);
			if (!resT)
			{
				ERROR_
					return false;
			}
			resT = net.callActivationF();
			if (!resT)
			{
				ERROR_
					return false;
			}

			int temp1 = 0;
			for (auto& x : net.getResults())
			{
				if (temp1 == 10) temp1 == 0;
				::std::cout << temp1++ << " " << x << ::std::endl;
			}
			return true;
		}

	private:
		//idx of this func - 2, 3, 4
		bool decreaseX2_RGB(int idxSource, _clr pFunc(_clr, _clr, _clr, _clr))
		{
			if (idxSource == -1)
			{
				if ((vinLayer.size() < 2) || (vinLayer.back().size() < 2))
				{
					ERROR_
						return false;
				}

				unsigned int rows = (vinLayer.size() % 2 == 0) ? static_cast<unsigned int>(vinLayer.size()) : static_cast<unsigned int>(vinLayer.size() - 1);
				unsigned int cols = (vinLayer[0].size() % 2 == 0) ? static_cast<unsigned int>(vinLayer[0].size()) : static_cast<unsigned int>(vinLayer[0].size() - 1);
				vlayer[0][0].clear();
				vlayer[0][0].reserve(static_cast<size_t>(rows * 0.5));
				for (unsigned int y = 0; y < rows; y += 2)
				{
					vlayer[0][0].emplace_back(::std::vector<rgb_T>());
					vlayer[0][0].back().reserve(static_cast<size_t>(cols* 0.5));
					for (unsigned int x = 0; x < cols; x += 2)
					{
						vlayer[0][0].back().emplace_back(
							pFunc(vinLayer[y][x].R, vinLayer[y + 1][x].R, vinLayer[y][x + 1].R, vinLayer[y + 1][x + 1].R),
							pFunc(vinLayer[y][x].G, vinLayer[y + 1][x].G, vinLayer[y][x + 1].G, vinLayer[y + 1][x + 1].G),
							pFunc(vinLayer[y][x].B, vinLayer[y + 1][x].B, vinLayer[y][x + 1].B, vinLayer[y + 1][x + 1].B));
					}
				}
			}
			else
			{
				for (auto& i : vlayer[idxSource])
				{
					if ((i.size() < 2) || (i.back().size() < 2))
					{
						ERROR_
							return false;
					}
				}

				for (size_t i = 0; i < vlayer[idxSource].size(); ++i)
				{
					unsigned int rows = (vlayer[idxSource][i].size() % 2 == 0) ? static_cast<unsigned int>(vlayer[idxSource][i].size()) : static_cast<unsigned int>(vlayer[idxSource][i].size() - 1);
					unsigned int cols = (vlayer[idxSource][i][0].size() % 2 == 0) ? static_cast<unsigned int>(vlayer[idxSource][i][0].size()) : static_cast<unsigned int>(vlayer[idxSource][i][0].size() - 1);
					vlayer[idxSource + 1][i].clear();
					vlayer[idxSource + 1][i].reserve(static_cast<size_t>(rows * 0.5));
					for (unsigned int y = 0; y < rows; y += 2)
					{
						vlayer[idxSource + 1][i].emplace_back(::std::vector<rgb_T>());
						vlayer[idxSource + 1][i].back().reserve(static_cast<size_t>(cols * 0.5));
						for (unsigned int x = 0; x < cols; x += 2)
						{
							vlayer[idxSource + 1][i].back().emplace_back(
								pFunc(vlayer[idxSource][i][y][x].R, vlayer[idxSource][i][y + 1][x].R, vlayer[idxSource][i][y][x + 1].R, vlayer[idxSource][i][y + 1][x + 1].R),
								pFunc(vlayer[idxSource][i][y][x].G, vlayer[idxSource][i][y + 1][x].G, vlayer[idxSource][i][y][x + 1].G, vlayer[idxSource][i][y + 1][x + 1].G),
								pFunc(vlayer[idxSource][i][y][x].B, vlayer[idxSource][i][y + 1][x].B, vlayer[idxSource][i][y][x + 1].B, vlayer[idxSource][i][y + 1][x + 1].B));
						}
					}
				}
			}
			return true;
		}

		//idx of this func - 10
		bool convFunc_RGB(int idxSource, size_t idxKernel)
		{
			auto szKrnl = vkernel[idxKernel].size();
			if (idxSource == -1)
			{
				for (size_t i = 0; i < szKrnl; ++i)
				{
					if ((vinLayer.size() < vkernel[idxKernel][i].size()) || (vinLayer[0].size() < vkernel[idxKernel][i][0].size()))
					{
						ERROR_
							return false;
					}
				}

				for (size_t i = 0; i < szKrnl; ++i)
				{
					vlayer[0][i].clear();
					int localx = 1 + static_cast<int>(vinLayer[0].size() - vkernel[idxKernel][i][0].size());
					int localy = 1 + static_cast<int>(vinLayer.size() - vkernel[idxKernel][i].size());
					vlayer[0][i].reserve(localy);
					for (size_t y = 0; y < localy; ++y)
					{
						vlayer[0][i].emplace_back(::std::vector<rgb_T>());
						vlayer[0][i].back().reserve(localx);
						for (size_t x = 0; x < localx; ++x)
						{
							double sumR = 0.;
							double sumG = 0.;
							double sumB = 0.;
							for (size_t ky = 0; ky < vkernel[idxKernel][i].size(); ++ky)
							{
								for (size_t kx = 0; kx < vkernel[idxKernel][i][0].size(); ++kx)
								{
									sumR += vkernel[idxKernel][i][ky][kx] * vinLayer[y + ky][x + kx].Rn;
									sumG += vkernel[idxKernel][i][ky][kx] * vinLayer[y + ky][x + kx].Gn;
									sumB += vkernel[idxKernel][i][ky][kx] * vinLayer[y + ky][x + kx].Bn;
								}
							}
							vlayer[0][i].back().emplace_back(ReLU(sumR), ReLU(sumG), ReLU(sumB));
						}
					}
				}
			}
			else
			{
				for (size_t i = 0; i < szKrnl; ++i)
				{
					if ((vlayer[idxSource][i].size() < vkernel[idxKernel][i].size()) || (vlayer[idxSource][i][0].size() < vkernel[idxKernel][i][0].size()))
					{
						ERROR_
							return false;
					}
				}

				for (size_t j = 0; j < vlayer[idxSource].size(); ++j)
				{
					for (size_t i = 0; i < szKrnl; ++i)
					{
						vlayer[idxSource + 1][szKrnl * j + i].clear();
						unsigned int localx = 1 + static_cast<int>(vlayer[idxSource][j][0].size() - vkernel[idxKernel][i][0].size());
						unsigned int localy = 1 + static_cast<int>(vlayer[idxSource][j].size() - vkernel[idxKernel][i].size());
						vlayer[idxSource + 1][szKrnl * j + i].reserve(localy);
						for (unsigned int y = 0; y < localy; ++y)
						{
							vlayer[idxSource + 1][szKrnl * j + i].emplace_back(::std::vector<rgb_T>());
							vlayer[idxSource + 1][szKrnl * j + i].back().reserve(localx);
							for (unsigned int x = 0; x < localx; ++x)
							{
								double sumR = 0.;
								double sumG = 0.;
								double sumB = 0.;
								for (size_t ky = 0; ky < vkernel[idxKernel][i].size(); ++ky)
								{
									for (size_t kx = 0; kx < vkernel[idxKernel][i][0].size(); ++kx)
									{
										sumR += vkernel[idxKernel][i][ky][kx] * vlayer[idxSource][j][y + ky][x + kx].Rn;
										sumG += vkernel[idxKernel][i][ky][kx] * vlayer[idxSource][j][y + ky][x + kx].Gn;
										sumB += vkernel[idxKernel][i][ky][kx] * vlayer[idxSource][j][y + ky][x + kx].Bn;
									}
								}
								vlayer[idxSource + 1][szKrnl * j + i].back().emplace_back(ReLU(sumR), ReLU(sumG), ReLU(sumB));
							}
						}
					}
				}
			}
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
				unsigned int locY = static_cast<unsigned int>(vlayer[idx][cur].size());
				unsigned int locX = static_cast<unsigned int>(vlayer[idx][cur][0].size());
				cv::Mat output(locY, locX, CV_8UC3, cv::Scalar(0, 0, 0));
				for (unsigned int y = 0; y < locY; ++y)
				{
					for (unsigned int x = 0; x < locX; ++x)
					{
						output.at<cv::Vec3b>(cv::Point(x, y))[2] = vlayer[idx][cur][y][x].R;
						output.at<cv::Vec3b>(cv::Point(x, y))[1] = vlayer[idx][cur][y][x].G;
						output.at<cv::Vec3b>(cv::Point(x, y))[0] = vlayer[idx][cur][y][x].B;
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
				unsigned int locY = static_cast<unsigned int>(vlayer[idx][cur].size());
				unsigned int locX = static_cast<unsigned int>(vlayer[idx][cur][0].size());
				cv::Mat output(locY, locX, CV_8UC1, cv::Scalar(0));
				for (unsigned int y = 0; y < locY; ++y)
				{
					for (unsigned int x = 0; x < locX; ++x)
					{
						output.at<_clr>(cv::Point(x, y)) = vlayer[idx][cur][y][x].Gray;
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

				unsigned int locY = static_cast<unsigned int>(vlayer[idx][cur].size());
				unsigned int locX = static_cast<unsigned int>(vlayer[idx][cur][0].size());
				write << locX << ' ' << locY << ::std::endl;
				for (unsigned int y = 0; y < locY; ++y)
				{
					for (unsigned int x = 0; x < locX; ++x)
					{
						write << vlayer[idx][cur][y][x].R << ' ';
						write << vlayer[idx][cur][y][x].G << ' ';
						write << vlayer[idx][cur][y][x].B << ::std::endl;
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

				unsigned int locY = static_cast<unsigned int>(vlayer[idx][cur].size());
				unsigned int locX = static_cast<unsigned int>(vlayer[idx][cur][0].size());
				write << locX << ' ' << locY << ::std::endl;
				for (unsigned int y = 0; y < locY; ++y)
				{
					for (unsigned int x = 0; x < locX; ++x)
					{
						write << vlayer[idx][cur][y][x].Grayn << ' ';
					}
					write << ::std::endl;
				}
				write << _filend;
				write.close();
			}
			return true;
		}
	};

	_clr max_4(_clr f1, _clr f2, _clr f3, _clr f4)
	{
		_clr max = f1 > f2 ? f1 : f2;
		max = max > f3 ? max : f3;
		max = max > f4 ? max : f4;
		return max;
	}

	_clr min_4(_clr f1, _clr f2, _clr f3, _clr f4)
	{
		_clr min = f1 > f2 ? f2 : f1;
		min = min > f3 ? f3 : min;
		min = min > f4 ? f4 : min;
		return static_cast<_clr>(min);
	}

	_clr mid_4(_clr f1, _clr f2, _clr f3, _clr f4)
	{
		return static_cast<_clr>((f1 + f2 + f3 + f4) / 4);
	}

	inline double ReLU(const double& x)
	{
		return (x > 0) ? x : 0;
	}

	::std::string nts(const int& example)
	{
		int a = example;
		::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);

		ss << a;

		::std::string s = ss.str();

		return s;
	}

	::std::string nts(const size_t& example)
	{
		size_t a = example;
		::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);

		ss << a;

		::std::string s = ss.str();

		return s;
	}
}