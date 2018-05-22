// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"

#include "DictionaryRenderer.h"
#include "UniqueValueRenderer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleLineSymbol.h"
#include "MultilayerPointSymbol.h"

#include "FeatureCollectionTable.h"
#include "FeatureCollectionLayer.h"
#include "FeatureCollection.h"

#include "Point.h"
#include "GeometryEngine.h"
#include "SpatialReference.h"

#include <iostream>
#include <sstream>
#include <QDir>
#include <QDebug>

#include "ChangeMilitarySymbolSize.h"

using namespace Esri::ArcGISRuntime;

ChangeMilitarySymbolSize::ChangeMilitarySymbolSize(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent)
{
}

ChangeMilitarySymbolSize::~ChangeMilitarySymbolSize()
{
}

void ChangeMilitarySymbolSize::componentComplete()
{
    QQuickItem::componentComplete();

    m_startX = -117.1825;
    m_startY =   34.0556;

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // Create a map using the navigationVector Basemap
    m_map = new Map(BasemapType::NavigationVector, m_startY, m_startX, 16, this);

    // Set map to map view
    m_mapView->setMap(m_map);

    Point sPointWGS = Point(m_startX, m_startY, SpatialReference::wgs84());
    Point sPointWMe = GeometryEngine::project(sPointWGS, SpatialReference::webMercator());

    m_startX = sPointWMe.x();
    m_startY = sPointWMe.y();

    // Create the Dictionary and Unique Value Renderer
    const QString stylePath = QDir::currentPath() + QStringLiteral("/styles/mil2525c_b2.stylx");
    qDebug() << "Style Path: " << stylePath;

    DictionarySymbolStyle* style = new DictionarySymbolStyle(QString("mil2525c_b2"), stylePath, this);

    m_dRend = new DictionaryRenderer(style, this);
    m_uRend = new UniqueValueRenderer(this);
    m_uRend->setFieldNames(QStringList() << FieldName);

    // Create the fields for the Feature Collection table
    QList<Field> fields;
    fields.push_back(Field::createText(FieldName, FieldName, 15));

    // Create the Feature Collection Tables
    //m_dTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference::wgs84(), this);
    //m_uTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference::wgs84(), this);
    m_dTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference::webMercator(), this);
    m_uTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference::webMercator(), this);

    // Assign the renderers to the tables
    m_dTable->setRenderer(m_dRend);
    m_uTable->setRenderer(m_uRend);

    // Create a feature collection and the tables
    m_uCollection = new FeatureCollection(this);
    m_uCollection->tables()->append(m_dTable);

    m_dCollection = new FeatureCollection(this);
    m_dCollection->tables()->append(m_uTable);

    // Add the collections to the map
    m_dLayer = new FeatureCollectionLayer(m_dCollection, this);
    m_uLayer = new FeatureCollectionLayer(m_uCollection, this);
    m_map->operationalLayers()->append(m_dLayer);
    m_map->operationalLayers()->append(m_uLayer);

    connect(style, &DictionarySymbolStyle::doneLoading, this, [this](Error error){
       if (!error.isEmpty())
           return;

        createFeatures();
    });

    style->load();
}

