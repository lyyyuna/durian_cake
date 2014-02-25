
public class ForecastDisplay implements Observer, DisplayElement {
	
	private float currentPreassure = 29.3f;
	private float lastPreassure;
	private WeatherData weartherData;
	
	public ForecastDisplay(WeatherData weatherData) {
		this.weartherData = weatherData;
		weatherData.registerObserver(this);
	}

	public void update(float temp, float humidity, float pressure) {
		lastPreassure = currentPreassure;
		currentPreassure = pressure;
		display();
	}
	
	public void display() {
		System.out.println("Forecast: ");
		if (currentPreassure > lastPreassure) {
			System.out.println("improving weather on the way");
		} else if (currentPreassure == lastPreassure) {
			System.out.println("same");
		} else if (currentPreassure < lastPreassure) {
			System.out.println("watch out for cooler, rainy weather");
		}
	}
}
