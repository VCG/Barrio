#include <QApplication>
#include <QGLFormat>

#include "mainwindow.h"
#include "inputform.h"



int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setVersion(4, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	//format.setDepthBufferSize(24);
	//format.setStencilBufferSize(8);

	QSurfaceFormat::setDefaultFormat(format);

	InputForm* input_form;
	input_form = new InputForm(nullptr);
	input_form->exec();

	if (input_form->exec() == InputForm::Accepted)
	{
		MainWindow* window = new MainWindow(nullptr, input_form);

		window->show();
	}

#ifdef QT_DEBUG
	qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "23654");
#endif
	return app.exec();
}
