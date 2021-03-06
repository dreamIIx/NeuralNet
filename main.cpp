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

#include <SFML/Graphics.hpp>

#if defined(__unix__)
#if defined(__linux__)
#include <X11/Xlib.h>
#else
#error This UNIX operating system is not supported by dx::NN
#endif
#endif

template <typename T>
T def_FI(::std::ifstream&);

#include "NN.h"

#define WEIGHT_FUNC ((nndx::randT(hProv) % 3) - 1)

constexpr unsigned int def_WIN_X	=	1000;
constexpr unsigned int def_WIN_Y	=	400;
constexpr size_t def__SIZE_			=	{12};
constexpr int def__NUM_ACTIVE_WALL	=	7;
constexpr int def_STEPA_			=	(def__NUM_ACTIVE_WALL + 2);
constexpr int def_TEXTURE_OBJ_X		=	20;
constexpr int def_TEXTURE_OBJ_Y		=	20;
constexpr int def_TEXTURE_WLL_X		=	20;
constexpr int def_TEXTURE_WLL_Y		=	20;
constexpr int def_SIZE_VECTOR_WALL	=	(def_WIN_X / def_TEXTURE_OBJ_X) + 1;
constexpr int def_POSX				=	410;
constexpr int def_POSX_WALL			=	(def_POSX + 60);
constexpr int def_POSY_WALL			=	140;
constexpr int def_POSY				=	(def_POSY_WALL + 80);
constexpr int def_SZ_TOPOLOGY		=	3;
#define			def_TOPOLOGY			2, 3, 2
constexpr size_t def_PRE_DISTANCE	=	{200};

constexpr double		def_KF_X				=	0.15;
constexpr double		def_KF_Y				=	0.75;

#if defined(_WIN32)

#define	def_FILECFG "inf.cfg"
#define	def_FILEIMAGE "files/image.png"
#define def_LOGFILE "files/output/log.txt"
#define def_ENDPOINT "output/endpoint_log.txt"

#elif defined(__unix__)
    #if defined(__linux__)
	
#define	def_FILECFG "./inf.cfg"
#define	def_FILEIMAGE "./files/image.png"
#define def_LOGFILE "./files/log.txt"
#define def_ENDPOINT "./output/endpoint_log.txt"

    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif
dxCRYPT hProv; // for random number generator(NN.h, Object.h, main.cpp)

#include "Object.h"

void radixSort(::std::vector<Object*>&);
void mA_gen(::std::vector<Object*>&, size_t);
void mT(::std::vector<nndx::dataW>&, size_t, size_t);
void mainA(sf::RenderWindow&, sf::View&, ::std::vector<Object>&, ::std::vector<::std::vector<Wall>>&, ::std::vector<int>&, sf::Texture&,
	volatile ::std::atomic_int&, volatile ::std::atomic_bool&, volatile ::std::atomic_bool&, volatile ::std::atomic_bool&,
	volatile ::std::atomic_bool&, volatile ::std::atomic_bool&);

int main()
{
#if defined(_WIN32)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#elif defined(__unix__)
    #if defined(__linux__)
		// terminal take off
    	XInitThreads();
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif

	sf::RenderWindow win({ def_WIN_X, def_WIN_Y }, "NN");
	win.setVerticalSyncEnabled(true);
	win.setActive(false);

	/* default filename with settings - inf.cfg*/
	::std::ifstream _tempRead;
	_tempRead.open(def_FILECFG);
	ER_IF(!_tempRead.is_open(),, return 1; )

	volatile ::std::atomic_int TIMESET_;
	TIMESET_.store(0);
	win.setFramerateLimit(def_FI<unsigned int>(_tempRead));
	TIMESET_.store(def_FI<unsigned int>(_tempRead));
	_tempRead.close();

	sf::Image image_;
	image_.loadFromFile(def_FILEIMAGE);
	sf::Texture t_;
	t_.loadFromImage(image_);

	sf::View view;
	view.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(def_WIN_X), static_cast<float>(def_WIN_Y)));

	::std::vector<Object> v_Obj_;
	::std::vector<::std::vector<Wall>> v_Wll_;
	::std::vector<int> target_;
	v_Obj_.reserve(def__SIZE_);
	v_Wll_.reserve(def_SIZE_VECTOR_WALL);
	target_.reserve(def_SIZE_VECTOR_WALL);

	volatile ::std::atomic_bool isOpen;
	volatile ::std::atomic_bool runA;
	volatile ::std::atomic_bool newA;
	volatile ::std::atomic_bool auto_move;
	volatile ::std::atomic_bool Lclick_;
	isOpen.store(true);
	runA.store(false);
	newA.store(false);
	auto_move.store(true);
	Lclick_.store(false);

	
