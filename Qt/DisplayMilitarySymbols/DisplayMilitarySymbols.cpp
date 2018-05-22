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

#include "DisplayMilitarySymbols.h"

using namespace std;
using namespace Esri::ArcGISRuntime;

DisplayMilitarySymbols::DisplayMilitarySymbols(QQuickItem* parent /* = nullptr */):
    QQuickItem(parent) {
}

DisplayMilitarySymbols::~DisplayMilitarySymbols() {
}

void DisplayMilitarySymbols::componentComplete() {
    QQuickItem::componentComplete();

    // find QML MapView component
    m_mapView = findChild<MapQuickView*>("mapView");

    // Create a map using the navigationVector Basemap
    m_map = new Map(Basemap::navigationVector(this), this);

    // Set map to map view
    m_mapView->setMap(m_map);

    // Symbols
    SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(Qt::black), 1, this);
    SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor(Qt::blue), 12, this);
    sms->setOutline(sls);

    // Create the Dictionary and Unique Value Renderer
    const QString stylePath = QDir::currentPath() + QStringLiteral("/styles/mil2525c_b2.stylx");
    const QString styleDict = QDir::currentPath() + QStringLiteral("/styles/master.sym");

    DictionarySymbolStyle* style = new DictionarySymbolStyle(QString("mil2525c_b2"), stylePath, this);
    //QMap<QString, QString> config;
    //config["legacy_standard"] = "mil2525bc2";
    //style->setConfigurationProperties(config);

    m_dRend = new DictionaryRenderer(style, this);
    m_uRend = new UniqueValueRenderer(this);
    m_uRend->setFieldNames(QStringList() << FieldName);
    m_uRend->setDefaultSymbol(sms);

    // Create the fields for the Feature Collection table
    QList<Field> fields;
    fields.push_back(Field::createText(FieldName, FieldName, 15));

    // Create the Feature Collection Tables
    m_dTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference(4326), this);
    m_uTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference(4326), this);

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

    connect(style, &DictionarySymbolStyle::doneLoading, this, [this](Error loadError) {
           if (!loadError.isEmpty())
               return;

           // Get the sic codes
           /*QStringList codes;
           codes << "IFAPSCC--------" << "IFAPSCO--------" << "IFAPSCP--------" << "IFAPSCS--------" << "IFAPSRAI-------" << "IFAPSRAS-------" << "IFAPSRC--------" << "IFAPSRD--------" << "IFAPSRE--------" << "IFAPSRF--------" << "IFAPSRI--------";
           codes << "IFAPSRMA-------" << "IFAPSRMD-------" << "IFAPSRMF-------" << "IFAPSRMG-------" << "IFAPSRMT-------" << "IFAPSRTA-------" << "IFAPSRTI-------" << "IFAPSRTT-------" << "IFAPSRU--------" << "IFGPSCC--------" << "IFGPSCO--------";
           codes << "IFGPSCP--------" << "IFGPSCS--------" << "IFGPSCT--------" << "IFGPSRAA-------" << "IFGPSRAT-------" << "IFGPSRB--------" << "IFGPSRCA-------" << "IFGPSRCS-------" << "IFGPSRD--------" << "IFGPSRE--------" << "IFGPSRF--------";
           codes << "IFGPSRH--------" << "IFGPSRI--------" << "IFGPSRMA-------" << "IFGPSRMF-------" << "IFGPSRMG-------" << "IFGPSRMM-------" << "IFGPSRMT-------" << "IFGPSRS--------" << "IFGPSRTA-------" << "IFGPSRTI-------" << "IFGPSRTT-------";
           //codes << "IFGPSRU--------" << "IFPPSCD--------" << "IFPPSRD--------" << "IFPPSRE--------" << "IFPPSRI--------" << "IFPPSRM--------" << "IFPPSRS--------" << "IFPPSRT--------" << "IFPPSRU--------" << "IFSPSCC--------" << "IFSPSCO--------";
           //codes << "IFSPSCP--------" << "IFSPSCS--------" << "IFSPSRAA-------" << "IFSPSRAT-------" << "IFSPSRCA-------" << "IFSPSRCI-------" << "IFSPSRD--------" << "IFSPSRE--------" << "IFSPSRF--------" << "IFSPSRH--------" << "IFSPSRI--------";
           //codes << "IFSPSRMA-------" << "IFSPSRMF-------" << "IFSPSRMG-------" << "IFSPSRMM-------" << "IFSPSRMT-------" << "IFSPSRS--------" << "IFSPSRTA-------" << "IFSPSRTI-------" << "IFSPSRTT-------" << "IFSPSRU--------" << "IFUPSCO--------";
           //codes << "IFUPSCP--------" << "IFUPSCS--------" << "IFUPSRD--------" << "IFUPSRE--------" << "IFUPSRM--------" << "IFUPSRS--------" << "IFUPSRT--------" << "IFUPSRU--------" << "IHAPSCC--------" << "IHAPSCO--------" << "IHAPSCP--------";
           //codes << "IHAPSCS--------" << "IHAPSRAI-------" << "IHAPSRAS-------" << "IHAPSRC--------" << "IHAPSRD--------" << "IHAPSRE--------" << "IHAPSRF--------" << "IHAPSRI--------" << "IHAPSRMA-------" << "IHAPSRMD-------" << "IHAPSRMF-------";
           //codes << "IHAPSRMG-------" << "IHAPSRMT-------" << "IHAPSRTA-------" << "IHAPSRTI-------" << "IHAPSRTT-------" << "IHAPSRU--------" << "IHGPSCC--------" << "IHGPSCO--------" << "IHGPSCP--------" << "IHGPSCS--------" << "IHGPSCT--------";
           //codes << "IHGPSRAA-------" << "IHGPSRAT-------" << "IHGPSRB--------" << "IHGPSRCA-------" << "IHGPSRCS-------" << "IHGPSRD--------" << "IHGPSRE--------" << "IHGPSRF--------" << "IHGPSRH--------" << "IHGPSRI--------" << "IHGPSRMA-------";
           */
           QStringList codes = GenerateSymbolCodes();

           // Get the AOI
           Point pnt = GeometryEngine::project(Point(-117.1825, 34.0556, SpatialReference(4326)),SpatialReference(3857));
           Envelope aoi = GeometryEngine::project(GeometryEngine::buffer(pnt, Meters).extent(), SpatialReference(4326));

           m_mapView->setViewpointGeometry(aoi);


           // Get the grid size
           int width = ceil(sqrt(codes.length()));
           double spacing = (aoi.xMax() - aoi.xMin()) / width;

           // Create features within the grid
           int count = 0;
           for (double x = aoi.xMin(); x <= aoi.xMax(); x += spacing) {
               if (count >= codes.length())
                   break;

               for (double y = aoi.yMax(); y >= aoi.yMin(); y -= spacing) {
                   if (count >= codes.length())
                       break;

                   Feature* dFeature = m_dTable->createFeature(this);
                   Feature* uFeature = m_uTable->createFeature(this);

                   Point dPoint = Point(x, y, SpatialReference(4326));
                   Point uPoint = Point(x + (spacing * 0.5), y, SpatialReference(4326));

                   dFeature->setGeometry(dPoint);
                   uFeature->setGeometry(uPoint);

                   dFeature->attributes()->replaceAttribute(FieldName, codes[count]);
                   uFeature->attributes()->replaceAttribute(FieldName, codes[count]);

                   m_dFeatures.push_back(dFeature);
                   m_uFeatures.push_back(uFeature);

                   // Add the FEature to the table
                   m_dTable->addFeature(dFeature);
                   m_uTable->addFeature(uFeature);

                   QString json = m_dRend->symbol(dFeature)->toJson();
                   //qDebug() << json;

                   //MultilayerPointSymbol* symbol = (MultilayerPointSymbol*) MultilayerPointSymbol::fromJson(json);
                   MultilayerPointSymbol* symbol = (MultilayerPointSymbol*) m_dRend->symbol(dFeature);
                   symbol->setSize(symbol->size() * 2);

                   UniqueValue* uval = new UniqueValue(codes[count], codes[count], QVariantList() << codes[count], symbol, this);
                   m_uRend->uniqueValues()->append(uval);

                   count++;

                   qDebug() << "Processed " << count << " of " << codes.length();
               }
           }
        });


        style->load();
}

