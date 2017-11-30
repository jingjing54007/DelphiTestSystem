#include "clsChannelSettings.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include "publicUtility.h"
#include "doubleType.h"
#include <QDebug>
#include "publicUtility.h"
clsChannelSettings::clsChannelSettings(clsMeter *parent) :
    clsMeter(parent)
{
    setupUi(this);
    connect(labelClose,SIGNAL(labelClicked()),this,SLOT(onLabelCloseClicked()));
    connect(labelChannel,SIGNAL(labelClicked()),this,SLOT(onLblChannelClicked()));
}

void clsChannelSettings::setChannel(const int channel)
{
    this->channel=channel;
}

void clsChannelSettings::setCondition(const QString condition)
{
    this->condition=condition;
    QJsonParseError error;
    QJsonDocument jsonDocument=QJsonDocument::fromJson(this->condition.toUtf8(),&error);
    if(error.error==QJsonParseError::NoError)
    {
        if(!(jsonDocument.isEmpty() || jsonDocument.isEmpty()))
        {
            if(jsonDocument.isObject())
            {
                QVariantMap conditionMap=jsonDocument.toVariant().toMap();
                this->item1=conditionMap["item1"].toString();
                this->item2=conditionMap["item2"].toString();
                this->isItem2Enable=conditionMap["isItem2Enable"].toBool();
                this->frequency=conditionMap["frequency"].toDouble();
                this->level=conditionMap["level"].toDouble();
                this->levelUnit=conditionMap["levelUnit"].toString();
                this->range=conditionMap["range"].toString();
                this->speed=conditionMap["speed"].toString();
                this->equcct=conditionMap["equcct"].toString();
                this->bias=conditionMap["bias"].toDouble();
                this->biasSwitch=conditionMap["biasSwitch"].toString();
                this->biasType=conditionMap["biasType"].toString();
                this->item1HiLimit=conditionMap["item1HiLimit"].toDouble();
                this->item1LowLimit=conditionMap["item1LowLimit"].toDouble();
                this->item2HiLimit=conditionMap["item2HiLimit"].toDouble();
                this->item2LowLimit=conditionMap["item2LowLimit"].toDouble();
                this->channel=conditionMap["channel"].toInt();
                this->suffix1=conditionMap["suffix1"].toString();
                this->suffix2=conditionMap["suffix2"].toString();
                this->relaySwitch=conditionMap["relaySwitch"].toString();

            }
        }
    }
}

void clsChannelSettings::updateLabels()
{
    QStringList colorList;
    colorList<<"#D891EF"<<"#FFBF00"<<"#00FFFF"<<"#FDEE00"<<"#D0FF14"<<"#D19FE8"
            <<"#FA6E79"<<"#FE6F5E"<<"#ACE5EE"<<"#66FF00"<<"#FF007F"<<"#84DE02"<<"#1F75FE"
           <<"#08E8DE"<<"#FFAA1D"<<"#FF55A3"<<"#FF033E"<<"#FF2052"<<"#E0218A"<<"#ACE5EE";
    int colorIndex = (channel-1) % colorList.length();
    QList<QLabel*> labelList;
    QList<QLabel*> labelList1;
    labelList<<labelBias<<labelFreq<<labelItem1
            <<labelItem2<<labelLimit1<<labelLimit2<<labelSpeed;
    labelList1<<labelBias1<<labelFreq1<<labelItem11
             <<labelItem22<<labelLimit11<<labelLimit22<<labelSpeed1;

    this->setStyleSheet(QString("QDialog{background-color:%1;border:2px solid %2;border-radius: 9px;}")
                        .arg(colorList.at(colorIndex)).arg(Qt::cyan));
    for(int i=0;i<labelList.length();i++)
    {
        labelList.at(i)->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
        labelList.at(i)->setFont(QFont("楷体",12));
        labelList1.at(i)->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
        labelList1.at(i)->setFont(QFont("楷体",12));
    }

    labelChannel->setStyleSheet(QString("border:0px solid %1").arg(colorList.at(colorIndex)));
    labelChannel->setStyleSheet(QString("border-radius: 0px"));
    labelChannel->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
    labelChannel->setText(tr("通道")+QString::number(channel));
    labelChannel->setFont(QFont("楷体",17));

    labelChannel1->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
    //labelClose->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
    labelRelay->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
    labelRelaySwitch->setStyleSheet(QString("background-color:%1").arg(colorList.at(colorIndex)));
    labelBias1->setText(QString::number(bias)+biasType+","+biasSwitch);
    //labelBias1->setFont(QFont("楷体",12));
    //labelEqucct1->setText(equcct);
    doubleType dt;
    dt.setData(frequency,"");
    labelFreq1->setText(dt.formateToString(6)+"Hz");

    dt.setData(level,"");
    //labelLevel1->setText(dt.formateToString(6)+levelUnit);

    labelItem11->setText(item1);

    if(isItem2Enable)
        labelItem22->setText(item2);
    else
        labelItem22->setText(tr("关"));

    //Item1 Limit
    QString hiLimit;
    dt.setData(item1HiLimit,"");
    if(suffix1 == tr("自动"))
        hiLimit=dt.formateToString(5)+publicUtility::getSuffix(item1);
    else
        hiLimit=dt.formateWithUnit(suffix1,5)+suffix1+publicUtility::getSuffix(item1);

    QString lowLimit;
    dt.setData(item1LowLimit,"");

    if(suffix1 == tr("自动"))
        lowLimit=dt.formateToString(5)+publicUtility::getSuffix(item1);
    else
        lowLimit=dt.formateWithUnit(suffix1,5)+suffix1+publicUtility::getSuffix(item1);
    labelLimit11->setText(lowLimit+"--"+hiLimit);

    //Item2 Limit
    dt.setData(item2HiLimit,"");
    if(suffix2 == tr("自动"))
        hiLimit=dt.formateToString(5)+publicUtility::getSuffix(item2);
    else
        hiLimit=dt.formateWithUnit(suffix2,5)+suffix2+publicUtility::getSuffix(item2);

    dt.setData(item2LowLimit,"");

    if(suffix2 == tr("自动"))
        lowLimit=dt.formateToString(5)+publicUtility::getSuffix(item2);
    else
        lowLimit=dt.formateWithUnit(suffix2,5)+suffix2+publicUtility::getSuffix(item2);

    if(isItem2Enable)
        labelLimit22->setText(lowLimit+"--"+hiLimit);
    else
        labelLimit22->setText(tr("关"));

    //labelRange1->setText(range);
    labelSpeed1->setText(speed);
    labelRelaySwitch->setText(relaySwitch);

    this->labelResItem1->setText(this->item1);


    this->labelResItem2->setText(this->item2);
    labelResItem2->setVisible(isItem2Enable);
    labelRes2->setVisible(isItem2Enable);
    labelResStatus2->setVisible(isItem2Enable);

}


