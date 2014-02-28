
public class TestMain {

	public static void main(String[] args) {
		SimpleRemoteControl remote = new SimpleRemoteControl();
		Light light = new Light();
		GarageDoor door = new GarageDoor();
		LightOnCommand lighton = new LightOnCommand(light);
		GarageDoorOpenCommand doorup = new GarageDoorOpenCommand(door);
		
		remote.setCommand(lighton);
		remote.buttonWasPressed();
		remote.setCommand(doorup);
		remote.buttonWasPressed();
	}
}