#if defined(_WIN32)
	if (!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, 0))
		if (GetLastError() == NTE_BAD_KEYSET)
			ER_IF(!CryptAcquireContext(&hProv, 0, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET),, return 0; )
#elif defined(__unix__)
    #if defined(__linux__)
		hProv.seed(::std::chrono::system_clock::to_time_t(::std::chrono::system_clock::now()));
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif

	::std::thread mainThread(mainA, ::std::ref(win), ::std::ref(view), ::std::ref(v_Obj_), ::std::ref(v_Wll_), ::std::ref(target_), ::std::ref(t_), ::std::ref(TIMESET_), ::std::ref(isOpen),
		::std::ref(runA), ::std::ref(newA), ::std::ref(auto_move), ::std::ref(Lclick_));

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
#if defined(_WIN32)
					ShowWindow(GetConsoleWindow(), SW_SHOW);
#elif defined(__unix__)
    #if defined(__linux__)
		// terminal take off
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif

					int _temp_int = 0;
					::std::cout << "TIME(0 - 9): ";
					::std::cin >> _temp_int;

#if defined(_WIN32)
					ShowWindow(GetConsoleWindow(), SW_HIDE);
					system("cls");
#elif defined(__unix__)
    #if defined(__linux__)
		// terminal take off
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif

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
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
					{
						win.setFramerateLimit(0);
					}
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					Lclick_.store(true);
				}
			}
			else if (event.type == sf::Event::Resized)
			{
				view.reset(sf::FloatRect(0.f, 0.f, static_cast<float>(win.getSize().x), static_cast<float>(win.getSize().y)));
			}
		}
	}

	mainThread.join();
#if defined(_WIN32)
	CryptReleaseContext(hProv, 0);
