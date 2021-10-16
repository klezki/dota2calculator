#include <DotaCalculator/DotaCalculator.h>

/**
 * TODO:
 * 1. separated skills(and traits) storage, available for read/write from anywhere (i. e. get target mp for antimage skill from options dialog)
 * 2. illusions
 * 3. armor and magres
 */

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	DotaCalculator calc;

	calc.show();

	return app.exec();
}