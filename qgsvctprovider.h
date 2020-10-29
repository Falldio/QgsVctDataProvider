#pragma once

#include "qgsvctprovider_global.h"
#include "qgsvectordataprovider.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsfields.h"
#include "qgsspatialindex.h"
#include "qgsprovidermetadata.h"

#include "QTextStream"

typedef QMap<QgsFeatureId, QgsFeature> QgsFeatureMap;

class QgsFeature;
class QgsField;
class QgsGeometry;
class QgsPointXY;
class QFile;
class QTextStream;

class QgsVctFeatureIterator;
class QgsExpression;
class QgsSpatialIndex;


class QGSVCTPROVIDER_EXPORT QgsVctProvider final: public QgsVectorDataProvider
{
	Q_OBJECT

public:

	static const QString VCT_PROVIDER_KEY;
	static const QString VCT_PROVIDER_DESCRIPTION;

	explicit QgsVctProvider(const QString &uri, const QgsDataProvider::ProviderOptions &providerOptions);
	~QgsVctProvider() override;

	/* Implementation of functions from QgsVectorDataProvider */
	QgsAbstractFeatureSource *featureSource() const override;
	QString storageType() const override;
	QgsFeatureIterator getFeatures(const QgsFeatureRequest &request) const override;
	QgsWkbTypes::Type wkbType() const override;
	long featureCount() const override;
	QgsFields fields() const override;
	QgsVectorDataProvider::Capabilities capabilities() const override;
	bool createSpatialIndex() override;
	QgsFeatureSource::SpatialIndexPresence hasSpatialIndex() const override;
	QString name() const override;
	QString description() const override;
	QgsRectangle extent() const override;
	bool isValid() const override;
	QgsCoordinateReferenceSystem crs() const override;
	bool setSubsetString(const QString &subset, bool updateFeatureCount = true) override;
	bool supportsSubsetString() const override { return false; }
	QString subsetString() const override
	{
		return mSubsetString;
	}
	QString dataComment() const override;
	bool addFeatures(QgsFeatureList &flist, QgsFeatureSink::Flags flags = nullptr) override;
	bool deleteFeatures(const QgsFeatureIds &id) override;
	bool addAttributes(const QList<QgsField> &attributes) override;
	bool renameAttributes(const QgsFieldNameMap &renamedAttributes) override;
	bool deleteAttributes(const QgsAttributeIds &attributes) override;
	bool changeAttributeValues(const QgsChangedAttributesMap &attr_map) override;
	bool changeGeometryValues(const QgsGeometryMap &geometry_map) override;
	void updateExtents() override;


private:

	QString mSubsetString;

	bool mLayerValid = false;
	int mNextFeatureId = 0;
	//Vct file writing functions
	void writeData();

	//Vct file reading functions
	QString mUri;
	void readData(QString uri);
	void readComment(QTextStream &stream);
	void readHead(QTextStream &stream);
	void readFeatureCode(QTextStream &stream);
	void readTableStructure(QTextStream &stream);
	void readPoint(QTextStream &stream);
	void readLine(QTextStream &stream);
	void readPolygon(QTextStream &stream);
	void readSolid(QTextStream &stream);
	void readAggregation(QTextStream &stream);
	void readAnnotation(QTextStream &stream);
	void readTopology(QTextStream &stream);
	void readAttribute(QTextStream &stream);
	void readStyle(QTextStream &stream);

	//注释
	QStringList mComments;

	//文件头
	QgsCoordinateReferenceSystem mCrs;
	mutable QgsRectangle mExtent;
	QVector<QString> mHead;

	//要素类型参数
	QString mFeatureTypeCode;//要素类型编码
	QString mFeatureTypeName;//要素类型名称
	QString mAttributeTableName;//属性表名
	QgsWkbTypes::Type mWkbType = QgsWkbTypes::NoGeometry;
	QgsWkbTypes::GeometryType mGeometryType = QgsWkbTypes::UnknownGeometry;
	QVector<QString> mCustomItems;//用户自定义项


	//属性数据结构
	QgsFields mFields;

	//Features
	QgsFeatureMap mFeatures;

	//std::unique_ptr< QgsExpression > mSubsetExpression;

	//Spatial index
	QgsSpatialIndex *mSpatialIndex = nullptr;



	/*mutable QList<quintptr> mSubsetIndex;
	mutable bool mUseSubsetIndex = false;
	mutable bool mCachedUseSubsetIndex;*/

	friend class QgsVctFeatureIterator;
	friend class QgsVctFeatureSource;
};

class QgsVctProviderMetadata final : public QgsProviderMetadata
{
public:
	QgsVctProviderMetadata();
	QgsDataProvider *createProvider(const QString &uri, const QgsDataProvider::ProviderOptions &options) override;
	QVariantMap decodeUri(const QString &uri) override;
	QString encodeUri(const QVariantMap &parts) override;
};
