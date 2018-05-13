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

#ifndef DISPLAYMILITARYSYMBOLS_H
#define DISPLAYMILITARYSYMBOLS_H

namespace Esri {
    namespace ArcGISRuntime {
        class Map;
        class MapQuickView;

        class FeatureCollectionTable;
        class FeatureCollection;
        class FeatureCollectionLayer;
        class Feature;

        class DictionaryRenderer;
        class UniqueValueRenderer;
    }
}

#include <QQuickItem>
#include <string>

class DisplayMilitarySymbols : public QQuickItem
{
    Q_OBJECT

    public:
        DisplayMilitarySymbols(QQuickItem* parent = nullptr);
        ~DisplayMilitarySymbols();

        void componentComplete() override;
        Q_INVOKABLE QStringList GenerateSymbolCodes(int count = 0, int skip = 0);

    private:
        Esri::ArcGISRuntime::Map*             m_map = nullptr;
        Esri::ArcGISRuntime::MapQuickView*    m_mapView = nullptr;

        const QString FieldName = "sidc";
        const int Meters = 1000;

        Esri::ArcGISRuntime::FeatureCollectionTable* m_dTable = nullptr;
        Esri::ArcGISRuntime::FeatureCollectionTable* m_uTable = nullptr;

        Esri::ArcGISRuntime::FeatureCollection* m_dCollection = nullptr;
        Esri::ArcGISRuntime::FeatureCollection* m_uCollection = nullptr;

        Esri::ArcGISRuntime::FeatureCollectionLayer* m_dLayer = nullptr;
        Esri::ArcGISRuntime::FeatureCollectionLayer* m_uLayer = nullptr;

        Esri::ArcGISRuntime::DictionaryRenderer* m_dRend = nullptr;
        Esri::ArcGISRuntime::UniqueValueRenderer* m_uRend = nullptr;

        QList<Esri::ArcGISRuntime::Feature*> m_dFeatures;
        QList<Esri::ArcGISRuntime::Feature*> m_uFeatures;


};

#endif // DISPLAYMILITARYSYMBOLS_H
