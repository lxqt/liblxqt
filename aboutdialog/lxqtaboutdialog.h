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

#ifndef LXQTABOUTDIALOG_HPP
#define LXQTABOUTDIALOG_HPP

#include <QDialog>
#include <QString>

#include "lxqtglobals.h"

namespace Ui {
class AboutDialog;
}

namespace LXQt {

enum LicenseKey {
    GPL,
    GPL_V2,
    GPL_V3,
    LGPL,
    LGPL_V2,
    LGPL_V3,
    BSD,
    QPL
};

/**
 * @brief The AboutDialog class provide a standard about dialog for
 * all LXQt apps.
 */
class LXQT_API AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(const QString &appname = QString(),
                         const QString &appversion = QString(),
                         const QString &appdescription = QString(),
                         const QString &copyright = QString(),
                         const QString &url = QString(),
                         QWidget *parent = nullptr);

    ~AboutDialog();

    /**
     * @brief Set the application name and the version.
     *
     * @param name
     * @param version
     */
    void setApplicationNameAndVersion(const QString &name,
                                      const QString &version);

    /**
     * @brief Set the application description.
     *
     * @param desc
     */
    void setApplicationShortDescription(const QString &desc);

    /**
     * @brief Insert copyright statement.
     *
     * @param copyright
     */
    void setCopyright(const QString &copyright);

    /**
     * @brief Set a license.
     *
     * @param key
     */
    void setLicense(LicenseKey key);

    /**
     * @brief Set a custom license.
     *
     * @param url
     */
    void setCustomLicense(const QString &name);

    /**
     * @brief Set the project url.
     *
     * @param url
     */
    void setProjectURL(const QString &url);

    /**
     * @brief Add an author.
     *
     * If any author arent added, this arent show.
     * The authors syntax is:
     * name <email>
     *
     * @param name Author name and email
     */
    void addAuthor(const QString &name);

    /**
     * @brief Add a list of authors.
     *
     * @param authorList List of authors
     */
    void addAuthorsList(const QStringList authorList);

    /**
     * @brief Add a translator.
     *
     * If any translator arent added, this arent show
     *
     * @param name Translator name and email
     */
    void addTranslator(const QString &name);

    /**
     * @brief Add a list of translators.
     *
     * If any translator arent added, this arent show
     *
     * @param translatorsList List of translators
     */
    void addTranslatorsList(const QStringList translatorsList);

    /**
     * @brief Add thanks to an contributor.
     *
     * @param name Contributor name and rason
     */
    void addThanks(const QString &name);

    /**
     * @brief Add a thanks list of contributors.
     *
     * @param thanksList List of contributors
     */
    void addThanksList(const QStringList thanksList);

    /**
     * @brief Configure the about data.
     *
     * Is olbigatory call it it, After your
     * configure the about data
     */
    void configureAboutDialogData();

private:
    /// Init the about dialog ui
    void init();
    /// Insert all "about" buffers in About tab
    void setupAboutTab();
    /// Insert authors buffer in Authors tab
    void setupAuthorsTab();
    /// Insert translator buffer in Translator tab
    void setupTranslatorsTab();
    /// Insert thanks list buffer in Thanks To tab
    void setupThanksToTab();

    Ui::AboutDialog *ui;

    // Buffers
    // Tab About
    QString mAppNameAndVersion;
    QString mAppDesc;
    QString mCopyright;
    QString mProjectURL;
    QString mLicense;
    // Tab Authors
    QStringList mAuthorsList;
    // Tab Translators
    QStringList mTranslatorsList;
    // Tab Thanks
    QStringList mThanksList;

};

} // namespace LXQt

#endif // LXQTABOUTDIALOG_HPP
