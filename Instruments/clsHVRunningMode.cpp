#include "clsHVRunningMode.h"
#include <QTextStream>
#include <stdio.h>
#include <QJsonDocument>
#include "clsTestConditon.h"
#include <QJsonParseError>
#include <QDebug>
#include "clsHVConnection.h"
#include "publicUtility.h"
#include "doubleType.h"
#include "clsFtdiOperation.h"
clsHVRunningMode::clsHVRunningMode(clsInstrument *parent): clsInstrument(parent)
{
    for(int i=0; i<4; i++)
    {
        limits << new clsMeterLimit();
        results <<0;
    }
}

void clsHVRunningMode::initConnection()
{
    isConnected = sngHVCnnt::Ins()->setupConnection();
}

bool clsHVRunningMode::connectionStatus()
{
    return isConnected;
}

void clsHVRunningMode::setCondition(QString value)
{
    this->strCondition = value;
    QJsonParseError error;
    QJsonDocument jsonDoucment = QJsonDocument::fromJson(strCondition.toUtf8(), &error);
    if(error.error != QJsonParseError::NoError)
        return;

    if(jsonDoucment.isNull() || jsonDoucment.isEmpty())
        return;

    QVariantMap tmpMap = jsonDoucment.toVariant().toMap();
    if(tmpMap["meter"].toString() != getInstrumentType()) //判断是否为当前仪表设置条件
        return;

    QString tmpCondition = tmpMap["conditions"].toString();
    if(tmpCondition.isEmpty())
        return;

    jsonDoucment = QJsonDocument::fromJson(tmpCondition.toUtf8(), &error);
    if(error.error != QJsonParseError::NoError)
        return;
    if(jsonDoucment.isEmpty() || jsonDoucment.isNull())
        return;

    if(jsonDoucment.isObject())
    {
        QVariantMap conditionMap = jsonDoucment.toVariant().toMap();
        this->item = conditionMap["item"].toString();
        this->suffix = conditionMap["suffix"].toString();
        this->voltage=conditionMap["voltage"].toDouble();
        this->relaySwitch=conditionMap["relaySwitch"].toString();
        this->hiLimit=conditionMap["hiLimit"].toDouble();
        this->lowLimit=conditionMap["lowLimit"].toDouble();
        this->dblRampDown=conditionMap["dblRampDown"].toDouble();
        this->dblDelay=conditionMap["dblDelay"].toDouble();
        this->dblRampUp=conditionMap["dblRampUp"].toDouble();

        if(dblRampUp<0.1)
            dblRampUp = 0.1;

        if(dblDelay < 0.4)
            dblDelay = 0.4;

        limits.at(0)->setAbsHi(hiLimit);
        limits.at(0)->setAbsLo(lowLimit);
        limits.at(0)->setLimitType("Norm");
    }
}

QString clsHVRunningMode::getCondition()
{
    return this->strCondition;
}

bool clsHVRunningMode::trig()
{
    updateGpibCommands(); //更新指令
    QTime starTime = QTime::currentTime();
    QString res = sngHVCnnt::Ins()->sendCommand("TEST", false);
    emit voltageOutput(true);
    publicUtility::sleepMs((dblRampUp + dblDelay + dblRampDown)* 1000.0 + 50);
    emit voltageOutput(false);

    res = sngHVCnnt::Ins()->sendCommand("TD?", true);
    res +=",,,,,,";
    qDebug()<< "Delay time: " << -1*QTime::currentTime().msecsTo(starTime) << "ms";

    QStringList resList = res.split(",");

    resStatus = true;
    if(resList.at(2)!= "Pass")
    {
        resStatus = false;
        sngHVCnnt::Ins()->sendCommand("RESET",false);
    }
    else
        resStatus = true;

   results.clear();
   QString testRes = resList.at(4);
   testRes = testRes.remove("uA");
   results.append(testRes.toDouble()/1E6);

    return true;

}

int clsHVRunningMode::getItemsCount()
{
    return 1; //只有一个返回值
}

bool clsHVRunningMode::getItemStatus(int /*i*/)
{
    return resStatus;
}

bool clsHVRunningMode::getTotleStatus()
{
    bool status = true;
    for(int i =0; i< getItemsCount(); i++)
    {
        status &= getItemStatus(i);
    }
    return status;
}

QString clsHVRunningMode::getItem(int i)
{
    QStringList items;
    items<< item << "" << "";
    return items.at(i);
}

QString clsHVRunningMode::getItemSuffix(int i)
{
    QStringList suffixs;
    suffixs<< suffix << "" << "";
    return suffixs.at(i);

}

double clsHVRunningMode::getItemValue(int i)
{
    return results.at(i);
}

double clsHVRunningMode::getItemValueWithSuffix(int i)
{
    doubleType dt;
    dt.setData(results.at(i));
    if(suffix == tr("自动"))
    {
        return getItemValue(i); //如果是自动的话，就返回原始数据
    }
    else
    {
        return dt.formateWithUnit(suffix, 7).toDouble();
    }
}

QString clsHVRunningMode::getInstrumentType()
{
    return "HV";
}

void clsHVRunningMode::turnOffOutput()
{
    /*
    sngHVCnnt::Ins()->sendCommand("RESET",false);
    emit voltageOutput(false);
*/
}

void clsHVRunningMode::clearGpibCommands()
{
   this->gpibCommands.clear();
}

void clsHVRunningMode::updateGpibCommands()
{
    //仪表的GPIB指令
    QStringList gpibList;
    gpibList << "MSS 01";
    gpibList << "DCW";
    gpibList << "CONN 0";
    gpibList << "VOLT " +QString::number(this->voltage/1000);
    gpibList << "RUP "+ QString::number(dblRampUp);
    gpibList << "DDT "+ QString::number(dblDelay);
    gpibList << "RDN "+ QString::number(dblRampDown);

    gpibList << "ARC 0";
    gpibList << "MAXL " + QString::number(hiLimit *1000000);
    gpibList << "MINL " + QString::number(lowLimit *1000000);
    gpibList << "CONT 0";
    gpibList << "CMAL 1";
    gpibList << "CMIL 0";
    gpibList << "COFF 0";

    if(gpibList.length() != this->gpibCommands.length())
    {
        for(int i=0; i< gpibList.length(); i++)
        {
            sngHVCnnt::Ins()->sendCommand(gpibList.at(i), false);
            publicUtility::sleepMs(100);
        }
    }
    else
    {
        for(int i=0; i< gpibList.length(); i++)
        {
            if(gpibList.at(i) != this->gpibCommands.at(i))
            {
                sngHVCnnt::Ins()->sendCommand(gpibList.at(i), false);
                publicUtility::sleepMs(100);
            }
        }
    }

    this->gpibCommands = gpibList;

    //控制盒的开关通断
    clsConnectSWBox::Ins()->setRelay(relaySwitch==tr("开")?false:true);
}
