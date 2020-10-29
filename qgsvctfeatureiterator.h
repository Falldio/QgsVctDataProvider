#pragma once
#include "qgsvctprovider.h"

class QgsVctFeatureSource final: public QgsAbstractFeatureSource
{
public:
	explicit QgsVctFeatureSource(const QgsVctProvider *p);
	QgsFeatureIterator getFeatures(const QgsFeatureRequest &request) override;

private:
	QgsRectangle mExtent;
	std::unique_ptr< QgsSpatialIndex > mSpatialIndex;
	QgsFields mFields;
	QgsWkbTypes::GeometryType mGeometryType;
	QgsWkbTypes::Type mWkbType = QgsWkbTypes::NoGeometry;
	QgsCoordinateReferenceSystem mCrs;
	QgsFeatureMap mFeatures;
	QgsExpressionContext mExpressionContext;


	friend class QgsVctFeatureIterator;
};

class QgsVctFeatureIterator final : public QgsAbstractFeatureIteratorFromSource<QgsVctFeatureSource>
{
public:
	QgsVctFeatureIterator(QgsVctFeatureSource *source, bool ownSource, const QgsFeatureRequest &request);

	~QgsVctFeatureIterator() override;

	bool rewind() override;
	bool close() override;
	
protected:
	bool fetchFeature(QgsFeature &feature) override;

private:
	QgsFeatureMap::const_iterator mSelectIterator;
	QgsCoordinateTransform mTransform;


};


