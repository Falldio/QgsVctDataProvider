#include "qgsvctprovider.h"
#include "qgsvctfeatureiterator.h"
#include "qgslogger.h"
#include "qgsgeometry.h"
#include "qgsmultilinestring.h"
#include "qgslinestring.h"
#include "qgsmessagelog.h"

const QString QgsVctProvider::VCT_PROVIDER_KEY = QStringLiteral("vctfile");
const QString QgsVctProvider::VCT_PROVIDER_DESCRIPTION = QStringLiteral("VCT data provider");

QgsVctProvider::QgsVctProvider(const QString &uri, const ProviderOptions &options)
	: QgsVectorDataProvider(uri, options)
{
	// Add supported types to enable creating expression fields in field calculator
	setNativeTypes(QList<NativeType>()
		//string type
		<< QgsVectorDataProvider::NativeType(tr("Char"), QStringLiteral("Char"), QVariant::Char, 0, 10)
		<< QgsVectorDataProvider::NativeType(tr("Varchar"), QStringLiteral("Varchar"), QVariant::String, -1, -1)

		//interger types
		<< QgsVectorDataProvider::NativeType(tr("Int1"), QStringLiteral("Int1"), QVariant::Int, -1, -1, 0, 0)
		<< QgsVectorDataProvider::NativeType(tr("Int2"), QStringLiteral("Int2"), QVariant::Int, -1, -1, 0, 0)
		<< QgsVectorDataProvider::NativeType(tr("Int4"), QStringLiteral("Int4"), QVariant::Int, -1, -1, 0, 0)
		<< QgsVectorDataProvider::NativeType(tr("Int8"), QStringLiteral("Int8"), QVariant::Int, -1, -1, 0, 0)
		
		//floating point
		<< QgsVectorDataProvider::NativeType(tr("Float"), QStringLiteral("Float"), QVariant::Double, -1, -1, -1, -1)
		<< QgsVectorDataProvider::NativeType(tr("Double"), QStringLiteral("Double"), QVariant::Double, -1, -1, -1, -1)

		//date types
		<< QgsVectorDataProvider::NativeType(tr("Date"), QStringLiteral("Date"), QVariant::Date, -1, -1, -1, -1)
		<< QgsVectorDataProvider::NativeType(tr("Time"), QStringLiteral("Time"), QVariant::Time, -1, -1, -1, -1)
		<< QgsVectorDataProvider::NativeType(tr("Datetime"), QStringLiteral("Datetime"), QVariant::DateTime, -1, -1, -1, -1)

		//binary type: store file path in a string
		<< QgsVectorDataProvider::NativeType(tr("Varbin"), QStringLiteral("Varbin"), QVariant::String, -1, -1)
	);

	mUri = uri;
	readData(mUri);
	mNextFeatureId = featureCount() + 1;
}

QgsVctProvider::~QgsVctProvider()
{
	if (mSpatialIndex != nullptr)
		delete mSpatialIndex;
}

QgsAbstractFeatureSource *QgsVctProvider::featureSource() const
{
	return new QgsVctFeatureSource(this);
}

QString QgsVctProvider::storageType() const
{
	return QStringLiteral("VCT file");
}

QgsFeatureIterator QgsVctProvider::getFeatures(const QgsFeatureRequest &request) const
{
	return QgsFeatureIterator(new QgsVctFeatureIterator(new QgsVctFeatureSource(this), true, request));
}

QString QgsVctProvider::dataComment() const
{
	QString comment;
	for (int i = 0; i < mComments.size(); i++)
	{
		comment += mComments[i];
	}
	return comment;
}

QgsWkbTypes::Type QgsVctProvider::wkbType() const
{
	return mWkbType;
}

long QgsVctProvider::featureCount() const
{
	return mFeatures.count();
}

QgsFields QgsVctProvider::fields() const
{
	return mFields;
}

QgsVectorDataProvider::Capabilities QgsVctProvider::capabilities() const
{
	return AddFeatures | DeleteFeatures | ChangeGeometries |
		ChangeAttributeValues | AddAttributes | DeleteAttributes | RenameAttributes;
}

bool QgsVctProvider::createSpatialIndex()
{
	return false;
}

