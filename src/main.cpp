/*
    SPDX-License-Identifier: GPL-3.0-or-later
    SPDX-FileCopyrightText: 2025 J <jean.chalard@gmail.com>
*/

#include <QApplication>
#include <QtGlobal>

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>

#include "app.h"
#include "version-manette.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "evdevcontroller.h"

#include "manetteconfig.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(u"org.kde.desktop"_s);
    }

    KLocalizedString::setApplicationDomain("manette");
    QCoreApplication::setOrganizationName(u"KDE"_s);

    KAboutData aboutData(
        // The program name used internally.
        u"manette"_s,
        // A displayable program name string.
        i18nc("@title", "manette"),
        // The program version string.
        QStringLiteral(MANETTE_VERSION_STRING),
        // Short description of what the app does.
        i18n("Application Description"),
        // The license this code is released under.
        KAboutLicense::GPL_V3,
        // Copyright Statement.
        i18n("(c) 2025"));
    aboutData.addAuthor(i18nc("@info:credit", "J"), i18nc("@info:credit", "Maintainer"), u"jean.chalard@gmail.com"_s, u""_s);
    //    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    KAboutData::setApplicationData(aboutData);
    QGuiApplication::setWindowIcon(QIcon::fromTheme(u"org.j.manette"_s));

    qmlRegisterType<EvdevController>("org.j.manette", 1, 0, "EvdevController");

    QQmlApplicationEngine engine;

    auto config = manetteConfig::self();

    qmlRegisterSingletonInstance("org.j.manette.private", 1, 0, "Config", config);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.j.manette", u"Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
