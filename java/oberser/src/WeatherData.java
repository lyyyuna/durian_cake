import java.util.*;

public class WeatherData implements Subject {
	
	private ArrayList<Observer> observers;
	private float temperature;
	private float humidity;
	private float preassure;
	
	public WeatherData() {
		observers = new ArrayList<Observer>();
	}
	
	public void registerObserver(Observer o) {
		observers.add(o);
	}
	
	public void removeObserver(Observer o) {
		int i = observers.indexOf(o);
		if (i >= 0) {
			observers.remove(i);
		}
	}
	
	public void notifyObserver() {
		for (int i = 0; i < observers.size(); i++) {
			Observer observer = (Observer)observers.get(i);
			observer.update(temperature, humidity, preassure);
		}
	}
	
	public void measurementsChanged() {
		notifyObserver();
	}
	
	public void setMeasurements(float temperature, float humidity, float preassure) {
		this.temperature = temperature;
		this.humidity = humidity;
		this.preassure = preassure;
		measurementsChanged();
	}
	
	public float getTemperature() {
		return temperature;
	}
	
	public float getHumidity() {
		return humidity;
	}
	
	public float getPreassure() {
		return preassure;
	}

}
