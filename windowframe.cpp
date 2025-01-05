#include "windowframe.h"
#include "ui_windowframe.h"

#include <windows.h>
#include <windowsx.h>

#include <QMouseEvent>

const QString appIcon           = ":/icons/iconthingy/favicon.ico";
const QString closeIcon         = ":/icons/iconthingy/close_light.png";
const QString maximizeIcon      = ":/icons/iconthingy/maximize_light.png";
const QString minimizeIcon      = ":/icons/iconthingy/minimize_light.png";
const QString defaultSizeIcon   = ":/icons/iconthingy/default_size_light.png";

/// @brief Constructor for the WindowFrame class.
/// @param parent The parent widget.
/// @param child The child widget to be added to the window (optional).
WindowFrame::WindowFrame(QWidget *parent, QWidget *child)
    : QFrame(parent), ui(new Ui::WindowFrame){

    ui->setupUi(this);
    mBorderSize = 5; // for borders that can be dragged to resize, since no frame has to rebuild native function
    initIcons();
    this->setTitle("Untitled");

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    if(child != nullptr) {
        ui->body->layout()->addWidget(child);
        mMainBody = child;
        mMainBody->installEventFilter(this);
        resize(child->size());
    }
    adjustSize();
}

/// @brief Destructor for the WindowFrame class.
WindowFrame::~WindowFrame(){
    delete ui;
}

/// @brief Init frame icons.
void WindowFrame::initIcons(){
    this->setIcon(appIcon);

    ui->close->setIcon(QIcon(closeIcon));
    ui->maximum->setIcon(QIcon(maximizeIcon));
    ui->minimum->setIcon(QIcon(minimizeIcon));
}

/// @brief Show header menu.
/// @param pos position mouse click.
void WindowFrame::showHeaderContextMenu(const QPoint &pos){
    QMenu contextMenu(this);

    QAction *exitAction = contextMenu.addAction(tr("&Exit"));
    connect(exitAction, &QAction::triggered, this, &WindowFrame::close);

    contextMenu.addAction(exitAction);
    contextMenu.exec(mapToGlobal(pos));
}

/// @brief Handler for the "Close" button click signal.
void WindowFrame::on_close_clicked(){
    close();
}

/// @brief Handler for the "Maximize/Restore" button click signal.
void WindowFrame::on_maximum_clicked(){
    if(isMaximized()) {
        ui->maximum->setIcon(QIcon(maximizeIcon));
        showNormal();
    } else {
        ui->maximum->setIcon(QIcon(defaultSizeIcon));
        showMaximized();
    }
}

/// @brief Handler for the "Minimize" button click signal.
void WindowFrame::on_minimum_clicked(){
    showMinimized();
}


/// @brief Handler for the mouse press event.
/// @param event Pointer to the QMouseEvent object containing event information.
void WindowFrame::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        QWidget* widget = childAt(event->pos());
        if(widget == ui->LHeader || widget == ui->title || widget == ui->icon) {
            mPosition.setX(event->pos().x());
            mPosition.setY(event->pos().y());
        }
    }
    if (event->button() == Qt::RightButton ) {
        QWidget* widget = childAt(event->pos());
        if (widget == ui->LHeader || widget == ui->title || widget == ui->icon){
            showHeaderContextMenu(event->pos());
        }
    }
}

/// @brief Handler for the mouse move event within the window.
/// @param event Pointer to the mouse move event object (QMouseEvent).
/// @return No return value.
void WindowFrame::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        if (mPosition.x() != 0 || mPosition.y() != 0) {
            move(event->globalPosition().x() - mPosition.x(), event->globalPosition().y() - mPosition.y());
        }
    }
}

/// @brief Handler for the mouse release event within the window.
/// @param event Pointer to the mouse release event object (QMouseEvent).
void WindowFrame::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    mPosition.setX(0);
    mPosition.setY(0);
}