QgsFeatureSource::SpatialIndexPresence QgsVctProvider::hasSpatialIndex() const
{
	return QgsFeatureSource::SpatialIndexNotPresent;
}

QString QgsVctProvider::name() const
{
	return VCT_PROVIDER_KEY;
}

QString QgsVctProvider::description() const
{
	return VCT_PROVIDER_DESCRIPTION;
}

QgsRectangle QgsVctProvider::extent() const
{
	return mExtent;
}

bool QgsVctProvider::isValid() const
{
	//return mLayerValid;
	return (mWkbType != QgsWkbTypes::Unknown);
}

QgsCoordinateReferenceSystem QgsVctProvider::crs() const
{
	return mCrs;
}

bool QgsVctProvider::setSubsetString(const QString &subset, bool updateFeatureCount)
{
	return false;
}

void QgsVctProvider::readData(QString uri)
{
	//read Data
	QFile vctFile(uri);
	vctFile.open(QIODevice::ReadOnly);
	QTextStream vctStream(&vctFile);
	vctStream.setCodec(QTextCodec::codecForName("UTF-8"));
	QString extra = vctStream.readLine();
	while (!vctStream.atEnd())
	{
		if (extra.contains("CommentBegin"))
			readComment(vctStream);
		else if (extra.contains("HeadBegin"))
			readHead(vctStream);
		else if (extra.contains("FeatureCodeBegin"))
			readFeatureCode(vctStream);
		else if (extra.contains("TableStructureBegin"))
			readTableStructure(vctStream);
		else if (extra.contains("PointBegin"))
			readPoint(vctStream);
		else if (extra.contains("LineBegin"))
			readLine(vctStream);
		else if (extra.contains("PolygonBegin"))
			readPolygon(vctStream);
		else if (extra.contains("SolidBegin"))
			readSolid(vctStream);
		else if (extra.contains("AggregationBegin"))
			readAggregation(vctStream);
		else if (extra.contains("AnnotationBegin"))
			readAnnotation(vctStream);
		else if (extra.contains("TopologyBegin"))
			readTopology(vctStream);
		else if (extra.contains("AttributeBegin"))
			readAttribute(vctStream);
		else if (extra.contains("StyleBegin"))
			readStyle(vctStream);
		extra = vctStream.readLine();
	}
	vctFile.close();
}

void QgsVctProvider::readComment(QTextStream &stream)
{
	QString comment = "";
	QString extra = stream.readLine();
	while (!extra.contains("CommentEnd"))
	{
		comment += extra;
		extra = stream.readLine();
	}
	mComments.append(comment);
}

void QgsVctProvider::readHead(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("HeadEnd"))
	{
		QStringList list = extra.split(':');
		QString key = list[0];
		QString value = list[1];
		mHead.append(extra);
		if (key.contains("Spheroid"))
		{
			QStringList values = value.split(',');
			//QgsMessageLog::logMessage("spheroidValues[0]: " + values[0], "vct", Qgis::Critical, true);
			if (values[0].contains("中国2000国家大地"))
			{
				mCrs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4526"));
			}
			else if (values[0].contains("克拉索夫斯基(1940)"))
				mCrs = QgsCoordinateReferenceSystem(QStringLiteral("EPSG:4024"));
		}
		else if (key.contains("ExtentMin"))
		{
			QStringList values = value.split(',');
			mExtent.setXMinimum(values[0].toDouble());
			mExtent.setYMinimum(values[1].toDouble());
		}
		else if (key.contains("ExtentMax"))
		{
			QStringList values = value.split(',');
			mExtent.setXMaximum(values[0].toDouble());
			mExtent.setYMaximum(values[1].toDouble());
		}
		extra = stream.readLine();
	}
}

void QgsVctProvider::readFeatureCode(QTextStream &stream)
{
	QStringList values = stream.readLine().split(',');
	mFeatureTypeCode = values[0];
	mFeatureTypeName = values[1];
	QString geometryType = values[2];
	mAttributeTableName = values[3];
	if (geometryType.contains("Point"))
	{
		mWkbType = QgsWkbTypes::MultiPoint;
		mGeometryType = QgsWkbTypes::PointGeometry;
	}
	else if (geometryType.contains("Line"))
	{
		mWkbType = QgsWkbTypes::MultiLineString;
		mGeometryType = QgsWkbTypes::LineGeometry;
	}
	else if (geometryType.contains("Polygon"))
	{
		mWkbType = QgsWkbTypes::MultiPolygon;
		mGeometryType = QgsWkbTypes::PolygonGeometry;
	}
	else
	{
		mWkbType = QgsWkbTypes::Unknown;
		mGeometryType = QgsWkbTypes::UnknownGeometry;
	}
	QString extra = stream.readLine();
	while (!extra.contains("FeatureCodeEnd"))
	{
		//略过用户项
		mCustomItems.append(extra);
		extra = stream.readLine();
	}
}

