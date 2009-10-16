#include "ShaderConfigDlg.h"
#include "Pass.h"

#include "MyLib/MyInputDlg.h"
#include "DisplayConf.h"

ShaderConfigDlg::ShaderConfigDlg(QWidget *parent, Pass* pass, T2GLWidget* view, Document* doc, DisplayConf& dconf)
	: QWidget(parent)
{
	ui.setupUi(this);
	PassConf &conf = pass->conf;

	(new ComboBoxIn<DisplayConf::RunType>(&conf.runType, ui.runTypeBox, true, &dconf))->reload();

	(new CheckBoxIn(&conf.incPoly, ui.incPoly))->reload();
	(new CheckBoxIn(&conf.incPoints, ui.incPoints))->reload();

	(new CheckBoxIn(&conf.vertexPointSize, ui.vertexPointSize))->reload();
	(new CheckBoxIn(&conf.pointSpirits, ui.pointSpirites))->reload();
	(new CheckBoxIn(&conf.spiriteReplace, ui.spiriteReplace))->reload();

	(new ComboBoxIn<PassConf::EGeomInType>(&conf.geomInput, ui.geomInput))->reload();
	(new ComboBoxIn<PassConf::EGeomOutType>(&conf.geomOutput, ui.geomOutput))->reload();
	(new WidgetTIn<int>::LineEditIn(&conf.geomVtxCount, ui.geomVtxCount))->reload();

	ui.paramContainer->init(view, doc, pass);
}

