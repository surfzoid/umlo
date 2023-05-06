#include "umlo.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Translation
    QTranslator qtTranslator;
    bool bsuc = false;
    bsuc = qtTranslator.load( QLocale(), QLatin1String("qt"), QLatin1String("_"),
                              QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    //surpress warning!
    if (bsuc == false ) {
        //I'm happy for yu
    }
    //surpress warning!

    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    bsuc = myappTranslator.load(QLocale(), QLatin1String("umlo"), QLatin1String("_"),
                                "/usr/share/umlo/translations");

    //surpress warning!
    if (bsuc == false ) {
        //I'm happy for yu
    }
    //surpress warning!
#if (defined(_WIN32))
    bsuc = myappTranslator.load(QLocale(), QLatin1String("umlo"), QLatin1String("_"),
                                QCoreApplication::applicationDirPath());
#endif

    a.installTranslator(&myappTranslator);


    //End Translation
    Umlo w;
    w.show();
    qputenv("QT_ASSUME_STDERR_HAS_CONSOLE", "1");
    return a.exec();
}