void ChangeMilitarySymbolSize::createFeatures() {

    QStringList codes;
    int count;
    int quotient;


    // Page 91
    qDebug() << "Page 91";
    codes << "SUPP-----------" << "SFPP-----------" << "SNPP-----------" << "SHPP-----------"
          << "SUPPS----------" << "SFPPS----------" << "SNPPS----------" << "SHPPS----------"
          << "SUPPV----------" << "SFPPV----------" << "SNPPV----------" << "SHPPV----------"
          << "SUPPT----------" << "SFPPT----------" << "SNPPT----------" << "SHPPT----------"
          << "SUPPL----------" << "SFPPL----------" << "SNPPL----------" << "SHPPL----------"
          << "SUAP-----------" << "SFAP-----------" << "SNAP-----------" << "SHAP-----------";

    count = 0;
    while(!codes.isEmpty()) {
        for (int n = 0; n < 4; n++) {
            quotient = count / 4;
            createFeature(codes.takeFirst(), m_startX + (n * 100), m_startY - (quotient * 100));
            count++;
        }
    }
    updateStartLocation();

    /*
    // Page 92
    qDebug() << "Page 92";
    codes << "SUAPM----------" << "SFAPM----------" << "SNAPM----------" << "SHAPM----------"
          << "SUAPMF---------" << "SFAPMF---------" << "SNAPMF---------" << "SHAPMF---------"
          << "SUAPMFB--------" << "SFAPMFB--------" << "SNAPMFB--------" << "SHAPMFB--------"
          << "SUAPMFF--------" << "SFAPMFF--------" << "SNAPMFF--------" << "SHAPMFF--------"
          << "SUAPMFFI-------" << "SFAPMFFI-------" << "SNAPMFFI-------" << "SHAPMFFI-------";

    count = 0;
    while(!codes.isEmpty()) {
        for (int n = 0; n < 4; n++) {
            quotient = count / 4;
            createFeature(codes.takeFirst(), m_startX + (n * 100), m_startY - (quotient * 100));
            count++;
        }
    }
    updateStartLocation();

    // Page 93
    qDebug() << "Page 93";
    codes << "SUAPMFT--------" << "SFAPMFT--------" << "SNAPMFT--------" << "SHAPMFT--------"
          << "SUAPMFA--------" << "SFAPMFA--------" << "SNAPMFA--------" << "SHAPMFA--------"
          << "SUAPMFL--------" << "SFAPMFL--------" << "SNAPMFL--------" << "SHAPMFL--------"
          << "SUAPMFK--------" << "SFAPMFK--------" << "SNAPMFK--------" << "SHAPMFK--------"
          << "SUAPMFKB-------" << "SFAPMFKB-------" << "SNAPMFKB-------" << "SHAPMFKB-------";

    count = 0;
    while(!codes.isEmpty()) {
        for (int n = 0; n < 4; n++) {
            quotient = count / 4;
            createFeature(codes.takeFirst(), m_startX + (n * 100), m_startY - (quotient * 100));
            count++;
        }
    }
    updateStartLocation();

    // Page 94
    qDebug() << "Page 94";
    codes << "SUAPMFKD-------" << "SFAPMFKD-------" << "SNAPMFKD-------" << "SHAPMFKD-------"
          << "SUAPMFC--------" << "SFAPMFC--------" << "SNAPMFC--------" << "SHAPMFC--------"
          << "SUAPMFCL-------" << "SFAPMFCL-------" << "SNAPMFCL-------" << "SHAPMFCL-------"
          << "SUAPMFCM-------" << "SFAPMFCM-------" << "SNAPMFCM-------" << "SHAPMFCM-------"
          << "SUAPMFCH-------" << "SFAPMFCH-------" << "SNAPMFCH-------" << "SHAPMFCH-------";

    count = 0;
    while(!codes.isEmpty()) {
        for (int n = 0; n < 4; n++) {
            quotient = count / 4;
            createFeature(codes.takeFirst(), m_startX + (n * 100), m_startY - (quotient * 100));
            count++;
        }
    }
    updateStartLocation();

    // Page 95
    qDebug() << "Page 95";
    codes << "SUAPMFJ--------" << "SFAPMFJ--------" << "SNAPMFJ--------" << "SHAPMFJ--------"
          << "SUAPMFO--------" << "SFAPMFO--------" << "SNAPMFO--------" << "SHAPMFO--------"
          << "SUAPMFR--------" << "SFAPMFR--------" << "SNAPMFR--------" << "SHAPMFR--------"
          << "SUAPMFRW-------" << "SFAPMFRW-------" << "SNAPMFRW-------" << "SHAPMFRW-------"
          << "SUAPMFRZ-------" << "SFAPMFRZ-------" << "SNAPMFRZ-------" << "SHAPMFRZ-------";

    count = 0;
    while(!codes.isEmpty()) {
        for (int n = 0; n < 4; n++) {
            quotient = count / 4;
            createFeature(codes.takeFirst(), m_startX + (n * 100), m_startY - (quotient * 100));
            count++;
        }
    }
    updateStartLocation();

    // Page 96
    qDebug() << "Page 96";
    codes << "SUAPMFRX-------" << "SFAPMFRX-------" << "SNAPMFRX-------" << "SHAPMFRX-------"
          << "SUAPMFP--------" << "SFAPMFP--------" << "SNAPMFP--------" << "SHAPMFP--------"
          << "SUAPMFPN-------" << "SFAPMFPN-------" << "SNAPMFPN-------" << "SHAPMFPN-------"
          << "SUAPMFPM-------" << "SFAPMFPM-------" << "SNAPMFPM-------" << "SHAPMFPM-------"
          << "SUAPMFU--------" << "SFAPMFU--------" << "SNAPMFU--------" << "SHAPMFU--------";

    count = 0;
    while(!codes.isEmpty()) {
        for (int n = 0; n < 4; n++) {
            quotient = count / 4;
            createFeature(codes.takeFirst(), m_startX + (n * 100), m_startY - (quotient * 100));
            count++;
        }
    }
    updateStartLocation();
    */
}

void ChangeMilitarySymbolSize::createFeature(QString sidc, double x, double y) {
    Feature* dFeature = m_dTable->createFeature(this);
    Feature* uFeature = m_uTable->createFeature(this);

    //Point dPoint = Point(x, y, SpatialReference::wgs84());
    //Point uPoint = Point(x + 0.5, y, SpatialReference::wgs84());
    Point dPoint = Point(x, y, SpatialReference::webMercator());
    Point uPoint = Point(x + 50, y, SpatialReference::webMercator());

    dFeature->setGeometry(dPoint);
    uFeature->setGeometry(uPoint);

    dFeature->attributes()->replaceAttribute(FieldName, sidc);
    uFeature->attributes()->replaceAttribute(FieldName, sidc);

    m_dTable->addFeature(dFeature);
    m_uTable->addFeature(uFeature);

    MultilayerPointSymbol* symbol = (MultilayerPointSymbol*) MultilayerPointSymbol::fromJson(m_dRend->symbol(dFeature)->toJson());
    symbol->setSize(44);

    UniqueValue* uval = new UniqueValue(sidc, sidc, QVariantList() << sidc, symbol, this);
    m_uRend->uniqueValues()->append(uval);
}

void ChangeMilitarySymbolSize::updateStartLocation() {
    //m_startX += 5;
    m_startX += 500;
}

void ChangeMilitarySymbolSize::btnUPressed() {
    m_uLayer->setVisible(!m_uLayer->isVisible());
}

void ChangeMilitarySymbolSize::btnDPressed() {
    m_dLayer->setVisible(!m_dLayer->isVisible());
}

void ChangeMilitarySymbolSize::btnSPressed(int position) {
    for(int i = 0; i < m_uRend->uniqueValues()->size(); i++) {
        UniqueValue* uval = m_uRend->uniqueValues()->at(i);

        // Change the symbol size
        MultilayerPointSymbol* symbol = (MultilayerPointSymbol*) uval->symbol();
        symbol->setSize(position);

        uval->setSymbol(symbol);
    }
}
