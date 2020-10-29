#include "qgsvctfeatureiterator.h"
#include "qgsexpressioncontextutils.h"
#include "qgsproject.h"
#include "qgsmessagelog.h"



QgsVctFeatureIterator::QgsVctFeatureIterator(QgsVctFeatureSource *source, bool ownSource, const QgsFeatureRequest &request)
	: QgsAbstractFeatureIteratorFromSource<QgsVctFeatureSource>(source, ownSource, request)
{
	if (mRequest.destinationCrs().isValid() && mRequest.destinationCrs() != mSource->mCrs)
	{
		mTransform = QgsCoordinateTransform(mSource->mCrs, mRequest.destinationCrs(), mRequest.transformContext());
	}
	rewind();
}

QgsVctFeatureIterator::~QgsVctFeatureIterator()
{
	close();
}

bool QgsVctFeatureIterator::rewind()
{
	if (mClosed)
		return false;
	mSelectIterator = mSource->mFeatures.constBegin();

	return true;
}

bool QgsVctFeatureIterator::close()
{
	if (mClosed)
		return false;

	iteratorClosed();

	return true;
}

bool QgsVctFeatureIterator::fetchFeature(QgsFeature &feature)
{
	feature.setValid(false);
	if (mClosed)
		return false;
	if (mSelectIterator != mSource->mFeatures.constEnd())
	{
		feature = mSelectIterator.value();
		feature.setValid(true);
		feature.setFields(mSource->mFields);
		geometryToDestinationCrs(feature, mTransform);
		++mSelectIterator;
		return true;
	}
	else
	{
		close();
		return false;
	}
}

QgsVctFeatureSource::QgsVctFeatureSource(const QgsVctProvider *p)
	: mExtent(p->mExtent)
	, mGeometryType(p->mGeometryType)
	, mCrs(p->mCrs)
	, mFeatures(p -> mFeatures)
	, mFields(p->mFields)
{
}

QgsFeatureIterator QgsVctFeatureSource::getFeatures(const QgsFeatureRequest &request)
{
	return QgsFeatureIterator(new QgsVctFeatureIterator(this, false, request));
}