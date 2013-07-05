#ifndef RULEFILE_H
#define RULEFILE_H

#include <QObject>
#include <QStringList>

#define RULE_FILE "/etc/udev/rules.d/50-perse.rules"

/**
 * Handles loading & saving the rule file
 */
class RuleFile : public QObject
{
    Q_OBJECT
public:
    explicit RuleFile(QObject *parent = 0);
    bool canWriteFile();
    void readFile();
    void writeFile(QStringList allowedIds);
signals:
    void deviceLoaded(QString name, QString file, QString manu, QString product, QString vid, QString pid);

public slots:
    
};

#endif // RULEFILE_H