void clsChannelSettings::onLabelCloseClicked()
{
    emit deleteChannelSettings(intStep);
}

void clsChannelSettings::onLblChannelClicked()
{
   int tabs = stackedWidget->count();

   int nextPageIndex = stackedWidget->currentIndex()+1;
   stackedWidget->setCurrentIndex(nextPageIndex % tabs);
}
void clsChannelSettings::setChannelSettings()
{
    stackedWidget->setCurrentIndex(0);
}

void clsChannelSettings::setChannelRunnings()
{
    stackedWidget->setCurrentIndex(1);
}

void clsChannelSettings::setCloseEnabled(bool bl)
{
    if(bl)
        labelClose->setEnabled(true);
    else
        labelClose->setEnabled(false);
}

void clsChannelSettings::updateRes(const QString res)
{
    QVariantMap resMap;
    QVariantList resList;
    doubleType dt;

    QString item;
    bool resStatus;
    double result;
    QString resUnit;

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(res.toUtf8(), &error);


    if(error.error != QJsonParseError::NoError)
        return;

    if(jsonDocument.isEmpty() || jsonDocument.isNull())
        return;

    resMap = jsonDocument.toVariant().toMap();

    if(resMap["channel"].toInt() != this->channel)
        return;

    resList = resMap["data"].toList();

    if(resList.length()>=1)
    {
        QVariantMap tmpMap = resList.at(0).toMap();
        item = tmpMap["item"].toString();
        resStatus = tmpMap["status"].toBool();
        result = tmpMap["result"].toDouble();
        resUnit = tmpMap["suffix"].toString();

        dt.setData(result);
        if(resUnit != tr("自动"))
            labelRes1->setText(dt.formateWithUnit(resUnit,7)+ publicUtility::getSuffix(item));
        else
            labelRes1->setText(dt.formateToString(7)+ publicUtility::getSuffix(item));

        if(resStatus)
            labelResStatus1->setPixmap(QPixmap(":/dotGreen.png"));
        else
            labelResStatus1->setPixmap(QPixmap(":/dotRed.png"));

    }

    qApp->processEvents();

    if(resList.length()>=2)
    {
        QVariantMap tmpMap = resList.at(1).toMap();
        item = tmpMap["item"].toString();
        resStatus = tmpMap["status"].toBool();
        result = tmpMap["result"].toDouble();
        resUnit = tmpMap["suffix"].toString();

        dt.setData(result);
        if(resUnit != tr("自动"))
            labelRes2->setText(dt.formateWithUnit(resUnit,7)+ publicUtility::getSuffix(item));
        else
            labelRes2->setText(dt.formateToString(7)+ publicUtility::getSuffix(item));

        if(resStatus)
            labelResStatus2->setPixmap(QPixmap(":/dotGreen.png"));
        else
            labelResStatus2->setPixmap(QPixmap(":/dotRed.png"));
    }
    else
    {
        labelRes2->setText("---");

        labelResStatus2->setText("---");
    }
    qApp->processEvents();
}


void clsChannelSettings::setStep(int i)
{
    labelChannel1->setText("s"+QString::number(i+1));
    this->intStep =i;
}

int clsChannelSettings::getStep() const
{
    return this->intStep;
}
