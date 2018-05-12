using Esri.ArcGISRuntime.Data;
using Esri.ArcGISRuntime.Geometry;
using Esri.ArcGISRuntime.Mapping;
using Esri.ArcGISRuntime.Symbology;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using Path = System.IO.Path;
using Geometry = Esri.ArcGISRuntime.Geometry.Geometry;
using System;

namespace DisplayMilitarySymbols
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string m_stylePath { get; }
        private const string FieldName = "sidc";
        private static Map m_map { get; set; }

        public MainWindow()
        {
            // Create relative paths to included data
            var srcPath = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            m_stylePath = Path.Combine(srcPath, @"data\mil2525c_b2.stylx");

            InitializeComponent();

            m_mapView.Loaded += async (s, e) =>
            {
                // Create a map using the navigationVector Basemap
                m_map = new Map(Basemap.CreateNavigationVector());

                // Set map to map view
                m_mapView.Map = m_map;

                // Create the default symbol
                SimpleLineSymbol sls = new SimpleLineSymbol(SimpleLineSymbolStyle.Solid, Colors.Black, 1);
                SimpleMarkerSymbol sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle.Circle, Colors.Red, 12) { Outline = sls };

                // Create the Dictionary and Unique Value Renderer
                DictionarySymbolStyle style = DictionarySymbolStyle.OpenAsync("mil2525c_b2", m_stylePath).Result;
                DictionaryRenderer dRend = new DictionaryRenderer(DictionarySymbolStyle.OpenAsync("mil2525c_b2", m_stylePath).Result);
                UniqueValueRenderer uRend = new UniqueValueRenderer(new[] { FieldName }, null, "MilSym", sms);

                // Create the fields for the Feature Collection table
                List<Field> fields = new List<Field> { new Field(FieldType.Text, FieldName, FieldName, 15) };

                // Create the Feature Collection tables and assign the renderer
                FeatureCollectionTable dTable = new FeatureCollectionTable(fields, GeometryType.Point, SpatialReferences.Wgs84) { Renderer = dRend };
                FeatureCollectionTable uTable = new FeatureCollectionTable(fields, GeometryType.Point, SpatialReferences.Wgs84) { Renderer = uRend };

                // Add the table to a feature collection
                FeatureCollection dCollection = new FeatureCollection(new[] { dTable });
                FeatureCollection uCollection = new FeatureCollection(new[] { uTable });

                // Add the collections to the map
                FeatureCollectionLayer dLayer = new FeatureCollectionLayer(dCollection);
                FeatureCollectionLayer uLayer = new FeatureCollectionLayer(uCollection);
                m_map.OperationalLayers.Add(dLayer);
                m_map.OperationalLayers.Add(uLayer);

                // Get the sic codes
                string[] codes = GenerateSymbolCodes(100);

                // Get the AOI
                MapPoint pnt = GeometryEngine.Project(new MapPoint(-117.1825, 34.0556, SpatialReferences.Wgs84), SpatialReferences.WebMercator) as MapPoint;
                Envelope aoi = GeometryEngine.Project(GeometryEngine.Buffer(pnt, 1000), SpatialReferences.Wgs84).Extent;

                await m_mapView.SetViewpointGeometryAsync(aoi, 20);

                // Get the grid size
                double width = Math.Ceiling(Math.Sqrt(codes.Length));
                double padding = (aoi.XMax - aoi.XMin) / width;

                // Create features within the grid
                int count = 0;
                for (double x = aoi.XMin; x <= aoi.XMax; x += padding)
                {
                    if (count >= codes.Length)
                        break;

                    for (double y = aoi.YMax; y >= aoi.YMin; y -= padding)
                    {
                        if (count >= codes.Length)
                            break;

                        if (codes[count].Length > 15)
                            System.Diagnostics.Debug.WriteLine(codes[count]);

                        var dFeature = dTable.CreateFeature(new Dictionary<string, object> { { FieldName, codes[count] } }, new MapPoint(x, y, SpatialReferences.Wgs84));
                        var uFeature = uTable.CreateFeature(new Dictionary<string, object> { { FieldName, codes[count] } }, new MapPoint(x + (padding * 0.5), y, SpatialReferences.Wgs84));

                        // Add the Feature to the table
                        await dTable.AddFeatureAsync(dFeature);
                        await uTable.AddFeatureAsync(uFeature);

                        //string json = dRend.GetSymbol(dFeature).ToJson();

                        MultilayerPointSymbol symbol = (MultilayerPointSymbol) dRend.GetSymbol(dFeature);
                        symbol.Size *= 2;

                        UniqueValue uval = new UniqueValue(codes[count], codes[count], symbol, codes[count]);
                        uRend.UniqueValues.Add(uval);

                        count++;

                        System.Diagnostics.Debug.WriteLine(string.Format("Processed feature {0} of {1}.", count, codes.Length));
                    }
                }


            };
        }

        private string[] GenerateSymbolCodes(int count = 0, int skip = 0)
        {
            if (count > 0)
                count++;

            string[] codes = new string[]
            {
                "IFAPSCC--------", "IFAPSCO--------", "IFAPSCP--------", "IFAPSCS--------", "IFAPSRAI-------", "IFAPSRAS-------", "IFAPSRC--------", "IFAPSRD--------", "IFAPSRE--------", "IFAPSRF--------", "IFAPSRI--------",
                "IFAPSRMA-------", "IFAPSRMD-------", "IFAPSRMF-------", "IFAPSRMG-------", "IFAPSRMT-------", "IFAPSRTA-------", "IFAPSRTI-------", "IFAPSRTT-------", "IFAPSRU--------", "IFGPSCC--------", "IFGPSCO--------",
                "IFGPSCP--------", "IFGPSCS--------", "IFGPSCT--------", "IFGPSRAA-------", "IFGPSRAT-------", "IFGPSRB--------", "IFGPSRCA-------", "IFGPSRCS-------", "IFGPSRD--------", "IFGPSRE--------", "IFGPSRF--------",
                "IFGPSRH--------", "IFGPSRI--------", "IFGPSRMA-------", "IFGPSRMF-------", "IFGPSRMG-------", "IFGPSRMM-------", "IFGPSRMT-------", "IFGPSRS--------", "IFGPSRTA-------", "IFGPSRTI-------", "IFGPSRTT-------",
                "IFGPSRU--------", "IFPPSCD--------", "IFPPSRD--------", "IFPPSRE--------", "IFPPSRI--------", "IFPPSRM--------", "IFPPSRS--------", "IFPPSRT--------", "IFPPSRU--------", "IFSPSCC--------", "IFSPSCO--------",
                "IFSPSCP--------", "IFSPSCS--------", "IFSPSRAA-------", "IFSPSRAT-------", "IFSPSRCA-------", "IFSPSRCI-------", "IFSPSRD--------", "IFSPSRE--------", "IFSPSRF--------", "IFSPSRH--------", "IFSPSRI--------",
                "IFSPSRMA-------", "IFSPSRMF-------", "IFSPSRMG-------", "IFSPSRMM-------", "IFSPSRMT-------", "IFSPSRS--------", "IFSPSRTA-------", "IFSPSRTI-------", "IFSPSRTT-------", "IFSPSRU--------", "IFUPSCO--------",
                "IFUPSCP--------", "IFUPSCS--------", "IFUPSRD--------", "IFUPSRE--------", "IFUPSRM--------", "IFUPSRS--------", "IFUPSRT--------", "IFUPSRU--------", "IHAPSCC--------", "IHAPSCO--------", "IHAPSCP--------",
                "IHAPSCS--------", "IHAPSRAI-------", "IHAPSRAS-------", "IHAPSRC--------", "IHAPSRD--------", "IHAPSRE--------", "IHAPSRF--------", "IHAPSRI--------", "IHAPSRMA-------", "IHAPSRMD-------", "IHAPSRMF-------",
                "IHAPSRMG-------", "IHAPSRMT-------", "IHAPSRTA-------", "IHAPSRTI-------", "IHAPSRTT-------", "IHAPSRU--------", "IHGPSCC--------", "IHGPSCO--------", "IHGPSCP--------", "IHGPSCS--------", "IHGPSCT--------",
                "IHGPSRAA-------", "IHGPSRAT-------", "IHGPSRB--------", "IHGPSRCA-------", "IHGPSRCS-------", "IHGPSRD--------", "IHGPSRE--------", "IHGPSRF--------", "IHGPSRH--------", "IHGPSRI--------", "IHGPSRMA-------",
                "IHGPSRMF-------", "IHGPSRMG-------", "IHGPSRMM-------", "IHGPSRMT-------", "IHGPSRS--------", "IHGPSRTA-------", "IHGPSRTI-------", "IHGPSRTT-------", "IHGPSRU--------", "IHPPSCD--------", "IHPPSRD--------",
                "IHPPSRE--------", "IHPPSRI--------", "IHPPSRM--------", "IHPPSRS--------", "IHPPSRT--------", "IHPPSRU--------", "IHSPSCC--------", "IHSPSCO--------", "IHSPSCP--------", "IHSPSCS--------", "IHSPSRAA-------",
                "IHSPSRAT-------", "IHSPSRCA-------", "IHSPSRCI-------", "IHSPSRD--------", "IHSPSRE--------", "IHSPSRF--------", "IHSPSRH--------", "IHSPSRI--------", "IHSPSRMA-------", "IHSPSRMF-------", "IHSPSRMG-------",
                "IHSPSRMM-------", "IHSPSRMT-------", "IHSPSRS--------", "IHSPSRTA-------", "IHSPSRTI-------", "IHSPSRTT-------", "IHSPSRU--------", "IHUPSCO--------", "IHUPSCP--------", "IHUPSCS--------", "IHUPSRD--------",
                "IHUPSRE--------", "IHUPSRM--------", "IHUPSRS--------", "IHUPSRT--------", "IHUPSRU--------", "INAPSCC--------", "INAPSCO--------", "INAPSCP--------", "INAPSCS--------", "INAPSRAI-------", "INAPSRAS-------",
                "INAPSRC--------", "INAPSRD--------", "INAPSRE--------", "INAPSRF--------", "INAPSRI--------", "INAPSRMA-------", "INAPSRMD-------", "INAPSRMF-------", "INAPSRMG-------", "INAPSRMT-------", "INAPSRTA-------",
                "INAPSRTI-------", "INAPSRTT-------", "INAPSRU--------", "INGPSCC--------", "INGPSCO--------", "INGPSCP--------", "INGPSCS--------", "INGPSCT--------", "INGPSRAA-------", "INGPSRAT-------", "INGPSRB--------",
                "INGPSRCA-------", "INGPSRCS-------", "INGPSRD--------", "INGPSRE--------", "INGPSRF--------", "INGPSRH--------", "INGPSRI--------", "INGPSRMA-------", "INGPSRMF-------", "INGPSRMG-------", "INGPSRMM-------",
                "INGPSRMT-------", "INGPSRS--------", "INGPSRTA-------", "INGPSRTI-------", "INGPSRTT-------", "INGPSRU--------", "INPPSCD--------", "INPPSRD--------", "INPPSRE--------", "INPPSRI--------", "INPPSRM--------",
                "INPPSRS--------", "INPPSRT--------", "INPPSRU--------", "INSPSCC--------", "INSPSCO--------", "INSPSCP--------", "INSPSCS--------", "INSPSRAA-------", "INSPSRAT-------", "INSPSRCA-------", "INSPSRCI-------",
                "INSPSRD--------", "INSPSRE--------", "INSPSRF--------", "INSPSRH--------", "INSPSRI--------", "INSPSRMA-------", "INSPSRMF-------", "INSPSRMG-------", "INSPSRMM-------", "INSPSRMT-------", "INSPSRS--------",
                "INSPSRTA-------", "INSPSRTI-------", "INSPSRTT-------", "INSPSRU--------", "INUPSCO--------", "INUPSCP--------", "INUPSCS--------", "INUPSRD--------", "INUPSRE--------", "INUPSRM--------", "INUPSRS--------",
                "INUPSRT--------", "INUPSRU--------", "IUAPSCC--------", "IUAPSCO--------", "IUAPSCP--------", "IUAPSCS--------", "IUAPSRAI-------", "IUAPSRAS-------", "IUAPSRC--------", "IUAPSRD--------", "IUAPSRE--------",
                "IUAPSRF--------", "IUAPSRI--------", "IUAPSRMA-------", "IUAPSRMD-------", "IUAPSRMF-------", "IUAPSRMG-------", "IUAPSRMT-------", "IUAPSRTA-------", "IUAPSRTI-------", "IUAPSRTT-------", "IUAPSRU--------",
                "IUGPSCC--------", "IUGPSCO--------", "IUGPSCP--------", "IUGPSCS--------", "IUGPSCT--------", "IUGPSRAA-------", "IUGPSRAT-------", "IUGPSRB--------", "IUGPSRCA-------", "IUGPSRCS-------", "IUGPSRD--------",
                "IUGPSRE--------", "IUGPSRF--------", "IUGPSRH--------", "IUGPSRI--------", "IUGPSRMA-------", "IUGPSRMF-------", "IUGPSRMG-------", "IUGPSRMM-------", "IUGPSRMT-------", "IUGPSRS--------", "IUGPSRTA-------",
                "IUGPSRTI-------", "IUGPSRTT-------", "IUGPSRU--------", "IUPPSCD--------", "IUPPSRD--------", "IUPPSRE--------", "IUPPSRI--------", "IUPPSRM--------", "IUPPSRS--------", "IUPPSRT--------", "IUPPSRU--------",
                "IUSPSCC--------", "IUSPSCO--------", "IUSPSCP--------", "IUSPSCS--------", "IUSPSRAA-------", "IUSPSRAT-------", "IUSPSRCA-------", "IUSPSRCI-------", "IUSPSRD--------", "IUSPSRE--------", "IUSPSRF--------",
                "IUSPSRH--------", "IUSPSRI--------", "IUSPSRMA-------", "IUSPSRMF-------", "IUSPSRMG-------", "IUSPSRMM-------", "IUSPSRMT-------", "IUSPSRS--------", "IUSPSRTA-------", "IUSPSRTI-------", "IUSPSRTT-------",
                "IUSPSRU--------", "IUUPSCO--------", "IUUPSCP--------", "IUUPSCS--------", "IUUPSRD--------", "IUUPSRE--------", "IUUPSRM--------", "IUUPSRS--------", "IUUPSRT--------", "IUUPSRU--------", "SFAP-----------",
                "SFAPC----------", "SFAPCF---------", "SFAPCH---------", "SFAPCL---------", "SFAPM----------", "SFAPME---------", "SFAPMF---------", "SFAPMFA--------", "SFAPMFB--------", "SFAPMFC--------", "SFAPMFCH-------",
                "SFAPMFCL-------", "SFAPMFCM-------", "SFAPMFD--------", "SFAPMFF--------", "SFAPMFFI-------", "SFAPMFH--------", "SFAPMFJ--------", "SFAPMFK--------", "SFAPMFKB-------", "SFAPMFKD-------", "SFAPMFL--------",
                "SFAPMFM--------", "SFAPMFO--------", "SFAPMFP--------", "SFAPMFPM-------", "SFAPMFPN-------", "SFAPMFQ--------", "SFAPMFQA-------", "SFAPMFQB-------", "SFAPMFQC-------", "SFAPMFQD-------", "SFAPMFQF-------",
                "SFAPMFQH-------", "SFAPMFQI-------", "SFAPMFQJ-------", "SFAPMFQK-------", "SFAPMFQL-------", "SFAPMFQM-------", "SFAPMFQN-------", "SFAPMFQO-------", "SFAPMFQP-------", "SFAPMFQR-------", "SFAPMFQRW------",
                "SFAPMFQRX------", "SFAPMFQRZ------", "SFAPMFQS-------", "SFAPMFQT-------", "SFAPMFQU-------", "SFAPMFQY-------", "SFAPMFR--------", "SFAPMFRW-------", "SFAPMFRX-------", "SFAPMFRZ-------", "SFAPMFS--------",
                "SFAPMFT--------", "SFAPMFU--------", "SFAPMFUH-------", "SFAPMFUL-------", "SFAPMFUM-------", "SFAPMFY--------", "SFAPMH---------", "SFAPMHA--------", "SFAPMHC--------", "SFAPMHCH-------", "SFAPMHCL-------",
                "SFAPMHCM-------", "SFAPMHD--------", "SFAPMHH--------", "SFAPMHI--------", "SFAPMHJ--------", "SFAPMHK--------", "SFAPMHM--------", "SFAPMHO--------", "SFAPMHQ--------", "SFAPMHR--------", "SFAPMHS--------",
                "SFAPMHT--------", "SFAPMHU--------", "SFAPMHUH-------", "SFAPMHUL-------", "SFAPMHUM-------", "SFAPML---------", "SFAPMV---------", "SFAPW----------", "SFAPWB---------", "SFAPWD---------", "SFAPWM---------",
                "SFAPWMA--------", "SFAPWMAA-------", "SFAPWMAP-------", "SFAPWMAS-------", "SFAPWMB--------", "SFAPWMCM-------", "SFAPWMS--------", "SFAPWMSA-------", "SFAPWMSB-------", "SFAPWMSS-------", "SFAPWMSU-------",
                "SFAPWMU--------", "SFFP-----------", "SFFPA----------", "SFFPAF---------", "SFFPAFA--------", "SFFPAFK--------", "SFFPAFU--------", "SFFPAFUH-------", "SFFPAFUL-------", "SFFPAFUM-------", "SFFPAH---------",
                "SFFPAHA--------", "SFFPAHH--------", "SFFPAHU--------", "SFFPAHUH-------", "SFFPAHUL-------", "SFFPAHUM-------", "SFFPAV---------", "SFFPB----------", "SFFPG----------", "SFFPGC---------", "SFFPGP---------",
                "SFFPGPA--------", "SFFPGR---------", "SFFPGS---------", "SFFPN----------", "SFFPNB---------", "SFFPNN---------", "SFFPNS---------", "SFFPNU---------", "SFGP-----------", "SFGPE----------", "SFGPES---------",
                "SFGPESE--------", "SFGPESR--------", "SFGPEV---------", "SFGPEVA--------", "SFGPEVAA-------", "SFGPEVAAR------", "SFGPEVAC-------", "SFGPEVAI-------", "SFGPEVAL-------", "SFGPEVAS-------", "SFGPEVAT-------",
                "SFGPEVATH------", "SFGPEVATHR-----", "SFGPEVATL------", "SFGPEVATLR-----", "SFGPEVATM------", "SFGPEVATMR-----", "SFGPEVC--------", "SFGPEVCA-------", "SFGPEVCAH------", "SFGPEVCAL------", "SFGPEVCAM------",
                "SFGPEVCF-------", "SFGPEVCFH------", "SFGPEVCFL------", "SFGPEVCFM------", "SFGPEVCJ-------", "SFGPEVCJH------", "SFGPEVCJL------", "SFGPEVCJM------", "SFGPEVCM-------", "SFGPEVCMH------", "SFGPEVCML------",
                "SFGPEVCMM------", "SFGPEVCO-------", "SFGPEVCOH------", "SFGPEVCOL------", "SFGPEVCOM------", "SFGPEVCT-------", "SFGPEVCTH------", "SFGPEVCTL------", "SFGPEVCTM------", "SFGPEVCU-------", "SFGPEVCUH------",
                "SFGPEVCUL------", "SFGPEVCUM------", "SFGPEVE--------", "SFGPEVEA-------", "SFGPEVEAA------", "SFGPEVEAT------", "SFGPEVEB-------", "SFGPEVEC-------", "SFGPEVED-------", "SFGPEVEDA------", "SFGPEVEE-------",
                "SFGPEVEF-------", "SFGPEVEH-------", "SFGPEVEM-------", "SFGPEVEML------", "SFGPEVEMV------", "SFGPEVER-------", "SFGPEVES-------", "SFGPEVM--------", "SFGPEVS--------", "SFGPEVSC-------", "SFGPEVSP-------",
                "SFGPEVSR-------", "SFGPEVST-------", "SFGPEVSW-------", "SFGPEVT--------", "SFGPEVU--------", "SFGPEVUA-------", "SFGPEVUAA------", "SFGPEVUB-------", "SFGPEVUL-------", "SFGPEVUR-------", "SFGPEVUS-------",
                "SFGPEVUSH------", "SFGPEVUSL------", "SFGPEVUSM------", "SFGPEVUT-------", "SFGPEVUTH------", "SFGPEVUTL------", "SFGPEVUX-------", "SFGPEWA--------", "SFGPEWAH-------", "SFGPEWAL-------", "SFGPEWAM-------",
                "SFGPEWD--------", "SFGPEWDH-------", "SFGPEWDHS------", "SFGPEWDL-------", "SFGPEWDLS------", "SFGPEWDM-------", "SFGPEWDMS------", "SFGPEWG--------", "SFGPEWGH-------", "SFGPEWGL-------", "SFGPEWGM-------",
                "SFGPEWGR-------", "SFGPEWH--------", "SFGPEWHH-------", "SFGPEWHHS------", "SFGPEWHL-------", "SFGPEWHLS------", "SFGPEWHM-------", "SFGPEWHMS------", "SFGPEWM--------", "SFGPEWMA-------", "SFGPEWMAI------",
                "SFGPEWMAIE-----", "SFGPEWMAIR-----", "SFGPEWMAL------", "SFGPEWMALE-----", "SFGPEWMALR-----", "SFGPEWMAS------", "SFGPEWMASE-----", "SFGPEWMASR-----", "SFGPEWMAT------", "SFGPEWMATE-----", "SFGPEWMATR-----",
                "SFGPEWMS-------", "SFGPEWMSI------", "SFGPEWMSL------", "SFGPEWMSS------", "SFGPEWMT-------", "SFGPEWMTH------", "SFGPEWMTL------", "SFGPEWMTM------", "SFGPEWO--------", "SFGPEWOH-------", "SFGPEWOL-------",
                "SFGPEWOM-------", "SFGPEWR--------", "SFGPEWRH-------", "SFGPEWRL-------", "SFGPEWRR-------", "SFGPEWS--------", "SFGPEWSH-------", "SFGPEWSL-------", "SFGPEWSM-------", "SFGPEWT--------", "SFGPEWTH-------",
                "SFGPEWTL-------", "SFGPEWTM-------", "SFGPEWX--------", "SFGPEWXH-------", "SFGPEWXL-------", "SFGPEWXM-------", "SFGPEWZ--------", "SFGPEWZH-------", "SFGPEWZL-------", "SFGPEWZM-------", "SFGPEXF--------",
                "SFGPEXI--------", "SFGPEXL--------", "SFGPEXM--------", "SFGPEXMC-------", "SFGPEXML-------", "SFGPEXN--------", "SFGPI----------", "SFGPIB---------", "SFGPIBA--------", "SFGPIBN--------", "SFGPIE---------",
                "SFGPIG---------", "SFGPIMA--------", "SFGPIMC--------", "SFGPIME--------", "SFGPIMF--------", "SFGPIMFA-------", "SFGPIMFP-------", "SFGPIMFPW------", "SFGPIMFS-------", "SFGPIMG--------", "SFGPIMM--------",
                "SFGPIMN--------", "SFGPIMNB-------", "SFGPIMS--------", "SFGPIMV--------", "SFGPIP---------", "SFGPIPD--------", "SFGPIR---------", "SFGPIRM--------", "SFGPIRN--------", "SFGPIRNB-------", "SFGPIRNC-------",
                "SFGPIRNN-------", "SFGPIRP--------", "SFGPIT---------", "SFGPIU---------", "SFGPIUE--------", "SFGPIUED-------", "SFGPIUEF-------", "SFGPIUEN-------", "SFGPIUP--------", "SFGPIUR--------", "SFGPIUT--------",
                "SFGPIX---------", "SFGPIXH--------", "SFGPU----------", "SFGPUC---------", "SFGPUCA--------", "SFGPUCAA-------", "SFGPUCAAA------", "SFGPUCAAAS-----", "SFGPUCAAAT-----", "SFGPUCAAAW-----", "SFGPUCAAC------",
                "SFGPUCAAD------", "SFGPUCAAL------", "SFGPUCAAM------", "SFGPUCAAO------", "SFGPUCAAOS-----", "SFGPUCAAS------", "SFGPUCAAU------", "SFGPUCAT-------", "SFGPUCATA------", "SFGPUCATH------", "SFGPUCATL------",
                "SFGPUCATM------", "SFGPUCATR------", "SFGPUCATW------", "SFGPUCATWR-----", "SFGPUCAW-------", "SFGPUCAWA------", "SFGPUCAWH------", "SFGPUCAWL------", "SFGPUCAWM------", "SFGPUCAWR------", "SFGPUCAWS------",
                "SFGPUCAWW------", "SFGPUCAWWR-----", "SFGPUCD--------", "SFGPUCDC-------", "SFGPUCDG-------", "SFGPUCDH-------", "SFGPUCDHH------", "SFGPUCDHP------", "SFGPUCDM-------", "SFGPUCDMH------", "SFGPUCDML------",
                "SFGPUCDMLA-----", "SFGPUCDMM------", "SFGPUCDO-------", "SFGPUCDS-------", "SFGPUCDSC------", "SFGPUCDSS------", "SFGPUCDSV------", "SFGPUCDT-------", "SFGPUCE--------", "SFGPUCEC-------", "SFGPUCECA------",
                "SFGPUCECC------", "SFGPUCECH------", "SFGPUCECL------", "SFGPUCECM------", "SFGPUCECO------", "SFGPUCECR------", "SFGPUCECS------", "SFGPUCECT------", "SFGPUCECW------", "SFGPUCEN-------", "SFGPUCENN------",
                "SFGPUCF--------", "SFGPUCFH-------", "SFGPUCFHA------", "SFGPUCFHC------", "SFGPUCFHE------", "SFGPUCFHH------", "SFGPUCFHL------", "SFGPUCFHM------", "SFGPUCFHO------", "SFGPUCFHS------", "SFGPUCFHX------",
                "SFGPUCFM-------", "SFGPUCFML------", "SFGPUCFMS------", "SFGPUCFMT------", "SFGPUCFMTA-----", "SFGPUCFMTC-----", "SFGPUCFMTO-----", "SFGPUCFMTS-----", "SFGPUCFMW------", "SFGPUCFO-------", "SFGPUCFOA------",
                "SFGPUCFOL------", "SFGPUCFOO------", "SFGPUCFOS------", "SFGPUCFR-------", "SFGPUCFRM------", "SFGPUCFRMR-----", "SFGPUCFRMS-----", "SFGPUCFRMT-----", "SFGPUCFRS------", "SFGPUCFRSR-----", "SFGPUCFRSS-----",
                "SFGPUCFRST-----", "SFGPUCFS-------", "SFGPUCFSA------", "SFGPUCFSL------", "SFGPUCFSO------", "SFGPUCFSS------", "SFGPUCFT-------", "SFGPUCFTA------", "SFGPUCFTC------", "SFGPUCFTCD-----", "SFGPUCFTCM-----",
                "SFGPUCFTF------", "SFGPUCFTR------", "SFGPUCFTS------", "SFGPUCI--------", "SFGPUCIA-------", "SFGPUCIC-------", "SFGPUCII-------", "SFGPUCIL-------", "SFGPUCIM-------", "SFGPUCIN-------", "SFGPUCIO-------",
                "SFGPUCIS-------", "SFGPUCIZ-------", "SFGPUCM--------", "SFGPUCMS-------", "SFGPUCMT-------", "SFGPUCR--------", "SFGPUCRA-------", "SFGPUCRC-------", "SFGPUCRH-------", "SFGPUCRL-------", "SFGPUCRO-------",
                "SFGPUCRR-------", "SFGPUCRRD------", "SFGPUCRRF------", "SFGPUCRRL------", "SFGPUCRS-------", "SFGPUCRV-------", "SFGPUCRVA------", "SFGPUCRVG------", "SFGPUCRVM------", "SFGPUCRVO------", "SFGPUCRX-------",
                "SFGPUCS--------", "SFGPUCSA-------", "SFGPUCSG-------", "SFGPUCSGA------", "SFGPUCSGD------", "SFGPUCSGM------", "SFGPUCSM-------", "SFGPUCSR-------", "SFGPUCSW-------", "SFGPUCV--------", "SFGPUCVC-------",
                "SFGPUCVF-------", "SFGPUCVFA------", "SFGPUCVFR------", "SFGPUCVFU------", "SFGPUCVR-------", "SFGPUCVRA------", "SFGPUCVRM------", "SFGPUCVRS------", "SFGPUCVRU------", "SFGPUCVRUC-----", "SFGPUCVRUE-----",
                "SFGPUCVRUH-----", "SFGPUCVRUL-----", "SFGPUCVRUM-----", "SFGPUCVRW------", "SFGPUCVS-------", "SFGPUCVU-------", "SFGPUCVUF------", "SFGPUCVUR------", "SFGPUCVV-------", "SFGPUH---------", "SFGPUS---------",
                "SFGPUSA--------", "SFGPUSAC-------", "SFGPUSAF-------", "SFGPUSAFC------", "SFGPUSAFT------", "SFGPUSAJ-------", "SFGPUSAJC------", "SFGPUSAJT------", "SFGPUSAL-------", "SFGPUSALC------", "SFGPUSALT------",
                "SFGPUSAM-------", "SFGPUSAMC------", "SFGPUSAMT------", "SFGPUSAO-------", "SFGPUSAOC------", "SFGPUSAOT------", "SFGPUSAP-------", "SFGPUSAPB------", "SFGPUSAPBC-----", "SFGPUSAPBT-----", "SFGPUSAPC------",
                "SFGPUSAPM------", "SFGPUSAPMC-----", "SFGPUSAPMT-----", "SFGPUSAPT------", "SFGPUSAQ-------", "SFGPUSAQC------", "SFGPUSAQT------", "SFGPUSAR-------", "SFGPUSARC------", "SFGPUSART------", "SFGPUSAS-------",
                "SFGPUSASC------", "SFGPUSAST------", "SFGPUSAT-------", "SFGPUSAW-------", "SFGPUSAWC------", "SFGPUSAWT------", "SFGPUSAX-------", "SFGPUSAXC------", "SFGPUSAXT------", "SFGPUSM--------", "SFGPUSMC-------",
                "SFGPUSMD-------", "SFGPUSMDC------", "SFGPUSMDT------", "SFGPUSMM-------", "SFGPUSMMC------", "SFGPUSMMT------", "SFGPUSMP-------", "SFGPUSMPC------", "SFGPUSMPT------", "SFGPUSMT-------", "SFGPUSMV-------",
                "SFGPUSMVC------", "SFGPUSMVT------", "SFGPUSS--------", "SFGPUSS1-------", "SFGPUSS1C------", "SFGPUSS1T------", "SFGPUSS2-------", "SFGPUSS2C------", "SFGPUSS2T------", "SFGPUSS3-------", "SFGPUSS3A------",
                "SFGPUSS3AC-----", "SFGPUSS3AT-----", "SFGPUSS3C------", "SFGPUSS3T------", "SFGPUSS4-------", "SFGPUSS4C------", "SFGPUSS4T------", "SFGPUSS5-------", "SFGPUSS5C------", "SFGPUSS5T------", "SFGPUSS6-------",
                "SFGPUSS6C------", "SFGPUSS6T------", "SFGPUSS7-------", "SFGPUSS7C------", "SFGPUSS7T------", "SFGPUSS8-------", "SFGPUSS8C------", "SFGPUSS8T------", "SFGPUSS9-------", "SFGPUSS9C------", "SFGPUSS9T------",
                "SFGPUSSC-------", "SFGPUSSL-------", "SFGPUSSLC------", "SFGPUSSLT------", "SFGPUSST-------", "SFGPUSSW-------", "SFGPUSSWC------", "SFGPUSSWP------", "SFGPUSSWPC-----", "SFGPUSSWPT-----", "SFGPUSSWT------",
                "SFGPUSSX-------", "SFGPUSSXC------", "SFGPUSSXT------", "SFGPUST--------", "SFGPUSTA-------", "SFGPUSTAC------", "SFGPUSTAT------", "SFGPUSTC-------", "SFGPUSTI-------", "SFGPUSTIC------", "SFGPUSTIT------",
                "SFGPUSTM-------", "SFGPUSTMC------", "SFGPUSTMT------", "SFGPUSTR-------", "SFGPUSTRC------", "SFGPUSTRT------", "SFGPUSTS-------", "SFGPUSTSC------", "SFGPUSTST------", "SFGPUSTT-------", "SFGPUSX--------",
                "SFGPUSXC-------", "SFGPUSXE-------", "SFGPUSXEC------", "SFGPUSXET------", "SFGPUSXH-------", "SFGPUSXHC------", "SFGPUSXHT------", "SFGPUSXO-------", "SFGPUSXOC------", "SFGPUSXOM------", "SFGPUSXOMC-----",
                "SFGPUSXOMT-----", "SFGPUSXOT------", "SFGPUSXR-------", "SFGPUSXRC------", "SFGPUSXRT------", "SFGPUSXT-------", "SFGPUU---------", "SFGPUUA--------", "SFGPUUAB-------", "SFGPUUABR------", "SFGPUUAC-------",
                "SFGPUUACC------", "SFGPUUACCK-----", "SFGPUUACCM-----", "SFGPUUACR------", "SFGPUUACRS-----", "SFGPUUACRW-----", "SFGPUUACS------", "SFGPUUACSA-----", "SFGPUUACSM-----", "SFGPUUAD-------", "SFGPUUAN-------",
                "SFGPUUE--------", "SFGPUUI--------", "SFGPUUL--------", "SFGPUULC-------", "SFGPUULD-------", "SFGPUULF-------", "SFGPUULM-------", "SFGPUULS-------", "SFGPUUM--------", "SFGPUUMA-------", "SFGPUUMC-------",
                "SFGPUUMJ-------", "SFGPUUMMO------", "SFGPUUMO-------", "SFGPUUMQ-------", "SFGPUUMR-------", "SFGPUUMRG------", "SFGPUUMRS------", "SFGPUUMRSS-----", "SFGPUUMRX------", "SFGPUUMS-------", "SFGPUUMSE------",
                "SFGPUUMSEA-----", "SFGPUUMSEC-----", "SFGPUUMSED-----", "SFGPUUMSEI-----", "SFGPUUMSEJ-----", "SFGPUUMSET-----", "SFGPUUMT-------", "SFGPUUP--------", "SFGPUUS--------", "SFGPUUSA-------", "SFGPUUSC-------",
                "SFGPUUSCL------", "SFGPUUSF-------", "SFGPUUSM-------", "SFGPUUSML------", "SFGPUUSMN------", "SFGPUUSMS------", "SFGPUUSO-------", "SFGPUUSR-------", "SFGPUUSRS------", "SFGPUUSRT------", "SFGPUUSRW------",
                "SFGPUUSS-------", "SFGPUUSW-------", "SFGPUUSX-------", "SFPP-----------", "SFPPL----------", "SFPPS----------", "SFPPT----------", "SFPPV----------", "SFSP-----------", "SFSPC----------", "SFSPCA---------",
                "SFSPCALA-------", "SFSPCALC-------", "SFSPCALS-------", "SFSPCALSM------", "SFSPCALST------", "SFSPCD---------", "SFSPCH---------", "SFSPCL---------", "SFSPCLBB-------", "SFSPCLCC-------", "SFSPCLCV-------",
                "SFSPCLDD-------", "SFSPCLFF-------", "SFSPCLLL-------", "SFSPCLLLAS-----", "SFSPCLLLMI-----", "SFSPCLLLSU-----", "SFSPCM---------", "SFSPCMMA-------", "SFSPCMMH-------", "SFSPCMML-------", "SFSPCMMS-------",
                "SFSPCP---------", "SFSPCPSB-------", "SFSPCPSU-------", "SFSPCPSUG------", "SFSPCPSUM------", "SFSPCPSUT------", "SFSPCU---------", "SFSPCUM--------", "SFSPCUN--------", "SFSPCUR--------", "SFSPCUS--------",
                "SFSPG----------", "SFSPGC---------", "SFSPGG---------", "SFSPGT---------", "SFSPGU---------", "SFSPN----------", "SFSPNF---------", "SFSPNH---------", "SFSPNI---------", "SFSPNM---------", "SFSPNR---------",
                "SFSPNS---------", "SFSPO----------", "SFSPXA---------", "SFSPXAR--------", "SFSPXAS--------", "SFSPXF---------", "SFSPXFDF-------", "SFSPXFDR-------", "SFSPXFTR-------", "SFSPXH---------", "SFSPXL---------",
                "SFSPXM---------", "SFSPXMC--------", "SFSPXMF--------", "SFSPXMH--------", "SFSPXMO--------", "SFSPXMP--------", "SFSPXMR--------", "SFSPXMTO-------", "SFSPXMTU-------", "SFSPXP---------", "SFSPXR---------",
                "SFUP-----------", "SFUPE----------", "SFUPND---------", "SFUPS----------", "SFUPS1---------", "SFUPS2---------", "SFUPS3---------", "SFUPS4---------", "SFUPSB---------", "SFUPSC---------", "SFUPSCA--------",
                "SFUPSCB--------", "SFUPSCF--------", "SFUPSCG--------", "SFUPSCM--------", "SFUPSF---------", "SFUPSK---------", "SFUPSL---------", "SFUPSN---------", "SFUPSNA--------", "SFUPSNB--------", "SFUPSNF--------",
                "SFUPSNG--------", "SFUPSNM--------", "SFUPSO---------", "SFUPSOF--------", "SFUPSR---------", "SFUPSU---------", "SFUPSUM--------", "SFUPSUN--------", "SFUPSUS--------", "SFUPSX---------", "SFUPV----------",
                "SFUPW----------", "SFUPWD---------", "SFUPWDM--------", "SFUPWDMG-------", "SFUPWDMM-------", "SFUPWM---------", "SFUPWMA--------", "SFUPWMB--------", "SFUPWMBD-------", "SFUPWMC--------", "SFUPWMD--------",
                "SFUPWME--------", "SFUPWMF--------", "SFUPWMFC-------", "SFUPWMFD-------", "SFUPWMFE-------", "SFUPWMFO-------", "SFUPWMFR-------", "SFUPWMFX-------", "SFUPWMG--------", "SFUPWMGC-------", "SFUPWMGD-------",
                "SFUPWMGE-------", "SFUPWMGO-------", "SFUPWMGR-------", "SFUPWMGX-------", "SFUPWMM--------", "SFUPWMMC-------", "SFUPWMMD-------", "SFUPWMME-------", "SFUPWMMO-------", "SFUPWMMR-------", "SFUPWMMX-------",
                "SFUPWMN--------", "SFUPWMO--------", "SFUPWMOD-------", "SFUPWMR--------", "SFUPWMS--------", "SFUPWMSD-------", "SFUPWMSX-------", "SFUPWMX--------", "SFUPWT---------", "SFUPX----------", "SHAP-----------",
                "SHAPC----------", "SHAPCF---------", "SHAPCH---------", "SHAPCL---------", "SHAPM----------", "SHAPME---------", "SHAPMF---------", "SHAPMFA--------", "SHAPMFB--------", "SHAPMFC--------", "SHAPMFCH-------",
                "SHAPMFCL-------", "SHAPMFCM-------", "SHAPMFD--------", "SHAPMFF--------", "SHAPMFFI-------", "SHAPMFH--------", "SHAPMFJ--------", "SHAPMFK--------", "SHAPMFKB-------", "SHAPMFKD-------", "SHAPMFL--------",
                "SHAPMFM--------", "SHAPMFO--------", "SHAPMFP--------", "SHAPMFPM-------", "SHAPMFPN-------", "SHAPMFQ--------", "SHAPMFQA-------", "SHAPMFQB-------", "SHAPMFQC-------", "SHAPMFQD-------", "SHAPMFQF-------",
                "SHAPMFQH-------", "SHAPMFQI-------", "SHAPMFQJ-------", "SHAPMFQK-------", "SHAPMFQL-------", "SHAPMFQM-------", "SHAPMFQN-------", "SHAPMFQO-------", "SHAPMFQP-------", "SHAPMFQR-------", "SHAPMFQRW------",
                "SHAPMFQRX------", "SHAPMFQRZ------", "SHAPMFQS-------", "SHAPMFQT-------", "SHAPMFQU-------", "SHAPMFQY-------", "SHAPMFR--------", "SHAPMFRW-------", "SHAPMFRX-------", "SHAPMFRZ-------", "SHAPMFS--------",
                "SHAPMFT--------", "SHAPMFU--------", "SHAPMFUH-------", "SHAPMFUL-------", "SHAPMFUM-------", "SHAPMFY--------", "SHAPMH---------", "SHAPMHA--------", "SHAPMHC--------", "SHAPMHCH-------", "SHAPMHCL-------",
                "SHAPMHCM-------", "SHAPMHD--------", "SHAPMHH--------", "SHAPMHI--------", "SHAPMHJ--------", "SHAPMHK--------", "SHAPMHM--------", "SHAPMHO--------", "SHAPMHQ--------", "SHAPMHR--------", "SHAPMHS--------",
                "SHAPMHT--------", "SHAPMHU--------", "SHAPMHUH-------", "SHAPMHUL-------", "SHAPMHUM-------", "SHAPML---------", "SHAPMV---------", "SHAPW----------", "SHAPWB---------", "SHAPWD---------", "SHAPWM---------",
                "SHAPWMA--------", "SHAPWMAA-------", "SHAPWMAP-------", "SHAPWMAS-------", "SHAPWMB--------", "SHAPWMCM-------", "SHAPWMS--------", "SHAPWMSA-------", "SHAPWMSB-------", "SHAPWMSS-------", "SHAPWMSU-------",
                "SHAPWMU--------", "SHFP-----------", "SHFPA----------", "SHFPAF---------", "SHFPAFA--------", "SHFPAFK--------", "SHFPAFU--------", "SHFPAFUH-------", "SHFPAFUL-------", "SHFPAFUM-------", "SHFPAH---------",
                "SHFPAHA--------", "SHFPAHH--------", "SHFPAHU--------", "SHFPAHUH-------", "SHFPAHUL-------", "SHFPAHUM-------", "SHFPAV---------", "SHFPB----------", "SHFPG----------", "SHFPGC---------", "SHFPGP---------",
                "SHFPGPA--------", "SHFPGR---------", "SHFPGS---------", "SHFPN----------", "SHFPNB---------", "SHFPNN---------", "SHFPNS---------", "SHFPNU---------", "SHGP-----------", "SHGPE----------", "SHGPES---------",
                "SHGPESE--------", "SHGPESR--------", "SHGPEV---------", "SHGPEVA--------", "SHGPEVAA-------", "SHGPEVAAR------", "SHGPEVAC-------", "SHGPEVAI-------", "SHGPEVAL-------", "SHGPEVAS-------", "SHGPEVAT-------",
                "SHGPEVATH------", "SHGPEVATHR-----", "SHGPEVATL------", "SHGPEVATLR-----", "SHGPEVATM------", "SHGPEVATMR-----", "SHGPEVC--------", "SHGPEVCA-------", "SHGPEVCAH------", "SHGPEVCAL------", "SHGPEVCAM------",
                "SHGPEVCF-------", "SHGPEVCFH------", "SHGPEVCFL------", "SHGPEVCFM------", "SHGPEVCJ-------", "SHGPEVCJH------", "SHGPEVCJL------", "SHGPEVCJM------", "SHGPEVCM-------", "SHGPEVCMH------", "SHGPEVCML------",
                "SHGPEVCMM------", "SHGPEVCO-------", "SHGPEVCOH------", "SHGPEVCOL------", "SHGPEVCOM------", "SHGPEVCT-------", "SHGPEVCTH------", "SHGPEVCTL------", "SHGPEVCTM------", "SHGPEVCU-------", "SHGPEVCUH------",
                "SHGPEVCUL------", "SHGPEVCUM------", "SHGPEVE--------", "SHGPEVEA-------", "SHGPEVEAA------", "SHGPEVEAT------", "SHGPEVEB-------", "SHGPEVEC-------", "SHGPEVED-------", "SHGPEVEDA------", "SHGPEVEE-------",
                "SHGPEVEF-------", "SHGPEVEH-------", "SHGPEVEM-------", "SHGPEVEML------", "SHGPEVEMV------", "SHGPEVER-------", "SHGPEVES-------", "SHGPEVM--------", "SHGPEVS--------", "SHGPEVSC-------", "SHGPEVSP-------",
                "SHGPEVSR-------", "SHGPEVST-------", "SHGPEVSW-------", "SHGPEVT--------", "SHGPEVU--------", "SHGPEVUA-------", "SHGPEVUAA------", "SHGPEVUB-------", "SHGPEVUL-------", "SHGPEVUR-------", "SHGPEVUS-------",
                "SHGPEVUSH------", "SHGPEVUSL------", "SHGPEVUSM------", "SHGPEVUT-------", "SHGPEVUTH------", "SHGPEVUTL------", "SHGPEVUX-------", "SHGPEWA--------", "SHGPEWAH-------", "SHGPEWAL-------", "SHGPEWAM-------",
                "SHGPEWD--------", "SHGPEWDH-------", "SHGPEWDHS------", "SHGPEWDL-------", "SHGPEWDLS------", "SHGPEWDM-------", "SHGPEWDMS------", "SHGPEWG--------", "SHGPEWGH-------", "SHGPEWGL-------", "SHGPEWGM-------",
                "SHGPEWGR-------", "SHGPEWH--------", "SHGPEWHH-------", "SHGPEWHHS------", "SHGPEWHL-------", "SHGPEWHLS------", "SHGPEWHM-------", "SHGPEWHMS------", "SHGPEWM--------", "SHGPEWMA-------", "SHGPEWMAI------",
                "SHGPEWMAIE-----", "SHGPEWMAIR-----", "SHGPEWMAL------", "SHGPEWMALE-----", "SHGPEWMALR-----", "SHGPEWMAS------", "SHGPEWMASE-----", "SHGPEWMASR-----", "SHGPEWMAT------", "SHGPEWMATE-----", "SHGPEWMATR-----",
                "SHGPEWMS-------", "SHGPEWMSI------", "SHGPEWMSL------", "SHGPEWMSS------", "SHGPEWMT-------", "SHGPEWMTH------", "SHGPEWMTL------", "SHGPEWMTM------", "SHGPEWO--------", "SHGPEWOH-------", "SHGPEWOL-------",
                "SHGPEWOM-------", "SHGPEWR--------", "SHGPEWRH-------", "SHGPEWRL-------", "SHGPEWRR-------", "SHGPEWS--------", "SHGPEWSH-------", "SHGPEWSL-------", "SHGPEWSM-------", "SHGPEWT--------", "SHGPEWTH-------",
                "SHGPEWTL-------", "SHGPEWTM-------", "SHGPEWX--------", "SHGPEWXH-------", "SHGPEWXL-------", "SHGPEWXM-------", "SHGPEWZ--------", "SHGPEWZH-------", "SHGPEWZL-------", "SHGPEWZM-------", "SHGPEXF--------",
                "SHGPEXI--------", "SHGPEXL--------", "SHGPEXM--------", "SHGPEXMC-------", "SHGPEXML-------", "SHGPEXN--------", "SHGPI----------", "SHGPIB---------", "SHGPIBA--------", "SHGPIBN--------", "SHGPIE---------",
                "SHGPIG---------", "SHGPIMA--------", "SHGPIMC--------", "SHGPIME--------", "SHGPIMF--------", "SHGPIMFA-------", "SHGPIMFP-------", "SHGPIMFPW------", "SHGPIMFS-------", "SHGPIMG--------", "SHGPIMM--------",
                "SHGPIMN--------", "SHGPIMNB-------", "SHGPIMS--------", "SHGPIMV--------", "SHGPIP---------", "SHGPIPD--------", "SHGPIR---------", "SHGPIRM--------", "SHGPIRN--------", "SHGPIRNB-------", "SHGPIRNC-------",
                "SHGPIRNN-------", "SHGPIRP--------", "SHGPIT---------", "SHGPIU---------", "SHGPIUE--------", "SHGPIUED-------", "SHGPIUEF-------", "SHGPIUEN-------", "SHGPIUP--------", "SHGPIUR--------", "SHGPIUT--------",
                "SHGPIX---------", "SHGPIXH--------", "SHGPU----------", "SHGPUC---------", "SHGPUCA--------", "SHGPUCAA-------", "SHGPUCAAA------", "SHGPUCAAAS-----", "SHGPUCAAAT-----", "SHGPUCAAAW-----", "SHGPUCAAC------",
                "SHGPUCAAD------", "SHGPUCAAL------", "SHGPUCAAM------", "SHGPUCAAO------", "SHGPUCAAOS-----", "SHGPUCAAS------", "SHGPUCAAU------", "SHGPUCAT-------", "SHGPUCATA------", "SHGPUCATH------", "SHGPUCATL------",
                "SHGPUCATM------", "SHGPUCATR------", "SHGPUCATW------", "SHGPUCATWR-----", "SHGPUCAW-------", "SHGPUCAWA------", "SHGPUCAWH------", "SHGPUCAWL------", "SHGPUCAWM------", "SHGPUCAWR------", "SHGPUCAWS------",
                "SHGPUCAWW------", "SHGPUCAWWR-----", "SHGPUCD--------", "SHGPUCDC-------", "SHGPUCDG-------", "SHGPUCDH-------", "SHGPUCDHH------", "SHGPUCDHP------", "SHGPUCDM-------", "SHGPUCDMH------", "SHGPUCDML------",
                "SHGPUCDMLA-----", "SHGPUCDMM------", "SHGPUCDO-------", "SHGPUCDS-------", "SHGPUCDSC------", "SHGPUCDSS------", "SHGPUCDSV------", "SHGPUCDT-------", "SHGPUCE--------", "SHGPUCEC-------", "SHGPUCECA------",
                "SHGPUCECC------", "SHGPUCECH------", "SHGPUCECL------", "SHGPUCECM------", "SHGPUCECO------", "SHGPUCECR------", "SHGPUCECS------", "SHGPUCECT------", "SHGPUCECW------", "SHGPUCEN-------", "SHGPUCENN------",
                "SHGPUCF--------", "SHGPUCFH-------", "SHGPUCFHA------", "SHGPUCFHC------", "SHGPUCFHE------", "SHGPUCFHH------", "SHGPUCFHL------", "SHGPUCFHM------", "SHGPUCFHO------", "SHGPUCFHS------", "SHGPUCFHX------",
                "SHGPUCFM-------", "SHGPUCFML------", "SHGPUCFMS------", "SHGPUCFMT------", "SHGPUCFMTA-----", "SHGPUCFMTC-----", "SHGPUCFMTO-----", "SHGPUCFMTS-----", "SHGPUCFMW------", "SHGPUCFO-------", "SHGPUCFOA------",
                "SHGPUCFOL------", "SHGPUCFOO------", "SHGPUCFOS------", "SHGPUCFR-------", "SHGPUCFRM------", "SHGPUCFRMR-----", "SHGPUCFRMS-----", "SHGPUCFRMT-----", "SHGPUCFRS------", "SHGPUCFRSR-----", "SHGPUCFRSS-----",
                "SHGPUCFRST-----", "SHGPUCFS-------", "SHGPUCFSA------", "SHGPUCFSL------", "SHGPUCFSO------", "SHGPUCFSS------", "SHGPUCFT-------", "SHGPUCFTA------", "SHGPUCFTC------", "SHGPUCFTCD-----", "SHGPUCFTCM-----",
                "SHGPUCFTF------", "SHGPUCFTR------", "SHGPUCFTS------", "SHGPUCI--------", "SHGPUCIA-------", "SHGPUCIC-------", "SHGPUCII-------", "SHGPUCIL-------", "SHGPUCIM-------", "SHGPUCIN-------", "SHGPUCIO-------",
                "SHGPUCIS-------", "SHGPUCIZ-------", "SHGPUCM--------", "SHGPUCMS-------", "SHGPUCMT-------", "SHGPUCR--------", "SHGPUCRA-------", "SHGPUCRC-------", "SHGPUCRH-------", "SHGPUCRL-------", "SHGPUCRO-------",
                "SHGPUCRR-------", "SHGPUCRRD------", "SHGPUCRRF------", "SHGPUCRRL------", "SHGPUCRS-------", "SHGPUCRV-------", "SHGPUCRVA------", "SHGPUCRVG------", "SHGPUCRVM------", "SHGPUCRVO------", "SHGPUCRX-------",
                "SHGPUCS--------", "SHGPUCSA-------", "SHGPUCSG-------", "SHGPUCSGA------", "SHGPUCSGD------", "SHGPUCSGM------", "SHGPUCSM-------", "SHGPUCSR-------", "SHGPUCSW-------", "SHGPUCV--------", "SHGPUCVC-------",
                "SHGPUCVF-------", "SHGPUCVFA------", "SHGPUCVFR------", "SHGPUCVFU------", "SHGPUCVR-------", "SHGPUCVRA------", "SHGPUCVRM------", "SHGPUCVRS------", "SHGPUCVRU------", "SHGPUCVRUC-----", "SHGPUCVRUE-----",
                "SHGPUCVRUH-----", "SHGPUCVRUL-----", "SHGPUCVRUM-----", "SHGPUCVRW------", "SHGPUCVS-------", "SHGPUCVU-------", "SHGPUCVUF------", "SHGPUCVUR------", "SHGPUCVV-------", "SHGPUH---------", "SHGPUS---------",
                "SHGPUSA--------", "SHGPUSAC-------", "SHGPUSAF-------", "SHGPUSAFC------", "SHGPUSAFT------", "SHGPUSAJ-------", "SHGPUSAJC------", "SHGPUSAJT------", "SHGPUSAL-------", "SHGPUSALC------", "SHGPUSALT------",
                "SHGPUSAM-------", "SHGPUSAMC------", "SHGPUSAMT------", "SHGPUSAO-------", "SHGPUSAOC------", "SHGPUSAOT------", "SHGPUSAP-------", "SHGPUSAPB------", "SHGPUSAPBC-----", "SHGPUSAPBT-----", "SHGPUSAPC------",
                "SHGPUSAPM------", "SHGPUSAPMC-----", "SHGPUSAPMT-----", "SHGPUSAPT------", "SHGPUSAQ-------", "SHGPUSAQC------", "SHGPUSAQT------", "SHGPUSAR-------", "SHGPUSARC------", "SHGPUSART------", "SHGPUSAS-------",
                "SHGPUSASC------", "SHGPUSAST------", "SHGPUSAT-------", "SHGPUSAW-------", "SHGPUSAWC------", "SHGPUSAWT------", "SHGPUSAX-------", "SHGPUSAXC------", "SHGPUSAXT------", "SHGPUSM--------", "SHGPUSMC-------",
                "SHGPUSMD-------", "SHGPUSMDC------", "SHGPUSMDT------", "SHGPUSMM-------", "SHGPUSMMC------", "SHGPUSMMT------", "SHGPUSMP-------", "SHGPUSMPC------", "SHGPUSMPT------", "SHGPUSMT-------", "SHGPUSMV-------",
                "SHGPUSMVC------", "SHGPUSMVT------", "SHGPUSS--------", "SHGPUSS1-------", "SHGPUSS1C------", "SHGPUSS1T------", "SHGPUSS2-------", "SHGPUSS2C------", "SHGPUSS2T------", "SHGPUSS3-------", "SHGPUSS3A------",
                "SHGPUSS3AC-----", "SHGPUSS3AT-----", "SHGPUSS3C------", "SHGPUSS3T------", "SHGPUSS4-------", "SHGPUSS4C------", "SHGPUSS4T------", "SHGPUSS5-------", "SHGPUSS5C------", "SHGPUSS5T------", "SHGPUSS6-------",
                "SHGPUSS6C------", "SHGPUSS6T------", "SHGPUSS7-------", "SHGPUSS7C------", "SHGPUSS7T------", "SHGPUSS8-------", "SHGPUSS8C------", "SHGPUSS8T------", "SHGPUSS9-------", "SHGPUSS9C------", "SHGPUSS9T------",
                "SHGPUSSC-------", "SHGPUSSL-------", "SHGPUSSLC------", "SHGPUSSLT------", "SHGPUSST-------", "SHGPUSSW-------", "SHGPUSSWC------", "SHGPUSSWP------", "SHGPUSSWPC-----", "SHGPUSSWPT-----", "SHGPUSSWT------",
                "SHGPUSSX-------", "SHGPUSSXC------", "SHGPUSSXT------", "SHGPUST--------", "SHGPUSTA-------", "SHGPUSTAC------", "SHGPUSTAT------", "SHGPUSTC-------", "SHGPUSTI-------", "SHGPUSTIC------", "SHGPUSTIT------",
                "SHGPUSTM-------", "SHGPUSTMC------", "SHGPUSTMT------", "SHGPUSTR-------", "SHGPUSTRC------", "SHGPUSTRT------", "SHGPUSTS-------", "SHGPUSTSC------", "SHGPUSTST------", "SHGPUSTT-------", "SHGPUSX--------",
                "SHGPUSXC-------", "SHGPUSXE-------", "SHGPUSXEC------", "SHGPUSXET------", "SHGPUSXH-------", "SHGPUSXHC------", "SHGPUSXHT------", "SHGPUSXO-------", "SHGPUSXOC------", "SHGPUSXOM------", "SHGPUSXOMC-----",
                "SHGPUSXOMT-----", "SHGPUSXOT------", "SHGPUSXR-------", "SHGPUSXRC------", "SHGPUSXRT------", "SHGPUSXT-------", "SHGPUU---------", "SHGPUUA--------", "SHGPUUAB-------", "SHGPUUABR------", "SHGPUUAC-------",
                "SHGPUUACC------", "SHGPUUACCK-----", "SHGPUUACCM-----", "SHGPUUACR------", "SHGPUUACRS-----", "SHGPUUACRW-----", "SHGPUUACS------", "SHGPUUACSA-----", "SHGPUUACSM-----", "SHGPUUAD-------", "SHGPUUAN-------",
                "SHGPUUE--------", "SHGPUUI--------", "SHGPUUL--------", "SHGPUULC-------", "SHGPUULD-------", "SHGPUULF-------", "SHGPUULM-------", "SHGPUULS-------", "SHGPUUM--------", "SHGPUUMA-------", "SHGPUUMC-------",
                "SHGPUUMJ-------", "SHGPUUMMO------", "SHGPUUMO-------", "SHGPUUMQ-------", "SHGPUUMR-------", "SHGPUUMRG------", "SHGPUUMRS------", "SHGPUUMRSS-----", "SHGPUUMRX------", "SHGPUUMS-------", "SHGPUUMSE------",
                "SHGPUUMSEA-----", "SHGPUUMSEC-----", "SHGPUUMSED-----", "SHGPUUMSEI-----", "SHGPUUMSEJ-----", "SHGPUUMSET-----", "SHGPUUMT-------", "SHGPUUP--------", "SHGPUUS--------", "SHGPUUSA-------", "SHGPUUSC-------",
                "SHGPUUSCL------", "SHGPUUSF-------", "SHGPUUSM-------", "SHGPUUSML------", "SHGPUUSMN------", "SHGPUUSMS------", "SHGPUUSO-------", "SHGPUUSR-------", "SHGPUUSRS------", "SHGPUUSRT------", "SHGPUUSRW------",
                "SHGPUUSS-------", "SHGPUUSW-------", "SHGPUUSX-------", "SHPP-----------", "SHPPL----------", "SHPPS----------", "SHPPT----------", "SHPPV----------", "SHSP-----------", "SHSPC----------", "SHSPCA---------",
                "SHSPCALA-------", "SHSPCALC-------", "SHSPCALS-------", "SHSPCALSM------", "SHSPCALST------", "SHSPCD---------", "SHSPCH---------", "SHSPCL---------", "SHSPCLBB-------", "SHSPCLCC-------", "SHSPCLCV-------",
                "SHSPCLDD-------", "SHSPCLFF-------", "SHSPCLLL-------", "SHSPCLLLAS-----", "SHSPCLLLMI-----", "SHSPCLLLSU-----", "SHSPCM---------", "SHSPCMMA-------", "SHSPCMMH-------", "SHSPCMML-------", "SHSPCMMS-------",
                "SHSPCP---------", "SHSPCPSB-------", "SHSPCPSU-------", "SHSPCPSUG------", "SHSPCPSUM------", "SHSPCPSUT------", "SHSPCU---------", "SHSPCUM--------", "SHSPCUN--------", "SHSPCUR--------", "SHSPCUS--------",
                "SHSPG----------", "SHSPGC---------", "SHSPGG---------", "SHSPGT---------", "SHSPGU---------", "SHSPN----------", "SHSPNF---------", "SHSPNH---------", "SHSPNI---------", "SHSPNM---------", "SHSPNR---------",
                "SHSPNS---------", "SHSPO----------", "SHSPXA---------", "SHSPXAR--------", "SHSPXAS--------", "SHSPXF---------", "SHSPXFDF-------", "SHSPXFDR-------", "SHSPXFTR-------", "SHSPXH---------", "SHSPXL---------",
                "SHSPXM---------", "SHSPXMC--------", "SHSPXMF--------", "SHSPXMH--------", "SHSPXMO--------", "SHSPXMP--------", "SHSPXMR--------", "SHSPXMTO-------", "SHSPXMTU-------", "SHSPXP---------", "SHSPXR---------",
                "SHUP-----------", "SHUPE----------", "SHUPND---------", "SHUPS----------", "SHUPS1---------", "SHUPS2---------", "SHUPS3---------", "SHUPS4---------", "SHUPSB---------", "SHUPSC---------", "SHUPSCA--------",
                "SHUPSCB--------", "SHUPSCF--------", "SHUPSCG--------", "SHUPSCM--------", "SHUPSF---------", "SHUPSK---------", "SHUPSL---------", "SHUPSN---------", "SHUPSNA--------", "SHUPSNB--------", "SHUPSNF--------",
                "SHUPSNG--------", "SHUPSNM--------", "SHUPSO---------", "SHUPSOF--------", "SHUPSR---------", "SHUPSU---------", "SHUPSUM--------", "SHUPSUN--------", "SHUPSUS--------", "SHUPSX---------", "SHUPV----------",
                "SHUPW----------", "SHUPWD---------", "SHUPWDM--------", "SHUPWDMG-------", "SHUPWDMM-------", "SHUPWM---------", "SHUPWMA--------", "SHUPWMB--------", "SHUPWMBD-------", "SHUPWMC--------", "SHUPWMD--------",
                "SHUPWME--------", "SHUPWMF--------", "SHUPWMFC-------", "SHUPWMFD-------", "SHUPWMFE-------", "SHUPWMFO-------", "SHUPWMFR-------", "SHUPWMFX-------", "SHUPWMG--------", "SHUPWMGC-------", "SHUPWMGD-------",
                "SHUPWMGE-------", "SHUPWMGO-------", "SHUPWMGR-------", "SHUPWMGX-------", "SHUPWMM--------", "SHUPWMMC-------", "SHUPWMMD-------", "SHUPWMME-------", "SHUPWMMO-------", "SHUPWMMR-------", "SHUPWMMX-------",
                "SHUPWMN--------", "SHUPWMO--------", "SHUPWMOD-------", "SHUPWMR--------", "SHUPWMS--------", "SHUPWMSD-------", "SHUPWMSX-------", "SHUPWMX--------", "SHUPWT---------", "SHUPX----------", "SNAP-----------",
                "SNAPC----------", "SNAPCF---------", "SNAPCH---------", "SNAPCL---------", "SNAPM----------", "SNAPME---------", "SNAPMF---------", "SNAPMFA--------", "SNAPMFB--------", "SNAPMFC--------", "SNAPMFCH-------",
                "SNAPMFCL-------", "SNAPMFCM-------", "SNAPMFD--------", "SNAPMFF--------", "SNAPMFFI-------", "SNAPMFH--------", "SNAPMFJ--------", "SNAPMFK--------", "SNAPMFKB-------", "SNAPMFKD-------", "SNAPMFL--------",
                "SNAPMFM--------", "SNAPMFO--------", "SNAPMFP--------", "SNAPMFPM-------", "SNAPMFPN-------", "SNAPMFQ--------", "SNAPMFQA-------", "SNAPMFQB-------", "SNAPMFQC-------", "SNAPMFQD-------", "SNAPMFQF-------",
                "SNAPMFQH-------", "SNAPMFQI-------", "SNAPMFQJ-------", "SNAPMFQK-------", "SNAPMFQL-------", "SNAPMFQM-------", "SNAPMFQN-------", "SNAPMFQO-------", "SNAPMFQP-------", "SNAPMFQR-------", "SNAPMFQRW------",
                "SNAPMFQRX------", "SNAPMFQRZ------", "SNAPMFQS-------", "SNAPMFQT-------", "SNAPMFQU-------", "SNAPMFQY-------", "SNAPMFR--------", "SNAPMFRW-------", "SNAPMFRX-------", "SNAPMFRZ-------", "SNAPMFS--------",
                "SNAPMFT--------", "SNAPMFU--------", "SNAPMFUH-------", "SNAPMFUL-------", "SNAPMFUM-------", "SNAPMFY--------", "SNAPMH---------", "SNAPMHA--------", "SNAPMHC--------", "SNAPMHCH-------", "SNAPMHCL-------",
                "SNAPMHCM-------", "SNAPMHD--------", "SNAPMHH--------", "SNAPMHI--------", "SNAPMHJ--------", "SNAPMHK--------", "SNAPMHM--------", "SNAPMHO--------", "SNAPMHQ--------", "SNAPMHR--------", "SNAPMHS--------",
                "SNAPMHT--------", "SNAPMHU--------", "SNAPMHUH-------", "SNAPMHUL-------", "SNAPMHUM-------", "SNAPML---------", "SNAPMV---------", "SNAPW----------", "SNAPWB---------", "SNAPWD---------", "SNAPWM---------",
                "SNAPWMA--------", "SNAPWMAA-------", "SNAPWMAP-------", "SNAPWMAS-------", "SNAPWMB--------", "SNAPWMCM-------", "SNAPWMS--------", "SNAPWMSA-------", "SNAPWMSB-------", "SNAPWMSS-------", "SNAPWMSU-------",
                "SNAPWMU--------", "SNFP-----------", "SNFPA----------", "SNFPAF---------", "SNFPAFA--------", "SNFPAFK--------", "SNFPAFU--------", "SNFPAFUH-------", "SNFPAFUL-------", "SNFPAFUM-------", "SNFPAH---------",
                "SNFPAHA--------", "SNFPAHH--------", "SNFPAHU--------", "SNFPAHUH-------", "SNFPAHUL-------", "SNFPAHUM-------", "SNFPAV---------", "SNFPB----------", "SNFPG----------", "SNFPGC---------", "SNFPGP---------",
                "SNFPGPA--------", "SNFPGR---------", "SNFPGS---------", "SNFPN----------", "SNFPNB---------", "SNFPNN---------", "SNFPNS---------", "SNFPNU---------", "SNGP-----------", "SNGPE----------", "SNGPES---------",
                "SNGPESE--------", "SNGPESR--------", "SNGPEV---------", "SNGPEVA--------", "SNGPEVAA-------", "SNGPEVAAR------", "SNGPEVAC-------", "SNGPEVAI-------", "SNGPEVAL-------", "SNGPEVAS-------", "SNGPEVAT-------",
                "SNGPEVATH------", "SNGPEVATHR-----", "SNGPEVATL------", "SNGPEVATLR-----", "SNGPEVATM------", "SNGPEVATMR-----", "SNGPEVC--------", "SNGPEVCA-------", "SNGPEVCAH------", "SNGPEVCAL------", "SNGPEVCAM------",
                "SNGPEVCF-------", "SNGPEVCFH------", "SNGPEVCFL------", "SNGPEVCFM------", "SNGPEVCJ-------", "SNGPEVCJH------", "SNGPEVCJL------", "SNGPEVCJM------", "SNGPEVCM-------", "SNGPEVCMH------", "SNGPEVCML------",
                "SNGPEVCMM------", "SNGPEVCO-------", "SNGPEVCOH------", "SNGPEVCOL------", "SNGPEVCOM------", "SNGPEVCT-------", "SNGPEVCTH------", "SNGPEVCTL------", "SNGPEVCTM------", "SNGPEVCU-------", "SNGPEVCUH------",
                "SNGPEVCUL------", "SNGPEVCUM------", "SNGPEVE--------", "SNGPEVEA-------", "SNGPEVEAA------", "SNGPEVEAT------", "SNGPEVEB-------", "SNGPEVEC-------", "SNGPEVED-------", "SNGPEVEDA------", "SNGPEVEE-------",
                "SNGPEVEF-------", "SNGPEVEH-------", "SNGPEVEM-------", "SNGPEVEML------", "SNGPEVEMV------", "SNGPEVER-------", "SNGPEVES-------", "SNGPEVM--------", "SNGPEVS--------", "SNGPEVSC-------", "SNGPEVSP-------",
                "SNGPEVSR-------", "SNGPEVST-------", "SNGPEVSW-------", "SNGPEVT--------", "SNGPEVU--------", "SNGPEVUA-------", "SNGPEVUAA------", "SNGPEVUB-------", "SNGPEVUL-------", "SNGPEVUR-------", "SNGPEVUS-------",
                "SNGPEVUSH------", "SNGPEVUSL------", "SNGPEVUSM------", "SNGPEVUT-------", "SNGPEVUTH------", "SNGPEVUTL------", "SNGPEVUX-------", "SNGPEWA--------", "SNGPEWAH-------", "SNGPEWAL-------", "SNGPEWAM-------",
                "SNGPEWD--------", "SNGPEWDH-------", "SNGPEWDHS------", "SNGPEWDL-------", "SNGPEWDLS------", "SNGPEWDM-------", "SNGPEWDMS------", "SNGPEWG--------", "SNGPEWGH-------", "SNGPEWGL-------", "SNGPEWGM-------",
                "SNGPEWGR-------", "SNGPEWH--------", "SNGPEWHH-------", "SNGPEWHHS------", "SNGPEWHL-------", "SNGPEWHLS------", "SNGPEWHM-------", "SNGPEWHMS------", "SNGPEWM--------", "SNGPEWMA-------", "SNGPEWMAI------",
                "SNGPEWMAIE-----", "SNGPEWMAIR-----", "SNGPEWMAL------", "SNGPEWMALE-----", "SNGPEWMALR-----", "SNGPEWMAS------", "SNGPEWMASE-----", "SNGPEWMASR-----", "SNGPEWMAT------", "SNGPEWMATE-----", "SNGPEWMATR-----",
                "SNGPEWMS-------", "SNGPEWMSI------", "SNGPEWMSL------", "SNGPEWMSS------", "SNGPEWMT-------", "SNGPEWMTH------", "SNGPEWMTL------", "SNGPEWMTM------", "SNGPEWO--------", "SNGPEWOH-------", "SNGPEWOL-------",
                "SNGPEWOM-------", "SNGPEWR--------", "SNGPEWRH-------", "SNGPEWRL-------", "SNGPEWRR-------", "SNGPEWS--------", "SNGPEWSH-------", "SNGPEWSL-------", "SNGPEWSM-------", "SNGPEWT--------", "SNGPEWTH-------",
                "SNGPEWTL-------", "SNGPEWTM-------", "SNGPEWX--------", "SNGPEWXH-------", "SNGPEWXL-------", "SNGPEWXM-------", "SNGPEWZ--------", "SNGPEWZH-------", "SNGPEWZL-------", "SNGPEWZM-------", "SNGPEXF--------",
                "SNGPEXI--------", "SNGPEXL--------", "SNGPEXM--------", "SNGPEXMC-------", "SNGPEXML-------", "SNGPEXN--------", "SNGPI----------", "SNGPIB---------", "SNGPIBA--------", "SNGPIBN--------", "SNGPIE---------",
                "SNGPIG---------", "SNGPIMA--------", "SNGPIMC--------", "SNGPIME--------", "SNGPIMF--------", "SNGPIMFA-------", "SNGPIMFP-------", "SNGPIMFPW------", "SNGPIMFS-------", "SNGPIMG--------", "SNGPIMM--------",
                "SNGPIMN--------", "SNGPIMNB-------", "SNGPIMS--------", "SNGPIMV--------", "SNGPIP---------", "SNGPIPD--------", "SNGPIR---------", "SNGPIRM--------", "SNGPIRN--------", "SNGPIRNB-------", "SNGPIRNC-------",
                "SNGPIRNN-------", "SNGPIRP--------", "SNGPIT---------", "SNGPIU---------", "SNGPIUE--------", "SNGPIUED-------", "SNGPIUEF-------", "SNGPIUEN-------", "SNGPIUP--------", "SNGPIUR--------", "SNGPIUT--------",
                "SNGPIX---------", "SNGPIXH--------", "SNGPU----------", "SNGPUC---------", "SNGPUCA--------", "SNGPUCAA-------", "SNGPUCAAA------", "SNGPUCAAAS-----", "SNGPUCAAAT-----", "SNGPUCAAAW-----", "SNGPUCAAC------",
                "SNGPUCAAD------", "SNGPUCAAL------", "SNGPUCAAM------", "SNGPUCAAO------", "SNGPUCAAOS-----", "SNGPUCAAS------", "SNGPUCAAU------", "SNGPUCAT-------", "SNGPUCATA------", "SNGPUCATH------", "SNGPUCATL------",
                "SNGPUCATM------", "SNGPUCATR------", "SNGPUCATW------", "SNGPUCATWR-----", "SNGPUCAW-------", "SNGPUCAWA------", "SNGPUCAWH------", "SNGPUCAWL------", "SNGPUCAWM------", "SNGPUCAWR------", "SNGPUCAWS------",
                "SNGPUCAWW------", "SNGPUCAWWR-----", "SNGPUCD--------", "SNGPUCDC-------", "SNGPUCDG-------", "SNGPUCDH-------", "SNGPUCDHH------", "SNGPUCDHP------", "SNGPUCDM-------", "SNGPUCDMH------", "SNGPUCDML------",
                "SNGPUCDMLA-----", "SNGPUCDMM------", "SNGPUCDO-------", "SNGPUCDS-------", "SNGPUCDSC------", "SNGPUCDSS------", "SNGPUCDSV------", "SNGPUCDT-------", "SNGPUCE--------", "SNGPUCEC-------", "SNGPUCECA------",
                "SNGPUCECC------", "SNGPUCECH------", "SNGPUCECL------", "SNGPUCECM------", "SNGPUCECO------", "SNGPUCECR------", "SNGPUCECS------", "SNGPUCECT------", "SNGPUCECW------", "SNGPUCEN-------", "SNGPUCENN------",
                "SNGPUCF--------", "SNGPUCFH-------", "SNGPUCFHA------", "SNGPUCFHC------", "SNGPUCFHE------", "SNGPUCFHH------", "SNGPUCFHL------", "SNGPUCFHM------", "SNGPUCFHO------", "SNGPUCFHS------", "SNGPUCFHX------",
                "SNGPUCFM-------", "SNGPUCFML------", "SNGPUCFMS------", "SNGPUCFMT------", "SNGPUCFMTA-----", "SNGPUCFMTC-----", "SNGPUCFMTO-----", "SNGPUCFMTS-----", "SNGPUCFMW------", "SNGPUCFO-------", "SNGPUCFOA------",
                "SNGPUCFOL------", "SNGPUCFOO------", "SNGPUCFOS------", "SNGPUCFR-------", "SNGPUCFRM------", "SNGPUCFRMR-----", "SNGPUCFRMS-----", "SNGPUCFRMT-----", "SNGPUCFRS------", "SNGPUCFRSR-----", "SNGPUCFRSS-----",
                "SNGPUCFRST-----", "SNGPUCFS-------", "SNGPUCFSA------", "SNGPUCFSL------", "SNGPUCFSO------", "SNGPUCFSS------", "SNGPUCFT-------", "SNGPUCFTA------", "SNGPUCFTC------", "SNGPUCFTCD-----", "SNGPUCFTCM-----",
                "SNGPUCFTF------", "SNGPUCFTR------", "SNGPUCFTS------", "SNGPUCI--------", "SNGPUCIA-------", "SNGPUCIC-------", "SNGPUCII-------", "SNGPUCIL-------", "SNGPUCIM-------", "SNGPUCIN-------", "SNGPUCIO-------",
                "SNGPUCIS-------", "SNGPUCIZ-------", "SNGPUCM--------", "SNGPUCMS-------", "SNGPUCMT-------", "SNGPUCR--------", "SNGPUCRA-------", "SNGPUCRC-------", "SNGPUCRH-------", "SNGPUCRL-------", "SNGPUCRO-------",
                "SNGPUCRR-------", "SNGPUCRRD------", "SNGPUCRRF------", "SNGPUCRRL------", "SNGPUCRS-------", "SNGPUCRV-------", "SNGPUCRVA------", "SNGPUCRVG------", "SNGPUCRVM------", "SNGPUCRVO------", "SNGPUCRX-------",
                "SNGPUCS--------", "SNGPUCSA-------", "SNGPUCSG-------", "SNGPUCSGA------", "SNGPUCSGD------", "SNGPUCSGM------", "SNGPUCSM-------", "SNGPUCSR-------", "SNGPUCSW-------", "SNGPUCV--------", "SNGPUCVC-------",
                "SNGPUCVF-------", "SNGPUCVFA------", "SNGPUCVFR------", "SNGPUCVFU------", "SNGPUCVR-------", "SNGPUCVRA------", "SNGPUCVRM------", "SNGPUCVRS------", "SNGPUCVRU------", "SNGPUCVRUC-----", "SNGPUCVRUE-----",
                "SNGPUCVRUH-----", "SNGPUCVRUL-----", "SNGPUCVRUM-----", "SNGPUCVRW------", "SNGPUCVS-------", "SNGPUCVU-------", "SNGPUCVUF------", "SNGPUCVUR------", "SNGPUCVV-------", "SNGPUH---------", "SNGPUS---------",
                "SNGPUSA--------", "SNGPUSAC-------", "SNGPUSAF-------", "SNGPUSAFC------", "SNGPUSAFT------", "SNGPUSAJ-------", "SNGPUSAJC------", "SNGPUSAJT------", "SNGPUSAL-------", "SNGPUSALC------", "SNGPUSALT------",
                "SNGPUSAM-------", "SNGPUSAMC------", "SNGPUSAMT------", "SNGPUSAO-------", "SNGPUSAOC------", "SNGPUSAOT------", "SNGPUSAP-------", "SNGPUSAPB------", "SNGPUSAPBC-----", "SNGPUSAPBT-----", "SNGPUSAPC------",
                "SNGPUSAPM------", "SNGPUSAPMC-----", "SNGPUSAPMT-----", "SNGPUSAPT------", "SNGPUSAQ-------", "SNGPUSAQC------", "SNGPUSAQT------", "SNGPUSAR-------", "SNGPUSARC------", "SNGPUSART------", "SNGPUSAS-------",
                "SNGPUSASC------", "SNGPUSAST------", "SNGPUSAT-------", "SNGPUSAW-------", "SNGPUSAWC------", "SNGPUSAWT------", "SNGPUSAX-------", "SNGPUSAXC------", "SNGPUSAXT------", "SNGPUSM--------", "SNGPUSMC-------",
                "SNGPUSMD-------", "SNGPUSMDC------", "SNGPUSMDT------", "SNGPUSMM-------", "SNGPUSMMC------", "SNGPUSMMT------", "SNGPUSMP-------", "SNGPUSMPC------", "SNGPUSMPT------", "SNGPUSMT-------", "SNGPUSMV-------",
                "SNGPUSMVC------", "SNGPUSMVT------", "SNGPUSS--------", "SNGPUSS1-------", "SNGPUSS1C------", "SNGPUSS1T------", "SNGPUSS2-------", "SNGPUSS2C------", "SNGPUSS2T------", "SNGPUSS3-------", "SNGPUSS3A------",
                "SNGPUSS3AC-----", "SNGPUSS3AT-----", "SNGPUSS3C------", "SNGPUSS3T------", "SNGPUSS4-------", "SNGPUSS4C------", "SNGPUSS4T------", "SNGPUSS5-------", "SNGPUSS5C------", "SNGPUSS5T------", "SNGPUSS6-------",
                "SNGPUSS6C------", "SNGPUSS6T------", "SNGPUSS7-------", "SNGPUSS7C------", "SNGPUSS7T------", "SNGPUSS8-------", "SNGPUSS8C------", "SNGPUSS8T------", "SNGPUSS9-------", "SNGPUSS9C------", "SNGPUSS9T------",
                "SNGPUSSC-------", "SNGPUSSL-------", "SNGPUSSLC------", "SNGPUSSLT------", "SNGPUSST-------", "SNGPUSSW-------", "SNGPUSSWC------", "SNGPUSSWP------", "SNGPUSSWPC-----", "SNGPUSSWPT-----", "SNGPUSSWT------",
                "SNGPUSSX-------", "SNGPUSSXC------", "SNGPUSSXT------", "SNGPUST--------", "SNGPUSTA-------", "SNGPUSTAC------", "SNGPUSTAT------", "SNGPUSTC-------", "SNGPUSTI-------", "SNGPUSTIC------", "SNGPUSTIT------",
                "SNGPUSTM-------", "SNGPUSTMC------", "SNGPUSTMT------", "SNGPUSTR-------", "SNGPUSTRC------", "SNGPUSTRT------", "SNGPUSTS-------", "SNGPUSTSC------", "SNGPUSTST------", "SNGPUSTT-------", "SNGPUSX--------",
                "SNGPUSXC-------", "SNGPUSXE-------", "SNGPUSXEC------", "SNGPUSXET------", "SNGPUSXH-------", "SNGPUSXHC------", "SNGPUSXHT------", "SNGPUSXO-------", "SNGPUSXOC------", "SNGPUSXOM------", "SNGPUSXOMC-----",
                "SNGPUSXOMT-----", "SNGPUSXOT------", "SNGPUSXR-------", "SNGPUSXRC------", "SNGPUSXRT------", "SNGPUSXT-------", "SNGPUU---------", "SNGPUUA--------", "SNGPUUAB-------", "SNGPUUABR------", "SNGPUUAC-------",
                "SNGPUUACC------", "SNGPUUACCK-----", "SNGPUUACCM-----", "SNGPUUACR------", "SNGPUUACRS-----", "SNGPUUACRW-----", "SNGPUUACS------", "SNGPUUACSA-----", "SNGPUUACSM-----", "SNGPUUAD-------", "SNGPUUAN-------",
                "SNGPUUE--------", "SNGPUUI--------", "SNGPUUL--------", "SNGPUULC-------", "SNGPUULD-------", "SNGPUULF-------", "SNGPUULM-------", "SNGPUULS-------", "SNGPUUM--------", "SNGPUUMA-------", "SNGPUUMC-------",
                "SNGPUUMJ-------", "SNGPUUMMO------", "SNGPUUMO-------", "SNGPUUMQ-------", "SNGPUUMR-------", "SNGPUUMRG------", "SNGPUUMRS------", "SNGPUUMRSS-----", "SNGPUUMRX------", "SNGPUUMS-------", "SNGPUUMSE------",
                "SNGPUUMSEA-----", "SNGPUUMSEC-----", "SNGPUUMSED-----", "SNGPUUMSEI-----", "SNGPUUMSEJ-----", "SNGPUUMSET-----", "SNGPUUMT-------", "SNGPUUP--------", "SNGPUUS--------", "SNGPUUSA-------", "SNGPUUSC-------",
                "SNGPUUSCL------", "SNGPUUSF-------", "SNGPUUSM-------", "SNGPUUSML------", "SNGPUUSMN------", "SNGPUUSMS------", "SNGPUUSO-------", "SNGPUUSR-------", "SNGPUUSRS------", "SNGPUUSRT------", "SNGPUUSRW------",
                "SNGPUUSS-------", "SNGPUUSW-------", "SNGPUUSX-------", "SNPP-----------", "SNPPL----------", "SNPPS----------", "SNPPT----------", "SNPPV----------", "SNSP-----------", "SNSPC----------", "SNSPCA---------",
                "SNSPCALA-------", "SNSPCALC-------", "SNSPCALS-------", "SNSPCALSM------", "SNSPCALST------", "SNSPCD---------", "SNSPCH---------", "SNSPCL---------", "SNSPCLBB-------", "SNSPCLCC-------", "SNSPCLCV-------",
                "SNSPCLDD-------", "SNSPCLFF-------", "SNSPCLLL-------", "SNSPCLLLAS-----", "SNSPCLLLMI-----", "SNSPCLLLSU-----", "SNSPCM---------", "SNSPCMMA-------", "SNSPCMMH-------", "SNSPCMML-------", "SNSPCMMS-------",
                "SNSPCP---------", "SNSPCPSB-------", "SNSPCPSU-------", "SNSPCPSUG------", "SNSPCPSUM------", "SNSPCPSUT------", "SNSPCU---------", "SNSPCUM--------", "SNSPCUN--------", "SNSPCUR--------", "SNSPCUS--------",
                "SNSPG----------", "SNSPGC---------", "SNSPGG---------", "SNSPGT---------", "SNSPGU---------", "SNSPN----------", "SNSPNF---------", "SNSPNH---------", "SNSPNI---------", "SNSPNM---------", "SNSPNR---------",
                "SNSPNS---------", "SNSPO----------", "SNSPXA---------", "SNSPXAR--------", "SNSPXAS--------", "SNSPXF---------", "SNSPXFDF-------", "SNSPXFDR-------", "SNSPXFTR-------", "SNSPXH---------", "SNSPXL---------",
                "SNSPXM---------", "SNSPXMC--------", "SNSPXMF--------", "SNSPXMH--------", "SNSPXMO--------", "SNSPXMP--------", "SNSPXMR--------", "SNSPXMTO-------", "SNSPXMTU-------", "SNSPXP---------", "SNSPXR---------",
                "SNUP-----------", "SNUPE----------", "SNUPND---------", "SNUPS----------", "SNUPS1---------", "SNUPS2---------", "SNUPS3---------", "SNUPS4---------", "SNUPSB---------", "SNUPSC---------", "SNUPSCA--------",
                "SNUPSCB--------", "SNUPSCF--------", "SNUPSCG--------", "SNUPSCM--------", "SNUPSF---------", "SNUPSK---------", "SNUPSL---------", "SNUPSN---------", "SNUPSNA--------", "SNUPSNB--------", "SNUPSNF--------",
                "SNUPSNG--------", "SNUPSNM--------", "SNUPSO---------", "SNUPSOF--------", "SNUPSR---------", "SNUPSU---------", "SNUPSUM--------", "SNUPSUN--------", "SNUPSUS--------", "SNUPSX---------", "SNUPV----------",
                "SNUPW----------", "SNUPWD---------", "SNUPWDM--------", "SNUPWDMG-------", "SNUPWDMM-------", "SNUPWM---------", "SNUPWMA--------", "SNUPWMB--------", "SNUPWMBD-------", "SNUPWMC--------", "SNUPWMD--------",
                "SNUPWME--------", "SNUPWMF--------", "SNUPWMFC-------", "SNUPWMFD-------", "SNUPWMFE-------", "SNUPWMFO-------", "SNUPWMFR-------", "SNUPWMFX-------", "SNUPWMG--------", "SNUPWMGC-------", "SNUPWMGD-------",
                "SNUPWMGE-------", "SNUPWMGO-------", "SNUPWMGR-------", "SNUPWMGX-------", "SNUPWMM--------", "SNUPWMMC-------", "SNUPWMMD-------", "SNUPWMME-------", "SNUPWMMO-------", "SNUPWMMR-------", "SNUPWMMX-------",
                "SNUPWMN--------", "SNUPWMO--------", "SNUPWMOD-------", "SNUPWMR--------", "SNUPWMS--------", "SNUPWMSD-------", "SNUPWMSX-------", "SNUPWMX--------", "SNUPWT---------", "SNUPX----------", "SUAP-----------",
                "SUAPC----------", "SUAPCF---------", "SUAPCH---------", "SUAPCL---------", "SUAPM----------", "SUAPME---------", "SUAPMF---------", "SUAPMFA--------", "SUAPMFB--------", "SUAPMFC--------", "SUAPMFCH-------",
                "SUAPMFCL-------", "SUAPMFCM-------", "SUAPMFD--------", "SUAPMFF--------", "SUAPMFFI-------", "SUAPMFH--------", "SUAPMFJ--------", "SUAPMFK--------", "SUAPMFKB-------", "SUAPMFKD-------", "SUAPMFL--------",
                "SUAPMFM--------", "SUAPMFO--------", "SUAPMFP--------", "SUAPMFPM-------", "SUAPMFPN-------", "SUAPMFQ--------", "SUAPMFQA-------", "SUAPMFQB-------", "SUAPMFQC-------", "SUAPMFQD-------", "SUAPMFQF-------",
                "SUAPMFQH-------", "SUAPMFQI-------", "SUAPMFQJ-------", "SUAPMFQK-------", "SUAPMFQL-------", "SUAPMFQM-------", "SUAPMFQN-------", "SUAPMFQO-------", "SUAPMFQP-------", "SUAPMFQR-------", "SUAPMFQRW------",
                "SUAPMFQRX------", "SUAPMFQRZ------", "SUAPMFQS-------", "SUAPMFQT-------", "SUAPMFQU-------", "SUAPMFQY-------", "SUAPMFR--------", "SUAPMFRW-------", "SUAPMFRX-------", "SUAPMFRZ-------", "SUAPMFS--------",
                "SUAPMFT--------", "SUAPMFU--------", "SUAPMFUH-------", "SUAPMFUL-------", "SUAPMFUM-------", "SUAPMFY--------", "SUAPMH---------", "SUAPMHA--------", "SUAPMHC--------", "SUAPMHCH-------", "SUAPMHCL-------",
                "SUAPMHCM-------", "SUAPMHD--------", "SUAPMHH--------", "SUAPMHI--------", "SUAPMHJ--------", "SUAPMHK--------", "SUAPMHM--------", "SUAPMHO--------", "SUAPMHQ--------", "SUAPMHR--------", "SUAPMHS--------",
                "SUAPMHT--------", "SUAPMHU--------", "SUAPMHUH-------", "SUAPMHUL-------", "SUAPMHUM-------", "SUAPML---------", "SUAPMV---------", "SUAPW----------", "SUAPWB---------", "SUAPWD---------", "SUAPWM---------",
                "SUAPWMA--------", "SUAPWMAA-------", "SUAPWMAP-------", "SUAPWMAS-------", "SUAPWMB--------", "SUAPWMCM-------", "SUAPWMS--------", "SUAPWMSA-------", "SUAPWMSB-------", "SUAPWMSS-------", "SUAPWMSU-------",
                "SUAPWMU--------", "SUFP-----------", "SUFPA----------", "SUFPAF---------", "SUFPAFA--------", "SUFPAFK--------", "SUFPAFU--------", "SUFPAFUH-------", "SUFPAFUL-------", "SUFPAFUM-------", "SUFPAH---------",
                "SUFPAHA--------", "SUFPAHH--------", "SUFPAHU--------", "SUFPAHUH-------", "SUFPAHUL-------", "SUFPAHUM-------", "SUFPAV---------", "SUFPB----------", "SUFPG----------", "SUFPGC---------", "SUFPGP---------",
                "SUFPGPA--------", "SUFPGR---------", "SUFPGS---------", "SUFPN----------", "SUFPNB---------", "SUFPNN---------", "SUFPNS---------", "SUFPNU---------", "SUGP-----------", "SUGPE----------", "SUGPES---------",
                "SUGPESE--------", "SUGPESR--------", "SUGPEV---------", "SUGPEVA--------", "SUGPEVAA-------", "SUGPEVAAR------", "SUGPEVAC-------", "SUGPEVAI-------", "SUGPEVAL-------", "SUGPEVAS-------", "SUGPEVAT-------",
                "SUGPEVATH------", "SUGPEVATHR-----", "SUGPEVATL------", "SUGPEVATLR-----", "SUGPEVATM------", "SUGPEVATMR-----", "SUGPEVC--------", "SUGPEVCA-------", "SUGPEVCAH------", "SUGPEVCAL------", "SUGPEVCAM------",
                "SUGPEVCF-------", "SUGPEVCFH------", "SUGPEVCFL------", "SUGPEVCFM------", "SUGPEVCJ-------", "SUGPEVCJH------", "SUGPEVCJL------", "SUGPEVCJM------", "SUGPEVCM-------", "SUGPEVCMH------", "SUGPEVCML------",
                "SUGPEVCMM------", "SUGPEVCO-------", "SUGPEVCOH------", "SUGPEVCOL------", "SUGPEVCOM------", "SUGPEVCT-------", "SUGPEVCTH------", "SUGPEVCTL------", "SUGPEVCTM------", "SUGPEVCU-------", "SUGPEVCUH------",
                "SUGPEVCUL------", "SUGPEVCUM------", "SUGPEVE--------", "SUGPEVEA-------", "SUGPEVEAA------", "SUGPEVEAT------", "SUGPEVEB-------", "SUGPEVEC-------", "SUGPEVED-------", "SUGPEVEDA------", "SUGPEVEE-------",
                "SUGPEVEF-------", "SUGPEVEH-------", "SUGPEVEM-------", "SUGPEVEML------", "SUGPEVEMV------", "SUGPEVER-------", "SUGPEVES-------", "SUGPEVM--------", "SUGPEVS--------", "SUGPEVSC-------", "SUGPEVSP-------",
                "SUGPEVSR-------", "SUGPEVST-------", "SUGPEVSW-------", "SUGPEVT--------", "SUGPEVU--------", "SUGPEVUA-------", "SUGPEVUAA------", "SUGPEVUB-------", "SUGPEVUL-------", "SUGPEVUR-------", "SUGPEVUS-------",
                "SUGPEVUSH------", "SUGPEVUSL------", "SUGPEVUSM------", "SUGPEVUT-------", "SUGPEVUTH------", "SUGPEVUTL------", "SUGPEVUX-------", "SUGPEWA--------", "SUGPEWAH-------", "SUGPEWAL-------", "SUGPEWAM-------",
                "SUGPEWD--------", "SUGPEWDH-------", "SUGPEWDHS------", "SUGPEWDL-------", "SUGPEWDLS------", "SUGPEWDM-------", "SUGPEWDMS------", "SUGPEWG--------", "SUGPEWGH-------", "SUGPEWGL-------", "SUGPEWGM-------",
                "SUGPEWGR-------", "SUGPEWH--------", "SUGPEWHH-------", "SUGPEWHHS------", "SUGPEWHL-------", "SUGPEWHLS------", "SUGPEWHM-------", "SUGPEWHMS------", "SUGPEWM--------", "SUGPEWMA-------", "SUGPEWMAI------",
                "SUGPEWMAIE-----", "SUGPEWMAIR-----", "SUGPEWMAL------", "SUGPEWMALE-----", "SUGPEWMALR-----", "SUGPEWMAS------", "SUGPEWMASE-----", "SUGPEWMASR-----", "SUGPEWMAT------", "SUGPEWMATE-----", "SUGPEWMATR-----",
                "SUGPEWMS-------", "SUGPEWMSI------", "SUGPEWMSL------", "SUGPEWMSS------", "SUGPEWMT-------", "SUGPEWMTH------", "SUGPEWMTL------", "SUGPEWMTM------", "SUGPEWO--------", "SUGPEWOH-------", "SUGPEWOL-------",
                "SUGPEWOM-------", "SUGPEWR--------", "SUGPEWRH-------", "SUGPEWRL-------", "SUGPEWRR-------", "SUGPEWS--------", "SUGPEWSH-------", "SUGPEWSL-------", "SUGPEWSM-------", "SUGPEWT--------", "SUGPEWTH-------",
                "SUGPEWTL-------", "SUGPEWTM-------", "SUGPEWX--------", "SUGPEWXH-------", "SUGPEWXL-------", "SUGPEWXM-------", "SUGPEWZ--------", "SUGPEWZH-------", "SUGPEWZL-------", "SUGPEWZM-------", "SUGPEXF--------",
                "SUGPEXI--------", "SUGPEXL--------", "SUGPEXM--------", "SUGPEXMC-------", "SUGPEXML-------", "SUGPEXN--------", "SUGPI----------", "SUGPIB---------", "SUGPIBA--------", "SUGPIBN--------", "SUGPIE---------",
                "SUGPIG---------", "SUGPIMA--------", "SUGPIMC--------", "SUGPIME--------", "SUGPIMF--------", "SUGPIMFA-------", "SUGPIMFP-------", "SUGPIMFPW------", "SUGPIMFS-------", "SUGPIMG--------", "SUGPIMM--------",
                "SUGPIMN--------", "SUGPIMNB-------", "SUGPIMS--------", "SUGPIMV--------", "SUGPIP---------", "SUGPIPD--------", "SUGPIR---------", "SUGPIRM--------", "SUGPIRN--------", "SUGPIRNB-------", "SUGPIRNC-------",
                "SUGPIRNN-------", "SUGPIRP--------", "SUGPIT---------", "SUGPIU---------", "SUGPIUE--------", "SUGPIUED-------", "SUGPIUEF-------", "SUGPIUEN-------", "SUGPIUP--------", "SUGPIUR--------", "SUGPIUT--------",
                "SUGPIX---------", "SUGPIXH--------", "SUGPU----------", "SUGPUC---------", "SUGPUCA--------", "SUGPUCAA-------", "SUGPUCAAA------", "SUGPUCAAAS-----", "SUGPUCAAAT-----", "SUGPUCAAAW-----", "SUGPUCAAC------",
                "SUGPUCAAD------", "SUGPUCAAL------", "SUGPUCAAM------", "SUGPUCAAO------", "SUGPUCAAOS-----", "SUGPUCAAS------", "SUGPUCAAU------", "SUGPUCAT-------", "SUGPUCATA------", "SUGPUCATH------", "SUGPUCATL------",
                "SUGPUCATM------", "SUGPUCATR------", "SUGPUCATW------", "SUGPUCATWR-----", "SUGPUCAW-------", "SUGPUCAWA------", "SUGPUCAWH------", "SUGPUCAWL------", "SUGPUCAWM------", "SUGPUCAWR------", "SUGPUCAWS------",
                "SUGPUCAWW------", "SUGPUCAWWR-----", "SUGPUCD--------", "SUGPUCDC-------", "SUGPUCDG-------", "SUGPUCDH-------", "SUGPUCDHH------", "SUGPUCDHP------", "SUGPUCDM-------", "SUGPUCDMH------", "SUGPUCDML------",
                "SUGPUCDMLA-----", "SUGPUCDMM------", "SUGPUCDO-------", "SUGPUCDS-------", "SUGPUCDSC------", "SUGPUCDSS------", "SUGPUCDSV------", "SUGPUCDT-------", "SUGPUCE--------", "SUGPUCEC-------", "SUGPUCECA------",
                "SUGPUCECC------", "SUGPUCECH------", "SUGPUCECL------", "SUGPUCECM------", "SUGPUCECO------", "SUGPUCECR------", "SUGPUCECS------", "SUGPUCECT------", "SUGPUCECW------", "SUGPUCEN-------", "SUGPUCENN------",
                "SUGPUCF--------", "SUGPUCFH-------", "SUGPUCFHA------", "SUGPUCFHC------", "SUGPUCFHE------", "SUGPUCFHH------", "SUGPUCFHL------", "SUGPUCFHM------", "SUGPUCFHO------", "SUGPUCFHS------", "SUGPUCFHX------",
                "SUGPUCFM-------", "SUGPUCFML------", "SUGPUCFMS------", "SUGPUCFMT------", "SUGPUCFMTA-----", "SUGPUCFMTC-----", "SUGPUCFMTO-----", "SUGPUCFMTS-----", "SUGPUCFMW------", "SUGPUCFO-------", "SUGPUCFOA------",
                "SUGPUCFOL------", "SUGPUCFOO------", "SUGPUCFOS------", "SUGPUCFR-------", "SUGPUCFRM------", "SUGPUCFRMR-----", "SUGPUCFRMS-----", "SUGPUCFRMT-----", "SUGPUCFRS------", "SUGPUCFRSR-----", "SUGPUCFRSS-----",
                "SUGPUCFRST-----", "SUGPUCFS-------", "SUGPUCFSA------", "SUGPUCFSL------", "SUGPUCFSO------", "SUGPUCFSS------", "SUGPUCFT-------", "SUGPUCFTA------", "SUGPUCFTC------", "SUGPUCFTCD-----", "SUGPUCFTCM-----",
                "SUGPUCFTF------", "SUGPUCFTR------", "SUGPUCFTS------", "SUGPUCI--------", "SUGPUCIA-------", "SUGPUCIC-------", "SUGPUCII-------", "SUGPUCIL-------", "SUGPUCIM-------", "SUGPUCIN-------", "SUGPUCIO-------",
                "SUGPUCIS-------", "SUGPUCIZ-------", "SUGPUCM--------", "SUGPUCMS-------", "SUGPUCMT-------", "SUGPUCR--------", "SUGPUCRA-------", "SUGPUCRC-------", "SUGPUCRH-------", "SUGPUCRL-------", "SUGPUCRO-------",
                "SUGPUCRR-------", "SUGPUCRRD------", "SUGPUCRRF------", "SUGPUCRRL------", "SUGPUCRS-------", "SUGPUCRV-------", "SUGPUCRVA------", "SUGPUCRVG------", "SUGPUCRVM------", "SUGPUCRVO------", "SUGPUCRX-------",
                "SUGPUCS--------", "SUGPUCSA-------", "SUGPUCSG-------", "SUGPUCSGA------", "SUGPUCSGD------", "SUGPUCSGM------", "SUGPUCSM-------", "SUGPUCSR-------", "SUGPUCSW-------", "SUGPUCV--------", "SUGPUCVC-------",
                "SUGPUCVF-------", "SUGPUCVFA------", "SUGPUCVFR------", "SUGPUCVFU------", "SUGPUCVR-------", "SUGPUCVRA------", "SUGPUCVRM------", "SUGPUCVRS------", "SUGPUCVRU------", "SUGPUCVRUC-----", "SUGPUCVRUE-----",
                "SUGPUCVRUH-----", "SUGPUCVRUL-----", "SUGPUCVRUM-----", "SUGPUCVRW------", "SUGPUCVS-------", "SUGPUCVU-------", "SUGPUCVUF------", "SUGPUCVUR------", "SUGPUCVV-------", "SUGPUH---------", "SUGPUS---------",
                "SUGPUSA--------", "SUGPUSAC-------", "SUGPUSAF-------", "SUGPUSAFC------", "SUGPUSAFT------", "SUGPUSAJ-------", "SUGPUSAJC------", "SUGPUSAJT------", "SUGPUSAL-------", "SUGPUSALC------", "SUGPUSALT------",
                "SUGPUSAM-------", "SUGPUSAMC------", "SUGPUSAMT------", "SUGPUSAO-------", "SUGPUSAOC------", "SUGPUSAOT------", "SUGPUSAP-------", "SUGPUSAPB------", "SUGPUSAPBC-----", "SUGPUSAPBT-----", "SUGPUSAPC------",
                "SUGPUSAPM------", "SUGPUSAPMC-----", "SUGPUSAPMT-----", "SUGPUSAPT------", "SUGPUSAQ-------", "SUGPUSAQC------", "SUGPUSAQT------", "SUGPUSAR-------", "SUGPUSARC------", "SUGPUSART------", "SUGPUSAS-------",
                "SUGPUSASC------", "SUGPUSAST------", "SUGPUSAT-------", "SUGPUSAW-------", "SUGPUSAWC------", "SUGPUSAWT------", "SUGPUSAX-------", "SUGPUSAXC------", "SUGPUSAXT------", "SUGPUSM--------", "SUGPUSMC-------",
                "SUGPUSMD-------", "SUGPUSMDC------", "SUGPUSMDT------", "SUGPUSMM-------", "SUGPUSMMC------", "SUGPUSMMT------", "SUGPUSMP-------", "SUGPUSMPC------", "SUGPUSMPT------", "SUGPUSMT-------", "SUGPUSMV-------",
                "SUGPUSMVC------", "SUGPUSMVT------", "SUGPUSS--------", "SUGPUSS1-------", "SUGPUSS1C------", "SUGPUSS1T------", "SUGPUSS2-------", "SUGPUSS2C------", "SUGPUSS2T------", "SUGPUSS3-------", "SUGPUSS3A------",
                "SUGPUSS3AC-----", "SUGPUSS3AT-----", "SUGPUSS3C------", "SUGPUSS3T------", "SUGPUSS4-------", "SUGPUSS4C------", "SUGPUSS4T------", "SUGPUSS5-------", "SUGPUSS5C------", "SUGPUSS5T------", "SUGPUSS6-------",
                "SUGPUSS6C------", "SUGPUSS6T------", "SUGPUSS7-------", "SUGPUSS7C------", "SUGPUSS7T------", "SUGPUSS8-------", "SUGPUSS8C------", "SUGPUSS8T------", "SUGPUSS9-------", "SUGPUSS9C------", "SUGPUSS9T------",
                "SUGPUSSC-------", "SUGPUSSL-------", "SUGPUSSLC------", "SUGPUSSLT------", "SUGPUSST-------", "SUGPUSSW-------", "SUGPUSSWC------", "SUGPUSSWP------", "SUGPUSSWPC-----", "SUGPUSSWPT-----", "SUGPUSSWT------",
                "SUGPUSSX-------", "SUGPUSSXC------", "SUGPUSSXT------", "SUGPUST--------", "SUGPUSTA-------", "SUGPUSTAC------", "SUGPUSTAT------", "SUGPUSTC-------", "SUGPUSTI-------", "SUGPUSTIC------", "SUGPUSTIT------",
                "SUGPUSTM-------", "SUGPUSTMC------", "SUGPUSTMT------", "SUGPUSTR-------", "SUGPUSTRC------", "SUGPUSTRT------", "SUGPUSTS-------", "SUGPUSTSC------", "SUGPUSTST------", "SUGPUSTT-------", "SUGPUSX--------",
                "SUGPUSXC-------", "SUGPUSXE-------", "SUGPUSXEC------", "SUGPUSXET------", "SUGPUSXH-------", "SUGPUSXHC------", "SUGPUSXHT------", "SUGPUSXO-------", "SUGPUSXOC------", "SUGPUSXOM------", "SUGPUSXOMC-----",
                "SUGPUSXOMT-----", "SUGPUSXOT------", "SUGPUSXR-------", "SUGPUSXRC------", "SUGPUSXRT------", "SUGPUSXT-------", "SUGPUU---------", "SUGPUUA--------", "SUGPUUAB-------", "SUGPUUABR------", "SUGPUUAC-------",
                "SUGPUUACC------", "SUGPUUACCK-----", "SUGPUUACCM-----", "SUGPUUACR------", "SUGPUUACRS-----", "SUGPUUACRW-----", "SUGPUUACS------", "SUGPUUACSA-----", "SUGPUUACSM-----", "SUGPUUAD-------", "SUGPUUAN-------",
                "SUGPUUE--------", "SUGPUUI--------", "SUGPUUL--------", "SUGPUULC-------", "SUGPUULD-------", "SUGPUULF-------", "SUGPUULM-------", "SUGPUULS-------", "SUGPUUM--------", "SUGPUUMA-------", "SUGPUUMC-------",
                "SUGPUUMJ-------", "SUGPUUMMO------", "SUGPUUMO-------", "SUGPUUMQ-------", "SUGPUUMR-------", "SUGPUUMRG------", "SUGPUUMRS------", "SUGPUUMRSS-----", "SUGPUUMRX------", "SUGPUUMS-------", "SUGPUUMSE------",
                "SUGPUUMSEA-----", "SUGPUUMSEC-----", "SUGPUUMSED-----", "SUGPUUMSEI-----", "SUGPUUMSEJ-----", "SUGPUUMSET-----", "SUGPUUMT-------", "SUGPUUP--------", "SUGPUUS--------", "SUGPUUSA-------", "SUGPUUSC-------",
                "SUGPUUSCL------", "SUGPUUSF-------", "SUGPUUSM-------", "SUGPUUSML------", "SUGPUUSMN------", "SUGPUUSMS------", "SUGPUUSO-------", "SUGPUUSR-------", "SUGPUUSRS------", "SUGPUUSRT------", "SUGPUUSRW------",
                "SUGPUUSS-------", "SUGPUUSW-------", "SUGPUUSX-------", "SUPP-----------", "SUPP-----------", "SUPPL----------", "SUPPS----------", "SUPPT----------", "SUPPV----------", "SUSP-----------", "SUSPC----------",
                "SUSPCA---------", "SUSPCALA-------", "SUSPCALC-------", "SUSPCALS-------", "SUSPCALSM------", "SUSPCALST------", "SUSPCD---------", "SUSPCH---------", "SUSPCL---------", "SUSPCLBB-------", "SUSPCLCC-------",
                "SUSPCLCV-------", "SUSPCLDD-------", "SUSPCLFF-------", "SUSPCLLL-------", "SUSPCLLLAS-----", "SUSPCLLLMI-----", "SUSPCLLLSU-----", "SUSPCM---------", "SUSPCMMA-------", "SUSPCMMH-------", "SUSPCMML-------",
                "SUSPCMMS-------", "SUSPCP---------", "SUSPCPSB-------", "SUSPCPSU-------", "SUSPCPSUG------", "SUSPCPSUM------", "SUSPCPSUT------", "SUSPCU---------", "SUSPCUM--------", "SUSPCUN--------", "SUSPCUR--------",
                "SUSPCUS--------", "SUSPG----------", "SUSPGC---------", "SUSPGG---------", "SUSPGT---------", "SUSPGU---------", "SUSPN----------", "SUSPNF---------", "SUSPNH---------", "SUSPNI---------", "SUSPNM---------",
                "SUSPNR---------", "SUSPNS---------", "SUSPO----------", "SUSPXA---------", "SUSPXAR--------", "SUSPXAS--------", "SUSPXF---------", "SUSPXFDF-------", "SUSPXFDR-------", "SUSPXFTR-------", "SUSPXH---------",
                "SUSPXL---------", "SUSPXM---------", "SUSPXMC--------", "SUSPXMF--------", "SUSPXMH--------", "SUSPXMO--------", "SUSPXMP--------", "SUSPXMR--------", "SUSPXMTO-------", "SUSPXMTU-------", "SUSPXP---------",
                "SUSPXR---------", "SUUP-----------", "SUUPE----------", "SUUPND---------", "SUUPS----------", "SUUPS1---------", "SUUPS2---------", "SUUPS3---------", "SUUPS4---------", "SUUPSB---------", "SUUPSC---------",
                "SUUPSCA--------", "SUUPSCB--------", "SUUPSCF--------", "SUUPSCG--------", "SUUPSCM--------", "SUUPSF---------", "SUUPSK---------", "SUUPSL---------", "SUUPSN---------", "SUUPSNA--------", "SUUPSNB--------",
                "SUUPSNF--------", "SUUPSNG--------", "SUUPSNM--------", "SUUPSO---------", "SUUPSOF--------", "SUUPSR---------", "SUUPSU---------", "SUUPSUM--------", "SUUPSUN--------", "SUUPSUS--------", "SUUPSX---------",
                "SUUPV----------", "SUUPW----------", "SUUPWD---------", "SUUPWDM--------", "SUUPWDMG-------", "SUUPWDMM-------", "SUUPWM---------", "SUUPWMA--------", "SUUPWMB--------", "SUUPWMBD-------", "SUUPWMC--------",
                "SUUPWMD--------", "SUUPWME--------", "SUUPWMF--------", "SUUPWMFC-------", "SUUPWMFD-------", "SUUPWMFE-------", "SUUPWMFO-------", "SUUPWMFR-------", "SUUPWMFX-------", "SUUPWMG--------", "SUUPWMGC-------",
                "SUUPWMGD-------", "SUUPWMGE-------", "SUUPWMGO-------", "SUUPWMGR-------", "SUUPWMGX-------", "SUUPWMM--------", "SUUPWMMC-------", "SUUPWMMD-------", "SUUPWMME-------", "SUUPWMMO-------", "SUUPWMMR-------",
                "SUUPWMMX-------", "SUUPWMN--------", "SUUPWMO--------", "SUUPWMOD-------", "SUUPWMR--------", "SUUPWMS--------", "SUUPWMSD-------", "SUUPWMSX-------", "SUUPWMX--------", "SUUPWT---------", "SUUPX----------"
            };

            return (count == 0 || (count + skip) >= codes.Length)
                ? codes
                : codes.Skip(skip).Take(count).ToArray();
        }

    }
}
