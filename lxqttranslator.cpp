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
        if (appTranslator->load(name + "_" + locale, path))
        {
            QCoreApplication::installTranslator(appTranslator);
            return true;
        }
        else if (locale == QLatin1String("C") ||
                    locale.startsWith(QLatin1String("en")))
        {
            // English is the default. Even if there isn't an translation file,
            // we return true. It's translated anyway.
            delete appTranslator;
            return true;
        }
    }

    // If we got here, no translation was loaded. appTranslator has no use.
    delete appTranslator;
    return false;
}


/************************************************

 ************************************************/
bool Translator::translateApplication(const QString &applicationName)
{
    QString locale = QLocale::system().name();
    QTranslator *qtTranslator = new QTranslator(qApp);

    if (qtTranslator->load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        qApp->installTranslator(qtTranslator);
    }
    else
    {
        delete qtTranslator;
        qWarning("LxQt::Translator: Qt translations not found");
    }

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
