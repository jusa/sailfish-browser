/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Petri M. Gerdt <petri.gerdt@jollamobile.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "dbmanager.h"

#include <QMetaObject>

#include "dbworker.h"

static DBManager *gDbManager = 0;

DBManager *DBManager::instance()
{
    if (!gDbManager) {
        gDbManager = new DBManager();
    }
    return gDbManager;
}

DBManager::DBManager(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QList<Tab> >("QList<Tab>");
    qRegisterMetaType<QList<Link> >("QList<Link>");
    qRegisterMetaType<Tab>("Tab");

    worker = new DBWorker();
    worker->moveToThread(&workerThread);

    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(tabsAvailable(QList<Tab>)), this, SIGNAL(tabsAvailable(QList<Tab>)));
    connect(worker, SIGNAL(historyAvailable(QList<Link>)), this, SIGNAL(historyAvailable(QList<Link>)));
    connect(worker, SIGNAL(tabHistoryAvailable(int,QList<Link>,int)), this, SIGNAL(tabHistoryAvailable(int,QList<Link>,int)));
    connect(worker, SIGNAL(titleChanged(QString,QString)), this, SIGNAL(titleChanged(QString,QString)));
    connect(worker, SIGNAL(thumbPathChanged(int,QString)), this, SIGNAL(thumbPathChanged(int,QString)));
    workerThread.start();

    QMetaObject::invokeMethod(worker, "init", Qt::BlockingQueuedConnection);
    QMetaObject::invokeMethod(worker, "getSettings", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(SettingsMap, m_settings));
}

DBManager::~DBManager()
{
    workerThread.exit();
    // Use timeout of 500ms to guaranty we won't block
    workerThread.wait(500);
    gDbManager = 0;
    foreach (QString connectionName, QSqlDatabase::connectionNames()) {
        QSqlDatabase::removeDatabase(connectionName);
    }
}

int DBManager::getMaxTabId()
{
    int maxTabId;
    QMetaObject::invokeMethod(worker, "getMaxTabId", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, maxTabId));
    return maxTabId;
}

void DBManager::createTab(const Tab &tab)
{
    QMetaObject::invokeMethod(worker, "createTab", Qt::QueuedConnection,
                              Q_ARG(Tab, tab));
}

void DBManager::navigateTo(int tabId, QString url, QString title, QString path)
{
    QMetaObject::invokeMethod(worker, "navigateTo", Qt::QueuedConnection,
                              Q_ARG(int, tabId), Q_ARG(QString, url),
                              Q_ARG(QString, title), Q_ARG(QString, path));
}

void DBManager::goForward(int tabId)
{
    QMetaObject::invokeMethod(worker, "goForward", Qt::BlockingQueuedConnection,
                              Q_ARG(int, tabId));
}

void DBManager::goBack(int tabId)
{
    QMetaObject::invokeMethod(worker, "goBack", Qt::BlockingQueuedConnection,
                              Q_ARG(int, tabId));
}

void DBManager::getAllTabs()
{
    QMetaObject::invokeMethod(worker, "getAllTabs", Qt::QueuedConnection);
}

void DBManager::removeTab(int tabId)
{
    QMetaObject::invokeMethod(worker, "removeTab", Qt::QueuedConnection,
                              Q_ARG(int, tabId));
}

void DBManager::removeAllTabs()
{
    QMetaObject::invokeMethod(worker, "removeAllTabs", Qt::BlockingQueuedConnection,
                              Q_ARG(bool, true));
}

void DBManager::updateTitle(int tabId, QString url, QString title)
{
    QMetaObject::invokeMethod(worker, "updateTitle", Qt::QueuedConnection,
                              Q_ARG(int, tabId), Q_ARG(QString, url), Q_ARG(QString, title));
}

void DBManager::updateThumbPath(int tabId, QString path)
{
    QMetaObject::invokeMethod(worker, "updateThumbPath", Qt::QueuedConnection,
                              Q_ARG(int, tabId), Q_ARG(QString, path));
}

void DBManager::removeHistoryEntry(int linkId)
{
    QMetaObject::invokeMethod(worker, "removeHistoryEntry", Qt::QueuedConnection,
                              Q_ARG(int, linkId));
}

void DBManager::clearHistory()
{
    QMetaObject::invokeMethod(worker, "clearHistory", Qt::QueuedConnection);
}

void DBManager::getHistory(const QString &filter)
{
    QMetaObject::invokeMethod(worker, "getHistory", Qt::QueuedConnection, Q_ARG(QString, filter));
}

void DBManager::getTabHistory(int tabId)
{
    QMetaObject::invokeMethod(worker, "getTabHistory", Qt::QueuedConnection, Q_ARG(int, tabId));
}

void DBManager::saveSetting(QString name, QString value)
{
    m_settings.insert(name, value);
    emit settingsChanged();
    QMetaObject::invokeMethod(worker, "saveSetting", Qt::BlockingQueuedConnection,
                              Q_ARG(QString, name), Q_ARG(QString, value));
}

QString DBManager::getSetting(QString name)
{
    if (m_settings.contains(name)) {
        return m_settings.value(name);
    }

    return "";
}

void DBManager::deleteSetting(QString name)
{
    if (m_settings.contains(name)) {
        m_settings.remove(name);
        emit settingsChanged();
        QMetaObject::invokeMethod(worker, "deleteSetting", Qt::BlockingQueuedConnection,
                                  Q_ARG(QString, name));
    }
}
