#include "tuio_handler.h"

#include <QHeaderView>
#include <QHostAddress>

#include "qtuiocursor_p.h"
#include "qtuiotoken_p.h"
#include "qoscbundle_p.h"
#include "qoscmessage_p.h"
/*
        UdpClient *client_;
        QMap<int, QTuioCursor> active_cursors_;
        QVector<QTuioCursor> dead_cursors_;
        QMap<int, QTuioToken> active_tokens_;
        QVector<QTuioToken> dead_tokens_;
*/
TuioHandler::TuioHandler(QObject *parent)
    : QObject(parent)
    , client_(0)
    , active_cursors_()
    , dead_cursors_()
    , active_tokens_()
    , dead_tokens_()
    , cursor_model_(0)
    , cursor_debug_view_(0)
    , token_model_(0)
    , token_debug_view_(0)
{
    // todo change this
    client_ = new UdpClient(3333, QHostAddress::LocalHost);

    connect(client_, &UdpClient::messageReceived,
            this, &TuioHandler::processPackets);

    initModels();
}

TuioHandler::TuioHandler(const QHostAddress &ip, unsigned port, QObject *parent)
    : QObject(parent)
    , client_(0)
    , active_cursors_()
    , dead_cursors_()
    , active_tokens_()
    , dead_tokens_()
    , cursor_model_(0)
    , cursor_debug_view_(0)
    , token_model_(0)
    , token_debug_view_(0)
{
    // todo change this
    client_ = new UdpClient(port, ip);

    connect(client_, &UdpClient::messageReceived,
            this, &TuioHandler::processPackets);

    initModels();
}

TuioHandler::~TuioHandler()
{
    if(cursor_debug_view_)
        cursor_debug_view_->deleteLater();
    if(token_debug_view_)
        token_debug_view_->deleteLater();
}

void TuioHandler::processPackets(const QByteArray& datagram, const QHostAddress& sender, unsigned sender_port)
{
    Q_UNUSED(sender);
    Q_UNUSED(sender_port);
    // "A typical TUIO bundle will contain an initial ALIVE message,
    // followed by an arbitrary number of SET messages that can fit into the
    // actual bundle capacity and a concluding FSEQ message. A minimal TUIO
    // bundle needs to contain at least the compulsory ALIVE and FSEQ
    // messages. The FSEQ frame ID is incremented for each delivered bundle,
    // while redundant bundles can be marked using the frame sequence ID
    // -1."
    QVector<QOscMessage> messages;
    QOscBundle bundle(datagram);
    if (bundle.isValid()) {
        messages = bundle.messages();
    } else {
        QOscMessage msg(datagram);
        if (!msg.isValid()) {
            qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
            qDebug() << "  >" << "Got invalid datagram.";
            return;
        }
        messages.push_back(msg);
    }

    for (const QOscMessage &message : messages) {
        if (message.addressPattern() == "/tuio/2Dcur") {

            QList<QVariant> arguments = message.arguments();
            if (arguments.count() == 0) {
                qWarning("Ignoring TUIO message with no arguments");
                return;
            }

            QByteArray message_type = arguments.at(0).toByteArray();
            if (message_type == "source") {
                process2DCurSource(message);
            } else if (message_type == "alive") {
                process2DCurAlive(message);
            } else if (message_type == "set") {
                process2DCurSet(message);
            } else if (message_type == "fseq") {
                process2DCurFseq(message);
            } else {
                qWarning() << "Ignoring unknown TUIO message type: " << message_type;
                return;
            }
        } else if (message.addressPattern() == "/tuio/2Dobj") {
            QList<QVariant> arguments = message.arguments();
            if (arguments.count() == 0) {
                qWarning("Ignoring TUIO message with no arguments");
                return;
            }

            QByteArray message_type = arguments.at(0).toByteArray();
            if (message_type == "source") {
                process2DObjSource(message);
            } else if (message_type == "alive") {
                process2DObjAlive(message);
            } else if (message_type == "set") {
                process2DObjSet(message);
            } else if (message_type == "fseq") {
                process2DObjFseq(message);
            } else {
                qWarning() << "Ignoring unknown TUIO message type: " << message_type;
                return;
            }
        } else {
            qWarning() << "Ignoring unknown address pattern " << message.addressPattern();
            return;
        }
    }
}

void TuioHandler::process2DCurSource(const QOscMessage &message)
{
    QList<QVariant> arguments = message.arguments();
    if (arguments.count() != 2) {
        qWarning() << "Ignoring malformed TUIO source message: " << arguments.count();
        return;
    }

    if (QMetaType::Type(arguments.at(1).type()) != QMetaType::QByteArray) {
        qWarning("Ignoring malformed TUIO source message (bad argument type)");
        return;
    }

//    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
//    qDebug() << "  >" << "Got TUIO source message from: " << arguments.at(1).toByteArray();
}

