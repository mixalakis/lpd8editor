#include "programsmodel.h"

#include "lpd8_sysex.h"

#include <QDataStream>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlQuery>

#include <QtDebug>

static const QString cc_field_name("cc");
static const QString channel_field_name("channel");
static const QString control_id_field_name("controlId");
static const QString high_field_name("high");
static const QString low_field_name("low");
static const QString name_field_name("name");
static const QString note_field_name("note");
static const QString pc_field_name("pc");
static const QString program_id_field_name("programId");
static const QString toggle_field_name("toggle");

static const int program_id_column_index = 0;


QString programIdFilter(int projectId) {
    return QString("%1='%2'").arg(program_id_field_name).arg(projectId);
}

bool deleteRecordsForProgramId(QSqlTableModel* model, const QString& table, int projectId) {
    Q_CHECK_PTR(model);

    model->setTable(table);
    model->setFilter(programIdFilter(projectId));
    model->select();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->removeRows(0, model->rowCount());
    return model->submitAll();
}


ProgramsModel::ProgramsModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_groups(Q_NULLPTR),
    m_pads(Q_NULLPTR),
    m_knobs(Q_NULLPTR),
    m_programs(Q_NULLPTR),
    m_empty(Q_NULLPTR)
{
    m_empty = new QStandardItemModel(this);

    m_knobs = new QSqlTableModel(this);
    m_knobs->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_knobs->setTable("knobs");

    m_pads = new QSqlTableModel(this);
    m_pads->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_pads->setTable("pads");
    m_pads->select();

    m_groups = new QStandardItemModel(this);
    m_groups->setHorizontalHeaderLabels({program_id_field_name, name_field_name});
//    m_groups->appendRow({new QStandardItem("Pads"), new QStandardItem("Knobs")});
//    QStandardItem* it = new QStandardItem("Pads");
//    m_groups->appendRow(it);
//    it = new QStandardItem("Knobs");
//    m_groups->appendRow(it);

    m_programs = new QSqlTableModel(this);
    m_programs->setTable("programs");
    m_programs->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_programs->select();

    for (int i = 0 ; i < m_programs->rowCount() ; ++i) {
        addFilters(m_programs->record(i).value(program_id_column_index).toInt());
    }

    qDebug() << "this" << this;
    qDebug() << "m_programs" << m_programs;
    qDebug() << "m_groups" << m_groups;
    for (auto it = m_groups_proxies.keyBegin() ; it != m_groups_proxies.keyEnd() ; ++it) {
        qDebug() << "m_groups_proxies" << *it << m_groups_proxies[*it];
    }
    qDebug() << "m_empty" << m_empty;
}

const QAbstractItemModel* ProgramsModel::modelFromParent(const QModelIndex &parent) const {
    QModelIndex idx = parent;
    int lvl = 0;
    while (idx.isValid()) {
        ++lvl;
        idx = idx.parent();
    }

    // parent is the top level invalid index
    if (lvl == 0) {
        return m_programs;
    }

    // Only first column can have children
    if (parent.column() != 0) {
        return m_empty;
    }

    // parent is in m_programs: returns a group proxy
    if (lvl == 1) {
        const int programId = parent.data().toInt();
        if (programId == 2) {
            QSortFilterProxyModel* m = m_groups_proxies[programId];
            Q_ASSERT(m->rowCount() == 2);
            Q_ASSERT(m->index(0, 1).data().toString() == "pads2");
            Q_ASSERT(m->index(1, 1).data().toString() == "knobs2");
        }
        return m_groups_proxies[programId];
    }

    // parent is a group proxy: returns the empty model
    if (lvl == 2) {
        return m_empty;
    }

    return m_empty;
}

int ProgramsModel::columnCount(const QModelIndex &parent) const {
    const QAbstractItemModel* m = modelFromParent(parent);
    Q_CHECK_PTR(m);

    int ret = m->columnCount();
    qDebug() << "columnCount" << parent << m << ret;
    return ret;
}

int ProgramsModel::rowCount(const QModelIndex &parent) const {
    const QAbstractItemModel* m = modelFromParent(parent);
    Q_CHECK_PTR(m);

    int ret = m->rowCount();
    qDebug() << "rowCount" << parent << m << ret;
    return ret;
}

