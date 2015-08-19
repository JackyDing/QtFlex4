#ifndef QTDOCKSITE_H
#define QTDOCKSITE_H

#include "QtFlexManager.h"

class DockSiteImpl;

class QT_FLEX_API DockSite : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive)
    Q_PROPERTY(Flex::ViewMode viewMode READ viewMode)
public:
    DockSite(DockWidget* widget = nullptr, QSize baseSize = QSize(0, 0), QWidget* parent = nullptr);
    ~DockSite();

public:
    bool addWidget(DockWidget* widget);

public:
    int currentIndex() const;

public:
    int count() const;

public:
    DockWidget* widget(int index) const;

public:
    QList<DockWidget*> widgets() const;

public:
    void removeWidgetAt(int index);

public:
    Flex::ViewMode viewMode() const;

public:
    Flex::DockMode dockMode() const;

public:
    void setDockMode(Flex::DockMode dockMode);

public:
    Flex::Features features() const;

public:
    FlexWidget* flexWidget() const;

public:
    bool isActive() const;

public:
    void setActive(bool active);

public:
    void activate();

protected:
    bool event(QEvent*);
    bool eventFilter(QObject*, QEvent*);
    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);

private Q_SLOTS:
    void on_tabBar_currentChanged(int index);
    void on_tabBar_tabCloseRequested(int index);
    void on_tabMdi_currentChanged(int index);
    void on_tabMdi_widgetRemoved(int index);
    void on_titleBar_buttonClicked(Flex::Button, bool*);

private:
    friend class DockSiteImpl;

private:
    QScopedPointer<DockSiteImpl> impl;
};

#endif
