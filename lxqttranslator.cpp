#include "lxqttranslator.h"
#include <QTranslator>
#include <QLocale>
#include <QDebug>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QStringList>
#include <QFileInfo>

#include <XdgDirs>

using namespace LxQt;

/************************************************

 ************************************************/
QStringList *getSearchPaths()
{
    static QStringList *searchPath = 0;

    if (searchPath == 0)
    {
        searchPath = new QStringList();
        *searchPath << XdgDirs::dataHome(false) + "/lxqt/translations";
        *searchPath << XdgDirs::dataDirs("/lxqt/translations");
        *searchPath << "/usr/local/share/lxqt/translations";
        *searchPath << "/usr/share/lxqt/translations";
    }

    return searchPath;
}


/************************************************

 ************************************************/
QStringList LxQt::Translator::translationSearchPaths()
{
    return *(getSearchPaths());
}


/************************************************

 ************************************************/
void Translator::setTranslationSearchPaths(const QStringList &paths)
{
    QStringList *p = getSearchPaths();
    p->clear();
    *p << paths;
}


/************************************************

 ************************************************/
bool translate(const QString &name)
{
    QString locale = QLocale::system().name();
    QTranslator *appTranslator = new QTranslator(qApp);

    QStringList *paths = getSearchPaths();
    foreach(QString path, *paths)
    {
        bool ok = appTranslator->load(name + "_" + locale, path);
        if (ok)
        {
            QCoreApplication::installTranslator(appTranslator);
            return ok;
        }
    }
    return false;
}


/************************************************

 ************************************************/
bool Translator::translateApplication(const QString &applicationName)
{
    QString locale = QLocale::system().name();
    QTranslator *qtTranslator = new QTranslator(qApp);
    qtTranslator->load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(qtTranslator);


    if (!applicationName.isEmpty())
        return translate(applicationName);
    else
        return translate(QFileInfo(QCoreApplication::applicationFilePath()).baseName());
}


/************************************************

 ************************************************/
bool Translator::translateLibrary(const QString &libraryName)
{
    static QSet<QString> loadedLibs;

    if (loadedLibs.contains(libraryName))
        return true;

    loadedLibs.insert(libraryName);

    return translate(libraryName);
}
