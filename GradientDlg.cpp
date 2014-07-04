#include "GradientDlg.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include "MyLib/MyInputDlg.h"
#include <vector>
#include "MyLib/Vec.h"
#include "DisplayConf.h" // for UserData



struct StepData : public ParamBase
{
    QWidget* containter;
    TypeProp<float> pos;
    TypeProp<QColor> col;
    StepData(float _pos, QColor _col)
        :pos(this, "pos", init<float>(_pos, 0.0, 1.0))
        ,col(this, "col", _col)
        ,containter(NULL)
    {}
};

struct GradData
{
    QVector<StepData*> steps;
};

GradientDlg::GradientDlg(QWidget *parent)
    : QDialog(parent), m_data(new GradData)
{
    ui.setupUi(this);
    addStep(0.0, QColor(0,0,0));
    addStep(1.0, QColor(255,255,255));
    connect(ui.sizeSel, SIGNAL(currentTextChanged(const QString&)), this, SLOT(updateImage()));

    updateImage();

}

GradientDlg::~GradientDlg()
{
}

void GradientDlg::on_addBot_clicked() 
{
    addStep(0.5, QColor(1,0,0));
}

int GradientDlg::resolution() {
    return ui.sizeSel->currentText().toInt();
}

void GradientDlg::addStep(float pos, QColor col)
{
    int resl = resolution();

    StepData* s = new StepData(pos, col);

    s->containter = new QWidget(this);
    QHBoxLayout *slayout = new QHBoxLayout(); // name edit, type combo, value edit, SOMETHING, x bot. 
    slayout->setMargin(0);
    slayout->setSpacing(6);
    s->containter->setLayout(slayout);
    s->containter->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    ui.stepsLayout->addWidget(s->containter);

    QPushButton *removeBot(new QPushButton("-", this));
    removeBot->setMaximumWidth(20);
    removeBot->setUserData(0, new UserData<StepData*>(s));
    slayout->addWidget(removeBot);
    connect(removeBot, SIGNAL(clicked()), this, SLOT(removeStep()));
    
    QSlider* slider(new QSlider(Qt::Horizontal));
    slider->setRange(0, resl);
    slayout->addWidget(slider);

    QLineEdit *numVal(new QLineEdit(this));
    numVal->setMaximumWidth(44);

    slayout->addWidget(numVal);
    QPushButton *colBot(new QPushButton(this));
    colBot->setMaximumWidth(20);

    slayout->addWidget(colBot);

    
    s->reset();
    connect(s, SIGNAL(changed()), this, SLOT(updateImage()));
    m_data->steps.push_back(s);


    //auto x =  WidgetIn::DoModel;
    (new ColorSelIn(&s->col, colBot, false, (WidgetIn::EFlags)1))->reload();
    (new SliderIn<float>(&s->pos, slider, NULL, resl, true, numVal))->reload();

    updateImage();
}

StepData* GradientDlg::getStepAfter(float f)
{
    float foundpos = 1.1;
    StepData *founds = NULL;
    for(int i = 0; i < m_data->steps.size(); ++i)
    {
        StepData *s = m_data->steps[i];
        if (s->pos < foundpos && s->pos > f) {
            founds = s;
            foundpos = s->pos;
        }
    }
    return founds;
}

void GradientDlg::removeStep()
{
    StepData *sd = ((UserData<StepData*>*)sender()->userData(0))->i;

    auto *w = sd->containter;
    delete sd;
    delete w;

    m_data->steps.remove(m_data->steps.indexOf(sd));
    updateImage();
}

void setPixel(uchar* line, int i, uint rgb) {
    uint *p = (uint*)line;
    p += i;
    *p = rgb;
}

// minor bug - last two color on the same last pixel but not the same pos, causes a miscoloring of the last pixel
void GradientDlg::updateImage()
{
    int resl = resolution();
    QImage img(resl, 1, QImage::Format_ARGB32);
    uchar* bits = img.scanLine(0);
    StepData* s = getStepAfter(-0.1);
    if (s == NULL)
        return;
    Vec3 lastCol, col = Vec3(s->col);
    int lastX = 0, x = (int)(s->pos * resl);
    int lastd = 0; // last pixel needs to have the last color

    for(int i = 0; i < x; ++i)
        setPixel(bits, i, col.toBgr());
    while(true)
    {
        lastX = x;
        lastCol = col;
        s = getStepAfter(s->pos);
        if (s == NULL)
            break;
        x = (int)(s->pos * resl);
        col = Vec3(s->col);
        if (s->pos == 1.0)
            lastd = 1;
        if (x - lastX - lastd == 0)
            continue;
        for(int i = lastX; i < x; ++i) {
            float f = (float)(i - lastX) / (x - lastX - lastd);
            Vec3 c = lastCol * (1.0 - f) + col * f;
            setPixel(bits, i, c.toBgr());
        }
    }
    for(int i = x; i < resl; ++i)
        setPixel(bits, i, col.toBgr());

    m_curImage = img;
    resizeEvent(NULL);
    emit changed(m_curImage);
}

void GradientDlg::resizeEvent(QResizeEvent* event) 
{
    QPixmap px = QPixmap::fromImage(m_curImage, Qt::ColorOnly).scaled(ui.gradImg->width() - 2, ui.gradImg->height() - 2);
    ui.gradImg->setPixmap(px);
}
