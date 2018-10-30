#include <iostream>
#include <fstream>

int mainqq()
{
	std::ofstream f("training_dataT.txt");
	const int num = 200000;
	f << num << std::endl;
	f << 2 << " " << 1 << std::endl;
	for (int i = 0; i < num; i++)
	{
		double x = rand() % 2;
		double y = rand() % 2;
		f << x << " " << y << " ";
		f << (x != y) ? 1 : 0;
		f << std::endl;
	}
	f.close();

	return 0;
}