//Autor - -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release [v0.1] on GitHub 01.12.2018
//Actual version 2.6[Uni-v]
//This project is the embodiment of a neural network with a genetic algorithm to solve the problem of passing a random maze

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include "SFML/Graphics.hpp"

template <typename T>
T def_FI(::std::ifstream&);

#include "NN.h"

enum idefinitions : int // in replacement DEFINE
{
	def_WIN_X = 1000,
	def_WIN_Y = 400,
	def__SIZE_ = 12,
	def__NUM_ACTIVE_WALL = 7,
	def_STEPA_ = (def__NUM_ACTIVE_WALL + 2),
	def_TEXTURE_OBJ_X = 20,
	def_TEXTURE_OBJ_Y = 20,
	def_TEXTURE_WLL_X = 20,
	def_TEXTURE_WLL_Y = 20,
	def_SIZE_VECTOR_WALL = (def_WIN_X / def_TEXTURE_OBJ_X) + 1,
	def_POSX = 410,
	def_POSX_WALL = (def_POSX + 60),
	def_POSY_WALL = 140,
	def_POSY = (def_POSY_WALL + 80),
	def_SZ_TOPOLOGY = 3,
	def_PRE_DISTANCE = 200
};

constexpr double def_KF = 0.5; // in replacement DEFINE
constexpr const char* def_FILEIMAGE = "files/image.png"; // in replacement DEFINE

HCRYPTPROV hProv; // for random number generator(NN.h, Object.h, main.cpp)

#include "Object.h"

void radixSort(::std::vector<Object*>&);
void mA_gen(::std::vector<Object*>&, int);
void mT(::std::vector<nndx::dataW>&, size_t, size_t);
void mainA(sf::RenderWindow&, sf::View&, ::std::vector<Object>&, ::std::vector<::std::vector<Wall>>&, ::std::vector<int>&, sf::Texture&, volatile ::std::atomic_uint&,
	volatile ::std::atomic_bool&, volatile ::std::atomic_bool&, volatile ::std::atomic_bool&, volatile ::std::atomic_bool&);

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	sf::RenderWindow win({ idefinitions::def_WIN_X, idefinitions::def_WIN_Y }, "NN");
	win.setVerticalSyncEnabled(true);
	win.setActive(false);

	/* default filename with settings - inf.cfg*/
	::std::ifstream _tempRead;
	_tempRead.open("inf.cfg");
	if (!_tempRead.is_open())
	{
		ERROR_
			return 1;
	}
	volatile ::std::atomic_uint TIMESET_ = 0;
	win.setFramerateLimit(def_FI<unsigned int>(_tempRead));
	TIMESET_.store(def_FI<unsigned int>(_tempRead));
	_tempRead.close();

	sf::Image image_;
	image_.loadFromFile("files/image.png");
	sf::Texture t_;
	t_.loadFromImage(image_);

	sf::View view;
	view.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(idefinitions::def_WIN_X), static_cast<float>(idefinitions::def_WIN_Y)));

	::std::vector<Object> v_Obj_;
	::std::vector<::std::vector<Wall>> v_Wll_;
	::std::vector<int> target_;
	v_Obj_.reserve(idefinitions::def__SIZE_);
	v_Wll_.reserve(idefinitions::def_SIZE_VECTOR_WALL);
	target_.reserve(idefinitions::def_SIZE_VECTOR_WALL);

	volatile ::std::atomic_bool isOpen = true;
	volatile ::std::atomic_bool runA = false;
	volatile ::std::atomic_bool newA = false;
	volatile ::std::atomic_bool auto_move = true;

	if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		if (GetLastError() == NTE_BAD_KEYSET)
			if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				ERROR_
					system("pause");
				return 0;
			}

	::std::thread mainThread(mainA, ::std::ref(win), ::std::ref(view), ::std::ref(v_Obj_), ::std::ref(v_Wll_), ::std::ref(target_), ::std::ref(t_), ::std::ref(TIMESET_), ::std::ref(isOpen),
		::std::ref(runA), ::std::ref(newA), ::std::ref(auto_move));

	while (isOpen.load())
	{
		sf::Event event;
		while (win.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				isOpen = false;
				runA = true;
				break;
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
				{
					auto_move = !auto_move;
					runA = false;
					newA = false;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				{
					ShowWindow(GetConsoleWindow(), SW_SHOW);

					int _temp_int = 0;
					::std::cout << "TIME(0 - 9): ";
					::std::cin >> _temp_int;

					ShowWindow(GetConsoleWindow(), SW_HIDE);
					system("cls");

					_temp_int = abs(_temp_int);
					if (_temp_int == 0)
					{
						TIMESET_ = 0xFFFFFFFF;
					}
					else
					{
						TIMESET_.store(_temp_int * 1000);
					}
				}
				if (!auto_move.load())
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					{
						runA = true;
					}
					else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
					{
						newA = true;
						runA = true;
					}
				}
				else
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
					{
						win.setFramerateLimit(1);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
					{
						win.setFramerateLimit(2);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
					{
						win.setFramerateLimit(5);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
					{
						win.setFramerateLimit(60);
					}
				}
			}
			else if (event.type == sf::Event::Resized)
			{
				view.reset(sf::FloatRect(0.f, 0.f, static_cast<float>(win.getSize().x), static_cast<float>(win.getSize().y)));
			}
		}
	}

	mainThread.join();
	CryptReleaseContext(hProv, 0);
	win.setActive(true);
	win.close();

	for (int i = 0; i < idefinitions::def__SIZE_; ++i)
	{
		::std::string s = "output/net_";
		char ch;
		if (i < 10)
		{
			ch = '0' + i;
			s += ch;
			s += ".txt";
		}
		else
		{
			s += '1';
			ch = '0' + (i % 10);
			s += ch;
			s += ".txt";
		}
		v_Obj_[i].net.saveF(::std::move(s));
	}

	::std::ofstream out("output/log.txt");
	for (int i = 0; i < idefinitions::def__SIZE_; ++i)
	{
		out << "net_" << i << " - " << v_Obj_[i].score << ::std::endl;
	}
	out.close();

	return 0;
}