void QgsVctProvider::readTableStructure(QTextStream &stream)
{
	QStringList list = stream.readLine().split(',');
	for (int i = 0; i < list[1].toInt(); i++)
	{
		QStringList extra = stream.readLine().split(',');
		QString field = extra[0];
		QString type = extra[1];
		int length=0, prec=0;
		if (type.contains("Int"))	type = "Int";
		if (extra.length() >= 3)
			length = extra[2].toInt();
		if (extra.length() == 4)
			prec = extra[3].toInt();
		if (type == "Double")
			mFields.append(QgsField(field, QVariant::Double, type, length, prec));
		else if (type == "Int")
			mFields.append(QgsField(field, QVariant::Int, type, length, prec));
		else
			mFields.append(QgsField(field, QVariant::String, type, length, prec));
	}
}

void QgsVctProvider::readPoint(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("PointEnd"))
	{
		int id = extra.toInt();
		QString featureTypeCode = stream.readLine();
		QString graphicCode = stream.readLine();
		int featureType = stream.readLine().toInt();
		QgsFeature f;
		if (featureType != 4)
		{
			//独立点、结点、有向点
			QStringList coordinates = stream.readLine().split(',');
			double x = coordinates[0].toDouble();
			double y = coordinates[1].toDouble();
			QgsMultiPointXY pt;
			pt.append(QgsPointXY(x, y));
			f.setGeometry(QgsGeometry::fromMultiPointXY(pt));
		}
		else {
			//点簇
			int count = stream.readLine().toInt();
			QgsMultiPointXY g;
			for (int i = 0; i < count; i++)
			{
				QStringList coordinates = stream.readLine().split(',');
				double x = coordinates[0].toDouble();
				double y = coordinates[1].toDouble();
				g.append(QgsPointXY(x, y));
			}
			f.setGeometry(QgsGeometry::fromMultiPointXY(g));
		}
		f.setId(id);
		mFeatures.insert(id, f);
		if (stream.readLine()=='0')
		{
			stream.readLine();
			extra = stream.readLine();
		}
	}
}

void QgsVctProvider::readLine(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("LineEnd"))
	{
		int id = extra.toInt();
		QString featureCode = stream.readLine();
		QString graphicCode = stream.readLine();
		int featureType = stream.readLine().toInt();
		QgsFeature f;
		QgsMultiPolylineXY g;
		if (featureType == 1)
		{
			//直接坐标线
			int count = stream.readLine().toInt();
			for (int i = 0; i < count; i++)
			{
				int lineType = stream.readLine().toInt();
				if (lineType == 11)
				{
					//折线
					int ptCount = stream.readLine().toInt();
					QgsPolylineXY pts;
					for (int j = 0; j < ptCount; j++)
					{
						QStringList coordinates = stream.readLine().split(',');
						double x = coordinates[0].toDouble();
						double y = coordinates[1].toDouble();
						pts.append(QgsPointXY(x, y));
					}
					g.append(QgsPolylineXY(pts));
				}
			}
		}
		f.setGeometry(QgsGeometry::fromMultiPolylineXY(g));
		f.setId(id);
		mFeatures.insert(id,f);
		if (stream.readLine()=='0')
		{
			stream.readLine();
			extra = stream.readLine();
		}
	}
}

