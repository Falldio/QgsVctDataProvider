#include "qgsvctsourceselect.h"
#include "qgsgui.h"
#include "qgsvctprovider.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include "qgsmessagelog.h"


QgsVctSourceSelect::QgsVctSourceSelect(QWidget *parent, Qt::WindowFlags fl, QgsProviderRegistry::WidgetMode theWidgetMode)
	: QgsAbstractDataSourceWidget( parent, fl, theWidgetMode )
{
	setupUi(this);
	QgsGui::instance()->enableAutoGeometryRestore(this);
	setupButtons(buttonBox);

	connect(toolButtonFilePath, &QAbstractButton::clicked, this, &QgsVctSourceSelect::openFileDialog);
	connect(lineEditFilePath, &QLineEdit::textChanged, this, &QgsVctSourceSelect::onFileChanged);

}

void QgsVctSourceSelect::addButtonClicked()
{
	emit addVectorLayer(lineEditFilePath->text(), lineEditLayerName->text());
}

void QgsVctSourceSelect::openFileDialog()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Choose a VCT File to Open"), "", tr("VCT files (*.vct *.VCT)"));
	lineEditFilePath->setText(path);
}

void QgsVctSourceSelect::onFileChanged()
{
	QFileInfo finfo(lineEditFilePath->text());
	lineEditLayerName->setText(finfo.baseName());
	bool addButtonEnabled = false;
	emit enableButtons(addButtonEnabled);


	//Check input
	if (lineEditFilePath->text().isEmpty())
		return;
	if (!finfo.exists())
		return;
	if (!finfo.isFile())
		return;
	
	addButtonEnabled = true;
	emit enableButtons(addButtonEnabled);
}
