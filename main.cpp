#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"

#ifndef def_ERR_FL_
#define def_ERR_FL_
#define def_XTT_S(x) #x
#define def_XTT_S_(x) def_XTT_S(x)
#define def_XTT_S__LINE__ def_XTT_S_(__LINE__)
#define def__FILELINE (__FILE__  " line " def_XTT_S__LINE__)
#define ERROR_ ::std::cout << def__FILELINE << ::std::endl;
#endif

#ifndef def_XTT_fast_ifstream
#define def_XTT_fast_ifstream
::std::string def_FIs(::std::ifstream& read_cont__)
{
	::std::string temp_string__;
	read_cont__ >> temp_string__;
	return temp_string__;
}
int def_FIi(::std::ifstream& read_cont__)
{
	int temp_int__;
	read_cont__ >> temp_int__;
	return temp_int__;
}
#endif

#include "NN.h"

#define def_WIN_X 1000
#define def_WIN_Y 400
#define def__SIZE_ 12
#define def__NUM_ACTIVE_WALL 4
#define def_FILEIMAGE "files/image.png"
#define def_TEXTURE_OBJ_X 20
#define def_TEXTURE_OBJ_Y 20
#define def_SIZE_VECTOR_WALL (def_WIN_X / def_TEXTURE_OBJ_X) + 1
#define def_POSX 410
#define def_POSY 200
#define def_POSY_WALL 140
#define def_KF 0.6


class Object
{
public:
	sf::Sprite obj;
	nndx::neuronet net;
	int score, stepA;
	bool life;

	Object(sf::Texture& t, const nndx::dy_tpl& tpl, int i) : obj(t), net(tpl, true), score(0), life(true), stepA(5)
	{
		//if (i != 0)	net.SPECinit(topology);
		obj.setTextureRect(sf::IntRect(0, def_TEXTURE_OBJ_Y * i, def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y));
		obj.setPosition(static_cast<float>(def_POSX), static_cast<float>(def_POSY));
		obj.setOrigin(sf::Vector2f(def_TEXTURE_OBJ_X / 2.0f, def_TEXTURE_OBJ_Y / 2.0f));
	}

	void mA(::std::vector<double>& args)
	{
		bool step_forward = true;
		if (args[0] > def_KF)
		{
			obj.move(20.0f, 0.0f);
			score += 20;
		}
		else
		{
			step_forward = false;
		}
		if (args[1] > def_KF)
		{
			if (obj.getPosition().y < 240)
			{
				obj.move(0.0f, 20.0f);
				score += 5;
			}
			else
			{
				--stepA;
			}
		}
		else if(args[1] < -def_KF)
		{
			if (obj.getPosition().y > 140)
			{
				obj.move(0.0f, -20.0f);
				score += 5;
			}
			else
			{
				--stepA;
			}
		}
		else
		{
			if (!step_forward)	--stepA;
		}
	}
};

class Wall
{
public:
	sf::Sprite wall;

	Wall(sf::Texture& t, float x, float y) : wall(t)
	{
		wall.setTextureRect(sf::IntRect(def_TEXTURE_OBJ_X, 0, def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y));
		wall.setPosition(x, y);
		wall.setOrigin(sf::Vector2f(def_TEXTURE_OBJ_X / 2.0f, def_TEXTURE_OBJ_Y / 2.0f));
	}
};

void radixSort(::std::vector<Object*>&);
void mA_gen(::std::vector<Object*>&, const int&);
void mT(::std::vector<nndx::dataW>&, size_t, size_t);
void mainA(sf::RenderWindow&, ::std::vector<Object>&, ::std::vector<::std::vector<Wall>>&, ::std::vector<int>&, sf::Texture&, bool&, bool&, bool&);

