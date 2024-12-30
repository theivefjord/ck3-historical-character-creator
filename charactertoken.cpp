#include "charactertoken.h"
#include "qgraphicsscene.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsItem>
#include <QGraphicsObject>

CharacterToken::CharacterToken(CharacterData *characterData, QGraphicsItem *parent)
    : QGraphicsObject(parent), character(characterData)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsScenePositionChanges);
}

QRectF CharacterToken::boundingRect() const
{
    return QRectF(0, 0, 100, 50);
}

void CharacterToken::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::lightGray);
    painter->drawRect(boundingRect());


    QString label;
    if (!character->dynasty.isEmpty() && !character->gender.isEmpty())
        label = QString("%1_%2\n%3\n%4").arg(character->dynasty).arg(character->characterNumber).arg(character->name).arg(character->gender);
    else if (!character->dynasty.isEmpty() && character->gender.isEmpty())
        label = QString("%1_%2\n%3\nmale").arg(character->dynasty).arg(character->characterNumber).arg(character->name);
    else
        label = QString("character_%1").arg(character->characterNumber);

    painter->drawText(boundingRect(), Qt::AlignCenter, label);
}

void CharacterToken::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    QList<QGraphicsItem *> itemsUnderCursor = scene()->items(mapToScene(event->pos()), Qt::IntersectsItemBoundingRect, Qt::DescendingOrder, QTransform());

    for (QGraphicsItem *item : itemsUnderCursor)
    {
        if (item != this)
        {
            CharacterToken *otherToken = dynamic_cast<CharacterToken*>(item);
            if (otherToken)
            {
                QMenu menu;
                QAction *spouseAction = menu.addAction("Set as spouse");
                QAction *setAsParentAction = menu.addAction("Set as parent");
                QAction *selectedAction = menu.exec(event->screenPos());

                if (selectedAction == spouseAction)
                {
                    //qDebug() << "spouse action selected";
                    character->spouseIds.append(otherToken->getCharacterData()->id);
                    otherToken->getCharacterData()->spouseIds.append(character->id);
                    // snap tokens a bit so they're not overlapping
                    this->setPos(otherToken->scenePos().x() - 120, otherToken->scenePos().y());
                    //qDebug() << "emitting spousSet signal for" << character->id + " and" << otherToken->getCharacterData()->id;
                    emit spousesSet(character->id, otherToken->getCharacterData()->id);
                }
                else if (selectedAction == setAsParentAction)
                {
                    if (otherToken->getCharacterData()->gender == "")
                    {
                        character->fatherId = otherToken->getCharacterData()->id;
                        qDebug() << "father acquired";
                    }
                    else if (otherToken->getCharacterData()->gender == "female")
                    {
                        character->motherId = otherToken->getCharacterData()->id;
                        qDebug() << "mother acquired";
                    }
                    emit parentSet(character->id, otherToken->getCharacterData()->id);
                }
                if (selectedAction == spouseAction || selectedAction == setAsParentAction)
                {
                    emit relationshipsChanged();
                }
                break;
            }
        } else {
            if (event->button() == Qt::RightButton){
                QMenu menu;
                QAction *removeSpouseAction = menu.addAction("Remove spouses");
                QAction *removeParentsAction = menu.addAction("Remove Parents");
                QAction *selectedAction = menu.exec(event->screenPos());

                if (selectedAction == removeSpouseAction){
                    qDebug() << "remove Spouse";
                    emit removeSpousesRequested(character->id);
                }
                if (selectedAction == removeParentsAction){
                    emit removeParentsRequested(character->id);
                }
                if (selectedAction == removeSpouseAction || selectedAction == removeParentsAction) {
                    emit relationshipsChanged();
                }
            }
        }
    }
}

QVariant CharacterToken::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        emit tokenMoved(character->id);
    }
    return QGraphicsItem::itemChange(change, value);
}

void CharacterToken::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

CharacterData* CharacterToken::getCharacterData() const
{
    return character;
}
