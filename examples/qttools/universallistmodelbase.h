#pragma once

#include <QAbstractListModel>
/*!
 * \brief The UniversalListModelBase class to make possible properties definition for UniversalListModel
 * This class should not be used as is, but leaves this possibility.
 */
class UniversalListModelBase : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit UniversalListModelBase(QObject *parent = nullptr);

    /*!
     * \brief count property that declares row count of UniversalListModel
     * \return
     */
    virtual int count() const = 0;
    Q_INVOKABLE virtual void remove(int valueIndex) = 0;

signals:
    void countChanged();
};