QStringList DisplayMilitarySymbols::GenerateSymbolCodes(int count, int skip) {
    if (count > 0)
        count++;

    QStringList codes;
    codes << "IFAPSCC--------" << "IFAPSCO--------" << "IFAPSCP--------" << "IFAPSCS--------" << "IFAPSRAI-------" << "IFAPSRAS-------" << "IFAPSRC--------" << "IFAPSRD--------" << "IFAPSRE--------" << "IFAPSRF--------" << "IFAPSRI--------";
    codes << "IFAPSRMA-------" << "IFAPSRMD-------" << "IFAPSRMF-------" << "IFAPSRMG-------" << "IFAPSRMT-------" << "IFAPSRTA-------" << "IFAPSRTI-------" << "IFAPSRTT-------" << "IFAPSRU--------" << "IFGPSCC--------" << "IFGPSCO--------";
    /*
    codes << "IFGPSCP--------" << "IFGPSCS--------" << "IFGPSCT--------" << "IFGPSRAA-------" << "IFGPSRAT-------" << "IFGPSRB--------" << "IFGPSRCA-------" << "IFGPSRCS-------" << "IFGPSRD--------" << "IFGPSRE--------" << "IFGPSRF--------";
    codes << "IFGPSRH--------" << "IFGPSRI--------" << "IFGPSRMA-------" << "IFGPSRMF-------" << "IFGPSRMG-------" << "IFGPSRMM-------" << "IFGPSRMT-------" << "IFGPSRS--------" << "IFGPSRTA-------" << "IFGPSRTI-------" << "IFGPSRTT-------";
    codes << "IFGPSRU--------" << "IFPPSCD--------" << "IFPPSRD--------" << "IFPPSRE--------" << "IFPPSRI--------" << "IFPPSRM--------" << "IFPPSRS--------" << "IFPPSRT--------" << "IFPPSRU--------" << "IFSPSCC--------" << "IFSPSCO--------";
    codes << "IFSPSCP--------" << "IFSPSCS--------" << "IFSPSRAA-------" << "IFSPSRAT-------" << "IFSPSRCA-------" << "IFSPSRCI-------" << "IFSPSRD--------" << "IFSPSRE--------" << "IFSPSRF--------" << "IFSPSRH--------" << "IFSPSRI--------";
    codes << "IFSPSRMA-------" << "IFSPSRMF-------" << "IFSPSRMG-------" << "IFSPSRMM-------" << "IFSPSRMT-------" << "IFSPSRS--------" << "IFSPSRTA-------" << "IFSPSRTI-------" << "IFSPSRTT-------" << "IFSPSRU--------" << "IFUPSCO--------";
    codes << "IFUPSCP--------" << "IFUPSCS--------" << "IFUPSRD--------" << "IFUPSRE--------" << "IFUPSRM--------" << "IFUPSRS--------" << "IFUPSRT--------" << "IFUPSRU--------" << "IHAPSCC--------" << "IHAPSCO--------" << "IHAPSCP--------";
    codes << "IHAPSCS--------" << "IHAPSRAI-------" << "IHAPSRAS-------" << "IHAPSRC--------" << "IHAPSRD--------" << "IHAPSRE--------" << "IHAPSRF--------" << "IHAPSRI--------" << "IHAPSRMA-------" << "IHAPSRMD-------" << "IHAPSRMF-------";
    codes << "IHAPSRMG-------" << "IHAPSRMT-------" << "IHAPSRTA-------" << "IHAPSRTI-------" << "IHAPSRTT-------" << "IHAPSRU--------" << "IHGPSCC--------" << "IHGPSCO--------" << "IHGPSCP--------" << "IHGPSCS--------" << "IHGPSCT--------";
    codes << "IHGPSRAA-------" << "IHGPSRAT-------" << "IHGPSRB--------" << "IHGPSRCA-------" << "IHGPSRCS-------" << "IHGPSRD--------" << "IHGPSRE--------" << "IHGPSRF--------" << "IHGPSRH--------" << "IHGPSRI--------" << "IHGPSRMA-------";
    codes << "IHGPSRMF-------" << "IHGPSRMG-------" << "IHGPSRMM-------" << "IHGPSRMT-------" << "IHGPSRS--------" << "IHGPSRTA-------" << "IHGPSRTI-------" << "IHGPSRTT-------" << "IHGPSRU--------" << "IHPPSCD--------" << "IHPPSRD--------";
    codes << "IHPPSRE--------" << "IHPPSRI--------" << "IHPPSRM--------" << "IHPPSRS--------" << "IHPPSRT--------" << "IHPPSRU--------" << "IHSPSCC--------" << "IHSPSCO--------" << "IHSPSCP--------" << "IHSPSCS--------" << "IHSPSRAA-------";
    codes << "IHSPSRAT-------" << "IHSPSRCA-------" << "IHSPSRCI-------" << "IHSPSRD--------" << "IHSPSRE--------" << "IHSPSRF--------" << "IHSPSRH--------" << "IHSPSRI--------" << "IHSPSRMA-------" << "IHSPSRMF-------" << "IHSPSRMG-------";
    codes << "IHSPSRMM-------" << "IHSPSRMT-------" << "IHSPSRS--------" << "IHSPSRTA-------" << "IHSPSRTI-------" << "IHSPSRTT-------" << "IHSPSRU--------" << "IHUPSCO--------" << "IHUPSCP--------" << "IHUPSCS--------" << "IHUPSRD--------";
    codes << "IHUPSRE--------" << "IHUPSRM--------" << "IHUPSRS--------" << "IHUPSRT--------" << "IHUPSRU--------" << "INAPSCC--------" << "INAPSCO--------" << "INAPSCP--------" << "INAPSCS--------" << "INAPSRAI-------" << "INAPSRAS-------";
    codes << "INAPSRC--------" << "INAPSRD--------" << "INAPSRE--------" << "INAPSRF--------" << "INAPSRI--------" << "INAPSRMA-------" << "INAPSRMD-------" << "INAPSRMF-------" << "INAPSRMG-------" << "INAPSRMT-------" << "INAPSRTA-------";
    codes << "INAPSRTI-------" << "INAPSRTT-------" << "INAPSRU--------" << "INGPSCC--------" << "INGPSCO--------" << "INGPSCP--------" << "INGPSCS--------" << "INGPSCT--------" << "INGPSRAA-------" << "INGPSRAT-------" << "INGPSRB--------";
    codes << "INGPSRCA-------" << "INGPSRCS-------" << "INGPSRD--------" << "INGPSRE--------" << "INGPSRF--------" << "INGPSRH--------" << "INGPSRI--------" << "INGPSRMA-------" << "INGPSRMF-------" << "INGPSRMG-------" << "INGPSRMM-------";
    codes << "INGPSRMT-------" << "INGPSRS--------" << "INGPSRTA-------" << "INGPSRTI-------" << "INGPSRTT-------" << "INGPSRU--------" << "INPPSCD--------" << "INPPSRD--------" << "INPPSRE--------" << "INPPSRI--------" << "INPPSRM--------";
    codes << "INPPSRS--------" << "INPPSRT--------" << "INPPSRU--------" << "INSPSCC--------" << "INSPSCO--------" << "INSPSCP--------" << "INSPSCS--------" << "INSPSRAA-------" << "INSPSRAT-------" << "INSPSRCA-------" << "INSPSRCI-------";
    codes << "INSPSRD--------" << "INSPSRE--------" << "INSPSRF--------" << "INSPSRH--------" << "INSPSRI--------" << "INSPSRMA-------" << "INSPSRMF-------" << "INSPSRMG-------" << "INSPSRMM-------" << "INSPSRMT-------" << "INSPSRS--------";
    codes << "INSPSRTA-------" << "INSPSRTI-------" << "INSPSRTT-------" << "INSPSRU--------" << "INUPSCO--------" << "INUPSCP--------" << "INUPSCS--------" << "INUPSRD--------" << "INUPSRE--------" << "INUPSRM--------" << "INUPSRS--------";
    codes << "INUPSRT--------" << "INUPSRU--------" << "IUAPSCC--------" << "IUAPSCO--------" << "IUAPSCP--------" << "IUAPSCS--------" << "IUAPSRAI-------" << "IUAPSRAS-------" << "IUAPSRC--------" << "IUAPSRD--------" << "IUAPSRE--------";
    codes << "IUAPSRF--------" << "IUAPSRI--------" << "IUAPSRMA-------" << "IUAPSRMD-------" << "IUAPSRMF-------" << "IUAPSRMG-------" << "IUAPSRMT-------" << "IUAPSRTA-------" << "IUAPSRTI-------" << "IUAPSRTT-------" << "IUAPSRU--------";
    codes << "IUGPSCC--------" << "IUGPSCO--------" << "IUGPSCP--------" << "IUGPSCS--------" << "IUGPSCT--------" << "IUGPSRAA-------" << "IUGPSRAT-------" << "IUGPSRB--------" << "IUGPSRCA-------" << "IUGPSRCS-------" << "IUGPSRD--------";
    codes << "IUGPSRE--------" << "IUGPSRF--------" << "IUGPSRH--------" << "IUGPSRI--------" << "IUGPSRMA-------" << "IUGPSRMF-------" << "IUGPSRMG-------" << "IUGPSRMM-------" << "IUGPSRMT-------" << "IUGPSRS--------" << "IUGPSRTA-------";
    codes << "IUGPSRTI-------" << "IUGPSRTT-------" << "IUGPSRU--------" << "IUPPSCD--------" << "IUPPSRD--------" << "IUPPSRE--------" << "IUPPSRI--------" << "IUPPSRM--------" << "IUPPSRS--------" << "IUPPSRT--------" << "IUPPSRU--------";
    codes << "IUSPSCC--------" << "IUSPSCO--------" << "IUSPSCP--------" << "IUSPSCS--------" << "IUSPSRAA-------" << "IUSPSRAT-------" << "IUSPSRCA-------" << "IUSPSRCI-------" << "IUSPSRD--------" << "IUSPSRE--------" << "IUSPSRF--------";
    codes << "IUSPSRH--------" << "IUSPSRI--------" << "IUSPSRMA-------" << "IUSPSRMF-------" << "IUSPSRMG-------" << "IUSPSRMM-------" << "IUSPSRMT-------" << "IUSPSRS--------" << "IUSPSRTA-------" << "IUSPSRTI-------" << "IUSPSRTT-------";
    codes << "IUSPSRU--------" << "IUUPSCO--------" << "IUUPSCP--------" << "IUUPSCS--------" << "IUUPSRD--------" << "IUUPSRE--------" << "IUUPSRM--------" << "IUUPSRS--------" << "IUUPSRT--------" << "IUUPSRU--------" << "SFAP-----------";
    codes << "SFAPC----------" << "SFAPCF---------" << "SFAPCH---------" << "SFAPCL---------" << "SFAPM----------" << "SFAPME---------" << "SFAPMF---------" << "SFAPMFA--------" << "SFAPMFB--------" << "SFAPMFC--------" << "SFAPMFCH-------";
    codes << "SFAPMFCL-------" << "SFAPMFCM-------" << "SFAPMFD--------" << "SFAPMFF--------" << "SFAPMFFI-------" << "SFAPMFH--------" << "SFAPMFJ--------" << "SFAPMFK--------" << "SFAPMFKB-------" << "SFAPMFKD-------" << "SFAPMFL--------";
    codes << "SFAPMFM--------" << "SFAPMFO--------" << "SFAPMFP--------" << "SFAPMFPM-------" << "SFAPMFPN-------" << "SFAPMFQ--------" << "SFAPMFQA-------" << "SFAPMFQB-------" << "SFAPMFQC-------" << "SFAPMFQD-------" << "SFAPMFQF-------";
    codes << "SFAPMFQH-------" << "SFAPMFQI-------" << "SFAPMFQJ-------" << "SFAPMFQK-------" << "SFAPMFQL-------" << "SFAPMFQM-------" << "SFAPMFQN-------" << "SFAPMFQO-------" << "SFAPMFQP-------" << "SFAPMFQR-------" << "SFAPMFQRW------";
    codes << "SFAPMFQRX------" << "SFAPMFQRZ------" << "SFAPMFQS-------" << "SFAPMFQT-------" << "SFAPMFQU-------" << "SFAPMFQY-------" << "SFAPMFR--------" << "SFAPMFRW-------" << "SFAPMFRX-------" << "SFAPMFRZ-------" << "SFAPMFS--------";
    codes << "SFAPMFT--------" << "SFAPMFU--------" << "SFAPMFUH-------" << "SFAPMFUL-------" << "SFAPMFUM-------" << "SFAPMFY--------" << "SFAPMH---------" << "SFAPMHA--------" << "SFAPMHC--------" << "SFAPMHCH-------" << "SFAPMHCL-------";
    codes << "SFAPMHCM-------" << "SFAPMHD--------" << "SFAPMHH--------" << "SFAPMHI--------" << "SFAPMHJ--------" << "SFAPMHK--------" << "SFAPMHM--------" << "SFAPMHO--------" << "SFAPMHQ--------" << "SFAPMHR--------" << "SFAPMHS--------";
    codes << "SFAPMHT--------" << "SFAPMHU--------" << "SFAPMHUH-------" << "SFAPMHUL-------" << "SFAPMHUM-------" << "SFAPML---------" << "SFAPMV---------" << "SFAPW----------" << "SFAPWB---------" << "SFAPWD---------" << "SFAPWM---------";
    codes << "SFAPWMA--------" << "SFAPWMAA-------" << "SFAPWMAP-------" << "SFAPWMAS-------" << "SFAPWMB--------" << "SFAPWMCM-------" << "SFAPWMS--------" << "SFAPWMSA-------" << "SFAPWMSB-------" << "SFAPWMSS-------" << "SFAPWMSU-------";
    codes << "SFAPWMU--------" << "SFFP-----------" << "SFFPA----------" << "SFFPAF---------" << "SFFPAFA--------" << "SFFPAFK--------" << "SFFPAFU--------" << "SFFPAFUH-------" << "SFFPAFUL-------" << "SFFPAFUM-------" << "SFFPAH---------";
    codes << "SFFPAHA--------" << "SFFPAHH--------" << "SFFPAHU--------" << "SFFPAHUH-------" << "SFFPAHUL-------" << "SFFPAHUM-------" << "SFFPAV---------" << "SFFPB----------" << "SFFPG----------" << "SFFPGC---------" << "SFFPGP---------";
    codes << "SFFPGPA--------" << "SFFPGR---------" << "SFFPGS---------" << "SFFPN----------" << "SFFPNB---------" << "SFFPNN---------" << "SFFPNS---------" << "SFFPNU---------" << "SFGP-----------" << "SFGPE----------" << "SFGPES---------";
    codes << "SFGPESE--------" << "SFGPESR--------" << "SFGPEV---------" << "SFGPEVA--------" << "SFGPEVAA-------" << "SFGPEVAAR------" << "SFGPEVAC-------" << "SFGPEVAI-------" << "SFGPEVAL-------" << "SFGPEVAS-------" << "SFGPEVAT-------";
    codes << "SFGPEVATH------" << "SFGPEVATHR-----" << "SFGPEVATL------" << "SFGPEVATLR-----" << "SFGPEVATM------" << "SFGPEVATMR-----" << "SFGPEVC--------" << "SFGPEVCA-------" << "SFGPEVCAH------" << "SFGPEVCAL------" << "SFGPEVCAM------";
    codes << "SFGPEVCF-------" << "SFGPEVCFH------" << "SFGPEVCFL------" << "SFGPEVCFM------" << "SFGPEVCJ-------" << "SFGPEVCJH------" << "SFGPEVCJL------" << "SFGPEVCJM------" << "SFGPEVCM-------" << "SFGPEVCMH------" << "SFGPEVCML------";
    codes << "SFGPEVCMM------" << "SFGPEVCO-------" << "SFGPEVCOH------" << "SFGPEVCOL------" << "SFGPEVCOM------" << "SFGPEVCT-------" << "SFGPEVCTH------" << "SFGPEVCTL------" << "SFGPEVCTM------" << "SFGPEVCU-------" << "SFGPEVCUH------";
    codes << "SFGPEVCUL------" << "SFGPEVCUM------" << "SFGPEVE--------" << "SFGPEVEA-------" << "SFGPEVEAA------" << "SFGPEVEAT------" << "SFGPEVEB-------" << "SFGPEVEC-------" << "SFGPEVED-------" << "SFGPEVEDA------" << "SFGPEVEE-------";
    codes << "SFGPEVEF-------" << "SFGPEVEH-------" << "SFGPEVEM-------" << "SFGPEVEML------" << "SFGPEVEMV------" << "SFGPEVER-------" << "SFGPEVES-------" << "SFGPEVM--------" << "SFGPEVS--------" << "SFGPEVSC-------" << "SFGPEVSP-------";
    codes << "SFGPEVSR-------" << "SFGPEVST-------" << "SFGPEVSW-------" << "SFGPEVT--------" << "SFGPEVU--------" << "SFGPEVUA-------" << "SFGPEVUAA------" << "SFGPEVUB-------" << "SFGPEVUL-------" << "SFGPEVUR-------" << "SFGPEVUS-------";
    codes << "SFGPEVUSH------" << "SFGPEVUSL------" << "SFGPEVUSM------" << "SFGPEVUT-------" << "SFGPEVUTH------" << "SFGPEVUTL------" << "SFGPEVUX-------" << "SFGPEWA--------" << "SFGPEWAH-------" << "SFGPEWAL-------" << "SFGPEWAM-------";
    codes << "SFGPEWD--------" << "SFGPEWDH-------" << "SFGPEWDHS------" << "SFGPEWDL-------" << "SFGPEWDLS------" << "SFGPEWDM-------" << "SFGPEWDMS------" << "SFGPEWG--------" << "SFGPEWGH-------" << "SFGPEWGL-------" << "SFGPEWGM-------";
    codes << "SFGPEWGR-------" << "SFGPEWH--------" << "SFGPEWHH-------" << "SFGPEWHHS------" << "SFGPEWHL-------" << "SFGPEWHLS------" << "SFGPEWHM-------" << "SFGPEWHMS------" << "SFGPEWM--------" << "SFGPEWMA-------" << "SFGPEWMAI------";
    codes << "SFGPEWMAIE-----" << "SFGPEWMAIR-----" << "SFGPEWMAL------" << "SFGPEWMALE-----" << "SFGPEWMALR-----" << "SFGPEWMAS------" << "SFGPEWMASE-----" << "SFGPEWMASR-----" << "SFGPEWMAT------" << "SFGPEWMATE-----" << "SFGPEWMATR-----";
    codes << "SFGPEWMS-------" << "SFGPEWMSI------" << "SFGPEWMSL------" << "SFGPEWMSS------" << "SFGPEWMT-------" << "SFGPEWMTH------" << "SFGPEWMTL------" << "SFGPEWMTM------" << "SFGPEWO--------" << "SFGPEWOH-------" << "SFGPEWOL-------";
    codes << "SFGPEWOM-------" << "SFGPEWR--------" << "SFGPEWRH-------" << "SFGPEWRL-------" << "SFGPEWRR-------" << "SFGPEWS--------" << "SFGPEWSH-------" << "SFGPEWSL-------" << "SFGPEWSM-------" << "SFGPEWT--------" << "SFGPEWTH-------";
    codes << "SFGPEWTL-------" << "SFGPEWTM-------" << "SFGPEWX--------" << "SFGPEWXH-------" << "SFGPEWXL-------" << "SFGPEWXM-------" << "SFGPEWZ--------" << "SFGPEWZH-------" << "SFGPEWZL-------" << "SFGPEWZM-------" << "SFGPEXF--------";
    codes << "SFGPEXI--------" << "SFGPEXL--------" << "SFGPEXM--------" << "SFGPEXMC-------" << "SFGPEXML-------" << "SFGPEXN--------" << "SFGPI----------" << "SFGPIB---------" << "SFGPIBA--------" << "SFGPIBN--------" << "SFGPIE---------";
    codes << "SFGPIG---------" << "SFGPIMA--------" << "SFGPIMC--------" << "SFGPIME--------" << "SFGPIMF--------" << "SFGPIMFA-------" << "SFGPIMFP-------" << "SFGPIMFPW------" << "SFGPIMFS-------" << "SFGPIMG--------" << "SFGPIMM--------";
    codes << "SFGPIMN--------" << "SFGPIMNB-------" << "SFGPIMS--------" << "SFGPIMV--------" << "SFGPIP---------" << "SFGPIPD--------" << "SFGPIR---------" << "SFGPIRM--------" << "SFGPIRN--------" << "SFGPIRNB-------" << "SFGPIRNC-------";
    codes << "SFGPIRNN-------" << "SFGPIRP--------" << "SFGPIT---------" << "SFGPIU---------" << "SFGPIUE--------" << "SFGPIUED-------" << "SFGPIUEF-------" << "SFGPIUEN-------" << "SFGPIUP--------" << "SFGPIUR--------" << "SFGPIUT--------";
    codes << "SFGPIX---------" << "SFGPIXH--------" << "SFGPU----------" << "SFGPUC---------" << "SFGPUCA--------" << "SFGPUCAA-------" << "SFGPUCAAA------" << "SFGPUCAAAS-----" << "SFGPUCAAAT-----" << "SFGPUCAAAW-----" << "SFGPUCAAC------";
    codes << "SFGPUCAAD------" << "SFGPUCAAL------" << "SFGPUCAAM------" << "SFGPUCAAO------" << "SFGPUCAAOS-----" << "SFGPUCAAS------" << "SFGPUCAAU------" << "SFGPUCAT-------" << "SFGPUCATA------" << "SFGPUCATH------" << "SFGPUCATL------";
    codes << "SFGPUCATM------" << "SFGPUCATR------" << "SFGPUCATW------" << "SFGPUCATWR-----" << "SFGPUCAW-------" << "SFGPUCAWA------" << "SFGPUCAWH------" << "SFGPUCAWL------" << "SFGPUCAWM------" << "SFGPUCAWR------" << "SFGPUCAWS------";
    codes << "SFGPUCAWW------" << "SFGPUCAWWR-----" << "SFGPUCD--------" << "SFGPUCDC-------" << "SFGPUCDG-------" << "SFGPUCDH-------" << "SFGPUCDHH------" << "SFGPUCDHP------" << "SFGPUCDM-------" << "SFGPUCDMH------" << "SFGPUCDML------";
    codes << "SFGPUCDMLA-----" << "SFGPUCDMM------" << "SFGPUCDO-------" << "SFGPUCDS-------" << "SFGPUCDSC------" << "SFGPUCDSS------" << "SFGPUCDSV------" << "SFGPUCDT-------" << "SFGPUCE--------" << "SFGPUCEC-------" << "SFGPUCECA------";
    codes << "SFGPUCECC------" << "SFGPUCECH------" << "SFGPUCECL------" << "SFGPUCECM------" << "SFGPUCECO------" << "SFGPUCECR------" << "SFGPUCECS------" << "SFGPUCECT------" << "SFGPUCECW------" << "SFGPUCEN-------" << "SFGPUCENN------";
    codes << "SFGPUCF--------" << "SFGPUCFH-------" << "SFGPUCFHA------" << "SFGPUCFHC------" << "SFGPUCFHE------" << "SFGPUCFHH------" << "SFGPUCFHL------" << "SFGPUCFHM------" << "SFGPUCFHO------" << "SFGPUCFHS------" << "SFGPUCFHX------";
    codes << "SFGPUCFM-------" << "SFGPUCFML------" << "SFGPUCFMS------" << "SFGPUCFMT------" << "SFGPUCFMTA-----" << "SFGPUCFMTC-----" << "SFGPUCFMTO-----" << "SFGPUCFMTS-----" << "SFGPUCFMW------" << "SFGPUCFO-------" << "SFGPUCFOA------";
    codes << "SFGPUCFOL------" << "SFGPUCFOO------" << "SFGPUCFOS------" << "SFGPUCFR-------" << "SFGPUCFRM------" << "SFGPUCFRMR-----" << "SFGPUCFRMS-----" << "SFGPUCFRMT-----" << "SFGPUCFRS------" << "SFGPUCFRSR-----" << "SFGPUCFRSS-----";
    codes << "SFGPUCFRST-----" << "SFGPUCFS-------" << "SFGPUCFSA------" << "SFGPUCFSL------" << "SFGPUCFSO------" << "SFGPUCFSS------" << "SFGPUCFT-------" << "SFGPUCFTA------" << "SFGPUCFTC------" << "SFGPUCFTCD-----" << "SFGPUCFTCM-----";
    codes << "SFGPUCFTF------" << "SFGPUCFTR------" << "SFGPUCFTS------" << "SFGPUCI--------" << "SFGPUCIA-------" << "SFGPUCIC-------" << "SFGPUCII-------" << "SFGPUCIL-------" << "SFGPUCIM-------" << "SFGPUCIN-------" << "SFGPUCIO-------";
    codes << "SFGPUCIS-------" << "SFGPUCIZ-------" << "SFGPUCM--------" << "SFGPUCMS-------" << "SFGPUCMT-------" << "SFGPUCR--------" << "SFGPUCRA-------" << "SFGPUCRC-------" << "SFGPUCRH-------" << "SFGPUCRL-------" << "SFGPUCRO-------";
    codes << "SFGPUCRR-------" << "SFGPUCRRD------" << "SFGPUCRRF------" << "SFGPUCRRL------" << "SFGPUCRS-------" << "SFGPUCRV-------" << "SFGPUCRVA------" << "SFGPUCRVG------" << "SFGPUCRVM------" << "SFGPUCRVO------" << "SFGPUCRX-------";
    codes << "SFGPUCS--------" << "SFGPUCSA-------" << "SFGPUCSG-------" << "SFGPUCSGA------" << "SFGPUCSGD------" << "SFGPUCSGM------" << "SFGPUCSM-------" << "SFGPUCSR-------" << "SFGPUCSW-------" << "SFGPUCV--------" << "SFGPUCVC-------";
    codes << "SFGPUCVF-------" << "SFGPUCVFA------" << "SFGPUCVFR------" << "SFGPUCVFU------" << "SFGPUCVR-------" << "SFGPUCVRA------" << "SFGPUCVRM------" << "SFGPUCVRS------" << "SFGPUCVRU------" << "SFGPUCVRUC-----" << "SFGPUCVRUE-----";
    codes << "SFGPUCVRUH-----" << "SFGPUCVRUL-----" << "SFGPUCVRUM-----" << "SFGPUCVRW------" << "SFGPUCVS-------" << "SFGPUCVU-------" << "SFGPUCVUF------" << "SFGPUCVUR------" << "SFGPUCVV-------" << "SFGPUH---------" << "SFGPUS---------";
    codes << "SFGPUSA--------" << "SFGPUSAC-------" << "SFGPUSAF-------" << "SFGPUSAFC------" << "SFGPUSAFT------" << "SFGPUSAJ-------" << "SFGPUSAJC------" << "SFGPUSAJT------" << "SFGPUSAL-------" << "SFGPUSALC------" << "SFGPUSALT------";
    codes << "SFGPUSAM-------" << "SFGPUSAMC------" << "SFGPUSAMT------" << "SFGPUSAO-------" << "SFGPUSAOC------" << "SFGPUSAOT------" << "SFGPUSAP-------" << "SFGPUSAPB------" << "SFGPUSAPBC-----" << "SFGPUSAPBT-----" << "SFGPUSAPC------";
    codes << "SFGPUSAPM------" << "SFGPUSAPMC-----" << "SFGPUSAPMT-----" << "SFGPUSAPT------" << "SFGPUSAQ-------" << "SFGPUSAQC------" << "SFGPUSAQT------" << "SFGPUSAR-------" << "SFGPUSARC------" << "SFGPUSART------" << "SFGPUSAS-------";
    codes << "SFGPUSASC------" << "SFGPUSAST------" << "SFGPUSAT-------" << "SFGPUSAW-------" << "SFGPUSAWC------" << "SFGPUSAWT------" << "SFGPUSAX-------" << "SFGPUSAXC------" << "SFGPUSAXT------" << "SFGPUSM--------" << "SFGPUSMC-------";
    codes << "SFGPUSMD-------" << "SFGPUSMDC------" << "SFGPUSMDT------" << "SFGPUSMM-------" << "SFGPUSMMC------" << "SFGPUSMMT------" << "SFGPUSMP-------" << "SFGPUSMPC------" << "SFGPUSMPT------" << "SFGPUSMT-------" << "SFGPUSMV-------";
    codes << "SFGPUSMVC------" << "SFGPUSMVT------" << "SFGPUSS--------" << "SFGPUSS1-------" << "SFGPUSS1C------" << "SFGPUSS1T------" << "SFGPUSS2-------" << "SFGPUSS2C------" << "SFGPUSS2T------" << "SFGPUSS3-------" << "SFGPUSS3A------";
    codes << "SFGPUSS3AC-----" << "SFGPUSS3AT-----" << "SFGPUSS3C------" << "SFGPUSS3T------" << "SFGPUSS4-------" << "SFGPUSS4C------" << "SFGPUSS4T------" << "SFGPUSS5-------" << "SFGPUSS5C------" << "SFGPUSS5T------" << "SFGPUSS6-------";
    codes << "SFGPUSS6C------" << "SFGPUSS6T------" << "SFGPUSS7-------" << "SFGPUSS7C------" << "SFGPUSS7T------" << "SFGPUSS8-------" << "SFGPUSS8C------" << "SFGPUSS8T------" << "SFGPUSS9-------" << "SFGPUSS9C------" << "SFGPUSS9T------";
    codes << "SFGPUSSC-------" << "SFGPUSSL-------" << "SFGPUSSLC------" << "SFGPUSSLT------" << "SFGPUSST-------" << "SFGPUSSW-------" << "SFGPUSSWC------" << "SFGPUSSWP------" << "SFGPUSSWPC-----" << "SFGPUSSWPT-----" << "SFGPUSSWT------";
    codes << "SFGPUSSX-------" << "SFGPUSSXC------" << "SFGPUSSXT------" << "SFGPUST--------" << "SFGPUSTA-------" << "SFGPUSTAC------" << "SFGPUSTAT------" << "SFGPUSTC-------" << "SFGPUSTI-------" << "SFGPUSTIC------" << "SFGPUSTIT------";
    codes << "SFGPUSTM-------" << "SFGPUSTMC------" << "SFGPUSTMT------" << "SFGPUSTR-------" << "SFGPUSTRC------" << "SFGPUSTRT------" << "SFGPUSTS-------" << "SFGPUSTSC------" << "SFGPUSTST------" << "SFGPUSTT-------" << "SFGPUSX--------";
    codes << "SFGPUSXC-------" << "SFGPUSXE-------" << "SFGPUSXEC------" << "SFGPUSXET------" << "SFGPUSXH-------" << "SFGPUSXHC------" << "SFGPUSXHT------" << "SFGPUSXO-------" << "SFGPUSXOC------" << "SFGPUSXOM------" << "SFGPUSXOMC-----";
    codes << "SFGPUSXOMT-----" << "SFGPUSXOT------" << "SFGPUSXR-------" << "SFGPUSXRC------" << "SFGPUSXRT------" << "SFGPUSXT-------" << "SFGPUU---------" << "SFGPUUA--------" << "SFGPUUAB-------" << "SFGPUUABR------" << "SFGPUUAC-------";
    codes << "SFGPUUACC------" << "SFGPUUACCK-----" << "SFGPUUACCM-----" << "SFGPUUACR------" << "SFGPUUACRS-----" << "SFGPUUACRW-----" << "SFGPUUACS------" << "SFGPUUACSA-----" << "SFGPUUACSM-----" << "SFGPUUAD-------" << "SFGPUUAN-------";
    codes << "SFGPUUE--------" << "SFGPUUI--------" << "SFGPUUL--------" << "SFGPUULC-------" << "SFGPUULD-------" << "SFGPUULF-------" << "SFGPUULM-------" << "SFGPUULS-------" << "SFGPUUM--------" << "SFGPUUMA-------" << "SFGPUUMC-------";
    codes << "SFGPUUMJ-------" << "SFGPUUMMO------" << "SFGPUUMO-------" << "SFGPUUMQ-------" << "SFGPUUMR-------" << "SFGPUUMRG------" << "SFGPUUMRS------" << "SFGPUUMRSS-----" << "SFGPUUMRX------" << "SFGPUUMS-------" << "SFGPUUMSE------";
    codes << "SFGPUUMSEA-----" << "SFGPUUMSEC-----" << "SFGPUUMSED-----" << "SFGPUUMSEI-----" << "SFGPUUMSEJ-----" << "SFGPUUMSET-----" << "SFGPUUMT-------" << "SFGPUUP--------" << "SFGPUUS--------" << "SFGPUUSA-------" << "SFGPUUSC-------";
    codes << "SFGPUUSCL------" << "SFGPUUSF-------" << "SFGPUUSM-------" << "SFGPUUSML------" << "SFGPUUSMN------" << "SFGPUUSMS------" << "SFGPUUSO-------" << "SFGPUUSR-------" << "SFGPUUSRS------" << "SFGPUUSRT------" << "SFGPUUSRW------";
    codes << "SFGPUUSS-------" << "SFGPUUSW-------" << "SFGPUUSX-------" << "SFPP-----------" << "SFPPL----------" << "SFPPS----------" << "SFPPT----------" << "SFPPV----------" << "SFSP-----------" << "SFSPC----------" << "SFSPCA---------";
    codes << "SFSPCALA-------" << "SFSPCALC-------" << "SFSPCALS-------" << "SFSPCALSM------" << "SFSPCALST------" << "SFSPCD---------" << "SFSPCH---------" << "SFSPCL---------" << "SFSPCLBB-------" << "SFSPCLCC-------" << "SFSPCLCV-------";
    codes << "SFSPCLDD-------" << "SFSPCLFF-------" << "SFSPCLLL-------" << "SFSPCLLLAS-----" << "SFSPCLLLMI-----" << "SFSPCLLLSU-----" << "SFSPCM---------" << "SFSPCMMA-------" << "SFSPCMMH-------" << "SFSPCMML-------" << "SFSPCMMS-------";
    codes << "SFSPCP---------" << "SFSPCPSB-------" << "SFSPCPSU-------" << "SFSPCPSUG------" << "SFSPCPSUM------" << "SFSPCPSUT------" << "SFSPCU---------" << "SFSPCUM--------" << "SFSPCUN--------" << "SFSPCUR--------" << "SFSPCUS--------";
    codes << "SFSPG----------" << "SFSPGC---------" << "SFSPGG---------" << "SFSPGT---------" << "SFSPGU---------" << "SFSPN----------" << "SFSPNF---------" << "SFSPNH---------" << "SFSPNI---------" << "SFSPNM---------" << "SFSPNR---------";
    codes << "SFSPNS---------" << "SFSPO----------" << "SFSPXA---------" << "SFSPXAR--------" << "SFSPXAS--------" << "SFSPXF---------" << "SFSPXFDF-------" << "SFSPXFDR-------" << "SFSPXFTR-------" << "SFSPXH---------" << "SFSPXL---------";
    codes << "SFSPXM---------" << "SFSPXMC--------" << "SFSPXMF--------" << "SFSPXMH--------" << "SFSPXMO--------" << "SFSPXMP--------" << "SFSPXMR--------" << "SFSPXMTO-------" << "SFSPXMTU-------" << "SFSPXP---------" << "SFSPXR---------";
    codes << "SFUP-----------" << "SFUPE----------" << "SFUPND---------" << "SFUPS----------" << "SFUPS1---------" << "SFUPS2---------" << "SFUPS3---------" << "SFUPS4---------" << "SFUPSB---------" << "SFUPSC---------" << "SFUPSCA--------";
    codes << "SFUPSCB--------" << "SFUPSCF--------" << "SFUPSCG--------" << "SFUPSCM--------" << "SFUPSF---------" << "SFUPSK---------" << "SFUPSL---------" << "SFUPSN---------" << "SFUPSNA--------" << "SFUPSNB--------" << "SFUPSNF--------";
    codes << "SFUPSNG--------" << "SFUPSNM--------" << "SFUPSO---------" << "SFUPSOF--------" << "SFUPSR---------" << "SFUPSU---------" << "SFUPSUM--------" << "SFUPSUN--------" << "SFUPSUS--------" << "SFUPSX---------" << "SFUPV----------";
    codes << "SFUPW----------" << "SFUPWD---------" << "SFUPWDM--------" << "SFUPWDMG-------" << "SFUPWDMM-------" << "SFUPWM---------" << "SFUPWMA--------" << "SFUPWMB--------" << "SFUPWMBD-------" << "SFUPWMC--------" << "SFUPWMD--------";
    codes << "SFUPWME--------" << "SFUPWMF--------" << "SFUPWMFC-------" << "SFUPWMFD-------" << "SFUPWMFE-------" << "SFUPWMFO-------" << "SFUPWMFR-------" << "SFUPWMFX-------" << "SFUPWMG--------" << "SFUPWMGC-------" << "SFUPWMGD-------";
    codes << "SFUPWMGE-------" << "SFUPWMGO-------" << "SFUPWMGR-------" << "SFUPWMGX-------" << "SFUPWMM--------" << "SFUPWMMC-------" << "SFUPWMMD-------" << "SFUPWMME-------" << "SFUPWMMO-------" << "SFUPWMMR-------" << "SFUPWMMX-------";
    codes << "SFUPWMN--------" << "SFUPWMO--------" << "SFUPWMOD-------" << "SFUPWMR--------" << "SFUPWMS--------" << "SFUPWMSD-------" << "SFUPWMSX-------" << "SFUPWMX--------" << "SFUPWT---------" << "SFUPX----------" << "SHAP-----------";
    codes << "SHAPC----------" << "SHAPCF---------" << "SHAPCH---------" << "SHAPCL---------" << "SHAPM----------" << "SHAPME---------" << "SHAPMF---------" << "SHAPMFA--------" << "SHAPMFB--------" << "SHAPMFC--------" << "SHAPMFCH-------";
    codes << "SHAPMFCL-------" << "SHAPMFCM-------" << "SHAPMFD--------" << "SHAPMFF--------" << "SHAPMFFI-------" << "SHAPMFH--------" << "SHAPMFJ--------" << "SHAPMFK--------" << "SHAPMFKB-------" << "SHAPMFKD-------" << "SHAPMFL--------";
    codes << "SHAPMFM--------" << "SHAPMFO--------" << "SHAPMFP--------" << "SHAPMFPM-------" << "SHAPMFPN-------" << "SHAPMFQ--------" << "SHAPMFQA-------" << "SHAPMFQB-------" << "SHAPMFQC-------" << "SHAPMFQD-------" << "SHAPMFQF-------";
    codes << "SHAPMFQH-------" << "SHAPMFQI-------" << "SHAPMFQJ-------" << "SHAPMFQK-------" << "SHAPMFQL-------" << "SHAPMFQM-------" << "SHAPMFQN-------" << "SHAPMFQO-------" << "SHAPMFQP-------" << "SHAPMFQR-------" << "SHAPMFQRW------";
    codes << "SHAPMFQRX------" << "SHAPMFQRZ------" << "SHAPMFQS-------" << "SHAPMFQT-------" << "SHAPMFQU-------" << "SHAPMFQY-------" << "SHAPMFR--------" << "SHAPMFRW-------" << "SHAPMFRX-------" << "SHAPMFRZ-------" << "SHAPMFS--------";
    codes << "SHAPMFT--------" << "SHAPMFU--------" << "SHAPMFUH-------" << "SHAPMFUL-------" << "SHAPMFUM-------" << "SHAPMFY--------" << "SHAPMH---------" << "SHAPMHA--------" << "SHAPMHC--------" << "SHAPMHCH-------" << "SHAPMHCL-------";
    codes << "SHAPMHCM-------" << "SHAPMHD--------" << "SHAPMHH--------" << "SHAPMHI--------" << "SHAPMHJ--------" << "SHAPMHK--------" << "SHAPMHM--------" << "SHAPMHO--------" << "SHAPMHQ--------" << "SHAPMHR--------" << "SHAPMHS--------";
    codes << "SHAPMHT--------" << "SHAPMHU--------" << "SHAPMHUH-------" << "SHAPMHUL-------" << "SHAPMHUM-------" << "SHAPML---------" << "SHAPMV---------" << "SHAPW----------" << "SHAPWB---------" << "SHAPWD---------" << "SHAPWM---------";
    codes << "SHAPWMA--------" << "SHAPWMAA-------" << "SHAPWMAP-------" << "SHAPWMAS-------" << "SHAPWMB--------" << "SHAPWMCM-------" << "SHAPWMS--------" << "SHAPWMSA-------" << "SHAPWMSB-------" << "SHAPWMSS-------" << "SHAPWMSU-------";
    codes << "SHAPWMU--------" << "SHFP-----------" << "SHFPA----------" << "SHFPAF---------" << "SHFPAFA--------" << "SHFPAFK--------" << "SHFPAFU--------" << "SHFPAFUH-------" << "SHFPAFUL-------" << "SHFPAFUM-------" << "SHFPAH---------";
    codes << "SHFPAHA--------" << "SHFPAHH--------" << "SHFPAHU--------" << "SHFPAHUH-------" << "SHFPAHUL-------" << "SHFPAHUM-------" << "SHFPAV---------" << "SHFPB----------" << "SHFPG----------" << "SHFPGC---------" << "SHFPGP---------";
    codes << "SHFPGPA--------" << "SHFPGR---------" << "SHFPGS---------" << "SHFPN----------" << "SHFPNB---------" << "SHFPNN---------" << "SHFPNS---------" << "SHFPNU---------" << "SHGP-----------" << "SHGPE----------" << "SHGPES---------";
    codes << "SHGPESE--------" << "SHGPESR--------" << "SHGPEV---------" << "SHGPEVA--------" << "SHGPEVAA-------" << "SHGPEVAAR------" << "SHGPEVAC-------" << "SHGPEVAI-------" << "SHGPEVAL-------" << "SHGPEVAS-------" << "SHGPEVAT-------";
    codes << "SHGPEVATH------" << "SHGPEVATHR-----" << "SHGPEVATL------" << "SHGPEVATLR-----" << "SHGPEVATM------" << "SHGPEVATMR-----" << "SHGPEVC--------" << "SHGPEVCA-------" << "SHGPEVCAH------" << "SHGPEVCAL------" << "SHGPEVCAM------";
    codes << "SHGPEVCF-------" << "SHGPEVCFH------" << "SHGPEVCFL------" << "SHGPEVCFM------" << "SHGPEVCJ-------" << "SHGPEVCJH------" << "SHGPEVCJL------" << "SHGPEVCJM------" << "SHGPEVCM-------" << "SHGPEVCMH------" << "SHGPEVCML------";
    codes << "SHGPEVCMM------" << "SHGPEVCO-------" << "SHGPEVCOH------" << "SHGPEVCOL------" << "SHGPEVCOM------" << "SHGPEVCT-------" << "SHGPEVCTH------" << "SHGPEVCTL------" << "SHGPEVCTM------" << "SHGPEVCU-------" << "SHGPEVCUH------";
    codes << "SHGPEVCUL------" << "SHGPEVCUM------" << "SHGPEVE--------" << "SHGPEVEA-------" << "SHGPEVEAA------" << "SHGPEVEAT------" << "SHGPEVEB-------" << "SHGPEVEC-------" << "SHGPEVED-------" << "SHGPEVEDA------" << "SHGPEVEE-------";
    codes << "SHGPEVEF-------" << "SHGPEVEH-------" << "SHGPEVEM-------" << "SHGPEVEML------" << "SHGPEVEMV------" << "SHGPEVER-------" << "SHGPEVES-------" << "SHGPEVM--------" << "SHGPEVS--------" << "SHGPEVSC-------" << "SHGPEVSP-------";
    codes << "SHGPEVSR-------" << "SHGPEVST-------" << "SHGPEVSW-------" << "SHGPEVT--------" << "SHGPEVU--------" << "SHGPEVUA-------" << "SHGPEVUAA------" << "SHGPEVUB-------" << "SHGPEVUL-------" << "SHGPEVUR-------" << "SHGPEVUS-------";
    codes << "SHGPEVUSH------" << "SHGPEVUSL------" << "SHGPEVUSM------" << "SHGPEVUT-------" << "SHGPEVUTH------" << "SHGPEVUTL------" << "SHGPEVUX-------" << "SHGPEWA--------" << "SHGPEWAH-------" << "SHGPEWAL-------" << "SHGPEWAM-------";
    codes << "SHGPEWD--------" << "SHGPEWDH-------" << "SHGPEWDHS------" << "SHGPEWDL-------" << "SHGPEWDLS------" << "SHGPEWDM-------" << "SHGPEWDMS------" << "SHGPEWG--------" << "SHGPEWGH-------" << "SHGPEWGL-------" << "SHGPEWGM-------";
    codes << "SHGPEWGR-------" << "SHGPEWH--------" << "SHGPEWHH-------" << "SHGPEWHHS------" << "SHGPEWHL-------" << "SHGPEWHLS------" << "SHGPEWHM-------" << "SHGPEWHMS------" << "SHGPEWM--------" << "SHGPEWMA-------" << "SHGPEWMAI------";
    codes << "SHGPEWMAIE-----" << "SHGPEWMAIR-----" << "SHGPEWMAL------" << "SHGPEWMALE-----" << "SHGPEWMALR-----" << "SHGPEWMAS------" << "SHGPEWMASE-----" << "SHGPEWMASR-----" << "SHGPEWMAT------" << "SHGPEWMATE-----" << "SHGPEWMATR-----";
    codes << "SHGPEWMS-------" << "SHGPEWMSI------" << "SHGPEWMSL------" << "SHGPEWMSS------" << "SHGPEWMT-------" << "SHGPEWMTH------" << "SHGPEWMTL------" << "SHGPEWMTM------" << "SHGPEWO--------" << "SHGPEWOH-------" << "SHGPEWOL-------";
    codes << "SHGPEWOM-------" << "SHGPEWR--------" << "SHGPEWRH-------" << "SHGPEWRL-------" << "SHGPEWRR-------" << "SHGPEWS--------" << "SHGPEWSH-------" << "SHGPEWSL-------" << "SHGPEWSM-------" << "SHGPEWT--------" << "SHGPEWTH-------";
    codes << "SHGPEWTL-------" << "SHGPEWTM-------" << "SHGPEWX--------" << "SHGPEWXH-------" << "SHGPEWXL-------" << "SHGPEWXM-------" << "SHGPEWZ--------" << "SHGPEWZH-------" << "SHGPEWZL-------" << "SHGPEWZM-------" << "SHGPEXF--------";
    codes << "SHGPEXI--------" << "SHGPEXL--------" << "SHGPEXM--------" << "SHGPEXMC-------" << "SHGPEXML-------" << "SHGPEXN--------" << "SHGPI----------" << "SHGPIB---------" << "SHGPIBA--------" << "SHGPIBN--------" << "SHGPIE---------";
    codes << "SHGPIG---------" << "SHGPIMA--------" << "SHGPIMC--------" << "SHGPIME--------" << "SHGPIMF--------" << "SHGPIMFA-------" << "SHGPIMFP-------" << "SHGPIMFPW------" << "SHGPIMFS-------" << "SHGPIMG--------" << "SHGPIMM--------";
    codes << "SHGPIMN--------" << "SHGPIMNB-------" << "SHGPIMS--------" << "SHGPIMV--------" << "SHGPIP---------" << "SHGPIPD--------" << "SHGPIR---------" << "SHGPIRM--------" << "SHGPIRN--------" << "SHGPIRNB-------" << "SHGPIRNC-------";
    codes << "SHGPIRNN-------" << "SHGPIRP--------" << "SHGPIT---------" << "SHGPIU---------" << "SHGPIUE--------" << "SHGPIUED-------" << "SHGPIUEF-------" << "SHGPIUEN-------" << "SHGPIUP--------" << "SHGPIUR--------" << "SHGPIUT--------";
    codes << "SHGPIX---------" << "SHGPIXH--------" << "SHGPU----------" << "SHGPUC---------" << "SHGPUCA--------" << "SHGPUCAA-------" << "SHGPUCAAA------" << "SHGPUCAAAS-----" << "SHGPUCAAAT-----" << "SHGPUCAAAW-----" << "SHGPUCAAC------";
    codes << "SHGPUCAAD------" << "SHGPUCAAL------" << "SHGPUCAAM------" << "SHGPUCAAO------" << "SHGPUCAAOS-----" << "SHGPUCAAS------" << "SHGPUCAAU------" << "SHGPUCAT-------" << "SHGPUCATA------" << "SHGPUCATH------" << "SHGPUCATL------";
    codes << "SHGPUCATM------" << "SHGPUCATR------" << "SHGPUCATW------" << "SHGPUCATWR-----" << "SHGPUCAW-------" << "SHGPUCAWA------" << "SHGPUCAWH------" << "SHGPUCAWL------" << "SHGPUCAWM------" << "SHGPUCAWR------" << "SHGPUCAWS------";
    codes << "SHGPUCAWW------" << "SHGPUCAWWR-----" << "SHGPUCD--------" << "SHGPUCDC-------" << "SHGPUCDG-------" << "SHGPUCDH-------" << "SHGPUCDHH------" << "SHGPUCDHP------" << "SHGPUCDM-------" << "SHGPUCDMH------" << "SHGPUCDML------";
    codes << "SHGPUCDMLA-----" << "SHGPUCDMM------" << "SHGPUCDO-------" << "SHGPUCDS-------" << "SHGPUCDSC------" << "SHGPUCDSS------" << "SHGPUCDSV------" << "SHGPUCDT-------" << "SHGPUCE--------" << "SHGPUCEC-------" << "SHGPUCECA------";
    codes << "SHGPUCECC------" << "SHGPUCECH------" << "SHGPUCECL------" << "SHGPUCECM------" << "SHGPUCECO------" << "SHGPUCECR------" << "SHGPUCECS------" << "SHGPUCECT------" << "SHGPUCECW------" << "SHGPUCEN-------" << "SHGPUCENN------";
    codes << "SHGPUCF--------" << "SHGPUCFH-------" << "SHGPUCFHA------" << "SHGPUCFHC------" << "SHGPUCFHE------" << "SHGPUCFHH------" << "SHGPUCFHL------" << "SHGPUCFHM------" << "SHGPUCFHO------" << "SHGPUCFHS------" << "SHGPUCFHX------";
    codes << "SHGPUCFM-------" << "SHGPUCFML------" << "SHGPUCFMS------" << "SHGPUCFMT------" << "SHGPUCFMTA-----" << "SHGPUCFMTC-----" << "SHGPUCFMTO-----" << "SHGPUCFMTS-----" << "SHGPUCFMW------" << "SHGPUCFO-------" << "SHGPUCFOA------";
    codes << "SHGPUCFOL------" << "SHGPUCFOO------" << "SHGPUCFOS------" << "SHGPUCFR-------" << "SHGPUCFRM------" << "SHGPUCFRMR-----" << "SHGPUCFRMS-----" << "SHGPUCFRMT-----" << "SHGPUCFRS------" << "SHGPUCFRSR-----" << "SHGPUCFRSS-----";
    codes << "SHGPUCFRST-----" << "SHGPUCFS-------" << "SHGPUCFSA------" << "SHGPUCFSL------" << "SHGPUCFSO------" << "SHGPUCFSS------" << "SHGPUCFT-------" << "SHGPUCFTA------" << "SHGPUCFTC------" << "SHGPUCFTCD-----" << "SHGPUCFTCM-----";
    codes << "SHGPUCFTF------" << "SHGPUCFTR------" << "SHGPUCFTS------" << "SHGPUCI--------" << "SHGPUCIA-------" << "SHGPUCIC-------" << "SHGPUCII-------" << "SHGPUCIL-------" << "SHGPUCIM-------" << "SHGPUCIN-------" << "SHGPUCIO-------";
    codes << "SHGPUCIS-------" << "SHGPUCIZ-------" << "SHGPUCM--------" << "SHGPUCMS-------" << "SHGPUCMT-------" << "SHGPUCR--------" << "SHGPUCRA-------" << "SHGPUCRC-------" << "SHGPUCRH-------" << "SHGPUCRL-------" << "SHGPUCRO-------";
    codes << "SHGPUCRR-------" << "SHGPUCRRD------" << "SHGPUCRRF------" << "SHGPUCRRL------" << "SHGPUCRS-------" << "SHGPUCRV-------" << "SHGPUCRVA------" << "SHGPUCRVG------" << "SHGPUCRVM------" << "SHGPUCRVO------" << "SHGPUCRX-------";
    codes << "SHGPUCS--------" << "SHGPUCSA-------" << "SHGPUCSG-------" << "SHGPUCSGA------" << "SHGPUCSGD------" << "SHGPUCSGM------" << "SHGPUCSM-------" << "SHGPUCSR-------" << "SHGPUCSW-------" << "SHGPUCV--------" << "SHGPUCVC-------";
    codes << "SHGPUCVF-------" << "SHGPUCVFA------" << "SHGPUCVFR------" << "SHGPUCVFU------" << "SHGPUCVR-------" << "SHGPUCVRA------" << "SHGPUCVRM------" << "SHGPUCVRS------" << "SHGPUCVRU------" << "SHGPUCVRUC-----" << "SHGPUCVRUE-----";
    codes << "SHGPUCVRUH-----" << "SHGPUCVRUL-----" << "SHGPUCVRUM-----" << "SHGPUCVRW------" << "SHGPUCVS-------" << "SHGPUCVU-------" << "SHGPUCVUF------" << "SHGPUCVUR------" << "SHGPUCVV-------" << "SHGPUH---------" << "SHGPUS---------";
    codes << "SHGPUSA--------" << "SHGPUSAC-------" << "SHGPUSAF-------" << "SHGPUSAFC------" << "SHGPUSAFT------" << "SHGPUSAJ-------" << "SHGPUSAJC------" << "SHGPUSAJT------" << "SHGPUSAL-------" << "SHGPUSALC------" << "SHGPUSALT------";
    codes << "SHGPUSAM-------" << "SHGPUSAMC------" << "SHGPUSAMT------" << "SHGPUSAO-------" << "SHGPUSAOC------" << "SHGPUSAOT------" << "SHGPUSAP-------" << "SHGPUSAPB------" << "SHGPUSAPBC-----" << "SHGPUSAPBT-----" << "SHGPUSAPC------";
    codes << "SHGPUSAPM------" << "SHGPUSAPMC-----" << "SHGPUSAPMT-----" << "SHGPUSAPT------" << "SHGPUSAQ-------" << "SHGPUSAQC------" << "SHGPUSAQT------" << "SHGPUSAR-------" << "SHGPUSARC------" << "SHGPUSART------" << "SHGPUSAS-------";
    codes << "SHGPUSASC------" << "SHGPUSAST------" << "SHGPUSAT-------" << "SHGPUSAW-------" << "SHGPUSAWC------" << "SHGPUSAWT------" << "SHGPUSAX-------" << "SHGPUSAXC------" << "SHGPUSAXT------" << "SHGPUSM--------" << "SHGPUSMC-------";
    codes << "SHGPUSMD-------" << "SHGPUSMDC------" << "SHGPUSMDT------" << "SHGPUSMM-------" << "SHGPUSMMC------" << "SHGPUSMMT------" << "SHGPUSMP-------" << "SHGPUSMPC------" << "SHGPUSMPT------" << "SHGPUSMT-------" << "SHGPUSMV-------";
    codes << "SHGPUSMVC------" << "SHGPUSMVT------" << "SHGPUSS--------" << "SHGPUSS1-------" << "SHGPUSS1C------" << "SHGPUSS1T------" << "SHGPUSS2-------" << "SHGPUSS2C------" << "SHGPUSS2T------" << "SHGPUSS3-------" << "SHGPUSS3A------";
    codes << "SHGPUSS3AC-----" << "SHGPUSS3AT-----" << "SHGPUSS3C------" << "SHGPUSS3T------" << "SHGPUSS4-------" << "SHGPUSS4C------" << "SHGPUSS4T------" << "SHGPUSS5-------" << "SHGPUSS5C------" << "SHGPUSS5T------" << "SHGPUSS6-------";
    codes << "SHGPUSS6C------" << "SHGPUSS6T------" << "SHGPUSS7-------" << "SHGPUSS7C------" << "SHGPUSS7T------" << "SHGPUSS8-------" << "SHGPUSS8C------" << "SHGPUSS8T------" << "SHGPUSS9-------" << "SHGPUSS9C------" << "SHGPUSS9T------";
    codes << "SHGPUSSC-------" << "SHGPUSSL-------" << "SHGPUSSLC------" << "SHGPUSSLT------" << "SHGPUSST-------" << "SHGPUSSW-------" << "SHGPUSSWC------" << "SHGPUSSWP------" << "SHGPUSSWPC-----" << "SHGPUSSWPT-----" << "SHGPUSSWT------";
    codes << "SHGPUSSX-------" << "SHGPUSSXC------" << "SHGPUSSXT------" << "SHGPUST--------" << "SHGPUSTA-------" << "SHGPUSTAC------" << "SHGPUSTAT------" << "SHGPUSTC-------" << "SHGPUSTI-------" << "SHGPUSTIC------" << "SHGPUSTIT------";
    codes << "SHGPUSTM-------" << "SHGPUSTMC------" << "SHGPUSTMT------" << "SHGPUSTR-------" << "SHGPUSTRC------" << "SHGPUSTRT------" << "SHGPUSTS-------" << "SHGPUSTSC------" << "SHGPUSTST------" << "SHGPUSTT-------" << "SHGPUSX--------";
    codes << "SHGPUSXC-------" << "SHGPUSXE-------" << "SHGPUSXEC------" << "SHGPUSXET------" << "SHGPUSXH-------" << "SHGPUSXHC------" << "SHGPUSXHT------" << "SHGPUSXO-------" << "SHGPUSXOC------" << "SHGPUSXOM------" << "SHGPUSXOMC-----";
    codes << "SHGPUSXOMT-----" << "SHGPUSXOT------" << "SHGPUSXR-------" << "SHGPUSXRC------" << "SHGPUSXRT------" << "SHGPUSXT-------" << "SHGPUU---------" << "SHGPUUA--------" << "SHGPUUAB-------" << "SHGPUUABR------" << "SHGPUUAC-------";
    codes << "SHGPUUACC------" << "SHGPUUACCK-----" << "SHGPUUACCM-----" << "SHGPUUACR------" << "SHGPUUACRS-----" << "SHGPUUACRW-----" << "SHGPUUACS------" << "SHGPUUACSA-----" << "SHGPUUACSM-----" << "SHGPUUAD-------" << "SHGPUUAN-------";
    codes << "SHGPUUE--------" << "SHGPUUI--------" << "SHGPUUL--------" << "SHGPUULC-------" << "SHGPUULD-------" << "SHGPUULF-------" << "SHGPUULM-------" << "SHGPUULS-------" << "SHGPUUM--------" << "SHGPUUMA-------" << "SHGPUUMC-------";
    codes << "SHGPUUMJ-------" << "SHGPUUMMO------" << "SHGPUUMO-------" << "SHGPUUMQ-------" << "SHGPUUMR-------" << "SHGPUUMRG------" << "SHGPUUMRS------" << "SHGPUUMRSS-----" << "SHGPUUMRX------" << "SHGPUUMS-------" << "SHGPUUMSE------";
    codes << "SHGPUUMSEA-----" << "SHGPUUMSEC-----" << "SHGPUUMSED-----" << "SHGPUUMSEI-----" << "SHGPUUMSEJ-----" << "SHGPUUMSET-----" << "SHGPUUMT-------" << "SHGPUUP--------" << "SHGPUUS--------" << "SHGPUUSA-------" << "SHGPUUSC-------";
    codes << "SHGPUUSCL------" << "SHGPUUSF-------" << "SHGPUUSM-------" << "SHGPUUSML------" << "SHGPUUSMN------" << "SHGPUUSMS------" << "SHGPUUSO-------" << "SHGPUUSR-------" << "SHGPUUSRS------" << "SHGPUUSRT------" << "SHGPUUSRW------";
    codes << "SHGPUUSS-------" << "SHGPUUSW-------" << "SHGPUUSX-------" << "SHPP-----------" << "SHPPL----------" << "SHPPS----------" << "SHPPT----------" << "SHPPV----------" << "SHSP-----------" << "SHSPC----------" << "SHSPCA---------";
    codes << "SHSPCALA-------" << "SHSPCALC-------" << "SHSPCALS-------" << "SHSPCALSM------" << "SHSPCALST------" << "SHSPCD---------" << "SHSPCH---------" << "SHSPCL---------" << "SHSPCLBB-------" << "SHSPCLCC-------" << "SHSPCLCV-------";
    codes << "SHSPCLDD-------" << "SHSPCLFF-------" << "SHSPCLLL-------" << "SHSPCLLLAS-----" << "SHSPCLLLMI-----" << "SHSPCLLLSU-----" << "SHSPCM---------" << "SHSPCMMA-------" << "SHSPCMMH-------" << "SHSPCMML-------" << "SHSPCMMS-------";
    codes << "SHSPCP---------" << "SHSPCPSB-------" << "SHSPCPSU-------" << "SHSPCPSUG------" << "SHSPCPSUM------" << "SHSPCPSUT------" << "SHSPCU---------" << "SHSPCUM--------" << "SHSPCUN--------" << "SHSPCUR--------" << "SHSPCUS--------";
    codes << "SHSPG----------" << "SHSPGC---------" << "SHSPGG---------" << "SHSPGT---------" << "SHSPGU---------" << "SHSPN----------" << "SHSPNF---------" << "SHSPNH---------" << "SHSPNI---------" << "SHSPNM---------" << "SHSPNR---------";
    codes << "SHSPNS---------" << "SHSPO----------" << "SHSPXA---------" << "SHSPXAR--------" << "SHSPXAS--------" << "SHSPXF---------" << "SHSPXFDF-------" << "SHSPXFDR-------" << "SHSPXFTR-------" << "SHSPXH---------" << "SHSPXL---------";
    codes << "SHSPXM---------" << "SHSPXMC--------" << "SHSPXMF--------" << "SHSPXMH--------" << "SHSPXMO--------" << "SHSPXMP--------" << "SHSPXMR--------" << "SHSPXMTO-------" << "SHSPXMTU-------" << "SHSPXP---------" << "SHSPXR---------";
    codes << "SHUP-----------" << "SHUPE----------" << "SHUPND---------" << "SHUPS----------" << "SHUPS1---------" << "SHUPS2---------" << "SHUPS3---------" << "SHUPS4---------" << "SHUPSB---------" << "SHUPSC---------" << "SHUPSCA--------";
    codes << "SHUPSCB--------" << "SHUPSCF--------" << "SHUPSCG--------" << "SHUPSCM--------" << "SHUPSF---------" << "SHUPSK---------" << "SHUPSL---------" << "SHUPSN---------" << "SHUPSNA--------" << "SHUPSNB--------" << "SHUPSNF--------";
    codes << "SHUPSNG--------" << "SHUPSNM--------" << "SHUPSO---------" << "SHUPSOF--------" << "SHUPSR---------" << "SHUPSU---------" << "SHUPSUM--------" << "SHUPSUN--------" << "SHUPSUS--------" << "SHUPSX---------" << "SHUPV----------";
    codes << "SHUPW----------" << "SHUPWD---------" << "SHUPWDM--------" << "SHUPWDMG-------" << "SHUPWDMM-------" << "SHUPWM---------" << "SHUPWMA--------" << "SHUPWMB--------" << "SHUPWMBD-------" << "SHUPWMC--------" << "SHUPWMD--------";
    codes << "SHUPWME--------" << "SHUPWMF--------" << "SHUPWMFC-------" << "SHUPWMFD-------" << "SHUPWMFE-------" << "SHUPWMFO-------" << "SHUPWMFR-------" << "SHUPWMFX-------" << "SHUPWMG--------" << "SHUPWMGC-------" << "SHUPWMGD-------";
    codes << "SHUPWMGE-------" << "SHUPWMGO-------" << "SHUPWMGR-------" << "SHUPWMGX-------" << "SHUPWMM--------" << "SHUPWMMC-------" << "SHUPWMMD-------" << "SHUPWMME-------" << "SHUPWMMO-------" << "SHUPWMMR-------" << "SHUPWMMX-------";
    codes << "SHUPWMN--------" << "SHUPWMO--------" << "SHUPWMOD-------" << "SHUPWMR--------" << "SHUPWMS--------" << "SHUPWMSD-------" << "SHUPWMSX-------" << "SHUPWMX--------" << "SHUPWT---------" << "SHUPX----------" << "SNAP-----------";
    codes << "SNAPC----------" << "SNAPCF---------" << "SNAPCH---------" << "SNAPCL---------" << "SNAPM----------" << "SNAPME---------" << "SNAPMF---------" << "SNAPMFA--------" << "SNAPMFB--------" << "SNAPMFC--------" << "SNAPMFCH-------";
    codes << "SNAPMFCL-------" << "SNAPMFCM-------" << "SNAPMFD--------" << "SNAPMFF--------" << "SNAPMFFI-------" << "SNAPMFH--------" << "SNAPMFJ--------" << "SNAPMFK--------" << "SNAPMFKB-------" << "SNAPMFKD-------" << "SNAPMFL--------";
    codes << "SNAPMFM--------" << "SNAPMFO--------" << "SNAPMFP--------" << "SNAPMFPM-------" << "SNAPMFPN-------" << "SNAPMFQ--------" << "SNAPMFQA-------" << "SNAPMFQB-------" << "SNAPMFQC-------" << "SNAPMFQD-------" << "SNAPMFQF-------";
    codes << "SNAPMFQH-------" << "SNAPMFQI-------" << "SNAPMFQJ-------" << "SNAPMFQK-------" << "SNAPMFQL-------" << "SNAPMFQM-------" << "SNAPMFQN-------" << "SNAPMFQO-------" << "SNAPMFQP-------" << "SNAPMFQR-------" << "SNAPMFQRW------";
    codes << "SNAPMFQRX------" << "SNAPMFQRZ------" << "SNAPMFQS-------" << "SNAPMFQT-------" << "SNAPMFQU-------" << "SNAPMFQY-------" << "SNAPMFR--------" << "SNAPMFRW-------" << "SNAPMFRX-------" << "SNAPMFRZ-------" << "SNAPMFS--------";
    codes << "SNAPMFT--------" << "SNAPMFU--------" << "SNAPMFUH-------" << "SNAPMFUL-------" << "SNAPMFUM-------" << "SNAPMFY--------" << "SNAPMH---------" << "SNAPMHA--------" << "SNAPMHC--------" << "SNAPMHCH-------" << "SNAPMHCL-------";
    codes << "SNAPMHCM-------" << "SNAPMHD--------" << "SNAPMHH--------" << "SNAPMHI--------" << "SNAPMHJ--------" << "SNAPMHK--------" << "SNAPMHM--------" << "SNAPMHO--------" << "SNAPMHQ--------" << "SNAPMHR--------" << "SNAPMHS--------";
    codes << "SNAPMHT--------" << "SNAPMHU--------" << "SNAPMHUH-------" << "SNAPMHUL-------" << "SNAPMHUM-------" << "SNAPML---------" << "SNAPMV---------" << "SNAPW----------" << "SNAPWB---------" << "SNAPWD---------" << "SNAPWM---------";
    codes << "SNAPWMA--------" << "SNAPWMAA-------" << "SNAPWMAP-------" << "SNAPWMAS-------" << "SNAPWMB--------" << "SNAPWMCM-------" << "SNAPWMS--------" << "SNAPWMSA-------" << "SNAPWMSB-------" << "SNAPWMSS-------" << "SNAPWMSU-------";
    codes << "SNAPWMU--------" << "SNFP-----------" << "SNFPA----------" << "SNFPAF---------" << "SNFPAFA--------" << "SNFPAFK--------" << "SNFPAFU--------" << "SNFPAFUH-------" << "SNFPAFUL-------" << "SNFPAFUM-------" << "SNFPAH---------";
    codes << "SNFPAHA--------" << "SNFPAHH--------" << "SNFPAHU--------" << "SNFPAHUH-------" << "SNFPAHUL-------" << "SNFPAHUM-------" << "SNFPAV---------" << "SNFPB----------" << "SNFPG----------" << "SNFPGC---------" << "SNFPGP---------";
    codes << "SNFPGPA--------" << "SNFPGR---------" << "SNFPGS---------" << "SNFPN----------" << "SNFPNB---------" << "SNFPNN---------" << "SNFPNS---------" << "SNFPNU---------" << "SNGP-----------" << "SNGPE----------" << "SNGPES---------";
    codes << "SNGPESE--------" << "SNGPESR--------" << "SNGPEV---------" << "SNGPEVA--------" << "SNGPEVAA-------" << "SNGPEVAAR------" << "SNGPEVAC-------" << "SNGPEVAI-------" << "SNGPEVAL-------" << "SNGPEVAS-------" << "SNGPEVAT-------";
    codes << "SNGPEVATH------" << "SNGPEVATHR-----" << "SNGPEVATL------" << "SNGPEVATLR-----" << "SNGPEVATM------" << "SNGPEVATMR-----" << "SNGPEVC--------" << "SNGPEVCA-------" << "SNGPEVCAH------" << "SNGPEVCAL------" << "SNGPEVCAM------";
    codes << "SNGPEVCF-------" << "SNGPEVCFH------" << "SNGPEVCFL------" << "SNGPEVCFM------" << "SNGPEVCJ-------" << "SNGPEVCJH------" << "SNGPEVCJL------" << "SNGPEVCJM------" << "SNGPEVCM-------" << "SNGPEVCMH------" << "SNGPEVCML------";
    codes << "SNGPEVCMM------" << "SNGPEVCO-------" << "SNGPEVCOH------" << "SNGPEVCOL------" << "SNGPEVCOM------" << "SNGPEVCT-------" << "SNGPEVCTH------" << "SNGPEVCTL------" << "SNGPEVCTM------" << "SNGPEVCU-------" << "SNGPEVCUH------";
    codes << "SNGPEVCUL------" << "SNGPEVCUM------" << "SNGPEVE--------" << "SNGPEVEA-------" << "SNGPEVEAA------" << "SNGPEVEAT------" << "SNGPEVEB-------" << "SNGPEVEC-------" << "SNGPEVED-------" << "SNGPEVEDA------" << "SNGPEVEE-------";
    codes << "SNGPEVEF-------" << "SNGPEVEH-------" << "SNGPEVEM-------" << "SNGPEVEML------" << "SNGPEVEMV------" << "SNGPEVER-------" << "SNGPEVES-------" << "SNGPEVM--------" << "SNGPEVS--------" << "SNGPEVSC-------" << "SNGPEVSP-------";
    codes << "SNGPEVSR-------" << "SNGPEVST-------" << "SNGPEVSW-------" << "SNGPEVT--------" << "SNGPEVU--------" << "SNGPEVUA-------" << "SNGPEVUAA------" << "SNGPEVUB-------" << "SNGPEVUL-------" << "SNGPEVUR-------" << "SNGPEVUS-------";
    codes << "SNGPEVUSH------" << "SNGPEVUSL------" << "SNGPEVUSM------" << "SNGPEVUT-------" << "SNGPEVUTH------" << "SNGPEVUTL------" << "SNGPEVUX-------" << "SNGPEWA--------" << "SNGPEWAH-------" << "SNGPEWAL-------" << "SNGPEWAM-------";
    codes << "SNGPEWD--------" << "SNGPEWDH-------" << "SNGPEWDHS------" << "SNGPEWDL-------" << "SNGPEWDLS------" << "SNGPEWDM-------" << "SNGPEWDMS------" << "SNGPEWG--------" << "SNGPEWGH-------" << "SNGPEWGL-------" << "SNGPEWGM-------";
    codes << "SNGPEWGR-------" << "SNGPEWH--------" << "SNGPEWHH-------" << "SNGPEWHHS------" << "SNGPEWHL-------" << "SNGPEWHLS------" << "SNGPEWHM-------" << "SNGPEWHMS------" << "SNGPEWM--------" << "SNGPEWMA-------" << "SNGPEWMAI------";
    codes << "SNGPEWMAIE-----" << "SNGPEWMAIR-----" << "SNGPEWMAL------" << "SNGPEWMALE-----" << "SNGPEWMALR-----" << "SNGPEWMAS------" << "SNGPEWMASE-----" << "SNGPEWMASR-----" << "SNGPEWMAT------" << "SNGPEWMATE-----" << "SNGPEWMATR-----";
    codes << "SNGPEWMS-------" << "SNGPEWMSI------" << "SNGPEWMSL------" << "SNGPEWMSS------" << "SNGPEWMT-------" << "SNGPEWMTH------" << "SNGPEWMTL------" << "SNGPEWMTM------" << "SNGPEWO--------" << "SNGPEWOH-------" << "SNGPEWOL-------";
    codes << "SNGPEWOM-------" << "SNGPEWR--------" << "SNGPEWRH-------" << "SNGPEWRL-------" << "SNGPEWRR-------" << "SNGPEWS--------" << "SNGPEWSH-------" << "SNGPEWSL-------" << "SNGPEWSM-------" << "SNGPEWT--------" << "SNGPEWTH-------";
    codes << "SNGPEWTL-------" << "SNGPEWTM-------" << "SNGPEWX--------" << "SNGPEWXH-------" << "SNGPEWXL-------" << "SNGPEWXM-------" << "SNGPEWZ--------" << "SNGPEWZH-------" << "SNGPEWZL-------" << "SNGPEWZM-------" << "SNGPEXF--------";
    codes << "SNGPEXI--------" << "SNGPEXL--------" << "SNGPEXM--------" << "SNGPEXMC-------" << "SNGPEXML-------" << "SNGPEXN--------" << "SNGPI----------" << "SNGPIB---------" << "SNGPIBA--------" << "SNGPIBN--------" << "SNGPIE---------";
    codes << "SNGPIG---------" << "SNGPIMA--------" << "SNGPIMC--------" << "SNGPIME--------" << "SNGPIMF--------" << "SNGPIMFA-------" << "SNGPIMFP-------" << "SNGPIMFPW------" << "SNGPIMFS-------" << "SNGPIMG--------" << "SNGPIMM--------";
    codes << "SNGPIMN--------" << "SNGPIMNB-------" << "SNGPIMS--------" << "SNGPIMV--------" << "SNGPIP---------" << "SNGPIPD--------" << "SNGPIR---------" << "SNGPIRM--------" << "SNGPIRN--------" << "SNGPIRNB-------" << "SNGPIRNC-------";
    codes << "SNGPIRNN-------" << "SNGPIRP--------" << "SNGPIT---------" << "SNGPIU---------" << "SNGPIUE--------" << "SNGPIUED-------" << "SNGPIUEF-------" << "SNGPIUEN-------" << "SNGPIUP--------" << "SNGPIUR--------" << "SNGPIUT--------";
    codes << "SNGPIX---------" << "SNGPIXH--------" << "SNGPU----------" << "SNGPUC---------" << "SNGPUCA--------" << "SNGPUCAA-------" << "SNGPUCAAA------" << "SNGPUCAAAS-----" << "SNGPUCAAAT-----" << "SNGPUCAAAW-----" << "SNGPUCAAC------";
    codes << "SNGPUCAAD------" << "SNGPUCAAL------" << "SNGPUCAAM------" << "SNGPUCAAO------" << "SNGPUCAAOS-----" << "SNGPUCAAS------" << "SNGPUCAAU------" << "SNGPUCAT-------" << "SNGPUCATA------" << "SNGPUCATH------" << "SNGPUCATL------";
    codes << "SNGPUCATM------" << "SNGPUCATR------" << "SNGPUCATW------" << "SNGPUCATWR-----" << "SNGPUCAW-------" << "SNGPUCAWA------" << "SNGPUCAWH------" << "SNGPUCAWL------" << "SNGPUCAWM------" << "SNGPUCAWR------" << "SNGPUCAWS------";
    codes << "SNGPUCAWW------" << "SNGPUCAWWR-----" << "SNGPUCD--------" << "SNGPUCDC-------" << "SNGPUCDG-------" << "SNGPUCDH-------" << "SNGPUCDHH------" << "SNGPUCDHP------" << "SNGPUCDM-------" << "SNGPUCDMH------" << "SNGPUCDML------";
    codes << "SNGPUCDMLA-----" << "SNGPUCDMM------" << "SNGPUCDO-------" << "SNGPUCDS-------" << "SNGPUCDSC------" << "SNGPUCDSS------" << "SNGPUCDSV------" << "SNGPUCDT-------" << "SNGPUCE--------" << "SNGPUCEC-------" << "SNGPUCECA------";
    codes << "SNGPUCECC------" << "SNGPUCECH------" << "SNGPUCECL------" << "SNGPUCECM------" << "SNGPUCECO------" << "SNGPUCECR------" << "SNGPUCECS------" << "SNGPUCECT------" << "SNGPUCECW------" << "SNGPUCEN-------" << "SNGPUCENN------";
    codes << "SNGPUCF--------" << "SNGPUCFH-------" << "SNGPUCFHA------" << "SNGPUCFHC------" << "SNGPUCFHE------" << "SNGPUCFHH------" << "SNGPUCFHL------" << "SNGPUCFHM------" << "SNGPUCFHO------" << "SNGPUCFHS------" << "SNGPUCFHX------";
    codes << "SNGPUCFM-------" << "SNGPUCFML------" << "SNGPUCFMS------" << "SNGPUCFMT------" << "SNGPUCFMTA-----" << "SNGPUCFMTC-----" << "SNGPUCFMTO-----" << "SNGPUCFMTS-----" << "SNGPUCFMW------" << "SNGPUCFO-------" << "SNGPUCFOA------";
    codes << "SNGPUCFOL------" << "SNGPUCFOO------" << "SNGPUCFOS------" << "SNGPUCFR-------" << "SNGPUCFRM------" << "SNGPUCFRMR-----" << "SNGPUCFRMS-----" << "SNGPUCFRMT-----" << "SNGPUCFRS------" << "SNGPUCFRSR-----" << "SNGPUCFRSS-----";
    codes << "SNGPUCFRST-----" << "SNGPUCFS-------" << "SNGPUCFSA------" << "SNGPUCFSL------" << "SNGPUCFSO------" << "SNGPUCFSS------" << "SNGPUCFT-------" << "SNGPUCFTA------" << "SNGPUCFTC------" << "SNGPUCFTCD-----" << "SNGPUCFTCM-----";
    codes << "SNGPUCFTF------" << "SNGPUCFTR------" << "SNGPUCFTS------" << "SNGPUCI--------" << "SNGPUCIA-------" << "SNGPUCIC-------" << "SNGPUCII-------" << "SNGPUCIL-------" << "SNGPUCIM-------" << "SNGPUCIN-------" << "SNGPUCIO-------";
    codes << "SNGPUCIS-------" << "SNGPUCIZ-------" << "SNGPUCM--------" << "SNGPUCMS-------" << "SNGPUCMT-------" << "SNGPUCR--------" << "SNGPUCRA-------" << "SNGPUCRC-------" << "SNGPUCRH-------" << "SNGPUCRL-------" << "SNGPUCRO-------";
    codes << "SNGPUCRR-------" << "SNGPUCRRD------" << "SNGPUCRRF------" << "SNGPUCRRL------" << "SNGPUCRS-------" << "SNGPUCRV-------" << "SNGPUCRVA------" << "SNGPUCRVG------" << "SNGPUCRVM------" << "SNGPUCRVO------" << "SNGPUCRX-------";
    codes << "SNGPUCS--------" << "SNGPUCSA-------" << "SNGPUCSG-------" << "SNGPUCSGA------" << "SNGPUCSGD------" << "SNGPUCSGM------" << "SNGPUCSM-------" << "SNGPUCSR-------" << "SNGPUCSW-------" << "SNGPUCV--------" << "SNGPUCVC-------";
    codes << "SNGPUCVF-------" << "SNGPUCVFA------" << "SNGPUCVFR------" << "SNGPUCVFU------" << "SNGPUCVR-------" << "SNGPUCVRA------" << "SNGPUCVRM------" << "SNGPUCVRS------" << "SNGPUCVRU------" << "SNGPUCVRUC-----" << "SNGPUCVRUE-----";
    codes << "SNGPUCVRUH-----" << "SNGPUCVRUL-----" << "SNGPUCVRUM-----" << "SNGPUCVRW------" << "SNGPUCVS-------" << "SNGPUCVU-------" << "SNGPUCVUF------" << "SNGPUCVUR------" << "SNGPUCVV-------" << "SNGPUH---------" << "SNGPUS---------";
    codes << "SNGPUSA--------" << "SNGPUSAC-------" << "SNGPUSAF-------" << "SNGPUSAFC------" << "SNGPUSAFT------" << "SNGPUSAJ-------" << "SNGPUSAJC------" << "SNGPUSAJT------" << "SNGPUSAL-------" << "SNGPUSALC------" << "SNGPUSALT------";
    codes << "SNGPUSAM-------" << "SNGPUSAMC------" << "SNGPUSAMT------" << "SNGPUSAO-------" << "SNGPUSAOC------" << "SNGPUSAOT------" << "SNGPUSAP-------" << "SNGPUSAPB------" << "SNGPUSAPBC-----" << "SNGPUSAPBT-----" << "SNGPUSAPC------";
    codes << "SNGPUSAPM------" << "SNGPUSAPMC-----" << "SNGPUSAPMT-----" << "SNGPUSAPT------" << "SNGPUSAQ-------" << "SNGPUSAQC------" << "SNGPUSAQT------" << "SNGPUSAR-------" << "SNGPUSARC------" << "SNGPUSART------" << "SNGPUSAS-------";
    codes << "SNGPUSASC------" << "SNGPUSAST------" << "SNGPUSAT-------" << "SNGPUSAW-------" << "SNGPUSAWC------" << "SNGPUSAWT------" << "SNGPUSAX-------" << "SNGPUSAXC------" << "SNGPUSAXT------" << "SNGPUSM--------" << "SNGPUSMC-------";
    codes << "SNGPUSMD-------" << "SNGPUSMDC------" << "SNGPUSMDT------" << "SNGPUSMM-------" << "SNGPUSMMC------" << "SNGPUSMMT------" << "SNGPUSMP-------" << "SNGPUSMPC------" << "SNGPUSMPT------" << "SNGPUSMT-------" << "SNGPUSMV-------";
    codes << "SNGPUSMVC------" << "SNGPUSMVT------" << "SNGPUSS--------" << "SNGPUSS1-------" << "SNGPUSS1C------" << "SNGPUSS1T------" << "SNGPUSS2-------" << "SNGPUSS2C------" << "SNGPUSS2T------" << "SNGPUSS3-------" << "SNGPUSS3A------";
    codes << "SNGPUSS3AC-----" << "SNGPUSS3AT-----" << "SNGPUSS3C------" << "SNGPUSS3T------" << "SNGPUSS4-------" << "SNGPUSS4C------" << "SNGPUSS4T------" << "SNGPUSS5-------" << "SNGPUSS5C------" << "SNGPUSS5T------" << "SNGPUSS6-------";
    codes << "SNGPUSS6C------" << "SNGPUSS6T------" << "SNGPUSS7-------" << "SNGPUSS7C------" << "SNGPUSS7T------" << "SNGPUSS8-------" << "SNGPUSS8C------" << "SNGPUSS8T------" << "SNGPUSS9-------" << "SNGPUSS9C------" << "SNGPUSS9T------";
    codes << "SNGPUSSC-------" << "SNGPUSSL-------" << "SNGPUSSLC------" << "SNGPUSSLT------" << "SNGPUSST-------" << "SNGPUSSW-------" << "SNGPUSSWC------" << "SNGPUSSWP------" << "SNGPUSSWPC-----" << "SNGPUSSWPT-----" << "SNGPUSSWT------";
    codes << "SNGPUSSX-------" << "SNGPUSSXC------" << "SNGPUSSXT------" << "SNGPUST--------" << "SNGPUSTA-------" << "SNGPUSTAC------" << "SNGPUSTAT------" << "SNGPUSTC-------" << "SNGPUSTI-------" << "SNGPUSTIC------" << "SNGPUSTIT------";
    codes << "SNGPUSTM-------" << "SNGPUSTMC------" << "SNGPUSTMT------" << "SNGPUSTR-------" << "SNGPUSTRC------" << "SNGPUSTRT------" << "SNGPUSTS-------" << "SNGPUSTSC------" << "SNGPUSTST------" << "SNGPUSTT-------" << "SNGPUSX--------";
    codes << "SNGPUSXC-------" << "SNGPUSXE-------" << "SNGPUSXEC------" << "SNGPUSXET------" << "SNGPUSXH-------" << "SNGPUSXHC------" << "SNGPUSXHT------" << "SNGPUSXO-------" << "SNGPUSXOC------" << "SNGPUSXOM------" << "SNGPUSXOMC-----";
    codes << "SNGPUSXOMT-----" << "SNGPUSXOT------" << "SNGPUSXR-------" << "SNGPUSXRC------" << "SNGPUSXRT------" << "SNGPUSXT-------" << "SNGPUU---------" << "SNGPUUA--------" << "SNGPUUAB-------" << "SNGPUUABR------" << "SNGPUUAC-------";
    codes << "SNGPUUACC------" << "SNGPUUACCK-----" << "SNGPUUACCM-----" << "SNGPUUACR------" << "SNGPUUACRS-----" << "SNGPUUACRW-----" << "SNGPUUACS------" << "SNGPUUACSA-----" << "SNGPUUACSM-----" << "SNGPUUAD-------" << "SNGPUUAN-------";
    codes << "SNGPUUE--------" << "SNGPUUI--------" << "SNGPUUL--------" << "SNGPUULC-------" << "SNGPUULD-------" << "SNGPUULF-------" << "SNGPUULM-------" << "SNGPUULS-------" << "SNGPUUM--------" << "SNGPUUMA-------" << "SNGPUUMC-------";
    codes << "SNGPUUMJ-------" << "SNGPUUMMO------" << "SNGPUUMO-------" << "SNGPUUMQ-------" << "SNGPUUMR-------" << "SNGPUUMRG------" << "SNGPUUMRS------" << "SNGPUUMRSS-----" << "SNGPUUMRX------" << "SNGPUUMS-------" << "SNGPUUMSE------";
    codes << "SNGPUUMSEA-----" << "SNGPUUMSEC-----" << "SNGPUUMSED-----" << "SNGPUUMSEI-----" << "SNGPUUMSEJ-----" << "SNGPUUMSET-----" << "SNGPUUMT-------" << "SNGPUUP--------" << "SNGPUUS--------" << "SNGPUUSA-------" << "SNGPUUSC-------";
    codes << "SNGPUUSCL------" << "SNGPUUSF-------" << "SNGPUUSM-------" << "SNGPUUSML------" << "SNGPUUSMN------" << "SNGPUUSMS------" << "SNGPUUSO-------" << "SNGPUUSR-------" << "SNGPUUSRS------" << "SNGPUUSRT------" << "SNGPUUSRW------";
    codes << "SNGPUUSS-------" << "SNGPUUSW-------" << "SNGPUUSX-------" << "SNPP-----------" << "SNPPL----------" << "SNPPS----------" << "SNPPT----------" << "SNPPV----------" << "SNSP-----------" << "SNSPC----------" << "SNSPCA---------";
    codes << "SNSPCALA-------" << "SNSPCALC-------" << "SNSPCALS-------" << "SNSPCALSM------" << "SNSPCALST------" << "SNSPCD---------" << "SNSPCH---------" << "SNSPCL---------" << "SNSPCLBB-------" << "SNSPCLCC-------" << "SNSPCLCV-------";
    codes << "SNSPCLDD-------" << "SNSPCLFF-------" << "SNSPCLLL-------" << "SNSPCLLLAS-----" << "SNSPCLLLMI-----" << "SNSPCLLLSU-----" << "SNSPCM---------" << "SNSPCMMA-------" << "SNSPCMMH-------" << "SNSPCMML-------" << "SNSPCMMS-------";
    codes << "SNSPCP---------" << "SNSPCPSB-------" << "SNSPCPSU-------" << "SNSPCPSUG------" << "SNSPCPSUM------" << "SNSPCPSUT------" << "SNSPCU---------" << "SNSPCUM--------" << "SNSPCUN--------" << "SNSPCUR--------" << "SNSPCUS--------";
    codes << "SNSPG----------" << "SNSPGC---------" << "SNSPGG---------" << "SNSPGT---------" << "SNSPGU---------" << "SNSPN----------" << "SNSPNF---------" << "SNSPNH---------" << "SNSPNI---------" << "SNSPNM---------" << "SNSPNR---------";
    codes << "SNSPNS---------" << "SNSPO----------" << "SNSPXA---------" << "SNSPXAR--------" << "SNSPXAS--------" << "SNSPXF---------" << "SNSPXFDF-------" << "SNSPXFDR-------" << "SNSPXFTR-------" << "SNSPXH---------" << "SNSPXL---------";
    codes << "SNSPXM---------" << "SNSPXMC--------" << "SNSPXMF--------" << "SNSPXMH--------" << "SNSPXMO--------" << "SNSPXMP--------" << "SNSPXMR--------" << "SNSPXMTO-------" << "SNSPXMTU-------" << "SNSPXP---------" << "SNSPXR---------";
    codes << "SNUP-----------" << "SNUPE----------" << "SNUPND---------" << "SNUPS----------" << "SNUPS1---------" << "SNUPS2---------" << "SNUPS3---------" << "SNUPS4---------" << "SNUPSB---------" << "SNUPSC---------" << "SNUPSCA--------";
    codes << "SNUPSCB--------" << "SNUPSCF--------" << "SNUPSCG--------" << "SNUPSCM--------" << "SNUPSF---------" << "SNUPSK---------" << "SNUPSL---------" << "SNUPSN---------" << "SNUPSNA--------" << "SNUPSNB--------" << "SNUPSNF--------";
    codes << "SNUPSNG--------" << "SNUPSNM--------" << "SNUPSO---------" << "SNUPSOF--------" << "SNUPSR---------" << "SNUPSU---------" << "SNUPSUM--------" << "SNUPSUN--------" << "SNUPSUS--------" << "SNUPSX---------" << "SNUPV----------";
    codes << "SNUPW----------" << "SNUPWD---------" << "SNUPWDM--------" << "SNUPWDMG-------" << "SNUPWDMM-------" << "SNUPWM---------" << "SNUPWMA--------" << "SNUPWMB--------" << "SNUPWMBD-------" << "SNUPWMC--------" << "SNUPWMD--------";
    codes << "SNUPWME--------" << "SNUPWMF--------" << "SNUPWMFC-------" << "SNUPWMFD-------" << "SNUPWMFE-------" << "SNUPWMFO-------" << "SNUPWMFR-------" << "SNUPWMFX-------" << "SNUPWMG--------" << "SNUPWMGC-------" << "SNUPWMGD-------";
    codes << "SNUPWMGE-------" << "SNUPWMGO-------" << "SNUPWMGR-------" << "SNUPWMGX-------" << "SNUPWMM--------" << "SNUPWMMC-------" << "SNUPWMMD-------" << "SNUPWMME-------" << "SNUPWMMO-------" << "SNUPWMMR-------" << "SNUPWMMX-------";
    codes << "SNUPWMN--------" << "SNUPWMO--------" << "SNUPWMOD-------" << "SNUPWMR--------" << "SNUPWMS--------" << "SNUPWMSD-------" << "SNUPWMSX-------" << "SNUPWMX--------" << "SNUPWT---------" << "SNUPX----------" << "SUAP-----------";
    codes << "SUAPC----------" << "SUAPCF---------" << "SUAPCH---------" << "SUAPCL---------" << "SUAPM----------" << "SUAPME---------" << "SUAPMF---------" << "SUAPMFA--------" << "SUAPMFB--------" << "SUAPMFC--------" << "SUAPMFCH-------";
    codes << "SUAPMFCL-------" << "SUAPMFCM-------" << "SUAPMFD--------" << "SUAPMFF--------" << "SUAPMFFI-------" << "SUAPMFH--------" << "SUAPMFJ--------" << "SUAPMFK--------" << "SUAPMFKB-------" << "SUAPMFKD-------" << "SUAPMFL--------";
    codes << "SUAPMFM--------" << "SUAPMFO--------" << "SUAPMFP--------" << "SUAPMFPM-------" << "SUAPMFPN-------" << "SUAPMFQ--------" << "SUAPMFQA-------" << "SUAPMFQB-------" << "SUAPMFQC-------" << "SUAPMFQD-------" << "SUAPMFQF-------";
    codes << "SUAPMFQH-------" << "SUAPMFQI-------" << "SUAPMFQJ-------" << "SUAPMFQK-------" << "SUAPMFQL-------" << "SUAPMFQM-------" << "SUAPMFQN-------" << "SUAPMFQO-------" << "SUAPMFQP-------" << "SUAPMFQR-------" << "SUAPMFQRW------";
    codes << "SUAPMFQRX------" << "SUAPMFQRZ------" << "SUAPMFQS-------" << "SUAPMFQT-------" << "SUAPMFQU-------" << "SUAPMFQY-------" << "SUAPMFR--------" << "SUAPMFRW-------" << "SUAPMFRX-------" << "SUAPMFRZ-------" << "SUAPMFS--------";
    codes << "SUAPMFT--------" << "SUAPMFU--------" << "SUAPMFUH-------" << "SUAPMFUL-------" << "SUAPMFUM-------" << "SUAPMFY--------" << "SUAPMH---------" << "SUAPMHA--------" << "SUAPMHC--------" << "SUAPMHCH-------" << "SUAPMHCL-------";
    codes << "SUAPMHCM-------" << "SUAPMHD--------" << "SUAPMHH--------" << "SUAPMHI--------" << "SUAPMHJ--------" << "SUAPMHK--------" << "SUAPMHM--------" << "SUAPMHO--------" << "SUAPMHQ--------" << "SUAPMHR--------" << "SUAPMHS--------";
    codes << "SUAPMHT--------" << "SUAPMHU--------" << "SUAPMHUH-------" << "SUAPMHUL-------" << "SUAPMHUM-------" << "SUAPML---------" << "SUAPMV---------" << "SUAPW----------" << "SUAPWB---------" << "SUAPWD---------" << "SUAPWM---------";
    codes << "SUAPWMA--------" << "SUAPWMAA-------" << "SUAPWMAP-------" << "SUAPWMAS-------" << "SUAPWMB--------" << "SUAPWMCM-------" << "SUAPWMS--------" << "SUAPWMSA-------" << "SUAPWMSB-------" << "SUAPWMSS-------" << "SUAPWMSU-------";
    codes << "SUAPWMU--------" << "SUFP-----------" << "SUFPA----------" << "SUFPAF---------" << "SUFPAFA--------" << "SUFPAFK--------" << "SUFPAFU--------" << "SUFPAFUH-------" << "SUFPAFUL-------" << "SUFPAFUM-------" << "SUFPAH---------";
    codes << "SUFPAHA--------" << "SUFPAHH--------" << "SUFPAHU--------" << "SUFPAHUH-------" << "SUFPAHUL-------" << "SUFPAHUM-------" << "SUFPAV---------" << "SUFPB----------" << "SUFPG----------" << "SUFPGC---------" << "SUFPGP---------";
    codes << "SUFPGPA--------" << "SUFPGR---------" << "SUFPGS---------" << "SUFPN----------" << "SUFPNB---------" << "SUFPNN---------" << "SUFPNS---------" << "SUFPNU---------" << "SUGP-----------" << "SUGPE----------" << "SUGPES---------";
    codes << "SUGPESE--------" << "SUGPESR--------" << "SUGPEV---------" << "SUGPEVA--------" << "SUGPEVAA-------" << "SUGPEVAAR------" << "SUGPEVAC-------" << "SUGPEVAI-------" << "SUGPEVAL-------" << "SUGPEVAS-------" << "SUGPEVAT-------";
    codes << "SUGPEVATH------" << "SUGPEVATHR-----" << "SUGPEVATL------" << "SUGPEVATLR-----" << "SUGPEVATM------" << "SUGPEVATMR-----" << "SUGPEVC--------" << "SUGPEVCA-------" << "SUGPEVCAH------" << "SUGPEVCAL------" << "SUGPEVCAM------";
    codes << "SUGPEVCF-------" << "SUGPEVCFH------" << "SUGPEVCFL------" << "SUGPEVCFM------" << "SUGPEVCJ-------" << "SUGPEVCJH------" << "SUGPEVCJL------" << "SUGPEVCJM------" << "SUGPEVCM-------" << "SUGPEVCMH------" << "SUGPEVCML------";
    codes << "SUGPEVCMM------" << "SUGPEVCO-------" << "SUGPEVCOH------" << "SUGPEVCOL------" << "SUGPEVCOM------" << "SUGPEVCT-------" << "SUGPEVCTH------" << "SUGPEVCTL------" << "SUGPEVCTM------" << "SUGPEVCU-------" << "SUGPEVCUH------";
    codes << "SUGPEVCUL------" << "SUGPEVCUM------" << "SUGPEVE--------" << "SUGPEVEA-------" << "SUGPEVEAA------" << "SUGPEVEAT------" << "SUGPEVEB-------" << "SUGPEVEC-------" << "SUGPEVED-------" << "SUGPEVEDA------" << "SUGPEVEE-------";
    codes << "SUGPEVEF-------" << "SUGPEVEH-------" << "SUGPEVEM-------" << "SUGPEVEML------" << "SUGPEVEMV------" << "SUGPEVER-------" << "SUGPEVES-------" << "SUGPEVM--------" << "SUGPEVS--------" << "SUGPEVSC-------" << "SUGPEVSP-------";
    codes << "SUGPEVSR-------" << "SUGPEVST-------" << "SUGPEVSW-------" << "SUGPEVT--------" << "SUGPEVU--------" << "SUGPEVUA-------" << "SUGPEVUAA------" << "SUGPEVUB-------" << "SUGPEVUL-------" << "SUGPEVUR-------" << "SUGPEVUS-------";
    codes << "SUGPEVUSH------" << "SUGPEVUSL------" << "SUGPEVUSM------" << "SUGPEVUT-------" << "SUGPEVUTH------" << "SUGPEVUTL------" << "SUGPEVUX-------" << "SUGPEWA--------" << "SUGPEWAH-------" << "SUGPEWAL-------" << "SUGPEWAM-------";
    codes << "SUGPEWD--------" << "SUGPEWDH-------" << "SUGPEWDHS------" << "SUGPEWDL-------" << "SUGPEWDLS------" << "SUGPEWDM-------" << "SUGPEWDMS------" << "SUGPEWG--------" << "SUGPEWGH-------" << "SUGPEWGL-------" << "SUGPEWGM-------";
    codes << "SUGPEWGR-------" << "SUGPEWH--------" << "SUGPEWHH-------" << "SUGPEWHHS------" << "SUGPEWHL-------" << "SUGPEWHLS------" << "SUGPEWHM-------" << "SUGPEWHMS------" << "SUGPEWM--------" << "SUGPEWMA-------" << "SUGPEWMAI------";
    codes << "SUGPEWMAIE-----" << "SUGPEWMAIR-----" << "SUGPEWMAL------" << "SUGPEWMALE-----" << "SUGPEWMALR-----" << "SUGPEWMAS------" << "SUGPEWMASE-----" << "SUGPEWMASR-----" << "SUGPEWMAT------" << "SUGPEWMATE-----" << "SUGPEWMATR-----";
    codes << "SUGPEWMS-------" << "SUGPEWMSI------" << "SUGPEWMSL------" << "SUGPEWMSS------" << "SUGPEWMT-------" << "SUGPEWMTH------" << "SUGPEWMTL------" << "SUGPEWMTM------" << "SUGPEWO--------" << "SUGPEWOH-------" << "SUGPEWOL-------";
    codes << "SUGPEWOM-------" << "SUGPEWR--------" << "SUGPEWRH-------" << "SUGPEWRL-------" << "SUGPEWRR-------" << "SUGPEWS--------" << "SUGPEWSH-------" << "SUGPEWSL-------" << "SUGPEWSM-------" << "SUGPEWT--------" << "SUGPEWTH-------";
    codes << "SUGPEWTL-------" << "SUGPEWTM-------" << "SUGPEWX--------" << "SUGPEWXH-------" << "SUGPEWXL-------" << "SUGPEWXM-------" << "SUGPEWZ--------" << "SUGPEWZH-------" << "SUGPEWZL-------" << "SUGPEWZM-------" << "SUGPEXF--------";
    codes << "SUGPEXI--------" << "SUGPEXL--------" << "SUGPEXM--------" << "SUGPEXMC-------" << "SUGPEXML-------" << "SUGPEXN--------" << "SUGPI----------" << "SUGPIB---------" << "SUGPIBA--------" << "SUGPIBN--------" << "SUGPIE---------";
    codes << "SUGPIG---------" << "SUGPIMA--------" << "SUGPIMC--------" << "SUGPIME--------" << "SUGPIMF--------" << "SUGPIMFA-------" << "SUGPIMFP-------" << "SUGPIMFPW------" << "SUGPIMFS-------" << "SUGPIMG--------" << "SUGPIMM--------";
    codes << "SUGPIMN--------" << "SUGPIMNB-------" << "SUGPIMS--------" << "SUGPIMV--------" << "SUGPIP---------" << "SUGPIPD--------" << "SUGPIR---------" << "SUGPIRM--------" << "SUGPIRN--------" << "SUGPIRNB-------" << "SUGPIRNC-------";
    codes << "SUGPIRNN-------" << "SUGPIRP--------" << "SUGPIT---------" << "SUGPIU---------" << "SUGPIUE--------" << "SUGPIUED-------" << "SUGPIUEF-------" << "SUGPIUEN-------" << "SUGPIUP--------" << "SUGPIUR--------" << "SUGPIUT--------";
    codes << "SUGPIX---------" << "SUGPIXH--------" << "SUGPU----------" << "SUGPUC---------" << "SUGPUCA--------" << "SUGPUCAA-------" << "SUGPUCAAA------" << "SUGPUCAAAS-----" << "SUGPUCAAAT-----" << "SUGPUCAAAW-----" << "SUGPUCAAC------";
    codes << "SUGPUCAAD------" << "SUGPUCAAL------" << "SUGPUCAAM------" << "SUGPUCAAO------" << "SUGPUCAAOS-----" << "SUGPUCAAS------" << "SUGPUCAAU------" << "SUGPUCAT-------" << "SUGPUCATA------" << "SUGPUCATH------" << "SUGPUCATL------";
    codes << "SUGPUCATM------" << "SUGPUCATR------" << "SUGPUCATW------" << "SUGPUCATWR-----" << "SUGPUCAW-------" << "SUGPUCAWA------" << "SUGPUCAWH------" << "SUGPUCAWL------" << "SUGPUCAWM------" << "SUGPUCAWR------" << "SUGPUCAWS------";
    codes << "SUGPUCAWW------" << "SUGPUCAWWR-----" << "SUGPUCD--------" << "SUGPUCDC-------" << "SUGPUCDG-------" << "SUGPUCDH-------" << "SUGPUCDHH------" << "SUGPUCDHP------" << "SUGPUCDM-------" << "SUGPUCDMH------" << "SUGPUCDML------";
    codes << "SUGPUCDMLA-----" << "SUGPUCDMM------" << "SUGPUCDO-------" << "SUGPUCDS-------" << "SUGPUCDSC------" << "SUGPUCDSS------" << "SUGPUCDSV------" << "SUGPUCDT-------" << "SUGPUCE--------" << "SUGPUCEC-------" << "SUGPUCECA------";
    codes << "SUGPUCECC------" << "SUGPUCECH------" << "SUGPUCECL------" << "SUGPUCECM------" << "SUGPUCECO------" << "SUGPUCECR------" << "SUGPUCECS------" << "SUGPUCECT------" << "SUGPUCECW------" << "SUGPUCEN-------" << "SUGPUCENN------";
    codes << "SUGPUCF--------" << "SUGPUCFH-------" << "SUGPUCFHA------" << "SUGPUCFHC------" << "SUGPUCFHE------" << "SUGPUCFHH------" << "SUGPUCFHL------" << "SUGPUCFHM------" << "SUGPUCFHO------" << "SUGPUCFHS------" << "SUGPUCFHX------";
    codes << "SUGPUCFM-------" << "SUGPUCFML------" << "SUGPUCFMS------" << "SUGPUCFMT------" << "SUGPUCFMTA-----" << "SUGPUCFMTC-----" << "SUGPUCFMTO-----" << "SUGPUCFMTS-----" << "SUGPUCFMW------" << "SUGPUCFO-------" << "SUGPUCFOA------";
    codes << "SUGPUCFOL------" << "SUGPUCFOO------" << "SUGPUCFOS------" << "SUGPUCFR-------" << "SUGPUCFRM------" << "SUGPUCFRMR-----" << "SUGPUCFRMS-----" << "SUGPUCFRMT-----" << "SUGPUCFRS------" << "SUGPUCFRSR-----" << "SUGPUCFRSS-----";
    codes << "SUGPUCFRST-----" << "SUGPUCFS-------" << "SUGPUCFSA------" << "SUGPUCFSL------" << "SUGPUCFSO------" << "SUGPUCFSS------" << "SUGPUCFT-------" << "SUGPUCFTA------" << "SUGPUCFTC------" << "SUGPUCFTCD-----" << "SUGPUCFTCM-----";
    codes << "SUGPUCFTF------" << "SUGPUCFTR------" << "SUGPUCFTS------" << "SUGPUCI--------" << "SUGPUCIA-------" << "SUGPUCIC-------" << "SUGPUCII-------" << "SUGPUCIL-------" << "SUGPUCIM-------" << "SUGPUCIN-------" << "SUGPUCIO-------";
    codes << "SUGPUCIS-------" << "SUGPUCIZ-------" << "SUGPUCM--------" << "SUGPUCMS-------" << "SUGPUCMT-------" << "SUGPUCR--------" << "SUGPUCRA-------" << "SUGPUCRC-------" << "SUGPUCRH-------" << "SUGPUCRL-------" << "SUGPUCRO-------";
    codes << "SUGPUCRR-------" << "SUGPUCRRD------" << "SUGPUCRRF------" << "SUGPUCRRL------" << "SUGPUCRS-------" << "SUGPUCRV-------" << "SUGPUCRVA------" << "SUGPUCRVG------" << "SUGPUCRVM------" << "SUGPUCRVO------" << "SUGPUCRX-------";
    codes << "SUGPUCS--------" << "SUGPUCSA-------" << "SUGPUCSG-------" << "SUGPUCSGA------" << "SUGPUCSGD------" << "SUGPUCSGM------" << "SUGPUCSM-------" << "SUGPUCSR-------" << "SUGPUCSW-------" << "SUGPUCV--------" << "SUGPUCVC-------";
    codes << "SUGPUCVF-------" << "SUGPUCVFA------" << "SUGPUCVFR------" << "SUGPUCVFU------" << "SUGPUCVR-------" << "SUGPUCVRA------" << "SUGPUCVRM------" << "SUGPUCVRS------" << "SUGPUCVRU------" << "SUGPUCVRUC-----" << "SUGPUCVRUE-----";
    codes << "SUGPUCVRUH-----" << "SUGPUCVRUL-----" << "SUGPUCVRUM-----" << "SUGPUCVRW------" << "SUGPUCVS-------" << "SUGPUCVU-------" << "SUGPUCVUF------" << "SUGPUCVUR------" << "SUGPUCVV-------" << "SUGPUH---------" << "SUGPUS---------";
    codes << "SUGPUSA--------" << "SUGPUSAC-------" << "SUGPUSAF-------" << "SUGPUSAFC------" << "SUGPUSAFT------" << "SUGPUSAJ-------" << "SUGPUSAJC------" << "SUGPUSAJT------" << "SUGPUSAL-------" << "SUGPUSALC------" << "SUGPUSALT------";
    codes << "SUGPUSAM-------" << "SUGPUSAMC------" << "SUGPUSAMT------" << "SUGPUSAO-------" << "SUGPUSAOC------" << "SUGPUSAOT------" << "SUGPUSAP-------" << "SUGPUSAPB------" << "SUGPUSAPBC-----" << "SUGPUSAPBT-----" << "SUGPUSAPC------";
    codes << "SUGPUSAPM------" << "SUGPUSAPMC-----" << "SUGPUSAPMT-----" << "SUGPUSAPT------" << "SUGPUSAQ-------" << "SUGPUSAQC------" << "SUGPUSAQT------" << "SUGPUSAR-------" << "SUGPUSARC------" << "SUGPUSART------" << "SUGPUSAS-------";
    codes << "SUGPUSASC------" << "SUGPUSAST------" << "SUGPUSAT-------" << "SUGPUSAW-------" << "SUGPUSAWC------" << "SUGPUSAWT------" << "SUGPUSAX-------" << "SUGPUSAXC------" << "SUGPUSAXT------" << "SUGPUSM--------" << "SUGPUSMC-------";
    codes << "SUGPUSMD-------" << "SUGPUSMDC------" << "SUGPUSMDT------" << "SUGPUSMM-------" << "SUGPUSMMC------" << "SUGPUSMMT------" << "SUGPUSMP-------" << "SUGPUSMPC------" << "SUGPUSMPT------" << "SUGPUSMT-------" << "SUGPUSMV-------";
    codes << "SUGPUSMVC------" << "SUGPUSMVT------" << "SUGPUSS--------" << "SUGPUSS1-------" << "SUGPUSS1C------" << "SUGPUSS1T------" << "SUGPUSS2-------" << "SUGPUSS2C------" << "SUGPUSS2T------" << "SUGPUSS3-------" << "SUGPUSS3A------";
    codes << "SUGPUSS3AC-----" << "SUGPUSS3AT-----" << "SUGPUSS3C------" << "SUGPUSS3T------" << "SUGPUSS4-------" << "SUGPUSS4C------" << "SUGPUSS4T------" << "SUGPUSS5-------" << "SUGPUSS5C------" << "SUGPUSS5T------" << "SUGPUSS6-------";
    codes << "SUGPUSS6C------" << "SUGPUSS6T------" << "SUGPUSS7-------" << "SUGPUSS7C------" << "SUGPUSS7T------" << "SUGPUSS8-------" << "SUGPUSS8C------" << "SUGPUSS8T------" << "SUGPUSS9-------" << "SUGPUSS9C------" << "SUGPUSS9T------";
    codes << "SUGPUSSC-------" << "SUGPUSSL-------" << "SUGPUSSLC------" << "SUGPUSSLT------" << "SUGPUSST-------" << "SUGPUSSW-------" << "SUGPUSSWC------" << "SUGPUSSWP------" << "SUGPUSSWPC-----" << "SUGPUSSWPT-----" << "SUGPUSSWT------";
    codes << "SUGPUSSX-------" << "SUGPUSSXC------" << "SUGPUSSXT------" << "SUGPUST--------" << "SUGPUSTA-------" << "SUGPUSTAC------" << "SUGPUSTAT------" << "SUGPUSTC-------" << "SUGPUSTI-------" << "SUGPUSTIC------" << "SUGPUSTIT------";
    codes << "SUGPUSTM-------" << "SUGPUSTMC------" << "SUGPUSTMT------" << "SUGPUSTR-------" << "SUGPUSTRC------" << "SUGPUSTRT------" << "SUGPUSTS-------" << "SUGPUSTSC------" << "SUGPUSTST------" << "SUGPUSTT-------" << "SUGPUSX--------";
    codes << "SUGPUSXC-------" << "SUGPUSXE-------" << "SUGPUSXEC------" << "SUGPUSXET------" << "SUGPUSXH-------" << "SUGPUSXHC------" << "SUGPUSXHT------" << "SUGPUSXO-------" << "SUGPUSXOC------" << "SUGPUSXOM------" << "SUGPUSXOMC-----";
    codes << "SUGPUSXOMT-----" << "SUGPUSXOT------" << "SUGPUSXR-------" << "SUGPUSXRC------" << "SUGPUSXRT------" << "SUGPUSXT-------" << "SUGPUU---------" << "SUGPUUA--------" << "SUGPUUAB-------" << "SUGPUUABR------" << "SUGPUUAC-------";
    codes << "SUGPUUACC------" << "SUGPUUACCK-----" << "SUGPUUACCM-----" << "SUGPUUACR------" << "SUGPUUACRS-----" << "SUGPUUACRW-----" << "SUGPUUACS------" << "SUGPUUACSA-----" << "SUGPUUACSM-----" << "SUGPUUAD-------" << "SUGPUUAN-------";
    codes << "SUGPUUE--------" << "SUGPUUI--------" << "SUGPUUL--------" << "SUGPUULC-------" << "SUGPUULD-------" << "SUGPUULF-------" << "SUGPUULM-------" << "SUGPUULS-------" << "SUGPUUM--------" << "SUGPUUMA-------" << "SUGPUUMC-------";
    codes << "SUGPUUMJ-------" << "SUGPUUMMO------" << "SUGPUUMO-------" << "SUGPUUMQ-------" << "SUGPUUMR-------" << "SUGPUUMRG------" << "SUGPUUMRS------" << "SUGPUUMRSS-----" << "SUGPUUMRX------" << "SUGPUUMS-------" << "SUGPUUMSE------";
    codes << "SUGPUUMSEA-----" << "SUGPUUMSEC-----" << "SUGPUUMSED-----" << "SUGPUUMSEI-----" << "SUGPUUMSEJ-----" << "SUGPUUMSET-----" << "SUGPUUMT-------" << "SUGPUUP--------" << "SUGPUUS--------" << "SUGPUUSA-------" << "SUGPUUSC-------";
    codes << "SUGPUUSCL------" << "SUGPUUSF-------" << "SUGPUUSM-------" << "SUGPUUSML------" << "SUGPUUSMN------" << "SUGPUUSMS------" << "SUGPUUSO-------" << "SUGPUUSR-------" << "SUGPUUSRS------" << "SUGPUUSRT------" << "SUGPUUSRW------";
    codes << "SUGPUUSS-------" << "SUGPUUSW-------" << "SUGPUUSX-------" << "SUPP-----------" << "SUPP-----------" << "SUPPL----------" << "SUPPS----------" << "SUPPT----------" << "SUPPV----------" << "SUSP-----------" << "SUSPC----------";
    codes << "SUSPCA---------" << "SUSPCALA-------" << "SUSPCALC-------" << "SUSPCALS-------" << "SUSPCALSM------" << "SUSPCALST------" << "SUSPCD---------" << "SUSPCH---------" << "SUSPCL---------" << "SUSPCLBB-------" << "SUSPCLCC-------";
    codes << "SUSPCLCV-------" << "SUSPCLDD-------" << "SUSPCLFF-------" << "SUSPCLLL-------" << "SUSPCLLLAS-----" << "SUSPCLLLMI-----" << "SUSPCLLLSU-----" << "SUSPCM---------" << "SUSPCMMA-------" << "SUSPCMMH-------" << "SUSPCMML-------";
    codes << "SUSPCMMS-------" << "SUSPCP---------" << "SUSPCPSB-------" << "SUSPCPSU-------" << "SUSPCPSUG------" << "SUSPCPSUM------" << "SUSPCPSUT------" << "SUSPCU---------" << "SUSPCUM--------" << "SUSPCUN--------" << "SUSPCUR--------";
    codes << "SUSPCUS--------" << "SUSPG----------" << "SUSPGC---------" << "SUSPGG---------" << "SUSPGT---------" << "SUSPGU---------" << "SUSPN----------" << "SUSPNF---------" << "SUSPNH---------" << "SUSPNI---------" << "SUSPNM---------";
    codes << "SUSPNR---------" << "SUSPNS---------" << "SUSPO----------" << "SUSPXA---------" << "SUSPXAR--------" << "SUSPXAS--------" << "SUSPXF---------" << "SUSPXFDF-------" << "SUSPXFDR-------" << "SUSPXFTR-------" << "SUSPXH---------";
    codes << "SUSPXL---------" << "SUSPXM---------" << "SUSPXMC--------" << "SUSPXMF--------" << "SUSPXMH--------" << "SUSPXMO--------" << "SUSPXMP--------" << "SUSPXMR--------" << "SUSPXMTO-------" << "SUSPXMTU-------" << "SUSPXP---------";
    codes << "SUSPXR---------" << "SUUP-----------" << "SUUPE----------" << "SUUPND---------" << "SUUPS----------" << "SUUPS1---------" << "SUUPS2---------" << "SUUPS3---------" << "SUUPS4---------" << "SUUPSB---------" << "SUUPSC---------";
    codes << "SUUPSCA--------" << "SUUPSCB--------" << "SUUPSCF--------" << "SUUPSCG--------" << "SUUPSCM--------" << "SUUPSF---------" << "SUUPSK---------" << "SUUPSL---------" << "SUUPSN---------" << "SUUPSNA--------" << "SUUPSNB--------";
    codes << "SUUPSNF--------" << "SUUPSNG--------" << "SUUPSNM--------" << "SUUPSO---------" << "SUUPSOF--------" << "SUUPSR---------" << "SUUPSU---------" << "SUUPSUM--------" << "SUUPSUN--------" << "SUUPSUS--------" << "SUUPSX---------";
    codes << "SUUPV----------" << "SUUPW----------" << "SUUPWD---------" << "SUUPWDM--------" << "SUUPWDMG-------" << "SUUPWDMM-------" << "SUUPWM---------" << "SUUPWMA--------" << "SUUPWMB--------" << "SUUPWMBD-------" << "SUUPWMC--------";
    codes << "SUUPWMD--------" << "SUUPWME--------" << "SUUPWMF--------" << "SUUPWMFC-------" << "SUUPWMFD-------" << "SUUPWMFE-------" << "SUUPWMFO-------" << "SUUPWMFR-------" << "SUUPWMFX-------" << "SUUPWMG--------" << "SUUPWMGC-------";
    codes << "SUUPWMGD-------" << "SUUPWMGE-------" << "SUUPWMGO-------" << "SUUPWMGR-------" << "SUUPWMGX-------" << "SUUPWMM--------" << "SUUPWMMC-------" << "SUUPWMMD-------" << "SUUPWMME-------" << "SUUPWMMO-------" << "SUUPWMMR-------";
    codes << "SUUPWMMX-------" << "SUUPWMN--------" << "SUUPWMO--------" << "SUUPWMOD-------" << "SUUPWMR--------" << "SUUPWMS--------" << "SUUPWMSD-------" << "SUUPWMSX-------" << "SUUPWMX--------" << "SUUPWT---------" << "SUUPX----------";
    */
    return codes;
}
