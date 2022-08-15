#include "DeviceStore.h"
#include "style_head.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	//------------------------------ StyleSheet ------------------------------
	QApplication::setDesktopSettingsAware(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
	QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
	QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
	QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
	QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#else
	QTextCodec *codec = QTextCodec::codecForName("gbk");
	QTextCodec::setCodecForLocale(codec);
#endif
	//------------------------------ StyleSheet ------------------------------ 


    QApplication a(argc, argv);
    DeviceStore w;
    w.show();
    return a.exec();
}