QVariant ProgramsModel::data(const QModelIndex &index, int role) const {
    const QAbstractItemModel* m = model(index);
    Q_CHECK_PTR(m);

    QModelIndex idx = m->index(index.row(), index.column());
    QVariant ret = m->data(idx, role);

    if (role == Qt::DisplayRole) {
        qDebug() << "data" << index << role << m << ret;
    }
    return ret;
}

QModelIndex ProgramsModel::index(int row, int column, const QModelIndex &parent) const {
    const QAbstractItemModel* m = modelFromParent(parent);
    Q_CHECK_PTR(m);

//    qDebug() << "index" << row << column << parent << m->index(row, column, parent);
    return createIndex(row, column, const_cast<QAbstractItemModel*>(m));
}


QModelIndex ProgramsModel::parent(const QModelIndex &child) const {
    const QAbstractItemModel* m = model(child);
    Q_CHECK_PTR(m);

    QModelIndex ret;

    const QSortFilterProxyModel* program_proxy = qobject_cast<const QSortFilterProxyModel*>(m);
    const int programId = m_groups_proxies.key(const_cast<QSortFilterProxyModel*>(program_proxy));

    if (programId > 0) {
        QModelIndexList indices = m_programs->match(
            m_programs->index(0, 0),
            Qt::DisplayRole,
            programId
        );
        if (indices.count() != 1) {
            for (int i = 0 ; m_groups_proxies.count() ; ++i) {
                qDebug() << m_groups_proxies[i];
            }
        }
        Q_ASSERT(indices.count() == 1);
        ret = createIndex(indices[0].row(), 0, m_programs);
    }

//    qDebug() << "parent" << child << ret;
    return ret;
}

int ProgramsModel::createProgram(const QString &name, const QByteArray &sysex) {
#if 0
    Q_CHECK_PTR(m_pads);
    Q_CHECK_PTR(m_knobs);

    QDataStream s(sysex);
    qint8 v = 0;

    // Header

    s >> v; // Sysex Start
    s >> v; // Manufacturer
    s >> v; // Model byte 1
    s >> v; // Model byte 2
    s >> v; // Opcode set program: byte1
    s >> v; // Opcode set program: byte2
    s >> v; // Opcode set program: byte3
    s >> v; // Program ID

    // Program

    QSqlRecord r(record());
    r.remove(r.indexOf(program_id_field_name));
    r.setValue(name_field_name, name);
    s >> v;
    r.setValue(channel_field_name, v);

    if (!insertRecord(-1, r)) {
         qWarning() << "Cannot create program:" << lastError().text();
         return -1;
    }

    const int programId = query().lastInsertId().toInt();

    // Pads

    for (int i = 0; i < sysex::padsCount(); ++i) {
        QSqlRecord r(m_pads->record());

        r.setValue(program_id_field_name, programId);
        r.setValue(control_id_field_name, i);

        s >> v;
        r.setValue(note_field_name, v);

        s >> v;
        r.setValue(pc_field_name, v);

        s >> v;
        r.setValue(cc_field_name, v);

        s >> v;
        r.setValue(toggle_field_name, v);

        if (!m_pads->insertRecord(-1, r)) {
             qWarning() << "Cannot create pad" << i << "for program:" << lastError().text();
             return -1;
        }
    }

    // Knobs

    for (int i = 0; i < sysex::knobsCount(); ++i) {
        QSqlRecord r(m_knobs->record());

        r.setValue(program_id_field_name, programId);
        r.setValue(control_id_field_name, i);

        s >> v;
        r.setValue(cc_field_name, v);

        s >> v;
        r.setValue(low_field_name, v);

        s >> v;
        r.setValue(high_field_name, v);

        if (!m_knobs->insertRecord(-1, r)) {
             qWarning() << "Cannot create pad" << i << "for program:" << lastError().text();
             return -1;
        }
    }

    addFilters(programId);

    return programId;
#else
    Q_UNUSED(name);
    Q_UNUSED(sysex);
    return -1;
#endif
}

bool ProgramsModel::deleteProgram(int programId) {
#if 0
    QScopedPointer<QSqlTableModel> m(new QSqlTableModel());

    if (!deleteRecordsForProgramId(m.data(), tableName(), programId)) {
        qWarning() << "Failed deleting program:" << programId;
        return false;
    }

    if (!deleteRecordsForProgramId(m.data(), m_pads->tableName(), programId)) {
        qWarning() << "Failed deleting pads for program:" << programId;
        return false;
    }

    if (!deleteRecordsForProgramId(m.data(), m_knobs->tableName(), programId)) {
        qWarning() << "Failed deleting knobs for program:" << programId;
        return false;
    }

    m_knobs->select();
    m_pads->select();
    select();

    return true;
#else
    Q_UNUSED(programId);
    return false;
#endif
}