void mainA
(
	sf::RenderWindow& win,
	sf::View& view,
	::std::vector<Object>& v_Obj,
	::std::vector<::std::vector<Wall>>& v_Wll,
	::std::vector<int>& target,
	sf::Texture& t,
	volatile ::std::atomic_uint& TIMESET,
	volatile ::std::atomic_bool& is_Open,
	volatile ::std::atomic_bool& run_,
	volatile ::std::atomic_bool& newA_,
	volatile ::std::atomic_bool& auto_move
)
{
	win.setActive(true);

	for (int i = 0; i < idefinitions::def__SIZE_; ++i)
	{
		v_Obj.emplace_back(t, nndx::dy_tpl(idefinitions::def_SZ_TOPOLOGY, 2, 3, 2), i);
		if (!v_Obj.back().net.getState()) ERROR_
	}

	int tmpy = nndx::randT(hProv) % idefinitions::def__NUM_ACTIVE_WALL;
	int next = nndx::randT(hProv) % idefinitions::def__NUM_ACTIVE_WALL;
	float wposx = static_cast<float>(idefinitions::def_POSX_WALL);

	target.emplace_back(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + tmpy * idefinitions::def_TEXTURE_WLL_Y);
	v_Wll.emplace_back(::std::vector<Wall>());
	v_Wll.back().reserve(idefinitions::def__NUM_ACTIVE_WALL + 2);
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL));
	for (int i = 0; i < idefinitions::def__NUM_ACTIVE_WALL; ++i)
	{
		if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + i * idefinitions::def_TEXTURE_WLL_Y));
	}
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + idefinitions::def__NUM_ACTIVE_WALL * idefinitions::def_TEXTURE_WLL_Y));
	tmpy = next;
	wposx += idefinitions::def_TEXTURE_WLL_X;

	size_t bufsizeO = idefinitions::def__SIZE_;
	TimeGet tm;

	int numITER = 0;
	::std::ofstream endP("output/endpoint_log.txt");
	if (!endP.is_open())
	{
		::std::cout << "error of opening file - endpoint_log.txt" << ::std::endl;
		ERROR_
		system("pause");
	}

	while (is_Open.load())
	{
		if (!auto_move.load())
		{
			while (!run_.load())
			{
				if (auto_move.load())
					break;
			}
			run_ = false;
		}

		::std::vector<Object*> _ptr;
		for (int i = 0; i < idefinitions::def__SIZE_; ++i)
		{
			if (v_Obj[i].life)	_ptr.emplace_back(&v_Obj[i]);
		}

		int minposx_obj = 0x7FFFFFFF;
		int maxposx_obj = -0x7FFFFFFF;
		for (size_t i = 0; i < _ptr.size(); ++i)
		{
			if (minposx_obj > _ptr[i]->obj.getPosition().x)	minposx_obj = static_cast<int>(_ptr[i]->obj.getPosition().x);
			if (maxposx_obj < _ptr[i]->obj.getPosition().x)	maxposx_obj = static_cast<int>(_ptr[i]->obj.getPosition().x);
		}

		if (_ptr.size())
		{
			if (v_Wll[0][0].wall.getPosition().x < minposx_obj)
			{
				for (auto i = 0; i < static_cast<int>((minposx_obj - v_Wll[0][0].wall.getPosition().x) / idefinitions::def_TEXTURE_WLL_X) - 1; ++i)
				{
					::std::vector<::std::vector<Wall>>::iterator it = v_Wll.begin();
					it = v_Wll.erase(it);
					::std::vector<int>::iterator itTg = target.begin();
					itTg = target.erase(itTg);
				}
			}
			if (static_cast<int>(wposx) - maxposx_obj < idefinitions::def_PRE_DISTANCE)
			{
				v_Wll.emplace_back(::std::vector<Wall>());
				v_Wll.back().reserve(idefinitions::def__NUM_ACTIVE_WALL + 2);
				v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL));
				next = nndx::randT(hProv) % idefinitions::def__NUM_ACTIVE_WALL;
				target.emplace_back(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + tmpy * idefinitions::def_TEXTURE_WLL_Y);
				for (int i = 0; i < idefinitions::def__NUM_ACTIVE_WALL; ++i)
				{
					if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))
						v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + i * idefinitions::def_TEXTURE_WLL_Y));
				}
				v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + idefinitions::def__NUM_ACTIVE_WALL * idefinitions::def_TEXTURE_WLL_Y));
				tmpy = next;
				wposx += idefinitions::def_TEXTURE_WLL_X;
			}
		}

		if (_ptr.size() && (((!auto_move.load()) && (!newA_.load()))
			|| (auto_move.load() && (((bufsizeO == _ptr.size()) ? *tm < TIMESET.load() : tm.restart()) || (TIMESET.load() == 0xFFFFFFFF)))))
		{
			bufsizeO = _ptr.size();
			for (size_t i = 0; i < _ptr.size(); ++i)
			{
				for (size_t j = 0; j < target.size(); ++j)
				{
					if (v_Wll[j][0].wall.getPosition().x > _ptr[i]->obj.getPosition().x)
					{
						double toY = target[j] - _ptr[i]->obj.getPosition().y;
						::std::vector<double> packet;
						packet.reserve(2);
						packet.emplace_back(1);
						packet.emplace_back(toY * 0.01);
						bool res = _ptr[i]->net.SPECmA(packet);
						if (res)
						{
							_ptr[i]->mA(std::move(_ptr[i]->net.getResults()));
						}
						else
						{
							ERROR_
						}
						break;
					}
				}
			}

			for (size_t i = 0; i < _ptr.size(); ++i)
			{
				for (size_t j = 0; j < v_Wll.size(); ++j)
				{
					for (size_t k = 0; k < v_Wll[j].size(); ++k)
					{
						if (_ptr[i]->obj.getGlobalBounds().intersects(v_Wll[j][k].wall.getGlobalBounds()))	_ptr[i]->life = false;
					}
				}
				if (_ptr[i]->stepA < 1)	_ptr[i]->life = false;
			}
		}
		else
		{
			newA_ = false;

			for (int i = 0; i < idefinitions::def__SIZE_; ++i)
			{
				_ptr.emplace_back(&v_Obj[i]);
			}
			radixSort(_ptr);
			endP << _ptr.back()->score << ::std::endl;
			++numITER;
			mA_gen(_ptr, idefinitions::def_SZ_TOPOLOGY);

			for (size_t i = 0; i < v_Obj.size(); ++i)
			{
				v_Obj[i].obj.setPosition(static_cast<float>(idefinitions::def_POSX), static_cast<float>(idefinitions::def_POSY));
				v_Obj[i].life = true;
				v_Obj[i].score = 0;
				v_Obj[i].stepA = idefinitions::def_STEPA_;
			}
			maxposx_obj = idefinitions::def_POSX;
			v_Wll.clear();
			target.clear();
			tmpy = nndx::randT(hProv) % idefinitions::def__NUM_ACTIVE_WALL;
			wposx = static_cast<float>(idefinitions::def_POSX_WALL);
			target.emplace_back(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + tmpy * idefinitions::def_TEXTURE_WLL_Y);
			v_Wll.emplace_back(::std::vector<Wall>());
			v_Wll.back().reserve(idefinitions::def__NUM_ACTIVE_WALL + 2);
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL));
			for (int i = 0; i < idefinitions::def__NUM_ACTIVE_WALL; ++i)
			{
				if (i != tmpy)	v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + i * idefinitions::def_TEXTURE_WLL_Y));
			}
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + idefinitions::def__NUM_ACTIVE_WALL * idefinitions::def_TEXTURE_WLL_Y));
			wposx += idefinitions::def_TEXTURE_WLL_X;

			bufsizeO = idefinitions::def__SIZE_;
			tm.restart();
		}

		view.setCenter(static_cast<float>(maxposx_obj), 250.0f);

		win.clear();
		win.setView(view);
		for (size_t i = 0; i < v_Wll.size(); ++i)
		{
			for (size_t j = 0; j < v_Wll[i].size(); ++j)
			{
				win.draw(v_Wll[i][j].wall);
			}
		}
		for (size_t i = 0; i < _ptr.size(); ++i)
		{
			win.draw(_ptr[i]->obj);
		}
		win.display();
	}

	endP << numITER;
	endP.close();
	win.setActive(false);
}

