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
	int operator*();
	void operator()();
private:
	sf::Clock *c = new sf::Clock();
};

Object::Object(sf::Texture& t, nndx::dy_tpl&& tpl, int i)
	: obj(t), net(::std::move(tpl), []() -> double { return (nndx::randT(hProv) % 6) - 3; }, nndx::neuron::_func::_fnTANH), score(0), life(true), stepA(idefinitions::def_STEPA_)
{
	//if (i != 0)	net.SPECinit(topology);
	obj.setTextureRect(sf::IntRect(0, idefinitions::def_TEXTURE_OBJ_Y * i, idefinitions::def_TEXTURE_OBJ_X, idefinitions::def_TEXTURE_OBJ_Y));
	obj.setPosition(static_cast<float>(idefinitions::def_POSX), static_cast<float>(idefinitions::def_POSY));
	obj.setOrigin(sf::Vector2f(idefinitions::def_TEXTURE_OBJ_X / 2.0f, idefinitions::def_TEXTURE_OBJ_Y / 2.0f));
}

void Object::mA(::std::vector<double>&& args)
{
	bool step_forward = true;
	if (args[0] > def_KF)
	{
		obj.move(static_cast<float>(idefinitions::def_TEXTURE_OBJ_X), 0.f);
		score += 20;
		stepA = idefinitions::def_STEPA_;
	}
	else
	{
		step_forward = false;
	}
	if (args[1] > def_KF)
	{
		if (obj.getPosition().y < idefinitions::def_POSY_WALL + idefinitions::def_TEXTURE_WLL_Y + idefinitions::def_TEXTURE_WLL_Y * idefinitions::def__NUM_ACTIVE_WALL)
		{
			obj.move(0.f, static_cast<float>(idefinitions::def_TEXTURE_OBJ_Y));
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
		if (obj.getPosition().y > idefinitions::def_POSY_WALL)
		{
			obj.move(0.f, static_cast<float>(-idefinitions::def_TEXTURE_OBJ_Y));
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
	wall.setTextureRect(sf::IntRect(idefinitions::def_TEXTURE_OBJ_X, 0, idefinitions::def_TEXTURE_OBJ_X, idefinitions::def_TEXTURE_OBJ_Y));
	wall.setPosition(x, y);
	wall.setOrigin(sf::Vector2f(idefinitions::def_TEXTURE_OBJ_X / 2.0f, idefinitions::def_TEXTURE_OBJ_Y / 2.0f));
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

int TimeGet::operator*()
{
	return static_cast<int>(c->getElapsedTime().asMilliseconds());
}

void TimeGet::operator()()
{
	::std::cout << c->getElapsedTime().asMicroseconds() << ::std::endl;
}