#elif defined(__unix__)
    #if defined(__linux__)
		// release dxCRYPT
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif
	win.setActive(true);
	win.close();

	for (size_t i = {0}; i < def__SIZE_; ++i)
	{
#if defined(_WIN32)
		::std::string s = "./output/net_"; // (?) !check it!
#elif defined(__unix__)
    #if defined(__linux__)
		::std::string s = "./output/net_";
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif
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
		ER_IFN(v_Obj_[i].net.saveF(s),
			::std::cout << "v_Obj_[i].net.saveF(s) returns - false" << ::std::endl;
			::std::cout << "s - " << s << ::std::endl;, return 1; )
	}

	::std::ofstream out(def_LOGFILE);
	for (size_t i = {0}; i < def__SIZE_; ++i)
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
	volatile ::std::atomic_int& TIMESET,
	volatile ::std::atomic_bool& is_Open,
	volatile ::std::atomic_bool& run_,
	volatile ::std::atomic_bool& newA_,
	volatile ::std::atomic_bool& auto_move,
	volatile ::std::atomic_bool& mLclick
)
{
	win.setActive(true);

	for (size_t i = {0}; i < def__SIZE_; ++i)
	{
		v_Obj.emplace_back(t, nndx::dy_tpl(def_SZ_TOPOLOGY, def_TOPOLOGY), i);
		ER_IF(!v_Obj.back().net.getState(),, )
	}
	Object& DBG_Obj = v_Obj.front();
	sf::Sprite DBG_Tx;
	DBG_Tx.setTexture(t);
	DBG_Tx.setTextureRect(sf::IntRect(def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y, def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y));
	DBG_Tx.setOrigin(DBG_Obj.obj.getOrigin());
	DBG_Tx.setPosition(DBG_Obj.obj.getPosition());

	int tmpy = nndx::randT(hProv) % def__NUM_ACTIVE_WALL;
	int next = nndx::randT(hProv) % def__NUM_ACTIVE_WALL;
	float wposx = static_cast<float>(def_POSX_WALL);

	target.emplace_back(def_POSY_WALL + def_TEXTURE_WLL_Y + tmpy * def_TEXTURE_WLL_Y);
	v_Wll.emplace_back(::std::vector<Wall>());
	v_Wll.back().reserve(def__NUM_ACTIVE_WALL + 2);
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL));
	for (int i = 0; i < def__NUM_ACTIVE_WALL; ++i)
	{
		if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + def_TEXTURE_WLL_Y + i * def_TEXTURE_WLL_Y));
	}
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + def_TEXTURE_WLL_Y + def__NUM_ACTIVE_WALL * def_TEXTURE_WLL_Y));
	tmpy = next;
	wposx += def_TEXTURE_WLL_X;

	size_t bufsizeO = def__SIZE_;
	TimeGet tm;

	int numITER = 0;
	::std::ofstream endP(def_ENDPOINT);
	ER_IF(!endP.is_open(),, )
