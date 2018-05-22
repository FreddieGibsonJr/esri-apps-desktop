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

#ifndef CHANGEMILITARYSYMBOLSIZE_H
#define CHANGEMILITARYSYMBOLSIZE_H

namespace Esri {
    namespace ArcGISRuntime {
        class Map;
        class MapQuickView;

        class DictionarySymbolStyle;
        class DictionaryRenderer;

        class FeatureCollectionTable;
        class FeatureCollectionLayer;
        class FeatureCollection;

        class UniqueValueRenderer;
    }
}

#include <QQuickItem>

class ChangeMilitarySymbolSize : public QQuickItem
{
    Q_OBJECT

public:
    ChangeMilitarySymbolSize(QQuickItem* parent = nullptr);
    ~ChangeMilitarySymbolSize();

    void componentComplete() override;
    static void init();
    Q_INVOKABLE void btnUPressed();
    Q_INVOKABLE void btnDPressed();
    Q_INVOKABLE void btnSPressed(int position);

private:
    Esri::ArcGISRuntime::Map*             m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView*    m_mapView = nullptr;

    Esri::ArcGISRuntime::FeatureCollectionTable* m_dTable = nullptr;
    Esri::ArcGISRuntime::FeatureCollectionTable* m_uTable = nullptr;

    Esri::ArcGISRuntime::FeatureCollection* m_dCollection = nullptr;
    Esri::ArcGISRuntime::FeatureCollection* m_uCollection = nullptr;

    Esri::ArcGISRuntime::FeatureCollectionLayer* m_dLayer = nullptr;
    Esri::ArcGISRuntime::FeatureCollectionLayer* m_uLayer = nullptr;

    Esri::ArcGISRuntime::DictionaryRenderer* m_dRend = nullptr;
    Esri::ArcGISRuntime::UniqueValueRenderer* m_uRend = nullptr;

    const QString FieldName = "sidc";

    void createFeature(QString sidc, double x, double y);
    void createFeatures();
    void updateStartLocation();

    double m_startX;
    double m_startY;

};

#endif // CHANGEMILITARYSYMBOLSIZE_H
