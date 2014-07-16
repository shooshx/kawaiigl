#ifndef GRADIENTDLG_H
#define GRADIENTDLG_H

#include <QDialog>
#include <memory>
#include "ui_GradientDlg.h"

class GradData;
class StepData;

class GradientDlg : public QDialog
{
    Q_OBJECT
public:
    GradientDlg(bool populate, QWidget *parent = 0);
    ~GradientDlg();
    void parseText(const QString& s);
    void clearSteps();

protected:
    virtual void resizeEvent(QResizeEvent* event);

public slots:
    void on_addBot_clicked();
    void removeStep();
    void updateImage();

signals:
    void changed(QImage img, QString repr);

private:
    void addStep(float pos, QColor col);
    int resolution();
    StepData* getStepAfter(float f);

private:
    Ui::GradientDlg ui;
    std::auto_ptr<GradData> m_data;
    QImage m_curImage;
};

#endif // GRADIENTDLG_H