void QgsVctProvider::readPolygon(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (extra != "PolygonEnd")
	{
		int id = extra.toInt();
		QString featureCode = stream.readLine();
		QString graphicCode = stream.readLine();
		int featureType = stream.readLine().toInt();
		QStringList coordinate = stream.readLine().split(',');
		QgsPointXY markPoint(coordinate[0].toDouble(), coordinate[1].toDouble());
		QgsFeature f;
		QgsMultiPolygonXY g;
		QgsPolygonXY *polygon = nullptr;
		int endFlag = -1;
		int originalShape = -1;//保存上一个主面的geometryShape
		if (featureType == 1)
		{
			//由直接坐标表示的面对象
			int borderCount = stream.readLine().toInt();
			int i = 0;
			//for (int i = 0; i < borderCount; i++)
			while (i < borderCount+1)//假设存在一个附属面
			{
				int geometryShape = stream.readLine().toInt();
				if (geometryShape == 0)
				{
					//全部读取完毕
					endFlag = 0;
					if (polygon != nullptr)//保存最后一个主面
						g.append(*polygon);
					break;
				}
				//int pos = stream.pos();//记录位置
				QString str = stream.readLine();
				int pointCount;
				if (!str.contains(','))
				{
					//主面
					originalShape = geometryShape;
					if (polygon != nullptr)//保存上一个主面
						g.append(*polygon);
					polygon = new QgsPolygonXY;
					pointCount = str.toInt();
					if (geometryShape == 11)
					{
						QgsPolylineXY polyline;
						for (int j = 0; j < pointCount; j++)
						{
							QStringList coordinates = stream.readLine().split(',');
							double x = coordinates[0].toDouble();
							double y = coordinates[1].toDouble();
							polyline.append(QgsPointXY(x, y));
						}
						polygon->append(polyline);
					}
				}
				else {
					//附属面
					pointCount = geometryShape;
					//stream.seek(pos);
					if (originalShape == 11)
					{
						borderCount++;//假设存在下一个附属面
						QgsPolylineXY polyline;
						QStringList coordinates = str.split(',');
						double x = coordinates[0].toDouble();
						double y = coordinates[1].toDouble();
						polyline.append(QgsPointXY(x, y));
						for (int j = 0; j < pointCount - 1; j++)
						{
							QStringList coordinates = stream.readLine().split(',');
							double x = coordinates[0].toDouble();
							double y = coordinates[1].toDouble();
							polyline.append(QgsPointXY(x, y));
						}
						polygon->append(polyline);
					}
				}
				i++;
			}
		}
		f.setGeometry(QgsGeometry::fromMultiPolygonXY(g));
		f.setId(id);
		mFeatures.insert(id, f);
		if (endFlag == 0)
		{
			stream.readLine();
			extra = stream.readLine();
		}
		//else if (stream.readLine() == '0')
		//{
		//	stream.readLine();
		//	extra = stream.readLine();
		//}
	}
}

void QgsVctProvider::readSolid(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("SolidEnd"))
	{
		extra = stream.readLine();
	}
}

void QgsVctProvider::readAggregation(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("AggregationEnd"))
	{
		extra = stream.readLine();
	}
}

void QgsVctProvider::readAnnotation(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("AnnotationEnd"))
	{
		extra = stream.readLine();
	}
}

void QgsVctProvider::readTopology(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("TopologyEnd"))
	{
		extra = stream.readLine();
	}
}

void QgsVctProvider::readAttribute(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("AttributeEnd"))
	{
		QString tableName = extra;
		extra = stream.readLine();
		while (!extra.contains("TableEnd"))
		{
			QStringList info = extra.split(',');
			int id = info[0].toInt();
			QgsFeature* f = &mFeatures[id];//深拷贝
			QVector<QVariant> attrs;
			for (int i = 1; i < info.size(); i++)
			{
				attrs.append(info[i]);
			}
			f->setAttributes(attrs);
			extra = stream.readLine();
		}
		extra = stream.readLine();
	}
}

void QgsVctProvider::readStyle(QTextStream &stream)
{
	QString extra = stream.readLine();
	while (!extra.contains("StyleEnd"))
	{
		extra = stream.readLine();
	}
}

