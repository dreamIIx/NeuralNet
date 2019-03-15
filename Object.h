#pragma once

class Object
{
public:
	sf::Sprite obj;
	nndx::neuronet net;
	int score, stepA;
	bool life;

	Object(sf::Texture&, nndx::dy_tpl&&, int);
	void mA(::std::vector<double>&&);
};

class Wall
{
public:
	sf::Sprite wall;

	Wall(sf::Texture&, float, float);
};

class TimeGet
{
public:
	TimeGet() {}
	~TimeGet();
	bool restart();
	sf::Int32 operator*();
	void operator()();
private:
	sf::Clock *c = new sf::Clock();
};

Object::Object(sf::Texture& t, nndx::dy_tpl&& tpl, int i) : obj(t), net(::std::move(tpl), _fnTANH), score(0), life(true), stepA(6)
{
	//if (i != 0)	net.SPECinit(topology);
	obj.setTextureRect(sf::IntRect(0, def_TEXTURE_OBJ_Y * i, def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y));
	obj.setPosition(static_cast<float>(def_POSX), static_cast<float>(def_POSY));
	obj.setOrigin(sf::Vector2f(def_TEXTURE_OBJ_X / 2.0f, def_TEXTURE_OBJ_Y / 2.0f));
}

void Object::mA(::std::vector<double>&& args)
{
	bool step_forward = true;
	if (args[0] > def_KF)
	{
		obj.move(20.0f, 0.0f);
		score += 20;
		stepA = 6;
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
			if (!step_forward)	--stepA;
		}
		else
		{
			--stepA;
		}
	}
	else if (args[1] < -def_KF)
	{
		if (obj.getPosition().y > 140)
		{
			obj.move(0.0f, -20.0f);
			score += 5;
			if (!step_forward)	--stepA;
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

Wall::Wall(sf::Texture& t, float x, float y) : wall(t)
{
	wall.setTextureRect(sf::IntRect(def_TEXTURE_OBJ_X, 0, def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y));
	wall.setPosition(x, y);
	wall.setOrigin(sf::Vector2f(def_TEXTURE_OBJ_X / 2.0f, def_TEXTURE_OBJ_Y / 2.0f));
}

TimeGet::~TimeGet()
{
	delete c;
}

bool TimeGet::restart()
{
	c->restart();
	return true;
}

sf::Int32 TimeGet::operator*()
{
	return c->getElapsedTime().asMilliseconds();
}

void TimeGet::operator()()
{
	::std::cout << c->getElapsedTime().asMicroseconds() << ::std::endl;
}