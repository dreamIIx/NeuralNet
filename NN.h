//Autor -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release on GitHub 27.09.2018

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
//#include <ctime>

class neuron
{
public:
	double data = 0.0f;
	double funcBP = 0.0f;
	bool BIAS = false;
	neuron(double num) : data(num) {}
	void goF() { funcBP = data * (1 - data); }
	void goF_BI() { funcBP = 1 - data * data; }
	void is_bias() { BIAS = true; }
};

class wWw
{
public:
	double wg = 0.0f;
	double dwg = 0.0f;
	double grad = 0.0f;
	wWw(double num) : wg(num) {}
	wWw() {}
};

typedef std::vector<neuron> dataA;
typedef std::vector<wWw> dataW;

class neuronet
{
public:
	double moment = 0.0; // 0.05
	double u = 0.0; // 0.1
	std::vector<dataA> data;
	std::vector<dataW> weight;
	std::vector<int> topology_save;
	std::string nameF = "";
	std::string nameT = "";

	void saveF()
	{
		std::cout << "Outputing weights..." << std::endl;

		std::ofstream f(nameF + "_OUT.txt");
		for (int i = 0; i < topology_save.size(); i++)
		{
			f << topology_save[i] << " ";
		}
		f << "0" << std::endl;

		for (int i = 0; i < weight.size(); i++)
		{
			for (int j = 0; j < weight[i].size(); j++)
			{
				f << weight[i][j].wg << std::endl;
				f << weight[i][j].dwg << std::endl;
			}
			f << "-----" << std::endl;
		}
		f << nameT;
		f.close();
		std::cout << "Weights is saved!(File - " << nameF << "_OUT.txt)" << std::endl;
	}

	void mA()
	{
		std::ifstream read(nameT);
		if (!read.is_open())
		{
			std::cout << "Error of opening file - " << nameT << std::endl;
			read.close();
			system("pause"); // after this moment, need to close programm
		}

		int num = 0;
		int nums;
		read >> nums;

		int *k = new int(0);
		read >> *k;
		if (*k != data[0].size() - 1)
		{
			std::cout << "Error, wrong training data!" << std::endl;
			delete k;
			system("pause"); // after this moment, need to close programm
		}
		read >> *k;
		if (*k != data.back().size())
		{
			std::cout << "Error, wrong number of output neurons!" << std::endl;
			delete k;
			system("pause"); // after this moment, need to close programm
		}

		system("cls");

		while (num < nums)
		{
			for (int i = 0; i < data[0].size() - 1; i++)
			{
				read >> data[0][i].data;
				if (!data[0][i].BIAS)	data[0][i].goF();
			}
			activationF();
			for (int i = 0; i < data.back().size(); i++)
			{
				std::cout << data.back()[i].data << std::endl;
			}
			std::cout << "------------" << std::endl;
			std::vector<double> errDat;
			for (int i = 0; i < data.back().size(); i++)
			{
				double j;
				read >> j;
				errDat.push_back(j);
			}
			backProp(errDat);
			num++;
		}
		read.close();
	}

	void SPECmA(std::vector<double> input)
	{
		for (int i = 0; i < data[0].size() - 1; i++)
		{
			data[0][i].data = input[i];
			if (!data[0][i].BIAS)	data[0][i].goF();
		}
		activationF();
		for (int i = 0; i < data.back().size(); i++)
		{
			std::cout << data.back()[i].data << std::endl;
		}
	}

	void SPECinit(std::vector<int> topology)
	{
		for (int i = 0; i < topology.size(); i++)
		{
			int a = topology[i];
			if (a > 0)
			{

				data.push_back(dataA());

				for (int i = 0; i < a; i++)
				{
					data.back().push_back(0);
				}
				topology_save.push_back(a);
			}
		}

		for (int i = 0; i < data.size() - 1; i++)
		{
			data[i].push_back(1);
			data[i].back().is_bias();
		}

		for (int i = 0; i < data.size() - 1; i++)
		{
			weight.push_back(dataW());
			for (int j = 0; j < data[i].size() * data[i + 1].size(); j++)
			{
				weight.back().push_back(rand() / double(RAND_MAX));
				std::cout << weight.back().back().wg << "   ";
			}
			std::cout << std::endl;
		}
	}

