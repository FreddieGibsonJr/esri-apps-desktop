using Esri.ArcGISRuntime.Data;
using Esri.ArcGISRuntime.Geometry;
using Esri.ArcGISRuntime.Mapping;
using Esri.ArcGISRuntime.Symbology;
using Esri.ArcGISRuntime.UI;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Media;

namespace MilitarySymbolsRevisionC
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string StylePath { get; }
        private string SidcPath { get; }
        private string FieldName { get; }

        private int Columns { get; }

        private double Meters { get; }

        private static int Page { get; set; }
        private Envelope AOI { get; }

        private Dictionary<string, object>[] Pages { get; set; }

        public MainWindow()
        {
            // Set the extent of the AOI
            Meters = 100;

            // Set the number of columns
            Columns = 4;

            // Set the Field name
            FieldName = "sidc";

            // Create relative paths to included data
            StylePath = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location), @"symbols\mil2525c_b2.stylx");
            SidcPath = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location), @"symbols\SIDC.json");

            // Create the AOI and zoom to it
            MapPoint pnt = GeometryEngine.Project(new MapPoint(-117.1825, 34.0556, SpatialReferences.Wgs84), SpatialReferences.WebMercator) as MapPoint;
            AOI = GeometryEngine.Buffer(pnt, Meters).Extent;

            InitializeComponent();

            MyMapView.Loaded += async (sender, args) =>
            {
                // Create a map using the navigationVector Basemap
                MyMapView.Map = new Map(Basemap.CreateNavigationVector());

                // Create a graphics overlay to display the symbol codes
                MyMapView.GraphicsOverlays.Add(new GraphicsOverlay());

                // Zoom to the aoi
                await MyMapView.SetViewpointGeometryAsync(AOI, 250);
                
                // Read the symbol identification codes from the json file
                using (System.IO.StreamReader reader = new System.IO.StreamReader(SidcPath))
                {
                    var json = reader.ReadToEnd();
                    Pages = Newtonsoft.Json.JsonConvert.DeserializeObject<Dictionary<string, object>[]>(json);
                }

                Page = 0;
                RenderSymbols(Pages.First(), true);

                btnPrevious.IsEnabled = false;
            };
        }

        private async void RenderSymbols(Dictionary<string, object> info, bool initialize = false)
        {
            FeatureCollectionTable dTable;
            FeatureCollectionTable uTable;

            DictionaryRenderer dRend;
            UniqueValueRenderer uRend;

            string[] codes = Newtonsoft.Json.JsonConvert.DeserializeObject<string[]>(info["codes"].ToString());
            int page = int.Parse(info["page"].ToString());

            if (initialize)
            {
                // Create the default symbol
                SimpleMarkerSymbol sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle.Circle, Colors.Red, 12)
                {
                    Outline = new SimpleLineSymbol(SimpleLineSymbolStyle.Solid, Colors.Black, 1)
                };

                // Create the Dictionary and Unique Value renderers
                dRend = new DictionaryRenderer(DictionarySymbolStyle.OpenAsync("mil2525c_b2", StylePath).Result);
                uRend = new UniqueValueRenderer(new[] { FieldName }, null, "MilSymbol", sms);

                // Create the fields for the Feature Collection table
                Field[] fields = { new Field(FieldType.Text, FieldName, FieldName, 15) };

                // Create the Feature Collection Tables
                dTable = new FeatureCollectionTable(fields, GeometryType.Point, SpatialReferences.WebMercator) { Renderer = dRend };
                uTable = new FeatureCollectionTable(fields, GeometryType.Point, SpatialReferences.WebMercator) { Renderer = uRend };

                // Add the tables to the feature collection
                FeatureCollection collection = new FeatureCollection(new[] { dTable, uTable });

                // Add the collection to the map
                MyMapView.Map.OperationalLayers.Add(new FeatureCollectionLayer(collection));

            }
            else
            {
                // Get the feature collection from the map
                FeatureCollection collection = (MyMapView.Map.OperationalLayers.First() as FeatureCollectionLayer).FeatureCollection;

                // Get the tables from the collection
                dTable = collection.Tables.First();
                uTable = collection.Tables.Last();

                // Clear the records from the table and overlay
                QueryParameters query = new QueryParameters { WhereClause = "1=1" };
                await dTable.DeleteFeaturesAsync(dTable.QueryFeaturesAsync(query).Result);
                await uTable.DeleteFeaturesAsync(uTable.QueryFeaturesAsync(query).Result);
                MyMapView.GraphicsOverlays.First().Graphics.Clear();

                // Clear the symbols from the unique value renderer
                (uTable.Renderer as UniqueValueRenderer)?.UniqueValues.Clear();
            }

            // Show the Page #
            MapPoint titleLocation = new MapPoint(AOI.GetCenter().X, AOI.Extent.YMax + 25);
            TextSymbol titleBar = new TextSymbol
            {
                Text = string.Format("Page: {0}", page),
                Color = Colors.Black,
                BackgroundColor = Colors.DodgerBlue,
                FontWeight = Esri.ArcGISRuntime.Symbology.FontWeight.Bold,
                Size = 50,
                HorizontalAlignment = Esri.ArcGISRuntime.Symbology.HorizontalAlignment.Center,
                VerticalAlignment = Esri.ArcGISRuntime.Symbology.VerticalAlignment.Bottom,
                OutlineColor = Colors.Black,
                OutlineWidth = 2
            };

            MyMapView.GraphicsOverlays.First().Graphics.Add(new Graphic(titleLocation, titleBar));

            int rows = codes.Length / Columns;

            double rowMargin = (AOI.XMax - AOI.XMin) / (Columns - 1);
            double colMargin = (AOI.YMax - AOI.YMin) / rows;

            int count = 0;
            for (double x = AOI.XMin; x < AOI.XMax; x += rowMargin)
            {
                if (count >= codes.Length)
                    break;

                for (double y = AOI.YMax; y > AOI.YMin; y -= colMargin)
                {
                    if (count >= codes.Length)
                        break;

                    string code = codes[count];

                    if (code.Length != 15)
                    {
                        System.Diagnostics.Debug.WriteLine(string.Format("Possible invalid code: {0}", code));
                    }
                    else
                    {
                        // Create features with the sidc code
                        Feature dFeature = dTable.CreateFeature(new Dictionary<string, object> { { FieldName, code } }, new MapPoint(x, y, SpatialReferences.WebMercator));
                        Feature uFeature = uTable.CreateFeature(new Dictionary<string, object> { { FieldName, code } }, new MapPoint(x + (rowMargin * 0.5), y, SpatialReferences.WebMercator));

                        // Create a label to display the code
                        TextSymbol label = new TextSymbol
                        {
                            Text = code,
                            FontWeight = Esri.ArcGISRuntime.Symbology.FontWeight.Bold,
                            Color = Colors.DarkGoldenrod,
                            BackgroundColor = Colors.Black,
                            FontFamily = "Consolas",
                            Size = 16
                        };

                        MapPoint labelPoint = new MapPoint(x + (rowMargin * 0.25), y - (colMargin * 0.25), SpatialReferences.WebMercator);
                        MyMapView.GraphicsOverlays.First().Graphics.Add(new Graphic(labelPoint, label));

                        // Add the feature to the table
                        await dTable.AddFeatureAsync(dFeature);
                        await uTable.AddFeatureAsync(uFeature);

                        // Triple the size of the Unique Value symbol
                        MultilayerPointSymbol symbol = (MultilayerPointSymbol)(dTable.Renderer as DictionaryRenderer).GetSymbol(dFeature);
                        symbol.Size *= 3;

                        // Add the military symbol to the unique value renderer
                        UniqueValue uval = new UniqueValue(code, code, symbol, code);
                        (uTable.Renderer as UniqueValueRenderer).UniqueValues.Add(uval);
                    }

                    count++;

                    System.Diagnostics.Debug.WriteLine(string.Format(@"Processed feature {0} for {1}...", count, codes.Length));
                }
            }

        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if ((sender as System.Windows.Controls.Button).Content.ToString() == "Previous")
                Page--;
            else
                Page++;
            
            btnPrevious.IsEnabled = Page > 0;
            btnNext.IsEnabled = Page < Pages.Count();

            RenderSymbols(Pages[Page]);
        }
    }
}
