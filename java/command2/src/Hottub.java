
public class Hottub {

	boolean on;
	int temperature;
	
	public Hottub() {}
	
	public void on() {
		on = true;
	}
	
	public void off() {
		on = false;
	}
	
	public void bubblesOn() {
		if (on) {
			System.out.println("Hottub is bubbling!");
		}
	}

	public void bubblesOff() {
		if (on) {
			System.out.println("Hottub is not bubbling");
		}
	}

	public void jetsOn() {
		if (on) {
			System.out.println("Hottub jets are on");
		}
	}

	public void jetsOff() {
		if (on) {
			System.out.println("Hottub jets are off");
		}
	}
	
	public void setTemperature(int temp) {
		this.temperature = temp;
	}
	
	public void heat() {
		temperature = 105;
		System.out.println("hottub is heatting to a steaiming 105 degree");
	}
	
	public void cool() {
		temperature = 98;
		System.out.println("hottub is cooling to 98 degree");
	}
}
