#include "clsHightVoltage.h"
#include "NumberInput.h"
#include "publicUtility.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include "doubleType.h"
#include "frmSetLimit.h"
#include "publicUtility.h"
#include "clsMeterUnit.h"
#include "clsSettings.h"
clsHightVoltage::clsHightVoltage(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    QString lastSettings = readSettings();
    if(lastSettings.isEmpty())
    {
        this->item = "I";
        this->suffix ="u";
        this->voltage=100.0;
        this->hiLimit=0.00;
        this->dblRampUp = 0.1;
        this->dblDelay = 0.4;
        this->dblRampDown = 0.0;
        lowLimit=0.00;
        relaySwitch="开";
    }
    else
    {
        this->setCondition(lastSettings);
    }
    connect(labelLimit,SIGNAL(labelClicked()),this,SLOT(onLabelLimitClicked()));
    updateButtons();
}

QString clsHightVoltage::getCondition() const
{
    QVariantMap conditionMap;
    conditionMap.insert("item", this->item);
    conditionMap.insert("suffix", this->suffix);
    conditionMap.insert("voltage",this->voltage);
    conditionMap.insert("relaySwitch",this->relaySwitch);
    conditionMap.insert("hiLimit",this->hiLimit);
    conditionMap.insert("lowLimit",this->lowLimit);
    conditionMap.insert("dblRampUp",this->dblRampUp);
    conditionMap.insert("dblDelay",this->dblDelay);
    conditionMap.insert("dblRampDown",this->dblRampDown);
    QJsonDocument jsonDocument=QJsonDocument::fromVariant(conditionMap);
    QString condition;
    if(!(jsonDocument.isNull()))
    {
        condition=jsonDocument.toJson();
    }
    return condition;
}

void clsHightVoltage::setCondition(const QString condition)
{
    QVariantMap conditionMap;

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(condition.toUtf8(), &error);

    if(error.error != QJsonParseError::NoError)
        return ;

    if(jsonDocument.isNull() || jsonDocument.isEmpty())
        return;

    conditionMap = jsonDocument.toVariant().toMap();
    this->item = conditionMap["item"].toString();
    this->suffix = conditionMap["suffix"].toString();
    this->voltage=conditionMap["voltage"].toDouble();
    this->relaySwitch=conditionMap["relaySwitch"].toString();
    this->hiLimit=conditionMap["hiLimit"].toDouble();
    this->lowLimit=conditionMap["lowLimit"].toDouble();
    this->dblRampUp=conditionMap["dblRampUp"].toDouble();
    this->dblDelay=conditionMap["dblDelay"].toDouble();
    this->dblRampDown=conditionMap["dblRampDown"].toDouble();
    updateButtons();
}

void clsHightVoltage::updateButtons()
{
    btnItem->setText(this->item);
    if(this->suffix == tr("自动"))
        this->btnUnit->setText(suffix);
    else
        btnUnit->setText(this->suffix + publicUtility::getSuffix(this->item));
    btnRelay->setText(relaySwitch);
    doubleType dt;
    dt.setData(voltage,"");
    btnVoltage->setText(dt.formateToString(5)+"V");
    mLimit.setAbsHi(hiLimit);
    mLimit.setAbsLo(lowLimit);
    mLimit.setLimitType("Norm");
    labelLimit->setText(mLimit.showLimits(publicUtility::getSuffix(this->item)));

    dblRampUp = dblRampUp< 0.1 ? 0.1 : dblRampUp;
    dt.setData(dblRampUp);
    btnRampUp->setText(dt.formateToString(5)+"s");
    dblDelay = dblDelay< 0.4 ? 0.4 : dblDelay;
    dt.setData(dblDelay);
    btnDelay->setText(dt.formateToString(5)+"s");
    dt.setData(dblRampDown);
    btnRampDown->setText(dt.formateToString(5)+"s");



}

void clsHightVoltage::on_btnVoltage_clicked()
{
    NumberInput *dlg=new NumberInput(this);
    if(dlg->exec()==QDialog::Accepted)
    {
        voltage=dlg->getNumber();
        updateButtons();
    }
}

void clsHightVoltage::on_btnOK_clicked()
{
    saveSettings();
    this->accept();
}

void clsHightVoltage::on_btnCancel_clicked()
{
    this->reject();
}

void clsHightVoltage::on_btnRelay_clicked()
{
    if(btnRelay->text()==tr("开"))
        relaySwitch="关";
    else
        relaySwitch="开";
    updateButtons();
}

void clsHightVoltage::onLabelLimitClicked()
{
    frmSetLimit *dlg=new frmSetLimit(this);
    dlg->setItem("I");
    dlg->setLimits(mLimit);
    if(dlg->exec()==QDialog::Accepted)
    {
        mLimit=dlg->getMeterLimit();
        labelLimit->setText(mLimit.showLimits("Ω"));
        hiLimit=mLimit.getAbsLimitHigh();
        lowLimit=mLimit.getAbsLimitLow();
        updateButtons();
    }
}

void clsHightVoltage::on_btnItem_clicked()
{
    /*if(item == "R")
        item = "I";
    else
        item = "R";*/
    item = "I";
    updateButtons();
}

void clsHightVoltage::on_btnUnit_clicked()
{
    clsMeterUnit *dlg = new clsMeterUnit(this);
    dlg->setWindowTitle(tr("选择后缀"));
    dlg->setAutoEnable(true);
    dlg->setItem(this->item);
    if(dlg->exec() == QDialog::Accepted)
    {
        this->suffix = dlg->getSuffix();
        this->updateButtons();
    }
}
QString clsHightVoltage::readSettings()
{
    clsSettings settings;
    QString strTmp;
    QString strNode = "HV/";
    settings.readSetting(strNode + "latestSettings", strTmp);
    return strTmp;
}

void clsHightVoltage::saveSettings()
{
    clsSettings settings;

    QString strNode = "HV/";
    settings.writeSetting(strNode + "latestSettings", this->getCondition());
}

void clsHightVoltage::on_btnRampUp_clicked()
{
    NumberInput *dlg = new NumberInput(this);
    dlg->setWindowTitle(tr("设置Ramp up时间"));

    if(dlg->exec()== QDialog::Accepted)
    {
        dblRampUp = dlg->getNumber();
        updateButtons();
    }

}

void clsHightVoltage::on_btnDelay_clicked()
{
    NumberInput *dlg = new NumberInput(this);
    dlg->setWindowTitle(tr("设置Delay时间"));

    if(dlg->exec()== QDialog::Accepted)
    {
        dblDelay = dlg->getNumber();
        updateButtons();
    }


}

void clsHightVoltage::on_btnRampDown_clicked()
{
    NumberInput *dlg = new NumberInput(this);
    dlg->setWindowTitle(tr("设置Ramp Down时间"));

    if(dlg->exec()== QDialog::Accepted)
    {
        dblRampDown = dlg->getNumber();
        updateButtons();
    }


}