/*
	if (!endP.is_open())
	{
		::std::cout << "error of opening file - endpoint_log.txt" << ::std::endl;
		ERROR_
#if defined(_WIN32)
		system("pause");
#elif defined(__unix__)
    #if defined(__linux__)
		// pause
    #else
        #error This UNIX operating system is not supported by dx::NN
    #endif
#else
    #error This operating system is not supported by dx::NN
#endif
	}
*/
	sf::Vector2i mPos;

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
		for (size_t i = {0}; i < def__SIZE_; ++i)
		{
			if (v_Obj[i].life)	_ptr.emplace_back(&v_Obj[i]);
		}

		int minposx_obj = 0x7FFFFFFF;
		int maxposx_obj = -0x7FFFFFFF;
		for (size_t i = {0}; i < _ptr.size(); ++i)
		{
			if (minposx_obj > _ptr[i]->obj.getPosition().x)	minposx_obj = static_cast<int>(_ptr[i]->obj.getPosition().x);
			if (maxposx_obj < _ptr[i]->obj.getPosition().x)	maxposx_obj = static_cast<int>(_ptr[i]->obj.getPosition().x);
		}

		if (_ptr.size())
		{
			mPos = static_cast<sf::Vector2i>(win.mapPixelToCoords(sf::Mouse::getPosition(win)));

			if (v_Wll[0][0].wall.getPosition().x < minposx_obj)
			{
				for (int i = 0; i < static_cast<int>((minposx_obj - v_Wll[0][0].wall.getPosition().x) / def_TEXTURE_WLL_X) - 1; ++i)
				{
					::std::vector<::std::vector<Wall>>::iterator it = v_Wll.begin();
					it = v_Wll.erase(it);
					::std::vector<int>::iterator itTg = target.begin();
					itTg = target.erase(itTg);
				}
			}
			if (static_cast<int>(wposx) - maxposx_obj < def_PRE_DISTANCE)
			{
				v_Wll.emplace_back(::std::vector<Wall>());
				v_Wll.back().reserve(def__NUM_ACTIVE_WALL + 2);
				v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL));
				next = nndx::randT(hProv) % def__NUM_ACTIVE_WALL;
				target.emplace_back(def_POSY_WALL + def_TEXTURE_WLL_Y + tmpy * def_TEXTURE_WLL_Y);
				for (int i = 0; i < def__NUM_ACTIVE_WALL; ++i)
				{
					if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))
						v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + def_TEXTURE_WLL_Y + i * def_TEXTURE_WLL_Y));
				}
				v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + def_TEXTURE_WLL_Y + def__NUM_ACTIVE_WALL * def_TEXTURE_WLL_Y));
				tmpy = next;
				wposx += def_TEXTURE_WLL_X;
			}
		}

		if (_ptr.size() && (((!auto_move.load()) && (!newA_.load()))
			|| (auto_move.load() && (((bufsizeO == _ptr.size()) ? *tm < TIMESET.load() : tm.restart()) || (TIMESET.load() == 0xFFFFFFFF)))))
		{
			if (mLclick.load())
			{
				mLclick.store(false);
				for (size_t i = {0}; i < _ptr.size(); ++i)
				{
					if (_ptr[i]->obj.getGlobalBounds().contains(mPos.x, mPos.y))
					{
						DBG_Obj = *_ptr[i];
						::std::cout << "SET" << ::std::endl;
						break;
					}
				}
			}

			bufsizeO = _ptr.size();
			for (size_t i = {0}; i < _ptr.size(); ++i)
			{
				for (size_t j = {0}; j < target.size(); ++j)
				{
					if (v_Wll[j][0].wall.getPosition().x > _ptr[i]->obj.getPosition().x)
					{
						double toY = target[j] - _ptr[i]->obj.getPosition().y;
						::std::vector<double> packet;
						packet.reserve(2);
						packet.emplace_back(1);
						packet.emplace_back(toY * 0.01);
						bool res = _ptr[i]->net.SPECmA(packet);
						ER_IFN(res,, )
						else
						{
							_ptr[i]->mA(std::move(_ptr[i]->net.getResults()));
							/*
							if (i == 0)
							{
								::std::cout << "[0] -  " << DBG_Obj.net.getResults()[0] << ::std::endl;
								::std::cout << "[1] -  " << DBG_Obj.net.getResults()[1] << ::std::endl;
								::std::cout << "----------------------------------------" << ::std::endl;
							}
							*/
						}
						break;
					}
				}
			}

			for (size_t i = {0}; i < _ptr.size(); ++i)
			{
				if (_ptr[i]->stepA < 1)
				{
					_ptr[i]->life = false;
					//_ptr[i]->net.callFuncHebb(); // optional
				}
				else
				{
					for (size_t j = {0}; j < v_Wll.size(); ++j)
					{
						for (size_t k = {0}; k < v_Wll[j].size(); ++k)
						{
							if (_ptr[i]->obj.getGlobalBounds().intersects(v_Wll[j][k].wall.getGlobalBounds()))
							{
								_ptr[i]->life = false;
								//_ptr[i]->net.callFuncHebb(); // optional
							}
						}
					}
				}
			}
		}
		else
		{
			newA_ = false;

			for (size_t i = {0}; i < def__SIZE_; ++i)
			{
				_ptr.emplace_back(&v_Obj[i]);
			}
			radixSort(_ptr);
			endP << _ptr.back()->score << ::std::endl;
			mA_gen(_ptr, def_SZ_TOPOLOGY);
			++numITER;

			for (size_t i = {0}; i < v_Obj.size(); ++i)
			{
				v_Obj[i].obj.setPosition(static_cast<float>(def_POSX), static_cast<float>(def_POSY));
				v_Obj[i].life = true;
				v_Obj[i].score = 0;
				v_Obj[i].stepA = def_STEPA_;
			}
			maxposx_obj = def_POSX;
			v_Wll.clear();
			target.clear();
			tmpy = nndx::randT(hProv) % def__NUM_ACTIVE_WALL;
			wposx = static_cast<float>(def_POSX_WALL);
			target.emplace_back(def_POSY_WALL + def_TEXTURE_WLL_Y + tmpy * def_TEXTURE_WLL_Y);
			v_Wll.emplace_back(::std::vector<Wall>());
			v_Wll.back().reserve(def__NUM_ACTIVE_WALL + 2);
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL));
			for (int i = 0; i < def__NUM_ACTIVE_WALL; ++i)
			{
				if (i != tmpy)	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + def_TEXTURE_WLL_Y + i * def_TEXTURE_WLL_Y));
			}
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + def_TEXTURE_WLL_Y + def__NUM_ACTIVE_WALL * def_TEXTURE_WLL_Y));
			wposx += def_TEXTURE_WLL_X;

			bufsizeO = def__SIZE_;
			tm.restart();
		}
		DBG_Tx.setPosition(DBG_Obj.obj.getPosition());

		view.setCenter(static_cast<float>(maxposx_obj), 250.0f);

		win.clear();
		win.setView(view);
		for (size_t i = {0}; i < v_Wll.size(); ++i)
		{
			for (size_t j = {0}; j < v_Wll[i].size(); ++j)
			{
				win.draw(v_Wll[i][j].wall);
			}
		}
		for (size_t i = {0}; i < _ptr.size(); ++i)
		{
			win.draw(_ptr[i]->obj);
		}
		win.draw(DBG_Tx);
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
		for (ptrdiff_t i = static_cast<int>(a.size() - 1); i >= 0; --i)
			b[--c[(a[i]->score >> 8 * p) & ((1 << 8) - 1)]] = a[i];
		a = b;
	}
}