template <typename T>
T def_FI(::std::ifstream& read_cont__)
{
	T temp_int__;
	read_cont__ >> temp_int__;
	return temp_int__;
}

void radixSort(::std::vector<Object*>& a)
{
	for (int p = 0; p < 4; ++p)
	{
		::std::vector<int> c(1 << 8, 0);
		::std::vector<Object*> b = a;

		for (size_t i = 0; i < a.size(); ++i)
			c[ (a[i]->score >> 8 * p) & ((1 << 8) - 1) ]++;
		for (size_t i = 1; i < 1 << 8; ++i)
			c[i] += c[i - 1];
		for (int i = static_cast<int>(a.size() - 1); i >= 0; --i)
			b[--c[(a[i]->score >> 8 * p) & ((1 << 8) - 1)]] = a[i];
		a = b;
	}
}

void mA_gen(::std::vector<Object*>& obj, int tg)
{
	size_t index;
	obj[8]->net.weight = obj[10]->net.weight;
	index = nndx::randT(hProv) % obj[10]->net.weight.size();
	mT(obj[8]->net.weight, index, nndx::randT(hProv) % obj[10]->net.weight[index].size());

	obj[7]->net.weight = obj[9]->net.weight;
	index = nndx::randT(hProv) % obj[9]->net.weight.size();
	mT(obj[7]->net.weight, index, nndx::randT(hProv) % obj[9]->net.weight[index].size());

	mT(obj[6]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());
	mT(obj[5]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());
	mT(obj[4]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());

	for (size_t i = 0; i < tg - 2; ++i)
	{
		obj[3]->net.weight[i] = obj[11]->net.weight[i];
	}
	if(nndx::randT(hProv) % 2 == 1)	obj[3]->net.weight[tg - 2] = obj[10]->net.weight[tg - 2];
	else	obj[3]->net.weight[tg - 2] = obj[9]->net.weight[tg - 2];
	if (nndx::randT(hProv) % 3 == 2)
	{
		index = nndx::randT(hProv) % obj[3]->net.weight.size();
		mT(obj[3]->net.weight, index, nndx::randT(hProv) % obj[3]->net.weight[index].size());
	}

	for (size_t i = 0; i < tg - 2; ++i)
	{
		obj[2]->net.weight[i] = obj[10]->net.weight[i];
	}
	obj[2]->net.weight[tg - 2] = obj[9]->net.weight[tg - 2];
	if (nndx::randT(hProv) % 3 == 2)
	{
		index = nndx::randT(hProv) % obj[2]->net.weight.size();
		mT(obj[2]->net.weight, index, nndx::randT(hProv) % obj[2]->net.weight[index].size());
	}

	mT(obj[1]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());
	mT(obj[0]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());
}

void mT(::std::vector<nndx::dataW>& v, size_t index1, size_t index2)
{
	for (size_t i = 0; i < index1; ++i)
	{
		if (i != index1 - 1)
		{
			for (size_t j = 0; j < v[i].size(); ++j)
			{
				v[i][j].wg = static_cast<double>(nndx::randT(hProv) % 6) - 2;
			}
		}
		else
		{
			for (size_t j = 0; j < index2; ++j)
			{
				v[i][j].wg = static_cast<double>(nndx::randT(hProv) % 6) - 2;
			}
		}
	}
}