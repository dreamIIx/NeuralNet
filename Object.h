#pragma once

class Object
{
public:
	int score;
	int stepA;
	bool life;
	sf::Sprite obj;
	nndx::neuronet net;

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
	inline int operator*();
	void operator()();
private:
	sf::Clock *c = new sf::Clock();
};

Object::Object(sf::Texture& t, nndx::dy_tpl&& tpl, int i)
	: score(0), stepA(def_STEPA_), life(true), obj(t), net(::std::move(tpl), []() -> double { return static_cast<double>(WEIGHT_FUNC); }, nndx::neuron::_func::_fnTANH)
{
	//if (i != 0)	net.SPECinit(topology);
	//net.setParams(0.5, 0.35); // just for U, moment is unnecessary (FOR callFuncHebb ONLY)
	obj.setTextureRect(sf::IntRect(0, def_TEXTURE_OBJ_Y * i, def_TEXTURE_OBJ_X, def_TEXTURE_OBJ_Y));
	obj.setPosition(static_cast<float>(def_POSX), static_cast<float>(def_POSY));
	obj.setOrigin(sf::Vector2f(def_TEXTURE_OBJ_X / 2.0f, def_TEXTURE_OBJ_Y / 2.0f));
}

void Object::mA(::std::vector<double>&& args)
{
	bool step_forward = true;
	if ((args[0] < def_KF_X) || (args[0] > -def_KF_X))
	{
		obj.move(static_cast<float>(def_TEXTURE_OBJ_X), 0.f);
		score += 20;
		stepA = def_STEPA_;
	}
	else
	{
		step_forward = false;
	}
	if (args[1] > def_KF_Y)
	{
		if (obj.getPosition().y < def_POSY_WALL + def_TEXTURE_WLL_Y + def_TEXTURE_WLL_Y * def__NUM_ACTIVE_WALL)
		{
			obj.move(0.f, static_cast<float>(def_TEXTURE_OBJ_Y));
			score += 5;
			if (!step_forward)	--stepA;
		}
		else
		{
			--stepA;
		}
	}
	else if (args[1] < -def_KF_Y)
	{
		if (obj.getPosition().y > def_POSY_WALL)
		{
			obj.move(0.f, static_cast<float>(-def_TEXTURE_OBJ_Y));
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

inline int TimeGet::operator*()
{
	return static_cast<int>(c->getElapsedTime().asMilliseconds());
}

void TimeGet::operator()()
{
	::std::cout << c->getElapsedTime().asMicroseconds() << ::std::endl;
}