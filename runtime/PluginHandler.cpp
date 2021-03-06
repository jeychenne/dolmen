/*
 * DMScriptApi.cpp
 *
 * Copyright (C) 2010-2017 Julien Eychenne
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose: Top-level scripting API
 * Created: 06/03/2012
 */


#include "PluginHandler.h"

PluginHandler::PluginHandler(QWidget *parent, sol::state_view lua) :
    QObject(parent), m_lua(lua)
{
    setObjectName("Dolmen");
    m_widget = parent;
    m_mainPlugin = nullptr;
}

PluginHandler::~PluginHandler()
{

}

Plugin* PluginHandler::mainPlugin() const
{
    return m_mainPlugin;
}

QList<Plugin*> PluginHandler::plugins() const
{
    return m_plugins;
}

void PluginHandler::setMainPlugin(Plugin *p)
{
    m_mainPlugin = p;
}

void PluginHandler::addPlugin(Plugin *p)
{
    if (!m_plugins.contains(p))
    {
        m_plugins.append(p);
        //connect(p, SIGNAL(scriptCalled(QString,QString)), this, SLOT(executeScript(QString,QString)));
    }
}

void PluginHandler::executeMainPlugin()
{
    if (m_mainPlugin) {
        QString script_name("main.lua");
        QString script = m_mainPlugin->scriptContent(script_name);

        if (!script.isEmpty()) {
            executeScript(script_name, script);
        }
    }
}

void PluginHandler::executeScript(const QString &script, const QString &content)
{
    if (content.isEmpty()) {
        return;
    }

    try {
         m_lua.script(content.toStdString());
    }
    catch (std::exception &e) {
        QMessageBox::critical(m_widget, tr("Runtime error"), tr("Script \"%1\" returned the following error: %2").arg(script).arg(e.what()));
    }
}

