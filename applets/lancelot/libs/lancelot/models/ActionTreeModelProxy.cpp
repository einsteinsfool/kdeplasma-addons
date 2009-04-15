/*
 *   Copyright (C) 2007 Ivan Cukic <ivan.cukic+kde@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser/Library General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser/Library General Public License for more details
 *
 *   You should have received a copy of the GNU Lesser/Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ActionTreeModelProxy.h"

namespace Lancelot
{

class ActionTreeModelProxy::Private {
public:
    ActionListModel * model;
    QString modelTitle;
    QIcon modelIcon;
};

ActionTreeModelProxy::ActionTreeModelProxy(ActionListModel * model, QString title, QIcon icon)
    : d(new Private())
{
    d->model = model;
    d->modelTitle = title;
    d->modelIcon = icon;

    connect(model, SIGNAL( itemActivated(int) ),
            this,  SIGNAL( itemActivated(int) ));
    connect(model, SIGNAL( itemInserted(int)  ),
            this,  SIGNAL( itemInserted(int)  ));
    connect(model, SIGNAL( itemAltered(int)   ),
            this,  SIGNAL( itemAltered(int)   ));
    connect(model, SIGNAL( itemDeleted(int)   ),
            this,  SIGNAL( itemDeleted(int)   ));
    connect(model, SIGNAL( updated()          ),
            this,  SIGNAL( updated()          ));
}

ActionTreeModelProxy::~ActionTreeModelProxy()
{
    delete d;
}

ActionListModel * ActionTreeModelProxy::model() const
{
    return d->model;
}

// ActionTreeModel
ActionTreeModel * ActionTreeModelProxy::child(int index)
{
    Q_UNUSED(index);
    return NULL;
}

QString ActionTreeModelProxy::modelTitle() const
{
    return d->modelTitle;
}

QIcon ActionTreeModelProxy::modelIcon()  const
{
    return d->modelIcon;
}

// ActionListModel
QString ActionTreeModelProxy::title(int index) const
{
    return d->model->title(index);
}

bool ActionTreeModelProxy::hasContextActions(int index) const
{
    return d->model->hasContextActions(index);
}

void ActionTreeModelProxy::setContextActions(int index, Lancelot::PopupMenu * menu)
{
    d->model->setContextActions(index, menu);
}

void ActionTreeModelProxy::contextActivate(int index, QAction * context)
{
    d->model->contextActivate(index, context);
}

QString ActionTreeModelProxy::description(int index) const
{
    return d->model->description(index);
}

QIcon ActionTreeModelProxy::icon(int index) const
{
    return d->model->icon(index);
}

bool ActionTreeModelProxy::isCategory(int index) const
{
    return d->model->isCategory(index);
}

int ActionTreeModelProxy::size() const
{
    return d->model->size();
}

void ActionTreeModelProxy::activate(int index)
{
    d->model->activated(index);
}

QMimeData * ActionTreeModelProxy::mimeData(int index) const
{
    return d->model->mimeData(index);
}

void ActionTreeModelProxy::setDropActions(int index,
            Qt::DropActions & actions, Qt::DropAction & defaultAction)
{
    d->model->setDropActions(index, actions, defaultAction);
}

} // namespace Lancelot