void TuioHandler::process2DCurAlive(const QOscMessage &message)
{
    QList<QVariant> arguments = message.arguments();

    // delta the notified cursors that are active, against the ones we already
    // know of.
    //
    // TBD: right now we're assuming one 2Dcur alive message corresponds to a
    // new data source from the input. is this correct, or do we need to store
    // changes and only process the deltas on fseq?
    QMap<int, QTuioCursor> old_active_cursors = active_cursors_;
    QMap<int, QTuioCursor> new_active_cursors;

    for (int i = 1; i < arguments.count(); ++i) {
        if (QMetaType::Type(arguments.at(i).type()) != QMetaType::Int) {
            qWarning() << "Ignoring malformed TUIO alive message (bad argument on position" << i << arguments << ')';
            return;
        }

        int cursor_id = arguments.at(i).toInt();
        if (!old_active_cursors.contains(cursor_id)) {
            // newly active
            QTuioCursor cursor(cursor_id);
            cursor.setState(Qt::TouchPointPressed);
            new_active_cursors.insert(cursor_id, cursor);
        } else {
            // we already know about it, remove it so it isn't marked as released
            QTuioCursor cursor = old_active_cursors.value(cursor_id);
            cursor.setState(Qt::TouchPointStationary); // position change in SET will update if needed
            new_active_cursors.insert(cursor_id, cursor);
            old_active_cursors.remove(cursor_id);
        }
    }

    // anything left is dead now
    QMap<int, QTuioCursor>::ConstIterator it = old_active_cursors.constBegin();

    // deadCursors should be cleared from the last FSEQ now
    dead_cursors_.reserve(old_active_cursors.size());

    // TODO: there could be an issue of resource exhaustion here if FSEQ isn't
    // sent in a timely fashion. we should probably track message counts and
    // force-flush if we get too many built up.
    while (it != old_active_cursors.constEnd()) {
        dead_cursors_.append(it.value());
        ++it;
    }

    active_cursors_ = new_active_cursors;
}

void TuioHandler::process2DCurSet(const QOscMessage &message)
{
    QList<QVariant> arguments = message.arguments();
    if (arguments.count() < 7) {
        qWarning() << "Ignoring malformed TUIO set message with too few arguments: " << arguments.count();
        return;
    }

    if (QMetaType::Type(arguments.at(1).type()) != QMetaType::Int   ||
            QMetaType::Type(arguments.at(2).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(3).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(4).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(5).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(6).type()) != QMetaType::Float
            ) {
        qWarning() << "Ignoring malformed TUIO set message with bad types: " << arguments;
        return;
    }

    int cursor_id = arguments.at(1).toInt();
    float x = arguments.at(2).toFloat();
    float y = arguments.at(3).toFloat();
    float vx = arguments.at(4).toFloat();
    float vy = arguments.at(5).toFloat();
    float acceleration = arguments.at(6).toFloat();

    QMap<int, QTuioCursor>::Iterator it = active_cursors_.find(cursor_id);
    if (it == active_cursors_.end()) {
        qWarning() << "Ignoring malformed TUIO set for nonexistent cursor " << cursor_id;
        return;
    }

//    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
//    qDebug() << "  >" << "Processing SET for " << cursorId << " x: " << x << y << vx << vy << acceleration;

    QTuioCursor &cur = *it;
    cur.setX(x);
    cur.setY(y);
    cur.setVX(vx);
    cur.setVY(vy);
    cur.setAcceleration(acceleration);
}


void TuioHandler::process2DCurFseq(const QOscMessage &message)
{
    Q_UNUSED(message);
    emit cursorEvent(active_cursors_, dead_cursors_);
    dead_cursors_.clear();
}

void TuioHandler::process2DObjSource(const QOscMessage &message)
{
    QList<QVariant> arguments = message.arguments();
    if (arguments.count() != 2) {
        qWarning() << "Ignoring malformed TUIO source message: " << arguments.count();
        return;
    }

    if (QMetaType::Type(arguments.at(1).type()) != QMetaType::QByteArray) {
        qWarning("Ignoring malformed TUIO source message (bad argument type)");
        return;
    }
}