int main()
{
	::std::ifstream _tempRead("inf.cfg");
	sf::Int32 TIMESET_;
	sf::RenderWindow win({ def_WIN_X, def_WIN_Y }, "NN");
	win.setFramerateLimit(def_FIi(_tempRead));
	win.setVerticalSyncEnabled(true);
	win.setActive(false);
	TIMESET_ = static_cast<sf::Int32>(def_FIi(_tempRead));
	_tempRead.close();

	sf::Image image_;
	image_.loadFromFile(def_FILEIMAGE);
	sf::Texture t_;
	t_.loadFromImage(image_);

	::std::vector<Object> v_Obj_;
	::std::vector<::std::vector<Wall>> v_Wll_;
	::std::vector<int> target_;
	v_Obj_.reserve(def__SIZE_);
	v_Wll_.reserve(def_SIZE_VECTOR_WALL);
	target_.reserve(def_SIZE_VECTOR_WALL);

	bool isOpen = true;
	bool runA = true;
	bool newA = true;

	::std::thread mainThread(mainA, ::std::ref(win), ::std::ref(v_Obj_), ::std::ref(v_Wll_), ::std::ref(target_), ::std::ref(t_), ::std::ref(isOpen),
		::std::ref(runA), ::std::ref(newA));

	while (isOpen)
	{
		sf::Event event;
		while (win.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				isOpen = false;
				break;
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					runA = true;
				}
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
				{
					newA = true;
				}
			}
		}
	}

	mainThread.join();
	win.setActive(true);
	win.close();

	for (int i = 0; i < def__SIZE_; ++i)
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
		v_Obj_[i].net.saveF(s);
	}

	::std::ofstream out("output/log.txt");
	for (int i = 0; i < def__SIZE_; ++i)
	{
		out << "net_" << i << " - " << v_Obj_[i].score << ::std::endl;
	}
	out.close();

	return 0;
}

void mainA
(
	sf::RenderWindow& win,
	::std::vector<Object>& v_Obj,
	::std::vector<::std::vector<Wall>>& v_Wll,
	::std::vector<int>& target,
	sf::Texture& t,
	bool& is_Open,
	bool& run_,
	bool& newA_
)
{
	win.setActive(true);
	sf::View view;
	view.reset(sf::FloatRect(0.0f, 0.0f, static_cast<float>(def_WIN_X), static_cast<float>(def_WIN_Y)));

	for (int i = 0; i < def__SIZE_; ++i)
	{
		v_Obj.emplace_back(t, nndx::dy_tpl(3, 2, 3, 2), i);
	}

	int tmpy = nndx::randT() % 4;
	int next = nndx::randT() % 4;
	float wposx = 470.0f;

	target.emplace_back(def_POSY_WALL + 20 + tmpy * 20);
	v_Wll.emplace_back(::std::vector<Wall>());
	v_Wll.back().reserve(6);
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL));
	for (int i = 0; i < def__NUM_ACTIVE_WALL; ++i)
	{
		if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + 20 + i * 20));
	}
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + 100));
	tmpy = next;
	wposx += 20.0f;

	while (is_Open)
	{
		while (!run_)
		{
			if (!is_Open)
			{
				run_ = true;
				break;
			}
		}
		run_ = false;

		::std::vector<Object*> _ptr;
		for (int i = 0; i < def__SIZE_; ++i)
		{
			if (v_Obj[i].life)	_ptr.emplace_back(&v_Obj[i]);
		}

		int minposx_obj = 0xFFFF;
		int maxposx_obj = -0xFFFF;
		for (size_t i = 0; i < _ptr.size(); ++i)
		{
			if (minposx_obj > _ptr[i]->obj.getPosition().x)	minposx_obj = static_cast<int>(_ptr[i]->obj.getPosition().x);
			if (maxposx_obj < _ptr[i]->obj.getPosition().x)	maxposx_obj = static_cast<int>(_ptr[i]->obj.getPosition().x);
		}
		if (v_Wll[0][0].wall.getPosition().x < minposx_obj)
		{
			::std::vector<::std::vector<Wall>>::iterator it = v_Wll.begin();
			it = v_Wll.erase(it);
			::std::vector<int>::iterator itTg = target.begin();
			itTg = target.erase(itTg);
		}
		if (static_cast<int>(wposx) - maxposx_obj < 100)
		{
			v_Wll.emplace_back(::std::vector<Wall>());
			v_Wll.back().reserve(6);
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL));
			next = nndx::randT() % 4;
			target.emplace_back(def_POSY_WALL + 20 + tmpy * 20);
			for (int i = 0; i < def__NUM_ACTIVE_WALL; ++i)
			{
				if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + 20 + i * 20));
			}
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + 100));
			tmpy = next;
			wposx += 20.0f;
		}

		if ((_ptr.size()) && (!newA_))
		{
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
						_ptr[i]->net.SPECmA(packet);
						::std::vector<double> values;
						for (size_t uy = 0; uy < _ptr[i]->net.data.back().size(); ++uy)
						{
							values.emplace_back(_ptr[i]->net.data.back()[uy].data);
						}
						_ptr[i]->mA(values);
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

			for (int i = 0; i < def__SIZE_; ++i)
			{
				_ptr.emplace_back(&v_Obj[i]);
			}
			radixSort(_ptr);
			mA_gen(_ptr, 3);

			for (size_t i = 0; i < v_Obj.size(); ++i)
			{
				v_Obj[i].obj.setPosition(static_cast<float>(def_POSX), static_cast<float>(def_POSY));
				v_Obj[i].life = true;
				v_Obj[i].score = 0;
				v_Obj[i].stepA = 5;
			}
			maxposx_obj = def_POSX;
			v_Wll.clear();
			target.clear();
			tmpy = nndx::randT() % 4;
			wposx = 470.0f;
			target.emplace_back(def_POSY_WALL + 20 + tmpy * 20);
			v_Wll.emplace_back(::std::vector<Wall>());
			v_Wll.back().reserve(6);
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL));
			for (int i = 0; i < def__NUM_ACTIVE_WALL; ++i)
			{
				if (i != tmpy)	v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + 20 + i * 20));
			}
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(def_POSY_WALL + 100));
			wposx += 20.0f;
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

	win.setActive(false);
}

