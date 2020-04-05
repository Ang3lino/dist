
import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.chart.NumberAxis;
import javafx.scene.chart.ScatterChart;
import javafx.scene.chart.XYChart;
import javafx.stage.Stage;


public class Server extends Application { 
    public XYChart.Series<Float, Float> getSeries(ArrayList<Float> x, ArrayList<Float> y) {
        //  Configuring Series and adding data to the series    
        XYChart.Series<Float, Float> series = new XYChart.Series<Float, Float>();  
        for (int i = 0; i < x.size(); ++i) {
            series.getData().add(new XYChart.Data<Float, Float>(x.get(i), y.get(i)));  
        }
        return series;
    }

    public void addPoints(XYChart.Series<Float, Float> holder, ArrayList<XYChart.Data<Float, Float>> points) {
        for (int i = 0; i < points.size(); ++i) {
            try {
                TimeUnit.MILLISECONDS.sleep(10);
            } catch (InterruptedException e) {
                System.out.println(e);
            }
            XYChart.Data<Float, Float> data = points.get(i);
            Platform.runLater(() -> holder.getData().add(data)); 
        }
    }

    public void removePoints(XYChart.Series<Float, Float> holder, ArrayList<XYChart.Data<Float, Float>> points) {
        int counter = points.size();
        while (--counter >= 0) {
            final XYChart.Data<Float, Float> data = points.get(counter);
            Platform.runLater(() -> holder.getData().remove(data)); 
            try {
                TimeUnit.MILLISECONDS.sleep(100);
            } catch (InterruptedException e) {
                System.out.println(e);
            }
        }
    }

    public void updateChart(XYChart.Series<Float, Float> holder) {
        ArrayList<XYChart.Data<Float, Float>> points = new ArrayList<>();
        for (int i = 0; i < 100; ++i) {
            final float p = (float) i, q = (float) i;
            System.out.printf("Passing %f %f\n", p,q);
            points.add(new XYChart.Data<Float, Float>(p, q));
        }
        addPoints(holder, points);
        removePoints(holder, points);
    }
  
    @Override  
    public void start(Stage primaryStage) throws Exception {  
        //Configuring Xaxis and Yaxis  
        int xMin = 0, xMax = 100;
        NumberAxis xaxis = new NumberAxis(xMin, xMax, 10);  
        NumberAxis yaxis = new NumberAxis(xMin, xMax, 10);  
        xaxis.setLabel("Weight");  
        yaxis.setLabel("Height");  
          
        //Configuring ScatterChart    
        ScatterChart s = new ScatterChart(xaxis, yaxis);  
        s.setTitle("Perfect height according to your weight");  

        ArrayList<Float> x = new ArrayList<>(), y = new ArrayList<>();
        for (int i = xMin; i < xMax; ++i) {
            x.add((float) i);
            y.add((float) 2*i);
        }
        XYChart.Series series = getSeries(x, y);
        series.setName("Height value");  
          
        //Adding series to the ScatterChart  
        s.getData().add(series);  

        Thread t = new Thread(() -> updateChart(series));
        configGroupAndScene(primaryStage, s);
        t.start();
    }  

    public void configGroupAndScene(Stage primaryStage, ScatterChart s) {
        //Configuring group and Scene   
        double horizontalSize = 800, verticalSize = 600;
        Group root = new Group();  
        s.setPrefSize(horizontalSize, verticalSize);
        root.getChildren().add(s);  
        Scene scene = new Scene(root, horizontalSize, verticalSize);  
        scene.getStylesheets().add("style.css");
        primaryStage.setScene(scene);  
        primaryStage.setTitle("ScatterChart Example");  
        primaryStage.show();      
    }

    public static void main(String[] args) {  
        launch(args);  
    }  
  
}  