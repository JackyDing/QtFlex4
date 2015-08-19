#include "QtFlexManager.h"
#include "QtFlexWidget.h"
#include "QtDockWidget.h"
#include "QtDockSite.h"
#include <QVariant>
#include <QIcon>
#include <QApplication>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

int Flex::Update = QEvent::registerEventType();

WId topLevelWindowAt(QWidget* widget, const QPoint& pos)
{
    WId hTmp = NULL;
#ifdef Q_OS_WIN
    HWND hWnd = GetWindow(reinterpret_cast<HWND>(widget->effectiveWinId()), GW_HWNDNEXT);

    while (hWnd != nullptr && reinterpret_cast<HWND>(hTmp) == nullptr)
    {
        POINT pnt = { pos.x(), pos.y() };
        ScreenToClient(hWnd, &pnt);
        hTmp = reinterpret_cast<WId>(ChildWindowFromPoint(hWnd, pnt));
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }

    hTmp = reinterpret_cast<WId>(GetAncestor(reinterpret_cast<HWND>(hTmp), GA_ROOT));
#endif
    return hTmp;
}

DockSite* getDockSite(QWidget* widget)
{
    for (DockSite* site = nullptr; widget && !site; widget = widget->parentWidget())
    {
        if ((site = qobject_cast<DockSite*>(widget)) != nullptr)
        {
            return site;
        }
    }
    return nullptr;
}

class FlexManagerImpl
{
public:
    FlexManagerImpl() : _ready(false)
    {
    }
public:
    QList<FlexWidget*> _flexWidgets;
    QList<DockWidget*> _dockWidgets;
    bool _ready;
    QList<QIcon> _buttonIcons;
};

FlexManager::FlexManager() : impl(new FlexManagerImpl)
{
    Q_ASSERT(qApp != nullptr);
    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), SLOT(on_app_focusChanged(QWidget*, QWidget*)));
    connect(this, SIGNAL(guiderHover(FlexWidget*, QWidget*)), SLOT(on_flexWidget_guiderHover(FlexWidget*, QWidget*)));
    connect(this, SIGNAL(guiderShow(FlexWidget*, QWidget*)), SLOT(on_flexWidget_guiderShow(FlexWidget*, QWidget*)));
    connect(this, SIGNAL(guiderHide(FlexWidget*, QWidget*)), SLOT(on_flexWidget_guiderHide(FlexWidget*, QWidget*)));
    connect(this, SIGNAL(guiderDrop(FlexWidget*, DockWidget*)), SLOT(on_flexWidget_guiderDrop(FlexWidget*, DockWidget*)));
    connect(this, SIGNAL(guiderDrop(FlexWidget*, FlexWidget*)), SLOT(on_flexWidget_guiderDrop(FlexWidget*, FlexWidget*)));
    QPixmap extentsPixmap(":/Resources/extents.png");
    for (int i = 0; i < 3; i++)
    {
        QIcon icon;
        icon.addPixmap(extentsPixmap.copy(i * 16,  0, 16, 16), QIcon::Active, QIcon::On);
        icon.addPixmap(extentsPixmap.copy(i * 16, 16, 16, 16), QIcon::Active, QIcon::Off);
        icon.addPixmap(extentsPixmap.copy(i * 16, 32, 16, 16), QIcon::Normal, QIcon::Off);
        impl->_buttonIcons.append(icon);
    }
    QPixmap buttonsPixmap(":/Resources/buttons.png");
    for (int i = 0; i < 4; i++)
    {
        QIcon icon;
        icon.addPixmap(buttonsPixmap.copy(i * 16,  0, 16, 16), QIcon::Active, QIcon::On);
        icon.addPixmap(buttonsPixmap.copy(i * 16, 16, 16, 16), QIcon::Active, QIcon::Off);
        icon.addPixmap(buttonsPixmap.copy(i * 16, 32, 16, 16), QIcon::Normal, QIcon::Off);
        impl->_buttonIcons.append(icon);
    }
    
}

FlexManager::~FlexManager()
{

}

FlexManager* FlexManager::instance()
{
    static FlexManager manager; return &manager;
}

FlexWidget* FlexManager::createFlexWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags)
{
    FlexWidget* widget = new FlexWidget(viewMode, parent, flags);
    connect(widget, SIGNAL(destroyed(QObject*)), SLOT(on_flexWidget_destroyed(QObject*)));
    connect(widget, SIGNAL(enterMove(QObject*)), SLOT(on_flexWidget_enterMove(QObject*)));
    connect(widget, SIGNAL(leaveMove(QObject*)), SLOT(on_flexWidget_leaveMove(QObject*)));
    connect(widget, SIGNAL(moving(QObject*)), SLOT(on_flexWidget_moving(QObject*)));
    impl->_flexWidgets.append(widget);
    return widget;
}

DockWidget* FlexManager::createDockWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags)
{
    DockWidget* widget = new DockWidget(viewMode, parent, flags);
    connect(widget, SIGNAL(destroyed(QObject*)), SLOT(on_dockWidget_destroyed(QObject*)));
    connect(widget, SIGNAL(enterMove(QObject*)), SLOT(on_dockWidget_enterMove(QObject*)));
    connect(widget, SIGNAL(leaveMove(QObject*)), SLOT(on_dockWidget_leaveMove(QObject*)));
    connect(widget, SIGNAL(moving(QObject*)), SLOT(on_dockWidget_moving(QObject*)));
    impl->_dockWidgets.append(widget);
    return widget;
}

