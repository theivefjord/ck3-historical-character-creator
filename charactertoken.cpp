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
    if (!character->dynasty.isEmpty())
        label = QString("%1_%2").arg(character->dynasty).arg(character->characterNumber);
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
                QAction *childAction = menu.addAction("Set as child");
                QAction *selectedAction = menu.exec(event->screenPos());

                if (selectedAction == spouseAction)
                {
                    character->spouseIds.append(otherToken->getCharacterData()->id);
                    otherToken->getCharacterData()->spouseIds.append(character->id);
                }
                else if (selectedAction == childAction)
                {
                    character->childrenIds.append(otherToken->getCharacterData()->id);
                    if (character->gender == "male")
                    {
                        otherToken->getCharacterData()->fatherId = character->id;
                    }
                    else if (character->gender == "female")
                    {
                        otherToken ->getCharacterData()->motherId = character->id;
                    }
                }
                if (selectedAction == spouseAction || selectedAction == childAction)
                {
                    emit relationshipsChanged();
                }
                break;
            }
        }
    }
}

void CharacterToken::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

CharacterData* CharacterToken::getCharacterData() const
{
    return character;
}
