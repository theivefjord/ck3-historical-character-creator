#include "charactertoken.h"
#include "qgraphicsscene.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsItem>
#include <QGraphicsObject>
// define charactertoken and set flags
CharacterToken::CharacterToken(CharacterData *characterData, QGraphicsItem *parent)
    : QGraphicsObject(parent), character(characterData)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsScenePositionChanges);
}
// define the size/shape of the tokens in the family tree scene
QRectF CharacterToken::boundingRect() const
{
    return QRectF(0, 0, 100, 50);
}
// paint the tokens and write on them
void CharacterToken::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(QColor(78, 78, 78));
    painter->setPen(QColor(54, 54, 54));
    painter->drawRect(boundingRect());


    QString label;
    if (!character->dynasty.isEmpty() && !character->gender.isEmpty())
        label = QString("%1_%2\n%3\n%4").arg(character->dynasty).arg(character->characterNumber).arg(character->name).arg(character->gender);
    else if (!character->dynasty.isEmpty() && character->gender.isEmpty())
        label = QString("%1_%2\n%3\nmale").arg(character->dynasty).arg(character->characterNumber).arg(character->name);
    else
        label = QString("character_%1").arg(character->characterNumber);

    painter->setPen(QColor(255, 255, 255));
    painter->drawText(boundingRect(), Qt::AlignCenter, label);
}
// this function has all the token functionality when in the family tree view
// to connect two tokens you drag one on top of the other and release it, which opens the context menu
// this is kinda clunky sometimes but I learned as I went and it works so fuck it we ball
// right clicking tokens opens the context menu to remove spouses/parents
void CharacterToken::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    QList<QGraphicsItem *> itemsUnderCursor = scene()->items(mapToScene(event->pos()), Qt::IntersectsItemBoundingRect, Qt::DescendingOrder, QTransform());

    for (QGraphicsItem *item : itemsUnderCursor) // for dragging
    {
        if (item != this) // if hovering another token with a token
        {
            CharacterToken *otherToken = dynamic_cast<CharacterToken*>(item); // cast the other token so we can modify its CharacterData
            if (otherToken)
            {
                QMenu menu;
                QAction *spouseAction = menu.addAction("Set as spouse");
                QAction *setAsParentAction = menu.addAction("Set as parent");
                QAction *selectedAction = menu.exec(event->screenPos());

                if (selectedAction == spouseAction)
                {
                    character->spouseIds.append(otherToken->getCharacterData()->id);
                    otherToken->getCharacterData()->spouseIds.append(character->id);

                    // snap tokens a bit so they're not overlapping, I tried to do it directly
                    this->setPos(otherToken->scenePos().x() - 120, otherToken->scenePos().y());

                    emit spousesSet(character->id, otherToken->getCharacterData()->id);
                }
                else if (selectedAction == setAsParentAction)
                {
                    if (otherToken->getCharacterData()->gender == "") // if male make dad
                    {
                        character->fatherId = otherToken->getCharacterData()->id;
                    }
                    else if (otherToken->getCharacterData()->gender == "female") // if female make mom
                    {
                        character->motherId = otherToken->getCharacterData()->id;
                    }
                    emit parentSet(character->id, otherToken->getCharacterData()->id);
                }
                if (selectedAction == spouseAction || selectedAction == setAsParentAction)
                {
                    emit relationshipsChanged(); // emit signal to update character sheet text
                }
                break;
            }
        } else { // for right clicking
            if (event->button() == Qt::RightButton){
                QMenu menu;
                QAction *removeSpouseAction = menu.addAction("Remove spouses");
                QAction *removeParentsAction = menu.addAction("Remove Parents");
                QAction *selectedAction = menu.exec(event->screenPos());

                if (selectedAction == removeSpouseAction){
                    emit removeSpousesRequested(character->id);
                }
                if (selectedAction == removeParentsAction){
                    emit removeParentsRequested(character->id);
                }
                if (selectedAction == removeSpouseAction || selectedAction == removeParentsAction) {
                    emit relationshipsChanged(); // emit signal to update character sheet text
                }
            }
        }
    }
}
// handler for charactertokens moving (being dragged), emits signal used by drawline in mainwindow.cpp to redraw lines
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

// for getting/setting CharacterData from the tokens
CharacterData* CharacterToken::getCharacterData() const
{
    return character;
}