void TuioHandler::process2DObjAlive(const QOscMessage &message)
{
    QList<QVariant> arguments = message.arguments();

    // delta the notified tokens that are active, against the ones we already
    // know of.
    //
    // TBD: right now we're assuming one 2DObj alive message corresponds to a
    // new data source from the input. is this correct, or do we need to store
    // changes and only process the deltas on fseq?
    QMap<int, QTuioToken> old_active_tokens = active_tokens_;
    QMap<int, QTuioToken> new_active_tokens;

    for (int i = 1; i < arguments.count(); ++i) {
        if (QMetaType::Type(arguments.at(i).type()) != QMetaType::Int) {
            qWarning() << "Ignoring malformed TUIO alive message (bad argument on position" << i << arguments << ')';
            return;
        }

        int session_id = arguments.at(i).toInt();
        if (!old_active_tokens.contains(session_id)) {
            // newly active
            QTuioToken token(session_id);
            token.setState(Qt::TouchPointPressed);
            new_active_tokens.insert(session_id, token);
        } else {
            // we already know about it, remove it so it isn't marked as released
            QTuioToken token = old_active_tokens.value(session_id);
            token.setState(Qt::TouchPointStationary); // position change in SET will update if needed
            new_active_tokens.insert(session_id, token);
            old_active_tokens.remove(session_id);
        }
    }

    // anything left is dead now
    QMap<int, QTuioToken>::ConstIterator it = old_active_tokens.constBegin();

    // deadTokens should be cleared from the last FSEQ now
    dead_tokens_.reserve(old_active_tokens.size());

    // TODO: there could be an issue of resource exhaustion here if FSEQ isn't
    // sent in a timely fashion. we should probably track message counts and
    // force-flush if we get too many built up.
    while (it != old_active_tokens.constEnd()) {
        dead_tokens_.append(it.value());
        ++it;
    }

    active_tokens_ = new_active_tokens;
}

void TuioHandler::process2DObjSet(const QOscMessage &message)
{
    QList<QVariant> arguments = message.arguments();
    if (arguments.count() < 7) {
        qWarning() << "Ignoring malformed TUIO set message with too few arguments: " << arguments.count();
        return;
    }

    if (QMetaType::Type(arguments.at(1).type()) != QMetaType::Int ||
            QMetaType::Type(arguments.at(2).type()) != QMetaType::Int ||
            QMetaType::Type(arguments.at(3).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(4).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(5).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(6).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(7).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(8).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(9).type()) != QMetaType::Float ||
            QMetaType::Type(arguments.at(10).type()) != QMetaType::Float) {
        qWarning() << "Ignoring malformed TUIO set message with bad types: " << arguments;
        return;
    }

    int id = arguments.at(1).toInt();
    int class_id = arguments.at(2).toInt();
    float x = arguments.at(3).toFloat();
    float y = arguments.at(4).toFloat();
    float angle = arguments.at(5).toFloat();
    float vx = arguments.at(6).toFloat();
    float vy = arguments.at(7).toFloat();
    float angular_velocity = arguments.at(8).toFloat();
    float acceleration = arguments.at(9).toFloat();
    float angular_acceleration = arguments.at(10).toFloat();

    QMap<int, QTuioToken>::Iterator it = active_tokens_.find(id);
    if (it == active_tokens_.end()) {
        qWarning() << "Ignoring malformed TUIO set for nonexistent token " << class_id;
        return;
    }

    QTuioToken &tok = *it;
    tok.setClassId(class_id);
    tok.setX(x);
    tok.setY(y);
    tok.setVX(vx);
    tok.setVY(vy);
    tok.setAcceleration(acceleration);
    tok.setAngle(angle);
    tok.setAngularVelocity(angular_velocity);
    tok.setAngularAcceleration(angular_acceleration);
}

void TuioHandler::process2DObjFseq(const QOscMessage &message)
{
    Q_UNUSED(message);

    emit tokenEvent(active_tokens_, dead_tokens_);
    dead_tokens_.clear();
}

void TuioHandler::initModels()
{
    token_model_ = new TuioTokenTableModel(this);
    connect(this, &TuioHandler::tokenEvent,
            token_model_, &TuioTokenTableModel::onTokenEvent);
    token_debug_view_ = new QTableView;
    token_debug_view_->setMinimumSize(600, 337);
    token_debug_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    token_debug_view_->setWindowTitle(tr("QTuioToken Debug View"));
    token_debug_view_->setModel(token_model_);
    token_debug_view_->show();

    cursor_model_ = new TuioCursorTableModel(this);
    connect(this, &TuioHandler::cursorEvent,
            cursor_model_, &TuioCursorTableModel::onCursorEvent);
    cursor_debug_view_ = new QTableView;
    cursor_debug_view_->setMinimumSize(600, 337);
    cursor_debug_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cursor_debug_view_->setWindowTitle(tr("QTuioCursor Debug View"));
    cursor_debug_view_->setModel(cursor_model_);
    cursor_debug_view_->show();
}