	void init()
	{
		int* a = new int(-1);

		std::cout << "Write -1000 for new topology, or any number for init topology from file: ";
		std::cin >> *a;
		if (*a == -1000)
		{
			while (*a != 0)
			{
				system("cls");
				std::cout << "Please write topology(size of every layer)" << std::endl;
				std::cin >> *a;
				if (*a > 0)
				{

					data.push_back(dataA());

					//srand(unsigned(time(NULL)));
					for (int i = 0; i < *a; i++)
					{
						data.back().push_back(0);
					}
					topology_save.push_back(*a);
				}
			}
			delete a;

			for (int i = 0; i < data.size() - 1; i++)
			{
				data[i].push_back(1);
				data[i].back().is_bias();
			}

			for (int i = 0; i < data.size() - 1; i++)
			{
				weight.push_back(dataW());
				for (int j = 0; j < data[i].size() * data[i + 1].size(); j++)
				{
					weight.back().push_back(rand() / double(RAND_MAX));
				}
			}
			std::cout << "Training file(default): " << nameT << std::endl;
			std::cout << "SUCCESS! Press anykey to start..." << std::endl;
		}
		else
		{
			std::ifstream read(nameF + ".txt");
			if (!read.is_open())
			{
				std::cout << "Wrong filename! ERROR" << std::endl;
				read.close();
				system("pause"); // after this moment, need to close programm
			}
			read >> *a;
			while (*a != 0)
			{
				std::cout << *a << std::endl;
				data.push_back(dataA());

				for (int i = 0; i < *a; i++)
				{
					data.back().push_back(0);
				}
				topology_save.push_back(*a);

				read >> *a;
			}
			delete a;
			double w;

			for (int i = 0; i < data.size() - 1; i++)
			{
				data[i].push_back(1);
				data[i].back().is_bias();
			}
			std::cout << "Init weights from file..." << std::endl;
			for (int i = 0; i < data.size() - 1; i++)
			{
				weight.push_back(dataW());
				for (int j = 0; j < data[i].size() * data[i + 1].size(); j++)
				{
					read >> w;
					weight.back().push_back(w);
					std::cout << w << std::endl;
					read >> w;
					weight.back().back().dwg = w;
					std::cout << w << std::endl;
				}
				std::string check;
				read >> check;
				if (check != "-----")
				{
					std::cout << "check = " << check << std::endl;
					std::cout << "ERROR!" << std::endl;
					system("pause"); // after this moment, need to close programm
				}
			}
			read >> nameT;
			read.close();
			std::cout << "Name of training file: " << nameT << " (check)" << std::endl;
			std::cout << "SUCCESS! The neuronet already...(press anykey to start)" << std::endl;
			system("pause");
		}
	}

	void activationF()
	{
		double local_sum = 0;

		for (int i = 1; i < data.size(); i++)
		{
			for (int j = 0; j < data[i].size(); j++)
			{
				local_sum = 0;
				for (int prev = 0; prev < data[i - 1].size(); prev++)
				{
					local_sum += data[i - 1][prev].data * weight[i - 1][prev * data[i].size() + j].wg;
				}
				if (!data[i][j].BIAS)	data[i][j].data = 1 / (1 + exp(-local_sum));
				if (!data[i][j].BIAS)	data[i][j].goF();
			}
		}
	}

	void activationF_BI()
	{
		double local_sum = 0;

		for (int i = 1; i < data.size(); i++)
		{
			for (int j = 0; j < data[i].size(); j++)
			{
				local_sum = 0;
				for (int prev = 0; prev < data[i - 1].size(); prev++)
				{
					local_sum += data[i - 1][prev].data * weight[i - 1][prev * data[i].size() + j].wg;
				}
				if (!data[i][j].BIAS)	data[i][j].data = tanh(local_sum);
				if (!data[i][j].BIAS)	data[i][j].goF_BI();
			}
		}
	}

	void backProp(std::vector<double> &d)
	{
		std::vector<dataW> errR;

		for (int i = 0; i < data.size(); i++)
		{
			errR.push_back(dataW());
		}

		for (int i = 0; i < data.back().size(); i++)
		{
			errR.back().push_back((d[i] - data.back()[i].data) * data.back()[i].funcBP);
		}

		double local_sum = 0;
		for (int i = data.size() - 2; i >= 0; i--)
		{
			for (int j = 0; j < data[i].size(); j++)
			{
				local_sum = 0;
				for (int next = 0; next < data[i + 1].size(); next++)
				{
					local_sum += errR[i + 1][next].wg * weight[i][data[i + 1].size() * j + next].wg;
					weight[i][data[i + 1].size() * j + next].grad = errR[i + 1][next].wg * data[i][j].data;
				}
				errR[i].push_back(local_sum * data[i][j].funcBP);
			}
		}

		for (int i = 1; i < data.size(); i++)
		{
			for (int j = 0; j < data[i].size(); j++)
			{
				for (int prev = 0; prev < data[i - 1].size(); prev++)
				{
					weight[i - 1][prev * data[i].size() + j].dwg = u * weight[i - 1][prev * data[i].size() + j].grad + (moment * weight[i - 1][prev * data[i].size() + j].dwg);
					weight[i - 1][prev * data[i].size() + j].wg += weight[i - 1][prev * data[i].size() + j].dwg;
				}
			}
		}
	}
};