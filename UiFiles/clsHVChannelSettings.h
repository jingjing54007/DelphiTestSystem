#ifndef CLSHVCHANNELSETTINGS_H
#define CLSHVCHANNELSETTINGS_H

#include "ui_clsHVChannelSettings.h"
#include "clsMeter.h"

class clsHVChannelSettings : public clsMeter, private Ui::clsHVChannelSettings
{
    Q_OBJECT

public:
    explicit clsHVChannelSettings(clsMeter *parent = 0);
    void setCondition(const QString condition);
    void setChannel(const int channel);
    void updateLabels();
    void setChannelSettings();
    void setChannelRunnings();
    void setCloseEnabled(const bool bl);
    void updateRes(const QString res);
 void setStep(int i) override;
    int getStep() const override;

public slots:
    void onCloseLabelClicked();
    void onChannelLabelClicked();
    void setLablChannelToolTip();
    void showTrigStar(int i);
private:
    int channel;
    QString condition;
    QString resStatus;
    QString item;
    QString suffix;
    double hiLimit, lowLimit, voltage;
    double res;
    QString relaySwitch;
    QString hvStatus;

    double dblRampUp, dblDelay, dblRampDown;

    bool isEditMode;
    int intStep;
};

#endif // CLSHVCHANNELSETTINGS_H
