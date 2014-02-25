
import java.util.Observer;
import java.util.Observable;

public class CurrentConditionsDisplay implements Observer, DisplayElement {
	Observable observable;
	private float temp;
	private float humidity;
	
	public CurrentConditionsDisplay(Observable o) {
		this.observable = o;
		o.addObserver(this);
	}
	
	public void update(Observable obs, Object arg) {
		if (obs instanceof WeatherData) {
			WeatherData weatherData = (WeatherData)obs;
			this.temp = weatherData.getTemp();
			this.humidity = weatherData.getHumidity();
			display();
		}
	}

	public void display() {
		System.out.println("current conditions: " + temp +
				" degree and " + humidity + " % humidity");
	}
}
