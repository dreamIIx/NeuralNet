#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "SFML/Graphics.hpp"

#ifndef _ERR_FL_
#define _ERR_FL_
#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)
#define FILELINE (__FILE__  " line " S__LINE__)
#define ERROR_ ::std::cout << FILELINE << ::std::endl;
#endif

#include "NN.h"
#include "MainH.h"

#define WIN_X 1000
#define WIN_Y 400
#define _SIZE_ 12
#define _NUM_ACTIVE_WALL 4
#define FILEIMAGE "files/image.png"
#define TEXTURE_OBJ_X 20
#define TEXTURE_OBJ_Y 20
#define SIZE_VECTOR_WALL (WIN_X / TEXTURE_OBJ_X) + 1
#define POSX 410
#define POSY 200
#define POSY_WALL 140
#define KF 0.5
#define TIME 2000


class Object
{
public:
	sf::Sprite obj;
	nndx::neuronet net;
	int score, i;
	bool life;

	Object(sf::Texture& t, const nndx::dy_tpl& tpl, int i) : obj(t), net(tpl, true), score(0), life(true), i(i)
	{
		//if (i != 0)	net.SPECinit(topology);
		obj.setTextureRect(sf::IntRect(0, TEXTURE_OBJ_Y * i, TEXTURE_OBJ_X, TEXTURE_OBJ_Y));
		obj.setPosition(static_cast<float>(POSX), static_cast<float>(POSY));
		obj.setOrigin(sf::Vector2f(TEXTURE_OBJ_X / 2, TEXTURE_OBJ_Y / 2));
	}

	void mA(::std::vector<double> args)
	{
		if (args[0] > KF)
		{
			obj.move(20.0f, 0.0f);
			score += 20;
		}
		if (args[1] > KF)
		{
			if (obj.getPosition().y < 240)
			{
				obj.move(0.0f, 20.0f);
				score += 5;
			}
		}
		else if(args[1] < -KF)
		{
			if (obj.getPosition().y > 140)
			{
				obj.move(0.0f, -20.0f);
				score += 5;
			}
		}
	}
};

class Wall
{
public:
	sf::Sprite wall;

	Wall(sf::Texture& t, float x, float y) : wall(t)
	{
		wall.setTextureRect(sf::IntRect(TEXTURE_OBJ_X, 0, TEXTURE_OBJ_X, TEXTURE_OBJ_Y));
		wall.setPosition(x, y);
		wall.setOrigin(sf::Vector2f(TEXTURE_OBJ_X / 2, TEXTURE_OBJ_Y / 2));
	}
};

void radixSort(::std::vector<Object*>& a);
void mA_gen(::std::vector<Object*>& obj, const int& tg);
void mT(::std::vector<nndx::dataW>& v, size_t index1, size_t index2);

int main()
{
	sf::RenderWindow win({ WIN_X, WIN_Y }, "test");
	//win.setFramerateLimit(2);
	win.setVerticalSyncEnabled(true);

	sf::Image image;
	image.loadFromFile(FILEIMAGE);
	sf::Texture t;
	t.loadFromImage(image);

	::std::vector<Object> v_Obj;
	v_Obj.reserve(_SIZE_);
	//v_Obj.emplace_back(t, topology__, 0);
	//v_Obj[0].net.nameF = "wWWw_OUT";
	//v_Obj[0].net.init();
	for (int i = 0; i < _SIZE_; ++i)
	{
		v_Obj.emplace_back(t, nndx::dy_tpl(3, 2, 3, 2), i);
	}
	::std::vector<::std::vector<Wall>> v_Wll;
	::std::vector<int> target;
	v_Wll.reserve(SIZE_VECTOR_WALL);
	target.reserve(SIZE_VECTOR_WALL);
	int tmpy = nndx::randT() % 4;
	int next = nndx::randT() % 4;
	float wposx = 470.0f;
	target.emplace_back(POSY_WALL + 20 + tmpy * 20);
	v_Wll.push_back(::std::vector<Wall>());
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL));
	for (int i = 0; i < _NUM_ACTIVE_WALL; ++i)
	{
		if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))	v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL + 20 + i * 20));
	}
	v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL + 100));
	tmpy = next;
	wposx += 20.0f;

	sf::View view;
	view.reset(sf::FloatRect(0.0f, 0.0f, WIN_X, WIN_Y));
	dx::TimeGet tm;
	size_t bufsizeO;

	int mI = -1;
	int mP = -1;

	while (win.isOpen())
	{
		sf::Event event;
		while (win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				win.close();
				break;
			}
		}

		::std::vector<Object*> _ptr;
		for (int i = 0; i < _SIZE_; ++i)
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
			v_Wll.push_back(::std::vector<Wall>());
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL));
			next = nndx::randT() % 4;
			target.emplace_back(POSY_WALL + 20 + tmpy * 20);
			for (int i = 0; i < _NUM_ACTIVE_WALL; ++i)
			{
				if ((i < (tmpy < next ? tmpy : next)) || (i > (tmpy > next ? tmpy : next)))	v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL + 20 + i * 20));
			}
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL + 100));
			tmpy = next;
			wposx += 20.0f;
		}

		bufsizeO = _ptr.size();
		if ((_ptr.size()) && (bufsizeO == _ptr.size() ? *tm < TIME : 1))
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
			}

		}
		else
		{
			for (int i = 0; i < _SIZE_; ++i)
			{
				_ptr.emplace_back(&v_Obj[i]);
			}
			radixSort(_ptr);
			if (mP < _ptr[11]->score)
			{
				mI = _ptr[11]->i;
				mP = _ptr[11]->score;
			}
			mA_gen(_ptr, 3);

			for (size_t i = 0; i < v_Obj.size(); ++i)
			{
				v_Obj[i].obj.setPosition(static_cast<float>(POSX), static_cast<float>(POSY));
				v_Obj[i].life = true;
				v_Obj[i].score = 0;
			}
			maxposx_obj = POSX;
			v_Wll.clear();
			target.clear();
			tmpy = nndx::randT() % 4;
			wposx = 470.0f;
			target.emplace_back(POSY_WALL + 20 + tmpy * 20);
			v_Wll.push_back(::std::vector<Wall>());
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL));
			for (int i = 0; i < _NUM_ACTIVE_WALL; ++i)
			{
				if (i != tmpy)	v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL + 20 + i * 20));
			}
			v_Wll.back().emplace_back(t, wposx, static_cast<float>(POSY_WALL + 100));
			wposx += 20.0f;
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

	for (int i = 0; i < _SIZE_; ++i)
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
		v_Obj[i].net.saveF(s);
	}

	::std::ofstream out("output/log.txt");
	for (int i = 0; i < _SIZE_; ++i)
	{
		out << "net_" << i << " - " << v_Obj[i].score << ::std::endl;
	}
	out.close();

	return 0;
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