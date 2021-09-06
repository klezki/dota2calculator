#include <DotaCalculator/DotaCalculator.h>



int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	DotaCalculator calc;

	calc.show();

	return app.exec();
}