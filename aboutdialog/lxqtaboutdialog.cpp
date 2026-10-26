/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
 * Authors:
 *   Ernest C. Suarez <ernestcsuarez@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "lxqtaboutdialog.h"
#include "ui_lxqtaboutdialog.h"

using namespace LXQt;

AboutDialog::AboutDialog(const QString &appname,
                         const QString &appversion,
                         const QString &appdescription,
                         const QString &copyright,
                         const QString &url,
                         QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{

    setApplicationNameAndVersion(appname, appversion);
    setApplicationShortDescription(appdescription);
    setCopyright(copyright);
    setProjectURL(url);


    // init gui
    init();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::init()
{
    ui->setupUi(this);
    // Conecionts
    // About Qt
    connect(ui->buttonAboutQt, &QPushButton::clicked, qApp, qApp->aboutQt);
}

void AboutDialog::setupAboutTab()
{
    // Set app name
    ui->labelAppName->setText(mAppNameAndVersion);
    // Check the license
    if(mLicense.isEmpty()) {
        mLicense = QStringLiteral("Unknown");
    }

    // Set textbrowser text
    QString browserBuffer = QStringLiteral("%1<br><br>%2<br><br>"
                                           "%3<br><br>License: %4").arg(
        mAppDesc, mCopyright, mProjectURL, mLicense);
    ui->tabAboutTextBrowser->append(browserBuffer);

}

void AboutDialog::setupAuthorsTab()
{
    // Check if empty the Authors List
    if(mAuthorsList.isEmpty()) {
        // Remove the tab Authors
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabAuthors));
    } else {
        for (const auto &author : qAsConst(mAuthorsList)) {
            QString buff = QStringLiteral("%1 \n").arg(author);
            ui->tabAuthorsTextBrowser->append(buff);
        }
    }
}

void AboutDialog::setupTranslatorsTab()
{
    // Check if empty the Trankslators List
    if(mTranslatorsList.isEmpty()) {
        // Remove the tab Translators
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabTranslators));
    } else {
        for (const auto &translator : qAsConst(mTranslatorsList)) {
            QString buff = QStringLiteral("%1 \n").arg(translator);
            ui->tabTranslatorsTextBrowser->append(buff);
        }
    }
}

void AboutDialog::setupThanksToTab()
{
    // Check if empty the Thanks list
    if(mThanksList.isEmpty()) {
        // Remove the tab Thanks
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabThanks));
    } else {
        for (const auto &thanks : qAsConst(mThanksList)) {
            QString buff = QStringLiteral("%1 \n").arg(thanks);
            ui->tabThanksTextBrowser->append(buff);
        }
    }
}

void AboutDialog::setApplicationNameAndVersion(const QString &name,
                                               const QString &version)
{
    // Check if the name  and version are empty
    QString bufname;
    QString bufver;

    if(name.isEmpty()) {
        // Use the aplication name provide for qt
        bufname = qApp->applicationName();
    } else {
        bufname = name;
    }

    if(version.isEmpty()) {
        // if empy, use version 0.1
        bufver = QStringLiteral("0.1");
    } else {
        bufver = version;
    }

    mAppNameAndVersion = QStringLiteral("<b>%1</b><br>%2").arg(
        bufname, bufver);

}

void AboutDialog::setApplicationShortDescription(const QString &desc)
{
    mAppDesc = desc;
}

void AboutDialog::setCopyright(const QString &copyright)
{
    mCopyright = copyright;
}

void AboutDialog::setLicense(LicenseKey key)
{
    switch(key) {
    case LicenseKey::GPL:
        mLicense = QStringLiteral("GPL");
    case LicenseKey::GPL_V2:
        mLicense = QStringLiteral("GPL-2.0");
    case LicenseKey::GPL_V3:
        mLicense = QStringLiteral("GPL-3.0");
    case LicenseKey::LGPL:
        mLicense = QStringLiteral("LGPL");
    case LicenseKey::LGPL_V2:
        mLicense = QStringLiteral("LGPL-2.0");
    case LicenseKey::LGPL_V3:
        mLicense = QStringLiteral("LGPL-3.0");
    case LicenseKey::BSD:
        mLicense = QStringLiteral("BSD-2-Clause");
    case LicenseKey::QPL:
        mLicense = QStringLiteral("QPL");
    }

}

void AboutDialog::setCustomLicense(const QString &name)
{
    mLicense = name;
}

void AboutDialog::setProjectURL(const QString &url)
{
    mProjectURL = url;
}

void AboutDialog::addAuthor(const QString &name)
{
    mAuthorsList.append(name);
}

void AboutDialog::addAuthorsList(const QStringList authorList)
{
    mAuthorsList.append(authorList);
}

void AboutDialog::addTranslator(const QString &name)
{
    mTranslatorsList.append(name);
}

void AboutDialog::addTranslatorsList(const QStringList translatorsList)
{
    mTranslatorsList.append(translatorsList);
}

void AboutDialog::addThanks(const QString &name)
{
    mThanksList.append(name);
}

void AboutDialog::addThanksList(const QStringList thanksList)
{
    mThanksList.append(thanksList);
}

void AboutDialog::configureAboutDialogData()
{
    // Call the setup* methods
    // Configure the about tab
    setupAboutTab();
    // Configure the authors
    setupAuthorsTab();
    // Configure the translators
    setupTranslatorsTab();
    // Configure the thanks
    setupThanksToTab();
}