bool QgsVctProvider::addFeatures(QgsFeatureList &flist, Flags)
{
	bool result = true;
	bool updateExtent = mFeatures.isEmpty() || !mExtent.isEmpty();
	int fieldCount = mFields.count();
	
	for (QgsFeatureList::iterator it = flist.begin(); it != flist.end(); it++)
	{
		it->setId(mNextFeatureId);
		it->setValid(true);
		if (it->attributes().count() < mFields.count())
		{
			QgsAttributes attributes = it->attributes();
			for (int i = it->attributes().count(); i < mFields.count(); i++)
			{
				attributes.append(QVariant(mFields.at(i).type()));
			}
			it->setAttributes(attributes);
		}
		else if (it->attributes().count() > mFields.count()) 
		{
			pushError(tr("Feature has too many attributes (expecting %1, received %2)").arg(fieldCount).arg(it->attributes().count()));
			QgsAttributes attributes = it -> attributes();
			attributes.resize(mFields.count());
			it->setAttributes(attributes);
		}

		if (it->hasGeometry() && mWkbType == QgsWkbTypes::NoGeometry)
		{
			it->clearGeometry();
		}
		else if (it->hasGeometry() && QgsWkbTypes::geometryType(it->geometry().wkbType()) !=
			QgsWkbTypes::geometryType(mWkbType))
		{
			pushError(tr("Could not add feature with geometry type %1 to layer of type %2").arg(QgsWkbTypes::displayString(it->geometry().wkbType()),
				QgsWkbTypes::displayString(mWkbType)));
			result = false;
			continue;
		}

		mFeatures.insert(mNextFeatureId, *it);
		mNextFeatureId++;

		if (it->hasGeometry())
		{
			if (updateExtent)
				mExtent.combineExtentWith(it->geometry().boundingBox());
		}
	}
	clearMinMaxCache();
	writeData();
	return result;
}

bool QgsVctProvider::deleteFeatures(const QgsFeatureIds &id)
{
	for (QgsFeatureIds::const_iterator it = id.begin(); it != id.end(); it++)
	{
		QgsFeatureMap::iterator fit = mFeatures.find(*it);

		if (fit == mFeatures.end())	continue;

		mFeatures.erase(fit);
	}

	updateExtents();
	clearMinMaxCache();
	writeData();

	return true;
}

bool QgsVctProvider::addAttributes(const QList<QgsField> &attributes)
{
	for (QList<QgsField>::const_iterator it = attributes.begin(); it != attributes.end(); it++)
	{
		switch (it->type())
		{
		case QVariant::Int:
		case QVariant::Double:
		case QVariant::String:
		case QVariant::Date:
		case QVariant::Time:
		case QVariant::DateTime:
		case QVariant::LongLong:
		case QVariant::StringList:
		case QVariant::List:
		case QVariant::Bool:
		case QVariant::ByteArray:
			break;
		default:
			QgsDebugMsg("Field type not supported: " + it->typeName());
			continue;
		}
		mFields.append(*it);
		for (QgsFeatureMap::iterator fit = mFeatures.begin(); fit != mFeatures.end(); fit++)
		{
			QgsFeature &f = fit.value();
			QgsAttributes attr = f.attributes();
			attr.append(QVariant());
			f.setAttributes(attr);
		}
	}
	writeData();
	return true;
}

bool QgsVctProvider::renameAttributes(const QgsFieldNameMap &renamedAttributes)
{
	bool result = true;
	for (QgsFieldNameMap::const_iterator renameIt = renamedAttributes.constBegin(); renameIt != renamedAttributes.constEnd(); renameIt++)
	{
		int fieldIndex = renameIt.key();
		if (fieldIndex < 0 || fieldIndex >= mFields.count())
		{
			result = false;
			continue;
		}
		if (mFields.indexFromName(renameIt.value()) >= 0)
		{
			//field name already in use
			result = false;
			continue;
		}

		mFields.rename(fieldIndex, renameIt.value());
	}
	writeData();
	return result;
}

bool QgsVctProvider::deleteAttributes(const QgsAttributeIds &attributes)
{
	QList<int>attrIdx = attributes.toList();
	std::sort(attrIdx.begin(), attrIdx.end(), std::greater<int>());

	for (QList<int>::const_iterator it = attrIdx.constBegin(); it != attrIdx.constEnd(); it++)
	{
		int idx = *it;
		mFields.remove(idx);

		for (QgsFeatureMap::iterator fit = mFeatures.begin(); fit != mFeatures.end(); fit++)
		{
			QgsFeature &f = fit.value();
			QgsAttributes attr = f.attributes();
			attr.remove(idx);
			f.setAttributes(attr);
		}
	}
	clearMinMaxCache();
	writeData();
	return true;
}