/// @brief Handler for the mouse double-click event within the window.
/// @param event Pointer to the mouse double-click event object (QMouseEvent).
void WindowFrame::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        QWidget* widget = childAt(event->pos());
        if(widget == ui->LHeader) {
            if(isMaximized()) {
                ui->maximum->setIcon(QIcon(maximizeIcon));
                showNormal();
            } else {
                ui->maximum->setIcon(QIcon(defaultSizeIcon));
                showMaximized();
            }
        }
    }
}

/// @brief Handler for the native window event.
/// @param eventType The type of event, as a byte array (QByteArray).
/// @param message Pointer to a structure containing event information (void*).
/// @param result Pointer to a variable for returning the result (long*).
/// @return The return value, true if the event was handled, otherwise false.
bool WindowFrame::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType)
    MSG *param = static_cast<MSG *>(message);

    if (param->message == WM_NCHITTEST) {
        if (isMaximized()) return false; // dont allow resizing if maximized
        QPoint globalPos(GET_X_LPARAM(param->lParam), GET_Y_LPARAM(param->lParam));
        QPoint localPos = mapFromGlobal(globalPos);

        int nX = localPos.x();
        int nY = localPos.y();

        if (nX >= 0 && nX < mBorderSize) {
            if (nY >= 0 && nY < mBorderSize) {
                *result = HTTOPLEFT;
            } else if (nY >= height() - mBorderSize) {
                *result = HTBOTTOMLEFT;
            } else {
                *result = HTLEFT;
            }
        } else if (nX >= width() - mBorderSize) {
            if (nY >= 0 && nY < mBorderSize) {
                *result = HTTOPRIGHT;
            } else if (nY >= height() - mBorderSize) {
                *result = HTBOTTOMRIGHT;
            } else {
                *result = HTRIGHT;
            }
        } else if (nY >= 0 && nY < mBorderSize) {
            *result = HTTOP;
        } else if (nY >= height() - mBorderSize) {
            *result = HTBOTTOM;
        } else {
            return QWidget::nativeEvent(eventType, message, result);
        }

        return true;
    }

    return QWidget::nativeEvent(eventType, message, result);
}

/// @brief Show or hide the window minimization button.
/// @param enable If true, the button will be shown; if false, it will be hidden.
void WindowFrame::enableMinimum(bool enable) {
    !enable ? ui->minimum->hide() : ui->minimum->show();
}

/// @brief Show or hide the window maximization button.
/// @param enable If true, the button will be shown; if false, it will be hidden.
void WindowFrame::enableMaximum(bool enable) {
    !enable ? ui->maximum->hide() : ui->maximum->show();
}

/// @brief Show or hide the window close button.
/// @param enable If true, the button will be shown; if false, it will be hidden.
void WindowFrame::enableClose(bool enable) {
    !enable ? ui->close->hide() : ui->close->show();
}

/// @brief set window icon
/// @param iconPath - path to icon file
void WindowFrame::setIcon(const QString &iconPath) {
    QPixmap pixmap(iconPath);
    ui->icon->setPixmap(pixmap);
    ui->icon->setScaledContents(true);
    ui->icon->setAlignment(Qt::AlignCenter);
    ui->icon->resize(24, 24);
}

/// @brief set title for the window
/// @param title
void WindowFrame::setTitle(const QString &title) {
    ui->title->setText(title);
    this->setWindowTitle(title);
}

/// @brief Override event filtering function for the WindowFrame class.
/// @param obj Pointer to the object for which the event was generated.
/// @param event Pointer to the QEvent object representing the event.
/// @return `true` if the event was handled, otherwise `false`.
bool WindowFrame::eventFilter(QObject *obj, QEvent *event) {
    if(obj == mMainBody) {
        if(event->type() == QEvent::HideToParent) {
            hide();
            return true;
        }
        if(event->type() == QEvent::ShowToParent) {
            show();
            return true;
        }
        return QObject::eventFilter(obj, event);
    } else {
        return QFrame::eventFilter(obj, event);
    }
    return false;
}