void radixSort(::std::vector<Object*>& a)
{
	int d = 8, w = 32;
	for (int p = 0; p < w / d; p++)
	{
		::std::vector<int> c(1 << d, 0);
		::std::vector<Object*> b = a;

		for (size_t i = 0; i < a.size(); i++)
			c[ (a[i]->score >> d * p) & ((1 << d) - 1) ]++;
		for (size_t i = 1; i < 1 << d; i++)
			c[i] += c[i - 1];
		for (int i = a.size() - 1; i >= 0; i--)
			b[--c[(a[i]->score >> d * p) & ((1 << d) - 1)]] = a[i];
		a = b;
	}
}

void mA_gen(::std::vector<Object*>& obj, const int& tg)
{
	size_t index;
	obj[8]->net.weight = obj[10]->net.weight;
	index = nndx::randT() % obj[10]->net.weight.size();
	mT(obj[8]->net.weight, index, nndx::randT() % obj[10]->net.weight[index].size());

	obj[7]->net.weight = obj[9]->net.weight;
	index = nndx::randT() % obj[9]->net.weight.size();
	mT(obj[7]->net.weight, index, nndx::randT() % obj[9]->net.weight[index].size());

	mT(obj[6]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());
	mT(obj[5]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());
	mT(obj[4]->net.weight, obj[11]->net.weight.size(), obj[11]->net.weight.back().size());

	for (size_t i = 0; i < tg - 2; ++i)
	{
		obj[3]->net.weight[i] = obj[11]->net.weight[i];
	}
	if(nndx::randT() % 2 == 1)	obj[3]->net.weight[tg - 2] = obj[10]->net.weight[tg - 2];
	else	obj[3]->net.weight[tg - 2] = obj[9]->net.weight[tg - 2];
	if (nndx::randT() % 3 == 2)
	{
		index = nndx::randT() % obj[3]->net.weight.size();
		mT(obj[3]->net.weight, index, nndx::randT() % obj[3]->net.weight[index].size());
	}

	for (size_t i = 0; i < tg - 2; ++i)
	{
		obj[2]->net.weight[i] = obj[10]->net.weight[i];
	}
	obj[2]->net.weight[tg - 2] = obj[9]->net.weight[tg - 2];
	if (nndx::randT() % 3 == 2)
	{
		index = nndx::randT() % obj[2]->net.weight.size();
		mT(obj[2]->net.weight, index, nndx::randT() % obj[2]->net.weight[index].size());
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
				v[i][j].wg = static_cast<double>(nndx::randT() % 6) - 2;
			}
		}
		else
		{
			for (size_t j = 0; j < index2; ++j)
			{
				v[i][j].wg = static_cast<double>(nndx::randT() % 6) - 2;
			}
		}
	}
}