QString ProgramsModel::name(int programId) const {
#if 0
    return record(programRow(programId)).value(name_field_name).toString();
#else
    Q_UNUSED(programId);
    return "NIY";
#endif
}

QByteArray ProgramsModel::sysex(int programId) const {
#if 0
    Q_CHECK_PTR(m_pads);
    Q_CHECK_PTR(m_knobs);

    QByteArray ret;
    sysex::addProgramHeader(ret, 1);

    QSqlRecord r;
    QScopedPointer<QSqlTableModel> m(new QSqlTableModel());
    const QString filter(QString("%1='%2'").arg(program_id_field_name).arg(programId));

    // Program

    m->setTable(tableName());
    m->setFilter(filter);
    m->select();
    Q_ASSERT(m->rowCount() == 1);

    r = m->record(0);

    ret += r.value(channel_field_name).toChar();

    // Pads

    m->setTable(m_pads->tableName());
    m->setFilter(filter);
    m->setSort(m_pads->fieldIndex(control_id_field_name), Qt::AscendingOrder);
    m->select();
    Q_ASSERT(m->rowCount() == sysex::padsCount());

    for (int i = 0 ; i < m->rowCount() ; ++i) {
        r = m->record(i);
        ret += r.value(note_field_name).toChar();
        ret += r.value(pc_field_name).toChar();
        ret += r.value(cc_field_name).toChar();
        ret += r.value(toggle_field_name).toChar();
    }

    // Knobs

    m->setTable(m_knobs->tableName());
    m->setFilter(filter);
    m->setSort(m_knobs->fieldIndex(control_id_field_name), Qt::AscendingOrder);
    m->select();
    Q_ASSERT(m->rowCount() == sysex::knobsCount());

    for (int i = 0 ; i < m->rowCount() ; ++i) {
        r = m->record(i);
        ret += r.value(cc_field_name).toChar();
        ret += r.value(low_field_name).toChar();
        ret += r.value(high_field_name).toChar();
    }

    // Footer

    sysex::addFooter(ret);

    removeFilters(programId);

    return ret;
#else
    Q_UNUSED(programId);
    return QByteArray();
#endif
}

int ProgramsModel::programRow(int programId) const {
#if 0
    const QModelIndexList indices(match(index(0, 0), Qt::DisplayRole, programId));
    Q_ASSERT(indices.count() == 1);
    return indices[0].row();
#else
    Q_UNUSED(programId);
    return -1;
#endif
}

QAbstractItemModel* ProgramsModel::model(const QModelIndex &index)  {
    return qobject_cast<QAbstractItemModel*>(static_cast<QObject*>(index.internalPointer()));
}

const QAbstractItemModel* ProgramsModel::model(const QModelIndex &index) const {
    return qobject_cast<QAbstractItemModel*>(static_cast<QObject*>(index.internalPointer()));
}

void ProgramsModel::addFilters(int programId) {
    Q_CHECK_PTR(m_groups);
    Q_ASSERT(!m_groups_proxies.contains(programId));

    // Regexes

    const QRegExp regex(QString::number(programId));

    // Groups

    m_groups->appendRow({new QStandardItem(QString::number(programId)), new QStandardItem(QString("pads%1").arg(programId))});
    m_groups->appendRow({new QStandardItem(QString::number(programId)), new QStandardItem(QString("knobs%1").arg(programId))});

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(m_groups);
    proxy->setFilterKeyColumn(program_id_column_index);
    proxy->setFilterRegExp(regex);
    m_groups_proxies[programId] = proxy;
}

void ProgramsModel::removeFilters(int programId) {
    Q_CHECK_PTR(m_groups);
    Q_ASSERT(m_groups_proxies.contains(programId));

    const QString findItemsText(QString::number(programId));

    // Groups

    m_groups_proxies.take(programId)->deleteLater();

    QList<QStandardItem*> items = m_groups->findItems(findItemsText);
    Q_ASSERT(items.count() == 2);
    Q_CHECK_PTR(items[0]);
    Q_CHECK_PTR(items[1]);
    Q_ASSERT((items[1]->row() - items[0]->row()) == 1); // The two indices are contiguous
    m_groups->removeRows(items[0]->row(), items.count());
}