bool QgsVctProvider::changeAttributeValues(const QgsChangedAttributesMap &attr_map)
{
	for (QgsChangedAttributesMap::const_iterator it = attr_map.begin(); it != attr_map.end(); it++)
	{
		QgsFeatureMap::iterator fit = mFeatures.find(it.key());
		if (fit == mFeatures.end())
			continue;

		const QgsAttributeMap &attrs = it.value();
		for (QgsAttributeMap::const_iterator it2 = attrs.constBegin(); it2 != attrs.constEnd(); ++it2)
			fit->setAttribute(it2.key(), it2.value());
	}
	clearMinMaxCache();
	writeData();
	return true;
}

bool QgsVctProvider::changeGeometryValues(const QgsGeometryMap &geometry_map)
{
	for (QgsGeometryMap::const_iterator it = geometry_map.begin(); it != geometry_map.end(); it++)
	{
		QgsFeatureMap::iterator fit = mFeatures.find(it.key());
		if (fit == mFeatures.end())
			continue;

		fit->setGeometry(it.value());
	}

	updateExtents();
	writeData();
	return true;
}

void QgsVctProvider::writeData()
{
	QFile vctFile(mUri);
	vctFile.open(QIODevice::WriteOnly);
	QTextStream vctStream(&vctFile);
	vctStream.setCodec(QTextCodec::codecForName("UTF-8"));

	vctStream << "HeadBegin\n";
	for (int i = 0;i < mHead.size(); i++)
	{
		vctStream << mHead[i] <<"\n";
	}
	vctStream << "HeadEnd\n";

	vctStream << "FeatureCodeBegin\n";
	if (mGeometryType == QgsWkbTypes::PointGeometry)
		vctStream << mFeatureTypeCode << "," << mFeatureTypeName << "," << "Point," << mAttributeTableName << "\n";
	else if (mGeometryType == QgsWkbTypes::LineGeometry)
		vctStream << mFeatureTypeCode << "," << mFeatureTypeName << "," << "Line," << mAttributeTableName << "\n";
	else if (mGeometryType == QgsWkbTypes::PolygonGeometry)
		vctStream << mFeatureTypeCode << "," << mFeatureTypeName << "," << "Polygon," << mAttributeTableName << "\n";
	if (mCustomItems.size() > 0)
	{
		for (int i = 0; i < mCustomItems.size(); i++)
		{
			vctStream << mCustomItems[i] << "\n";
		}
	}
	vctStream << "FeatureCodeEnd\n";

	vctStream << "TableStructureBegin\n";
	vctStream << mAttributeTableName << "," << mFields.size() << "\n";
	for (int i = 0; i < mFields.size(); i++)
	{
		vctStream << mFields[i].name() << "," << mFields[i].typeName();
		if (mFields[i].typeName() == "Double")
			vctStream << mFields[i].length() << "," << mFields[i].precision() << "\n";
		else
			vctStream << "\n";
	}
	vctStream << "0\nTableStructureEnd\n";

	vctStream << "PointBegin\n";
	if (mGeometryType == QgsWkbTypes::PointGeometry)
	{
		for (QgsFeatureMap::const_iterator it = mFeatures.begin(); it != mFeatures.end(); it++)
		{
			vctStream << it.key() << "\n";
			vctStream << mFeatureTypeCode << "\n";
			vctStream << mFeatureTypeCode << "\n";//图形表现编码
			QgsMultiPointXY g = it.value().geometry().asMultiPoint();
			if (g.size() > 1)
			{
				vctStream << 4 << "\n";
			}
			else
			{
				vctStream << 1 << "\n";
			}
			for (int i = 0; i < g.size(); i++)
			{
				vctStream << g[i].toString() << "\n";
			}
			vctStream << 0 << "\n\n";
		}
	}
	vctStream << "PointEnd\n";

	vctStream << "LineBegin\n";
	if (mGeometryType == QgsWkbTypes::LineGeometry)
	{
		for (QgsFeatureMap::const_iterator it = mFeatures.begin(); it != mFeatures.end(); it++)
		{
			vctStream << it.key() << "\n";
			vctStream << mFeatureTypeCode << "\n";
			vctStream << mFeatureTypeCode << "\n";//图形表现编码
			QgsMultiPolylineXY g = it.value().geometry().asMultiPolyline();
			if(g.size()>0)
			{
				vctStream << 1 << "\n" << g.size() << "\n";//直接坐标线
				for (int i = 0; i < g.size(); i++)
				{
					vctStream << 11 << "\n";//折线
					vctStream << g[i].size() << "\n";
					for (int j = 0; j < g[i].size(); j++)
					{
						vctStream << g[i][j].toString() << "\n";
					}
				}
			}
			else
			{
				QgsPolylineXY g = it.value().geometry().asPolyline();
				vctStream << 1 << "\n" << 1 << "\n";//直接坐标线
				vctStream << 11 << "\n";//折线
				vctStream << g.size() << "\n";
				for (int i = 0; i < g.size(); i++)
				{
					vctStream << g[i].toString() << "\n";
				}
			}
			vctStream << 0 << "\n\n";
		}
	}
	vctStream << "LineEnd\n";

	vctStream << "PolygonBegin\n";
	if (mGeometryType == QgsWkbTypes::PolygonGeometry)
	{
		for (QgsFeatureMap::const_iterator it = mFeatures.begin(); it != mFeatures.end(); it++)
		{
			vctStream << it.key() << "\n";
			vctStream << mFeatureTypeCode << "\n";
			vctStream << mFeatureTypeCode << "\n";//图形表现编码
			QgsMultiPolygonXY g = it.value().geometry().asMultiPolygon();
			vctStream << 1 << "\n" << "0.0,0.0\n";//由直接坐标表示的面对象
			if (g.size() > 0)
			{
				vctStream << g.size() << "\n";//圈数
				for (int i = 0; i < g.size(); i++)
				{
					vctStream << 11 << "\n";//多边形
					for (int j = 0; j < g[i].size(); j++)
					{
						vctStream << g[i][j].size() << "\n";//点数
						for (int k = 0; k < g[i][j].size(); k++)
						{
							vctStream << g[i][j][k].toString() << "\n";
						}
					}
				}
			}
			else
			{
				QgsPolygonXY g = it.value().geometry().asPolygon();
				vctStream << 1 << "\n";//圈数
				vctStream << 11 << "\n";//多边形
				for (int i = 0; i < g.size(); i++)
				{
					vctStream << g[i].size() << "\n";//点数
					for (int j = 0; j < g[i].size(); j++)
					{
						vctStream << g[i][j].toString() << "\n";
					}
				}
			}
			vctStream << 0 << "\n\n";
		}
	}
	vctStream << "PolygonEnd\n";

	vctStream << "AnnotationBegin\n";
	vctStream << "AnnotationEnd\n";

	vctStream << "AttributeBegin\n";
	vctStream << mAttributeTableName << "\n";
	for (QgsFeatureMap::const_iterator it = mFeatures.begin(); it != mFeatures.end(); it++)
	{
		QgsFeature f = it.value();
		vctStream << f.id() << ",";
		for (int i = 0; i < f.attributes().size(); i++)
		{
			vctStream << f.attributes()[i].toString();
			if (i != f.attributes().size() - 1)
				vctStream << ",";
			else
				vctStream << "\n";
		}
	}
	vctStream << "TableEnd\n";
	vctStream << "AttributeEnd\n";

	vctFile.close();
}

void QgsVctProvider::updateExtents()
{
	mExtent.setMinimal();
}

QVariantMap QgsVctProviderMetadata::decodeUri(const QString &uri )
{
	QVariantMap components;
	components.insert(QStringLiteral("path"), QUrl(uri).toLocalFile());
	return components;
}

QString QgsVctProviderMetadata::encodeUri(const QVariantMap &parts)
{
	return QStringLiteral("file://%1").arg(parts.value(QStringLiteral("path")).toString());
}

QgsDataProvider *QgsVctProviderMetadata::createProvider(const QString &uri, const QgsDataProvider::ProviderOptions &options)
{
	return new QgsVctProvider(uri, options);
}

QgsVctProviderMetadata::QgsVctProviderMetadata():
	QgsProviderMetadata(QgsVctProvider::VCT_PROVIDER_KEY, QgsVctProvider::VCT_PROVIDER_DESCRIPTION)
{
}

QGISEXTERN QgsProviderMetadata *providerMetadataFactory()
{
	return new QgsVctProviderMetadata();
}

