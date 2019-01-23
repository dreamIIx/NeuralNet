//Autor -dreamIIx
//GitHub - https://github.com/dreamIIx
//Release on GitHub 27.09.2018

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
#include <Windows.h>
//#include <ctime>

namespace nndx
{
	inline int randT();

	struct dy_tpl
	{
		std::vector<int> tempDATA;

		dy_tpl(int size, ...)
		{
			va_list args;
			va_start(args, size);

			tempDATA.reserve(size);
			for (int i = 0; i < size; ++i)
			{
				int value = va_arg(args, int);
				tempDATA.emplace_back(value);
			}
			va_end(args);
		}

		~dy_tpl()
		{
			tempDATA.clear();
			//std::cout << "called ~dy_tpl()" << std::endl;
		}
	};

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
		bool BIfunc = false;
		double moment = 0.0; // 0.05
		double u = 0.0; // 0.1
		std::vector<dataA> data;
		std::vector<dataW> weight;
		std::vector<int> topology_save;
		std::string nameF = "";
		std::string nameT = "";

		neuronet() {}

		neuronet(dy_tpl& temp, bool setBIfunc) : BIfunc(setBIfunc)
		{
			/// \<- SPECinit()
			int* pos = temp.tempDATA.data();
			for (int i = 0; i < temp.tempDATA.size(); i++)
			{
				int a = *pos++;
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
					weight.back().push_back((nndx::randT() % 6) - 2);
					//weight.back().push_back((randT() % 101) * 0.01);
					//weight.back().push_back(randT() * 0.001);
					//weight.back().push_back(rand() / double(RAND_MAX));
					//std::cout << weight.back().back().wg << "   ";
				}
				//std::cout << std::endl;
			}

			//temp.~dy_tpl();
		}

		void saveF(std::string s)
		{
			std::cout << "Outputing weights..." << std::endl;

			std::ofstream f("output/" + s);
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
			f << s;
			f.close();
			std::cout << "Weights is saved!(File - " << s << ")" << std::endl;
		}

		/*void mA()
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
				/*for (int i = 0; i < weight.size(); i++)
				{
					std::cout << weight[i][weight[i].size() - 1].wg << std::endl;
				}
				std::vector<double> errDat;
				for (int i = 0; i < data.back().size(); i++)
				{
					double j;
					read >> j;
					errDat.push_back(j);
					//std::cout << fabs(j - data.back()[i].data) << std::endl;
				}
				backProp(errDat);
				num++;
				std::cout << "------------" << std::endl;
			}
			read.close();
		}*/

		void SPECmA(std::vector<int>& dataT)
		{
			if (dataT.size() == data[0].size() - 1)
			{
				for (int i = 0; i < data[0].size() - 1; i++)
				{
					data[0][i].data = dataT[i];
					if (!data[0][i].BIAS)	data[0][i].goF();
					else std::cout << "checkout here!" << std::endl;
				}
				if(BIfunc)	activationF_BI();
				else	activationF();
			}
			else
			{
				std::cout << "ERROR || !@#$" << std::endl;
			}
			/*for (int i = 0; i < data.back().size(); i++)
			{
				std::cout << data.back()[i].data << std::endl;
			}*/
		}

		/*void SPECinit(std::vector<int> topology)
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
					weight.back().push_back((nndx::randT() % 6) - 2);
					//weight.back().push_back((randT() % 101) * 0.01);
					//weight.back().push_back(randT() * 0.001);
					//weight.back().push_back(rand() / double(RAND_MAX));
					//std::cout << weight.back().back().wg << "   ";
				}
				//std::cout << std::endl;
			}
		}*/

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
						weight.back().push_back(randT() * 0.001);
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
						std::cout << "ERROR! CHECKOUT!" << std::endl;
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

		/*void backProp(std::vector<double> &d)
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
			for (size_t i = data.size() - 2; i >= 0; i--)
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
		}*/
	};

	inline int randT()
	{
		HCRYPTPROV hProv;
		HCRYPTPROV *phProv = &hProv;

		BYTE Buf1;
		BYTE Buf2;

		BOOL retval;
		retval = CryptAcquireContext(phProv, 0, 0, PROV_RSA_FULL, 0);

		if (retval != 0)
		{
			CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf1);
			CryptReleaseContext(hProv, 0);
		}
		retval = CryptAcquireContext(phProv, 0, 0, PROV_RSA_FULL, 0);

		if (retval != 0)
		{
			CryptGenRandom(hProv, DWORD(sizeof(BYTE)), &Buf2);
			CryptReleaseContext(hProv, 0);
		}

		int i = (int)Buf1;
		i += (int)Buf2;
		//std::cout << i << std::endl;
		return i;
	}
}