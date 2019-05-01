#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "opencv2\opencv.hpp"

#ifndef def_ERR_FL_
#define def_ERR_FL_
#define def_XTT_S(x) #x
#define def_XTT_S_(x) def_XTT_S(x)
#define def_XTT_S__LINE__ def_XTT_S_(__LINE__)
#define def__FILELINE (__FILE__  " line " def_XTT_S__LINE__)
#define ERROR_ ::std::cout << "Error - " << def__FILELINE << ::std::endl;
#endif

//#define _mainDEBUG

#define _u8 uint8_t

namespace nndx
{
	_u8 max_4(_u8, _u8, _u8, _u8);
	_u8 min_4(_u8, _u8, _u8, _u8);
	_u8 mid_4(_u8, _u8, _u8, _u8);
	::std::string nts(int);
	::std::string nts(size_t);

#pragma pack(push, 1)
	struct rgb_T
	{
		_u8 R;
		_u8 G;
		_u8 B;
		double Rn;
		double Gn;
		double Bn;

		rgb_T() : R(0), G(0), B(0), Rn(0.), Gn(0.), Bn(0.) {}

		rgb_T(_u8 r, _u8 g, _u8 b) : R(r), G(g), B(b), Rn(r / 255.), Gn(g / 255.), Bn(b / 255.) {}

		rgb_T(double r, double g, double b) : Rn(r), Gn(g), Bn(b)
		{
			if (r > 255.)
			{
				R = 255;
				Rn = 1.;
			}
			else if (r < 0)
			{
				R = 0;
				Rn = 0.;
			}
			else
			{
				R = static_cast<_u8>(r * 255);
#ifdef _mainDEBUG
				::std::wcout << r << " to " << R << ::std::endl;
#endif
			}

			if (g > 255.)
			{
				G = 255;
				Gn = 1.;
			}
			else if (g < 0)
			{
				G = 0;
				Gn = 0.;
			}
			else
			{
				G = static_cast<_u8>(g * 255);
#ifdef _mainDEBUG
				::std::wcout << g << " to " << G << ::std::endl;
#endif
			}

			if (b > 255.)
			{
				B = 255;
				Bn = 1.;
			}
			else if (b < 0)
			{
				B = 0;
				Bn = 0.;
			}
			else
			{
				B = static_cast<_u8>(b * 255);
#ifdef _mainDEBUG
				::std::wcout << b << " to " << B << ::std::endl;
#endif
			}
		}

		rgb_T(const cv::Vec3b& v)
		{
			B = v[0];
			G = v[1];
			R = v[2];
			Bn = v[0] / 255.;
			Gn = v[1] / 255.;
			Rn = v[2] / 255.;
		}
	};
#pragma pack(pop)

	class CNN
	{
	public:
		using image = ::std::vector<::std::vector<rgb_T>>;
		unsigned int nConv;
		image vinLayer;
		::std::vector<::std::vector<::std::vector<::std::vector<double>>>> vkernel;
		::std::vector<::std::vector<image>> vlayer;
		::std::vector<int> vfunc;

		CNN() : nConv(0) {}

