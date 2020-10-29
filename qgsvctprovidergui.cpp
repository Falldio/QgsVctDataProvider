#include "qgssourceselectprovider.h"
#include "qgsproviderguimetadata.h"
#include "qgsapplication.h"
#include "qgsvctsourceselect.h"
#include "qgsvctprovider.h"

//Provider for vct file source select
class QgsVctSourceSelectProvider :public QgsSourceSelectProvider
{
public:

	QString providerKey() const override { return QStringLiteral("vctfile"); }
	QString text() const override { return QObject::tr("VCT File"); }
	int ordering() const override { return QgsSourceSelectProvider::OrderLocalProvider + 20; }
	QIcon icon() const override { return QgsApplication::getThemeIcon(QStringLiteral("")); }
	QgsAbstractDataSourceWidget *createDataSourceWidget(QWidget *parent = nullptr, Qt::WindowFlags fl = Qt::Widget, QgsProviderRegistry::WidgetMode widgetMode = QgsProviderRegistry::WidgetMode::Embedded) const override
	{
		return new QgsVctSourceSelect(parent, fl,widgetMode);
	}
};

class QgsVctProviderGuiMetadata : public QgsProviderGuiMetadata
{
public:
	QgsVctProviderGuiMetadata()
		: QgsProviderGuiMetadata(QgsVctProvider::VCT_PROVIDER_KEY)
	{
	}

	QList<QgsSourceSelectProvider *> sourceSelectProviders() override
	{
		QList<QgsSourceSelectProvider *> providers;
		providers << new QgsVctSourceSelectProvider;
		return providers;
	}
};

QGISEXTERN QgsProviderGuiMetadata *providerGuiMetadataFactory()
{
	return new QgsVctProviderGuiMetadata();
}