QIcon FlexManager::icon(Flex::Button button)
{
    return impl->_buttonIcons[button];
}

void FlexManager::on_dockWidget_destroyed(QObject* widget)
{
    impl->_dockWidgets.removeOne(static_cast<DockWidget*>(widget));
}

void FlexManager::on_flexWidget_destroyed(QObject* widget)
{
    impl->_flexWidgets.removeOne(static_cast<FlexWidget*>(widget));
}

void FlexManager::on_flexWidget_guiderShow(FlexWidget* flexWidget, QWidget* widget)
{
    Q_ASSERT(widget != nullptr); 
#ifdef Q_OS_WIN
    SetWindowPos(flexWidget->effectiveWinId(), widget->effectiveWinId(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
#endif
    flexWidget->showGuider(widget);
}

void FlexManager::on_flexWidget_guiderHide(FlexWidget* flexWidget, QWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    flexWidget->hideGuider(widget);
}

void FlexManager::on_flexWidget_guiderHover(FlexWidget* flexWidget, QWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    flexWidget->hoverGuider(widget);
}

void FlexManager::on_flexWidget_guiderDrop(FlexWidget* flexWidget, DockWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    if (flexWidget->dropGuider(widget))
    {
        flexWidget->raise();
    }
    else
    {
#ifdef Q_OS_WIN
        SetWindowPos(widget->effectiveWinId(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | 1800);
#endif
    }
}

void FlexManager::on_flexWidget_guiderDrop(FlexWidget* flexWidget, FlexWidget* widget)
{
    Q_ASSERT(widget != nullptr);
    if (flexWidget->dropGuider(widget))
    {
#ifdef Q_OS_WIN
        SetWindowPos(flexWidget->effectiveWinId(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | 1800);
#endif
    }
    else
    {
#ifdef Q_OS_WIN
        SetWindowPos(widget->effectiveWinId(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | 1800);
#endif
    }
}

void FlexManager::on_flexWidget_enterMove(QObject*)
{
    impl->_ready = true;
}

void FlexManager::on_flexWidget_leaveMove(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<FlexWidget*>(object);

    auto pos = QCursor::pos();

    auto wnd = topLevelWindowAt(widget, pos);

    impl->_ready = false;

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget == widget)
        {
            continue;
        }

        if (flexWidget->window()->effectiveWinId() == wnd && flexWidget->rect().contains(flexWidget->mapFromGlobal(pos)))
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderDrop(flexWidget, widget); break;
                }
                else
                {
                    emit guiderHide(flexWidget, widget); break;
                }
            }
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget); break;
            }
        }
    }
}

void FlexManager::on_flexWidget_moving(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<FlexWidget*>(object);

    auto pos = QCursor::pos();

    auto top = topLevelWindowAt(widget, pos);

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget == widget)
        {
            continue;
        }

        if (flexWidget->window()->effectiveWinId() == top && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderHover(flexWidget, widget);
                }
            }
            else
            {
                emit guiderShow(flexWidget, widget);
            }
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget);
            }
        }
    }
}

void FlexManager::on_dockWidget_enterMove(QObject*)
{
    impl->_ready = true;
}

void FlexManager::on_dockWidget_leaveMove(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<DockWidget*>(object);

    auto pos = QCursor::pos();

    auto top = topLevelWindowAt(widget, pos);

    impl->_ready = false;

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget->window()->effectiveWinId() == top && flexWidget->rect().contains(flexWidget->mapFromGlobal(pos)))
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderDrop(flexWidget, widget); break;
                }
                else
                {
                    emit guiderHide(flexWidget, widget); break;
                }
            }
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget); break;
            }
        }
    }
}

void FlexManager::on_dockWidget_moving(QObject* object)
{
    if (!impl->_ready)
    {
        return;
    }

    auto widget = static_cast<DockWidget*>(object);

    auto pos = QCursor::pos();

    auto wnd = topLevelWindowAt(widget, pos);

    for (auto iter = impl->_flexWidgets.begin(); iter != impl->_flexWidgets.end(); ++iter)
    {
        auto flexWidget = *iter;

        if (flexWidget->window()->effectiveWinId() == wnd && flexWidget->isDockAllowed(widget, flexWidget->mapFromGlobal(pos)))
        {
            if (flexWidget->isGuiderExists())
            {
                if (flexWidget->isGuiderVisible())
                {
                    emit guiderHover(flexWidget, widget);
                }
            }
            else
            {
                emit guiderShow(flexWidget, widget);
            }
        }
        else
        {
            if (flexWidget->isGuiderExists())
            {
                emit guiderHide(flexWidget, widget);
            }
        }
    }
}

void FlexManager::on_app_focusChanged(QWidget* old, QWidget* now)
{
    FlexWidget* flexWidget = nullptr;

    auto oldDockSite = getDockSite(old);
    auto nowDockSite = getDockSite(now);

    if (nowDockSite != nullptr && oldDockSite != nowDockSite)
    {
        if (oldDockSite)
        {
            oldDockSite->setActive(false);
        }

        if (!nowDockSite->isActive())
        {
            if ((flexWidget = nowDockSite->flexWidget()) != nullptr)
            {
                flexWidget->setCurrent(nowDockSite);
            }
            if (nowDockSite->isActive())
            {
                emit dockSiteActivated(nowDockSite);
            }
        }
    }
}

QWidget* Flex::window()
{
    return qobject_cast<QWidget*>(qApp->property("window").value<QObject*>());
}
