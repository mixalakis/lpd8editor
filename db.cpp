#include "db.h"

QString sqlErrorMessage(const QString& msg, const QSqlError& err) {
    return msg + QString("\n") + err.text();
}

bool isInitialized(const QSqlDatabase& db) {
    return db.tables().contains("presets");
}

bool addPreset(const QString& name) {
    QSqlQuery q;
    if (!q.prepare("insert into presets(name) values(?)")) return false;
    q.addBindValue(name);
    return q.exec();
}

bool addPreset(int presetId) {
    QSqlQuery q;
    if (!q.prepare("insert into presets(presetId, name) values(?, ?)")) return false;
    q.addBindValue(presetId);
    q.addBindValue("default");
    return q.exec();
}

bool deletePreset(int presetId) {
    Q_ASSERT(presetId > 0);

    QSqlQuery q;
    if(!q.prepare("delete from presets where presetId = ?")) return false;
    q.addBindValue(presetId);
    return q.exec();
}

bool setPresetName(int presetId, const QString& name) {
    QSqlQuery q;
    if(!q.prepare("update presets set name = ? where presetId = ?")) return false;
    q.addBindValue(name);
    q.addBindValue(presetId);
    return q.exec();
}

bool isValidPresetId(int presetId) {
    QSqlQuery q;
    if (!q.exec("select presetId from presets;")) {
        qWarning() << "Cannot select presets";
        qWarning() << q.lastError().text();
        return false;
    }
    for (int i = 0 ; i < q.size() ; ++i) {
        if (presetId == q.value(i)) {
            return true;
        }
    }
    return false;
}

bool isValidProgramId(int programId) {
    return programId >= 0 && programId <= 3;
}

bool initialize() {
    QSqlQuery q;
    if (!q.exec("create table presets(presetId integer primary key, name varchar)")) return false;
    if (!q.exec("create table pads(presetId integer,"
                                  "programId integer NOT NULL,"
                                  "controlId integer NOT NULL,"
                                  "note integer NOT NULL,"
                                  "pc integer NOT NULL,"
                                  "cc integer NOT NULL,"
                                  "momentary integer NOT NULL,"
                                  "PRIMARY KEY (presetId, programId, controlId),"
                                  "FOREIGN KEY (presetId) REFERENCES presets (presetId)"
                                  ");")) return false;
    if (!q.exec("create table knobs(presetId integer,"
                                   "programId integer NOT NULL,"
                                   "controlId integer NOT NULL,"
                                   "cc integer NOT NULL,"
                                   "PRIMARY KEY (presetId, programId, controlId),"
                                   "FOREIGN KEY (presetId) REFERENCES presets (presetId)"
                                   ");")) return false;
    if (!q.exec("create trigger aft_insert after insert on presets "
                "begin "

                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 1, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 2, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 3, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 4, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 5, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 6, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 7, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 1, 8, 1, 2, 3, 0);"

                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 1, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 2, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 3, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 4, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 5, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 6, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 7, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 1, 8, 1);"

                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 1, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 2, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 3, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 4, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 5, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 6, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 7, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 2, 8, 1, 2, 3, 0);"

                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 1, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 2, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 3, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 4, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 5, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 6, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 7, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 2, 8, 1);"

                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 1, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 2, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 3, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 4, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 5, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 6, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 7, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 3, 8, 1, 2, 3, 0);"

                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 1, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 2, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 3, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 4, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 5, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 6, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 7, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 3, 8, 1);"

                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 1, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 2, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 3, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 4, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 5, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 6, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 7, 1, 2, 3, 0);"
                "insert into pads(presetId, programId, controlId, note, pc, cc, momentary) values(NEW.presetId, 4, 8, 1, 2, 3, 0);"

                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 1, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 2, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 3, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 4, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 5, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 6, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 7, 1);"
                "insert into knobs(presetId, programId, controlId, cc) values(NEW.presetId, 4, 8, 1);"

                "end;")) return false;

    if (!q.exec("create trigger aft_delete after delete on presets "
                "begin "
                "delete from pads where presetId = OLD.presetId;"
                "delete from knobs where presetId = OLD.presetId;"
                "end;")) return false;

    return true;
}

bool ensureDefaultPreset() {
    if (!isValidPresetId(0)) {
        return addPreset(0);
    }
    return true;
}

QSqlError initDb(const QString& path) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        qWarning() << "Failed to open db" << path;
        goto end;
    }

    if (!isInitialized(db)) {
        if (!initialize()) {
            qWarning() << "Failed to initialize db" << path;
            goto end;
        }
    }

    if (!ensureDefaultPreset()) {
        qWarning() << "Failed to ensure default preset in db" << path;
        goto end;
    }

    end:
    return db.lastError();
}