		CNN(CNN&& data)
		{
			this->nConv = ::std::forward<decltype(data.nConv)>(data.nConv);
			this->vinLayer = ::std::forward<decltype(data.vinLayer)>(data.vinLayer);
			this->vkernel = ::std::forward<decltype(data.vkernel)>(data.vkernel);
			this->vlayer = ::std::forward<decltype(data.vlayer)>(data.vlayer);
			this->vfunc = ::std::forward<decltype(data.vfunc)>(data.vfunc);
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

		//size (vfunc.size(), vlayer.size()), vlayer[0].size(), vfunc[0], ...
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
					int temp = va_arg(args, int);
					vlayer.back().reserve(temp);
					vfunc.emplace_back(va_arg(args, int));
					if (vfunc.back() == 1)	++nConv;
					for (int j = 0; j < temp; ++j)
					{
						vlayer.back().emplace_back(image());
					}
				}
				va_end(args);
			}
			else
			{
				ERROR_
			}
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
		bool init_image(unsigned int X, unsigned int Y, _u8 R, _u8 G, _u8 B)
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
		}

		bool init_kernel(size_t idx_, size_t idxKernel, unsigned int sizex, unsigned int sizey, ...)
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

			::std::string sfile = "data/" + nts(idx_);
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
			if (sfile != "endOFfile")
			{
				ERROR_
					return false;
			}
			read.close();
			return true;
		}

		//vlayer must be filled! size = vlayer.size()
		bool init_vfunc(unsigned int size, ...)
		{
			if ((size < 1) || (size != vlayer.size()))
			{
				ERROR_
					return false;
			}
			if (!vfunc.empty())
			{
				vfunc.clear();
				nConv = 0;
			}
			va_list args;
			va_start(args, size);
			vfunc.reserve(size);
			for (unsigned int i = 0; i < size; ++i)
			{
				vfunc.emplace_back(va_arg(args, int));
				if (vfunc.back() == 1)	++nConv;
			}
			va_end(args);
			return true;
		}

		//size (vfunc.size(), vlayer.size()), vlayer[0].size(), vfunc[0], ...
		bool init_vfuncEx(unsigned int size, ...)
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
			if (size > 0)
			{
				va_list args;
				va_start(args, size);
				vlayer.reserve(size);
				vfunc.reserve(size);
				for (unsigned int i = 0; i < size; ++i)
				{
					vlayer.emplace_back(::std::vector<image>());
					int temp = va_arg(args, int);
					vlayer.back().reserve(temp);
					vfunc.emplace_back(va_arg(args, int));
					if (vfunc.back() == 1)	++nConv;
					for (int j = 0; j < temp; ++j)
					{
						vlayer.back().emplace_back(image());
					}
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

		bool mA(const ::std::vector<size_t>& Kernel)
		{
			if (Kernel.size() != nConv)
			{
				ERROR_
					return false;
			}
			if (vinLayer.empty())
			{
				ERROR_
					return false;
			}
			if (vfunc.empty())
			{
				ERROR_
					return false;
			}
			int current = -1;
			size_t idxK = 0;
			bool res = false;
			for (auto& x : vfunc)
			{
				switch (x)
				{
				case 1:
					res = convFunc(current, Kernel[idxK]);
					++idxK;
#ifdef _mainDEBUG
					::std::cout << "1 - " << (res ? "true" : "false") << ::std::endl;
#endif
					break;
				case 2:
					res = decrease_X2(current, max_4);
#ifdef _mainDEBUG
					::std::cout << "2 - " << (res ? "true" : "false") << ::std::endl;
#endif
					break;
				case 3:
					res = decrease_X2(current, min_4);
#ifdef _mainDEBUG
					::std::cout << "3 - " << (res ? "true" : "false") << ::std::endl;
#endif
					break;
				case 4:
					res = decrease_X2(current, mid_4);
#ifdef _mainDEBUG
					::std::cout << "4 - " << (res ? "true" : "false") << ::std::endl;
#endif
					break;
				}
				++current;
				res = saveIm(static_cast<size_t>(current));
#ifdef _mainDEBUG
				::std::cout << "save - " << (res ? "true" : "false") << ::std::endl;
#endif
				res = saveDat_BW(static_cast<size_t>(current));
#ifdef _mainDEBUG
				::std::cout << "save - " << (res ? "true" : "false") << ::std::endl;
#endif
			}
			return true;
		}

	private:
		//idx of this func - 2, 3, 4
		bool decrease_X2(int idxSource, _u8(*pFunc)(_u8, _u8, _u8, _u8))
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

				for (int i = 1; i < vlayer[0].size(); ++i)
				{
					vlayer[0][i] = vlayer[0][0];
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

		//idx of this func - 1
		bool convFunc(int idxSource, size_t idxKernel)
		{
			if (idxSource == -1)
			{
				for (size_t i = 0; i < vkernel[idxKernel].size(); ++i)
				{
					if ((vinLayer.size() < vkernel[idxKernel][i].size()) || (vinLayer[0].size() < vkernel[idxKernel][i][0].size()))
					{
						ERROR_
							return false;
					}
				}

				for (size_t i = 0; i < vkernel[idxKernel].size(); ++i)
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
							double sumR = 0;
							double sumG = 0;
							double sumB = 0;
							for (size_t ky = 0; ky < vkernel[idxKernel][i].size(); ++ky)
							{
								for (size_t kx = 0; kx < vkernel[idxKernel][i][0].size(); ++kx)
								{
									sumR += vkernel[idxKernel][i][ky][kx] * vinLayer[y + ky][x + kx].Rn;
									sumG += vkernel[idxKernel][i][ky][kx] * vinLayer[y + ky][x + kx].Gn;
									sumB += vkernel[idxKernel][i][ky][kx] * vinLayer[y + ky][x + kx].Bn;
								}
							}
							vlayer[0][i].back().emplace_back(sumR, sumG, sumB);
						}
					}
				}
			}
			else
			{
				if (vkernel[idxKernel].size() != vlayer[idxSource].size())
				{
					ERROR_
						return false;
				}
				for (size_t i = 0; i < vkernel[idxKernel].size(); ++i)
				{
					if ((vlayer[idxSource][i].size() < vkernel[idxKernel][i].size()) || (vlayer[idxSource][i][0].size() < vkernel[idxKernel][i][0].size()))
					{
						ERROR_
							return false;
					}
				}

				for (size_t i = 0; i < vkernel[idxKernel].size(); ++i)
				{
					vlayer[idxSource + 1][i].clear();
					unsigned int localx = 1 + static_cast<int>(vlayer[idxSource][i][0].size() - vkernel[idxKernel][i][0].size());
					unsigned int localy = 1 + static_cast<int>(vlayer[idxSource][i].size() - vkernel[idxKernel][i].size());
					vlayer[idxSource + 1][i].reserve(localy);
					for (unsigned int y = 0; y < localy; ++y)
					{
						vlayer[idxSource + 1][i].emplace_back(::std::vector<rgb_T>());
						vlayer[idxSource + 1][i].back().reserve(localx);
						for (unsigned int x = 0; x < localx; ++x)
						{
							double sumR = 0;
							double sumG = 0;
							double sumB = 0;
							for (size_t ky = 0; ky < vkernel[idxKernel][i].size(); ++ky)
							{
								for (size_t kx = 0; kx < vkernel[idxKernel][i][0].size(); ++kx)
								{
									sumR += vkernel[idxKernel][i][ky][kx] * vlayer[idxSource][i][y + ky][x + kx].Rn;
									sumG += vkernel[idxKernel][i][ky][kx] * vlayer[idxSource][i][y + ky][x + kx].Gn;
									sumB += vkernel[idxKernel][i][ky][kx] * vlayer[idxSource][i][y + ky][x + kx].Bn;
								}
							}
							vlayer[idxSource + 1][i].back().emplace_back(sumR, sumG, sumB);
						}
					}
				}
			}
			return true;
		}

		bool saveIm(size_t idx)
		{
			if (idx >= vlayer.size())
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
				::std::string s = "output/output_" + nts(idx);
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
			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				::std::string s = "output/output_" + nts(idx);
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
				write << "endOFfile";
				write.close();
			}
			return true;
		}

		bool saveDat_BW(size_t idx)
		{
			if (idx >= vlayer.size())
			{
				ERROR_
					return false;
			}
			for (size_t cur = 0; cur < vlayer[idx].size(); ++cur)
			{
				::std::string s = "output/output_" + nts(idx);
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
				unsigned short localsum = 0;

				write << locX << ' ' << locY << ::std::endl;
				for (unsigned int y = 0; y < locY; ++y)
				{
					for (unsigned int x = 0; x < locX; ++x)
					{
						localsum = 0;
						localsum += vlayer[idx][cur][y][x].R;
						localsum += vlayer[idx][cur][y][x].G;
						localsum += vlayer[idx][cur][y][x].B;
						if (localsum >= 128)
						{
							write << "1.0" << ' ';
						}
						else
						{
							write << "0.0" << ' ';
						}
					}
					write << ::std::endl;
				}
				write << "endOFfile";
				write.close();
			}
			return true;
		}
	};

	_u8 max_4(_u8 f1, _u8 f2, _u8 f3, _u8 f4)
	{
		_u8 max = f1 > f2 ? f1 : f2;
		max = max > f3 ? max : f3;
		max = max > f4 ? max : f4;
		return max;
	}

	_u8 min_4(_u8 f1, _u8 f2, _u8 f3, _u8 f4)
	{
		_u8 min = f1 > f2 ? f2 : f1;
		min = min > f3 ? f3 : min;
		min = min > f4 ? f4 : min;
		return static_cast<_u8>(min);
	}

	_u8 mid_4(_u8 f1, _u8 f2, _u8 f3, _u8 f4)
	{
		return static_cast<_u8>((f1 + f2 + f3 + f4) / 4);
	}

	::std::string nts(int example)
	{
		int a = example;
		::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);

		ss << a;

		::std::string s = ss.str();

		return s;
	}

	::std::string nts(size_t example)
	{
		size_t a = example;
		::std::stringstream ss(::std::stringstream::in | ::std::stringstream::out);

		ss << a;

		::std::string s = ss.str();

		return s;
	}
}