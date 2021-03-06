/*
 * group box, with an additional button to check all the items on or off.
 * Created: 25/09/2010
 */

#include "CheckList.h"


CheckList::CheckList(const QStringList &labels, QWidget *parent, QStringList toolTips) :
    QListWidget(parent)
{
	group = new QButtonGroup;
	group->setExclusive(false);
    //this->setContentsMargins(0,0,0,0);
#ifdef Q_OS_WIN
    this->setSpacing(1);
#endif
    resetLabels(labels, toolTips);
}

void CheckList::resetLabels(QStringList labels, QStringList toolTips)
{
    m_tooltips.clear();
    m_labels.clear();
    this->clear();

    hasTips = (labels.size() == toolTips.size());

    for (int i = 0; i < labels.size(); i++)
    {
        QString label = labels[i];
        QString tip = hasTips? toolTips[i]: "";

        this->appendItem(label, tip);
    }
}

void CheckList::appendItem(QString label, QString tooltip)
{
	QListWidgetItem *item = new QListWidgetItem(this);
	QCheckBox *button = new QCheckBox(label);
//	QFont font("Liberation Mono");
//	font.setStyleHint(QFont::TypeWriter);
//	button->setFont(font);
	if (hasTips)
	{
		button->setToolTip(tooltip);
		m_tooltips << tooltip;
	}
	group->addButton(button);
	connect(button, SIGNAL(stateChanged(int)), this, SLOT(forwardState(int)));
	setItemWidget(item, button);

	m_labels << label;
}

void CheckList::removeItem(QString text)
{
	// try to remove item, starting from the end
	int index = -1;

	for (int i = this->count()-1; i >= 0; --i)
	{
		QCheckBox *box = qobject_cast<QCheckBox*>(itemWidget(item(i)));
		if (box->text() == text)
		{
			index = i;
			break;
		}
	}

	if (index != -1 && index < m_labels.count())
	{
		QListWidgetItem *oldItem = this->takeItem(index);
		if (oldItem)
			delete oldItem;
		m_labels.removeAt(index);
		if (hasTips)
			m_tooltips.removeAt(index);
	}
}

void CheckList::forwardState(int state)
{
	int index = indexFromCheckbox(qobject_cast<QCheckBox*>(sender()));
	emit stateChanged(index, state);
}

QList<QCheckBox*> CheckList::buttons()
{
	QList<QCheckBox*> buttons;

	foreach (QAbstractButton *btn, group->buttons())
		buttons << static_cast<QCheckBox*>(btn);

	return buttons;
}

QList<QCheckBox*> CheckList::checkedItems()
{
	QList<QCheckBox*> items;

	foreach (QCheckBox *item, buttons())
	{
		if (item->isChecked())
			items << item;
	}

	return items;
}

int CheckList::indexFromCheckbox(QCheckBox *box)
{
	int i;
	for (i = 0; i < this->count(); ++i)
	{
		if (qobject_cast<QCheckBox*>(this->itemWidget(item(i))) == box)
			return i;
	}

	return -1;
}

/*****************************************************************************************************
 *****************************************************************************************************
 *****************************************************************************************************/

CheckListBox::CheckListBox(const QString &title, const QStringList &labels, QWidget *parent) :
        QWidget(parent)
{
    m_title = title;
	layout = new QVBoxLayout;
    layout->setContentsMargins(5,5,5,5);
    //layout->setSpacing(2);
    QHBoxLayout *hl = new QHBoxLayout;
    switch_button = new QCheckBox;
#ifdef Q_OS_WIN
    hl->addSpacing(2);
#endif
    hl->setSpacing(13); // make sure the button and the label don't overlap
    hl->addWidget(switch_button);
    hl->addWidget(new QLabel("<b>" + title + "</b>"));
    hl->addStretch();
	checkList = new CheckList(labels);
    checkList->setContentsMargins(0,0,0,0);
    this->setContentsMargins(0,20,0,10);

    layout->addLayout(hl);
	layout->addWidget(checkList);
	setLayout(layout);

	connect(switch_button, SIGNAL(stateChanged(int)), this, SLOT(checkAll(int)));
	connect(checkList, SIGNAL(stateChanged(int,int)), this, SIGNAL(stateChanged(int,int)));
}

QString CheckListBox::title() const
{
    return m_title;
}

void CheckListBox::checkAll(int state)
{
	if (state == Qt::Checked)
	{
		foreach (QCheckBox *btn, checkList->buttons())
			btn->setChecked(true);
	}
	else if (state == Qt::Unchecked)
	{
		foreach (QCheckBox *btn, checkList->buttons())
			btn->setChecked(false);
	}
	else
		qDebug() << tr("Unmanaged state in CheckListBox::checkAll()");

}

QStringList CheckListBox::checkedLabels()
{
	QStringList results;

	foreach (QCheckBox *btn, checkList->buttons())
	{
		if (btn->isChecked())
			results << btn->text();
	}

	return results;
}

QString CheckListBox::text(int index) const
{
	QCheckBox *box = qobject_cast<QCheckBox*>(checkList->itemWidget(checkList->item(index)));
	return box->text();
}

int CheckListBox::index(QString text) const
{
	int i;
	for (i = 0; i < checkList->count(); ++i)
	{
		QCheckBox *box = qobject_cast<QCheckBox*>(checkList->itemWidget(checkList->item(i)));
		if (box->text() == text)
			return i;
	}

	return -1;
}

void CheckListBox::addItem(QString item)
{
	checkList->appendItem(item);
}

void CheckListBox::removeItem(QString item)
{
	checkList->removeItem(item);
}
