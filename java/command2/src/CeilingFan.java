
public class CeilingFan {

	String location = "";
	int level;
	public static final int HIGH = 2;
	public static final int MEDIUM  = 1;
	public static final int LOW = 0;
	
	public CeilingFan(String location) {
		this.location = location;
	}
	
	public void hight() {
		level = HIGH;
		System.out.println(location + " ceiling fan is on high");
	}
	
	public void medium() {
		level = MEDIUM;
		System.out.println(location + " ceiling fan is on medium");
	}
	
	public void low() {
		level = LOW;
		System.out.println(location + " ceiling fan is on LOW");
	}
	
	public void off() {
		level = LOW;
		System.out.println(location + " ceiling fan is off");
	}
	
	public int getSpeed() {
		return level;
	}
}