void mA_gen(::std::vector<Object*>& obj, size_t tg)
{
	obj[8]->net.weight = obj[11]->net.weight;
	size_t index = nndx::randT(hProv) % obj[11]->net.weight.size();
	mT(obj[8]->net.weight, index, index + 1);

	obj[7]->net.weight = obj[11]->net.weight;
	index = nndx::randT(hProv) % obj[11]->net.weight.size();
	mT(obj[7]->net.weight, index, index + 1);

	obj[6]->net.weight = obj[10]->net.weight;
	index = nndx::randT(hProv) % obj[10]->net.weight.size();
	mT(obj[6]->net.weight, index, index + 1);

	obj[5]->net.weight = obj[9]->net.weight;
	index = nndx::randT(hProv) % obj[9]->net.weight.size();
	mT(obj[5]->net.weight, index, index + 1);

	mT(obj[4]->net.weight, static_cast<size_t>(nndx::randT(hProv) % obj[11]->net.weight.size()), obj[11]->net.weight.size());

	for (size_t i = {0}; i < tg - 2; ++i)
	{
		obj[3]->net.weight[i] = obj[11]->net.weight[i];
	}
	if(nndx::randT(hProv) % 2 == 1)	obj[3]->net.weight[tg - 2] = obj[10]->net.weight[tg - 2];
	else	obj[3]->net.weight[tg - 2] = obj[9]->net.weight[tg - 2];
	if (nndx::randT(hProv) % 3 == 2)
	{
		index = nndx::randT(hProv) % obj[3]->net.weight.size();
		mT(obj[3]->net.weight, index, index + 1);
	}

	for (size_t i = {0}; i < tg - 2; ++i)
	{
		obj[2]->net.weight[i] = obj[10]->net.weight[i];
	}
	obj[2]->net.weight[tg - 2] = obj[9]->net.weight[tg - 2];
	if (nndx::randT(hProv) % 3 == 2)
	{
		index = nndx::randT(hProv) % obj[2]->net.weight.size();
		mT(obj[2]->net.weight, index, index + 1);
	}

	mT(obj[1]->net.weight, 0, obj[11]->net.weight.size());
	mT(obj[0]->net.weight, 0, obj[11]->net.weight.size());
}

void mT(::std::vector<nndx::dataW>& v, size_t index1, size_t index2)
{
	for (size_t i = index1; i < index2; ++i)
	{
		for (size_t j = {0}; j < v[i].size(); ++j)
		{
			v[i][j].wg = static_cast<double>(WEIGHT_FUNC);
		}
	}
}