#include <QtCore/QScopedPointer>
#include <QtWidgets/QApplication>
#ifdef STORM_OS_WINDOWS
#include <WinBase.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	auto argc  = __argc;
	auto &argv = __argv;
#else
int main(int argc, char **argv) {
#endif
	QApplication app{argc, argv};

//	return app.;
}
