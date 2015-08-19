#ifndef QTDOCKWIDGET_H
#define QTDOCKWIDGET_H

#include "QtFlexManager.h"

class DockWidgetImpl;

class QT_FLEX_API DockWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive)
    Q_PROPERTY(Flex::ViewMode viewMode READ viewMode)
private:
    DockWidget(Flex::ViewMode viewMode, QWidget* parent, Qt::WindowFlags flags);
    ~DockWidget();

public:
    Flex::ViewMode viewMode() const;
    void setViewMode(Flex::ViewMode viewMode);

public:
    Flex::Features dockFeatures() const;
    void setDockFeatures(Flex::Features features);
    Flex::Features siteFeatures() const;
    void setSiteFeatures(Flex::Features features);

public:
    QSize sizeHint() const;

public:
    QSize minimumSizeHint() const;

Q_SIGNALS:
    void enterMove(QObject*);
    void leaveMove(QObject*);
    void moving(QObject*);
    void activated(DockWidget*);

public:
    bool isFloating() const;

public:
    bool isActive() const;

protected:
    bool event(QEvent* evt);
    void paintEvent(QPaintEvent*);
#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif

private Q_SLOTS:
    void on_titleBar_buttonClicked(Flex::Button, bool*);

private:
    friend class FlexManager;

private:
    friend class DockWidgetImpl;

private:
    QScopedPointer<DockWidgetImpl> impl;
};

#endif
